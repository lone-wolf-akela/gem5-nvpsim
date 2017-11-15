import m5
from m5.objects import *

import sys  
cap = float(sys.argv[1])/10



system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1MHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'atomic'
system.mem_ranges = [AddrRange('512MB')]

#vdev
system.has_vdev = 1
system.vdev_ranges = [AddrRange('512MB', '512MB'), AddrRange('513MB', '513MB'), AddrRange('514MB', '514MB')]
system.vaddr_vdev_ranges = [AddrRange('1000MB', '1000MB'), AddrRange('1001MB', '1001MB'), AddrRange('1002MB', '1002MB')]
###

#energy mgmt
system.energy_mgmt = EnergyMgmt(path_energy_profile = 'profile/energy_prof', energy_time_unit = '10us')
system.energy_mgmt.state_machine = DFS_LRY(thres_off = 1000,
                                           thres_retention = 2500,
                                           thres_2 = 5000, 
                                           thres_3 = 7500,
                                           thres_4 = 15000,
                                           thres_5 = 20000)
system.energy_mgmt.capacity = cap;	#uF
system.energy_mgmt.energy_consumed_per_harvest = 0.02; 
###

#set some parameters for the CPU
system.cpu = AtomicSimpleCPU(energy_consumed_per_cycle_5 = 2.25/100,
                             energy_consumed_per_cycle_4 = 1.5/100,
                             energy_consumed_per_cycle_3 = 1/100,
                             energy_consumed_per_cycle_2 = 0.5/100,
                             energy_consumed_per_cycle_1 = 0.25/100,
	  
                             energy_consumed_poweron = 1/100,  
                             clockPeriod_to_poweron = 1/100,
	  
                             clock_mult_5 = 1/2.25,
                             clock_mult_4 = 1/1.5,
                             clock_mult_3 = 1,
                             clock_mult_2 = 1/0.5,
                             clock_mult_1 = 1/0.25)

#add by LiuRuoyang
#Attention: There's a limit that we can only simulate 18446744073709551615 ticks
#or the simulator will say: "Exiting @ tick 18446744073709551615 because simulate() limit reached"
system.cpu.max_insts_any_thread = 400000
###

system.cpu.s_energy_port = system.energy_mgmt.m_energy_port

system.membus = SystemXBar()

system.cpu.icache_port = system.membus.slave
system.cpu.dcache_port = system.membus.slave

system.cpu.createInterruptController()

system.mem_ctrl = DDR3_1600_x64()
system.mem_ctrl.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.master

system.system_port = system.membus.slave

system.vdev1 = VirtualDevice()
system.vdev1.cpu = system.cpu
system.vdev1.range = system.vdev_ranges[0]
system.vdev1.energy_consumed_per_cycle_vdev = [Float(0), Float(0.25*1), Float(1.75*1)] # The energy consumption of each cycle at power-off, idle and active mode.
system.vdev1.delay_self = '10ms'
system.vdev1.delay_cpu_interrupt = '100us'
system.vdev1.delay_set = '200us'
system.vdev1.delay_recover = '100us'
system.vdev1.is_interruptable = 0
system.vdev1.port = system.membus.master
system.vdev1.s_energy_port = system.energy_mgmt.m_energy_port

system.vdev2 = VirtualDevice()
system.vdev2.cpu = system.cpu
system.vdev2.range = system.vdev_ranges[1]
system.vdev2.energy_consumed_per_cycle_vdev = [Float(0), Float(0.5*1), Float(1.5*1)] # The energy consumption of each cycle at power-off, idle and active mode.
system.vdev2.delay_self = '10ms'
system.vdev2.delay_cpu_interrupt = '100us'
system.vdev2.delay_set = '200us'
system.vdev2.delay_recover = '100us'
system.vdev2.is_interruptable = 0
system.vdev2.port = system.membus.master
system.vdev2.s_energy_port = system.energy_mgmt.m_energy_port

system.vdev3 = VirtualDevice()
system.vdev3.cpu = system.cpu
system.vdev3.range = system.vdev_ranges[2]
system.vdev3.energy_consumed_per_cycle_vdev = [Float(0), Float(0.1*1), Float(3.0*1)] # The energy consumption of each cycle at power-off, idle and active mode.
system.vdev3.delay_self = '100us'
system.vdev3.delay_cpu_interrupt = '100us'
system.vdev3.delay_set = '200us'
system.vdev3.delay_recover = '100us'
system.vdev3.is_interruptable = 0
system.vdev3.port = system.membus.master
system.vdev3.s_energy_port = system.energy_mgmt.m_energy_port

process = LiveProcess()
process.cmd = ['tests/test-progs/brgMonitor/main_trans']
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system = system)
m5.instantiate()

print "Beginning simulation! cap: %f" %cap
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())

f = open("m5out/batch_res.csv","a")
f.write("%f,%i,%s\n" % (cap, m5.curTick(), exit_event.getCause()))
f.close()