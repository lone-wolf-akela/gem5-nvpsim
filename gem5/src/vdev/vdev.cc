//
// Created by lf-z on 4/24/17.
//

#include "vdev/vdev.hh"
#include "engy/state_machine.hh"
#include "debug/EnergyMgmt.hh"
#include "debug/VirtualDevice.hh"
#include "debug/MemoryAccess.hh"
//#include "engy/DVFS.hh"
#include "engy/two_thres.hh"

extern int Temp_Workaround_Vdev_State;

VirtualDevice::DevicePort::DevicePort(const std::string &_name, VirtualDevice *_vdev)
    : SlavePort(_name, _vdev), vdev(_vdev)
{

}

Tick
VirtualDevice::DevicePort::recvAtomic(PacketPtr pkt)
{
    assert(vdev);
    return vdev->recvAtomic(pkt);
}

void
VirtualDevice::DevicePort::recvFunctional(PacketPtr pkt)
{
    assert(vdev);
    vdev->recvFunctional(pkt);
}

bool
VirtualDevice::DevicePort::recvTimingReq(PacketPtr pkt)
{
    assert(vdev);
    return vdev->recvTimingReq(pkt);
}

void
VirtualDevice::DevicePort::recvRespRetry()
{
    assert(vdev);
    vdev->recvRespRetry();
}

AddrRangeList
VirtualDevice::DevicePort::getAddrRanges() const
{
    assert(vdev);

    AddrRangeList rangeList;
    rangeList.push_back(vdev->getAddrRange());

    return rangeList;
}

VirtualDevice::VirtualDevice(const Params *p)
    : MemObject(p),
      id(0),
      port(name() + ".port", this),
      cpu(p->cpu),
      range(p->range),
      delay_set(p->delay_set),
      //delay_self(p->delay_self),
      delay_recover(p->delay_recover),
      delay_cpu_interrupt(p->delay_cpu_interrupt),
      is_interruptable(p->is_interruptable),
      //delay_remained(p->delay_remained + p->delay_recover),
      event_interrupt(this, false, Event::Virtual_Interrupt)
{
    trace.resize(0);
    pmem = (uint8_t*) malloc(range.size() * sizeof(uint8_t));
    memset(pmem, 0, range.size() * sizeof(uint8_t));
}

void
VirtualDevice::init()
{
    MemObject::init();

    if (port.isConnected()) {
        port.sendRangeChange();
    }

    cpu->registerVDev(delay_recover, id);
    DPRINTF(VirtualDevice, "Virtual Device started with range: %#lx - %#lx\n",
            range.start(), range.end());

    execution_state = STATE_POWEROFF;
}

void
VirtualDevice::triggerInterrupt()
{
		if(circnn.finished)
		{
	    /* Todo: add static of finish success. */
	    DPRINTF(VirtualDevice, "Virtual device triggers an interrupt.\n");
	    execution_state = STATE_IDLE; // The virtual device enter/keep in the active status.
	    finishSuccess();
	    assert(*pmem & VDEV_WORK);
	    /* Change register byte. */
	    *pmem |= VDEV_FINISH;
	    *pmem &= ~VDEV_WORK;

	    /* Tell cpu. */
	    cpu->virtualDeviceInterrupt(delay_cpu_interrupt);
	    cpu->virtualDeviceEnd(id);    
	    
	    //output result
	    uint8_t *current_pointer = pmem + 1;
	    for(size_t i=0;i<circnn.Result.size();i++)
	    {
	    	memcpy( current_pointer,
	    			circnn.Result[i].data(),
	    			sizeof(double) * circnn.Result[i].size()
	    		);
	    	current_pointer += sizeof(double) * circnn.Result[i].size();
	  	}
	  }
	  else //not finished, run next inst
	  {
	  	/* Schedule interrupt. */
	  	circnn.state = Temp_Workaround_Vdev_State ? CirCNN::State::HighEnergy : CirCNN::State::LowEnergy;
      std::pair<double,double> time_energy = circnn.Run();
      delay_self = time_energy.first;
      energy_need = time_energy.second;
      totalTime += delay_self;
			totalEnergy += energy_need;
			DPRINTF(VirtualDevice, "Inst total use: T: %lf E: %lf\n", totalTime, totalEnergy);
      schedule(event_interrupt, curTick() + delay_self);
      /* Energy consumption. */
      DPRINTF(VirtualDevice, "Next Inst, Need Ticks:%i, Cycles:%i, Energy: %lf .\n", 
      	delay_self, 
      	ticksToCycles(delay_self), 
      	time_energy.second
      );
      DPRINTF(VirtualDevice, "[VirtualDevice] Energy %lf is consumed.\n", energy_need);
      consumeEnergy(energy_need);
	  }
}

