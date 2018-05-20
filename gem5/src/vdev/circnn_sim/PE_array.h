#pragma once
#include "define.h"
#include "PE.h"

class PE_array
{
public:
	IN bool mac_en = false;
	IN bool accu_en = false;
	IN bool mac_clear = false;
	IN bool accu_clear = false;

	IN std::array<Num<NUM_LEN>, 16> activ_re[4];
	IN std::array<Num<NUM_LEN>, 16> activ_im[4];
	IN std::bitset<16> weightbit_re[4];
	IN std::bitset<16> weightbit_im[4];
	//IN Inst<5> bitchoose = 0;


	IN bool data_in = false;

	//第一个下标决定是哪个输入，第二个下标决定是哪个w
	OUT std::array<Num<NUM_LEN * 2>, 16> accu_result_out_re_full[4][4];
	OUT std::array<Num<NUM_LEN * 2>, 16> accu_result_out_im_full[4][4];

	void run();

private:
	//第一个下标决定是哪个输入，第二个下标决定是哪个w
	std::array<PE, 16> pe[4][4];

	std::array<Num<NUM_LEN>, 16> activ_re_buffer[4];
	std::array<Num<NUM_LEN>, 16> activ_im_buffer[4];
};

