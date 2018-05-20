#pragma once
#include "define.h"

class PE
{
public:
	IN bool mac_en = false;
	IN bool accu_en = false;
	IN bool mac_clear = false;
	IN bool accu_clear = false;
	IN Num<NUM_LEN> activ_re = 0;
	IN Num<NUM_LEN> activ_im = 0;
	IN bool weightbit_re = false;
	IN bool weightbit_im = false;

	OUT Num<NUM_LEN*2> accu_result_out_re = 0;
	OUT Num<NUM_LEN*2> accu_result_out_im = 0;

	void run();

private:
	int w_bitcounter = 0;
	Num<NUM_LEN> wre_full = 0;
	Num<NUM_LEN> wim_full = 0;
};


