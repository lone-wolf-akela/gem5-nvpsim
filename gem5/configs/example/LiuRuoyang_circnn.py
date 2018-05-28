import m5
from m5.objects import *

#import os
#if os.path.exists("m5out/devicedata"):
#	os.remove("m5out/devicedata")

import sys  
cap = 20
profilemult = 0.5

#ticks_per_sec = 1000000000000
#max_sec = 100

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1MHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'atomic'
system.mem_ranges = [AddrRange('512MB')]

#vdev
system.has_vdev = 1
system.vdev_ranges = [AddrRange('512MB', '513MB')]#, AddrRange('513MB', '513MB'), AddrRange('514MB', '514MB')]
system.vaddr_vdev_ranges = [AddrRange('1000MB', '1001MB')]#, AddrRange('1001MB', '1001MB'), AddrRange('1002MB', '1002MB')]
###

#energy mgmt
system.energy_mgmt = EnergyMgmt(path_energy_profile = 'profile/Solar2.txt', energy_time_unit = '100us')
system.energy_mgmt.state_machine = TwoThresSM()
system.energy_mgmt.state_machine.thres_high = 500
system.energy_mgmt.state_machine.thres_low = 30
system.energy_mgmt.state_machine.thres_vdevHigh = 500
system.energy_mgmt.state_machine.thres_vdevLow = 400

system.energy_mgmt.capacity = cap;	#uF
system.energy_mgmt.energy_profile_mult = profilemult; 
###

#set some parameters for the CPU
system.cpu = AtomicSimpleCPU(energy_consumed_per_cycle = 1,
                             energy_consumed_poweroff = 20,
                             energy_consumed_poweron = 50,  
                             clockPeriod_to_poweron = 10,
	  													)

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
#system.vdev1.energy_consumed_per_cycle_vdev = 1
#system.vdev1.delay_self = '10ms'
system.vdev1.delay_cpu_interrupt = '10us'
system.vdev1.delay_set = '10us'
system.vdev1.delay_recover = '10us'
system.vdev1.is_interruptable = 0
system.vdev1.port = system.membus.master
system.vdev1.s_energy_port = system.energy_mgmt.m_energy_port

#system.vdev3.need_log = 1

process = LiveProcess()
process.cmd = ['tests/test-progs/circnn/main']
#process.cmd = ['tests/test-progs/queens/queens']
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system = system)
m5.instantiate()

print "Beginning simulation! cap: %f" % cap
#exit_event = m5.simulate(int(max_sec * ticks_per_sec))
exit_event = m5.simulate()
print 'Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause())

#if os.path.exists("m5out/devicedata"):
#	fi = open("m5out/devicedata","r")
#	line = fi.readline()
#	vdev_access = int(line)
#	fi.close()
#else:
#	vdev_access = 0
#
#fo = open("m5out/batch_res.csv","a")
#fo.write("%f,%f,%i,%i,%s\n" % (cap, profilemult, vdev_access, m5.curTick(), exit_event.getCause()))
#fo.close()
#
#print "%f,%f,%i,%i" % (cap, profilemult, vdev_access, m5.curTick())