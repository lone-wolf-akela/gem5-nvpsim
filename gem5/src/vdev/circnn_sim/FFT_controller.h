#pragma once
#include "FFT128.h"

class FFT_controller
{
public:
	IN Inst<32> inst_in;
	IN Inst<4> batch_num;
	IN Inst<4> bit_num;

	//本来这个应该从inst_in里解析出来的，现在暂时偷懒由simulator输入进来
	IN Inst<3> fft_level;
	//IN bool PC_en_total;

	OUT bool FFT_ctrl_done = true;
	//OUT Inst<7> shift_mode = 0;
	//OUT Inst<4> mac_shift = 0;
	OUT Inst<4> out_switch = 0;
	OUT Inst<FFT_LEVEL> bfu_en = 0;
	OUT Inst<FFT_LEVEL> mac_en = 0;
	OUT Inst<FFT_LEVEL> fft_clear = 0;
	OUT bool x_sram_en = false;
	OUT Inst<4> twiddle_bitchoose;
	OUT bool FFTx_sram_en = false;
	//OUT bool tsram_en = false;

	OUT Inst<10> x_addr = 0;
	OUT Inst<10> FFTx_addr = 0;
	OUT bool en_A = false;

	void run();


	//以12周期为一层次，层次内的周期序号
	//放到public来只是为了便于调试
	int cycle_current = 0;

private:
	enum { IDLE = 0, RUN = 1 } state = IDLE;

	size_t x_addr_next = 0;
	size_t x_addr_step = 0;

	size_t FFTx_addr_next = 0;

	//一次运行的总层次数和当前层次序号
	int pipeline_max = 0;
	int pipeline_current = 0;	
};