Tick
VirtualDevice::access(PacketPtr pkt)
{
    /* Todo: what if the cpu ask to work on a task when the vdev is busy? **/
    DPRINTF(MemoryAccess, "Virtual Device accessed at %#lx.\n", pkt->getAddr());
    Addr offset = pkt->getAddr() - range.start();
    if (pkt->isRead()) {
        memcpy(pkt->getPtr<uint8_t>(), pmem+offset, pkt->getSize());
    } else if (pkt->isWrite()) {
        const uint8_t* pkt_addr = pkt->getConstPtr<uint8_t>();
        if (offset == 0) {
            /* Might be a request. */
            if (*pkt_addr & VDEV_SET) {
                /* Request */
                if (*pmem & VDEV_WORK) {
                    /* Request fails because the vdev is working. */
                    DPRINTF(VirtualDevice, "Request discarded!\n");
                } else {
                    /* Request succeeds. */
                    execution_state = STATE_ACTIVE; // The virtual device enter/keep in the active status.
                    DPRINTF(VirtualDevice, "Virtual Device starts working.\n");                
                    
                    /* Set the virtual device to working mode */
                    *pmem |= VDEV_WORK;
                    *pmem &= ~VDEV_FINISH;
                    /* Schedule interrupt. */
                    circnn.state = Temp_Workaround_Vdev_State ? CirCNN::State::HighEnergy : CirCNN::State::LowEnergy;
                    std::pair<double,double> time_energy = circnn.Run();
                    delay_self = time_energy.first;
                    energy_need = time_energy.second;
                    totalTime += (delay_self + delay_set);
										totalEnergy += energy_need;
										DPRINTF(VirtualDevice, "Inst total use: T: %lf E: %lf\n", totalTime, totalEnergy);							
                    schedule(event_interrupt, curTick() + delay_set + delay_self);
                    /* Energy consumption. */
                    DPRINTF(VirtualDevice, "Need Ticks:%i, Cycles:%i, Energy: %lf .\n", 
                    	delay_self, 
                    	ticksToCycles(delay_self), 
                    	time_energy.second
                    );
                    DPRINTF(VirtualDevice, "[VirtualDevice] Energy %lf is consumed.\n", energy_need);
                    consumeEnergy(energy_need);
                    cpu->virtualDeviceSet(delay_set);
                    cpu->virtualDeviceStart(id);
                }
            } else {
                /* Not a request, but the first byte cannot be written. */
            }
        } else {
            /* Normal write. */
            memcpy(pmem+offset, pkt_addr, pkt->getSize());
        }
    }
    return 0;
}


//By LiuRuoyang：这个tick()函数是干什么的？似乎它会按时间每tick扣掉一点能量。
//我想我们可以安全的把那行consumeEnergy给注释掉吧。

