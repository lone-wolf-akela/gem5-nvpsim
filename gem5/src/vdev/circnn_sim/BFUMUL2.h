#pragma once
#include "define.h"
#include "data.h"

//BUMUL_out_re = w_re*in_re - w_im*in_im
//BUMUL_out_im = w_re*in_im + w_im*in_re
//
//BUMUL_out = w * in
class BFUMUL2
{
public:
	IN Inst<5> mac_shift = 0;

	IN Num<NUM_LEN> in_re = 0;
	IN Num<NUM_LEN> in_im = 0;
	IN bool w_re = false;
	IN bool w_im = false;

	IN bool w_re_sign = false; 
	IN bool w_im_sign = false; 

	IN bool fft_clear = false;

	OUT Num<NUM_LEN> BUMUL_out_re = 0;
	OUT Num<NUM_LEN> BUMUL_out_im = 0;

	void run();

private:
	//状态变量
	Num<NUM_LEN + 4> mul_result_re = 0;
	Num<NUM_LEN + 4> mul_result_im = 0;
};


