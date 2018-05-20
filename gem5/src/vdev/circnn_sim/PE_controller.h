#pragma once
#include "define.h"

class PE_controller
{
public:
	IN Inst<32> inst_in = 0;
	//input bool PC_en_total;
	IN Inst<4> mac_num = 0;
	IN Inst<4> bit_num = 0;
	IN int FFTx_addr_step = 0;

	//本来这两个值是从inst_in里解析出来的，但是实际硬件和模拟器不一样所以还是分出来了
	IN size_t FFTx_addr_init = 0;
	IN size_t FFTw_addr_init = 0;

	OUT bool PE_ctrl_done = true;
	OUT bool mac_en = false;
	OUT bool accu_en = false;
	OUT bool mac_clear = false;
	OUT bool accu_clear = false;
	//output bool [4:0]bitchoose;
	OUT bool FFTx_sram_en = false;
	//OUT bool tsram_en = false;
	OUT Inst<10> FFTx_addr = 0;
	OUT Inst<12> FFTw_addr = 0;
	OUT bool en_B = false;

	//自己加的一个刷新PE_array内部输入缓存的信号
	OUT bool PE_data_in = false;

	void run();

	//以12周期为一层次，层次内的周期序号
	//放到public来只是为了便于调试
	int cycle_current = 0;

private:
	enum { RUN, IDLE }state = IDLE;

	size_t FFTx_addr_next = 0;
	size_t FFTw_addr_next = 0;

	//一次运行的总层次数和当前层次序号
	int pipeline_max = 0;
	int pipeline_current = 0;

	bool accu_has_cleared = false;
};