void 
VirtualDevice::tick()
{
    
    /*switch(execution_state)
    {
        case STATE_POWEROFF:    
            DPRINTF(VirtualDevice, "Tick: state is POWER OFF.\n"); 
            break;
        case STATE_IDLE:             
            DPRINTF(VirtualDevice, "Tick: state is IDLE(SLEEP).\n", execution_state); 
            break;
        case STATE_ACTIVE:         
            DPRINTF(VirtualDevice, "Tick: state is ACTIVE.\n", execution_state); 
            break;
        default:    DPRINTF(VirtualDevice, "Tick: undefined state!\n", ); break;
    }*/
    //DPRINTF(VirtualDevice, "Tick\n");
    /** Energy consumption **/
    //consumeEnergy(energy_consumed_per_cycle_vdev[execution_state] * ticksToCycles(1));

    //schedule(tickEvent, curTick() + 1);
}


int
VirtualDevice::handleMsg(const EnergyMsg &msg)
{
    DPRINTF(EnergyMgmt, "Device handleMsg called at %lu, msg.type=%d\n", curTick(), msg.type);
    switch(msg.type) {
        case (int) TwoThresSM::MsgType::POWEROFF:
            /** Vdev shutdown **/   
            DPRINTF(VirtualDevice, "power-off msg\n");
            execution_state = STATE_POWEROFF;        
            /* Re-calculate the delay if the device is interruptable */
            if (*pmem & VDEV_WORK) 
            {
            	/* This should be handled if the device is on a task **/
              assert(event_interrupt.scheduled());
            	DPRINTF(VirtualDevice, "device power off occurs in the middle of a task at %lu\n", curTick());
	            /* Calculate the remaining delay*/
              delay_remained = event_interrupt.when() - curTick();
              deschedule(event_interrupt);
	            if (!is_interruptable)
	            {
	            	delay_remained = delay_recover + delay_self;
	            }
         		}
            break;
        case (int) TwoThresSM::MsgType::POWERON:
            /** Vdev poweron **/
            execution_state = STATE_IDLE;
            if (*pmem & VDEV_WORK) {
            		execution_state = STATE_ACTIVE;
                assert(!event_interrupt.scheduled());
                DPRINTF(VirtualDevice, "device power on to finish a task at %lu\n", curTick());
                if(!is_interruptable)
                {               	
                	circnn.Rewind();
                	circnn.state = Temp_Workaround_Vdev_State ? CirCNN::State::HighEnergy : CirCNN::State::LowEnergy;
                	std::pair<double,double> time_energy = circnn.Run();
                  delay_self = time_energy.first;
                  energy_need = time_energy.second;
                  totalTime += (delay_self + delay_recover);
									totalEnergy += energy_need;
									DPRINTF(VirtualDevice, "Inst total use: T: %lf E: %lf\n", totalTime, totalEnergy);							
                  DPRINTF(VirtualDevice, "device rewind. Need Time: %lf\n", time_energy.first);
                  delay_remained = delay_recover + delay_self;
                }
                schedule(event_interrupt, curTick() + delay_remained);
                /** Energy consumption **/
                DPRINTF(VirtualDevice, "[VirtualDevice] Energy %lf is consumed.\n", energy_need);
                consumeEnergy(energy_need);
            }
            break;
        default:
            return 0;
    }
    return 1;
}

BaseSlavePort&
VirtualDevice::getSlavePort(const std::string &if_name, PortID idx)
{
    if (if_name == "port") {
        return port;
    } else {
        return MemObject::getSlavePort(if_name, idx);
    }
}

AddrRange
VirtualDevice::getAddrRange() const
{
    return range;
}

Tick
VirtualDevice::recvAtomic(PacketPtr pkt)
{
    return access(pkt);
}

void
VirtualDevice::recvFunctional(PacketPtr pkt)
{
    fatal("Functional access is not supported now.");
}

bool
VirtualDevice::recvTimingReq(PacketPtr pkt)
{
    fatal("Timing access is not supported now.");
}

void
VirtualDevice::recvRespRetry()
{
    fatal("Timing access is not supported now.");
}

bool
VirtualDevice::finishSuccess()
{
    /* Todo: Need further implementation. */
    return 1;
}

VirtualDevice *
VirtualDeviceParams::create()
{
    return new VirtualDevice(this);
}
