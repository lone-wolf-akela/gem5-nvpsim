#pragma once
#include "define.h"

//BU_out_re1 = in_re1 + in_re2
//BU_out_re2 = in_re1 - in_re2
//BU_out_im1 = in_im1 + in_im2
//BU_out_im2 = in_im1 - in_im2
//
//BU_out_1 = in_1 + in_2
//BU_out_2 = in_1 - in_2
class BFU2
{
public:
	IN Num<NUM_LEN> in_re1 = 0;
	IN Num<NUM_LEN> in_re2 = 0;
	IN Num<NUM_LEN> in_im1 = 0;
	IN Num<NUM_LEN> in_im2 = 0;
	IN bool shift_mode = false;

	OUT Num<NUM_LEN> BU_out_re1 = 0;
	OUT Num<NUM_LEN> BU_out_re2 = 0;
	OUT Num<NUM_LEN> BU_out_im1 = 0;
	OUT Num<NUM_LEN> BU_out_im2 = 0;

	void run();
};

