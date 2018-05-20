#include "BFUMUL2.h"
void BFUMUL2::run()
{
	const Num<NUM_LEN> activ_valid_rere = w_re ? in_re : 0;
	const Num<NUM_LEN> activ_valid_reim = w_im ? in_re : 0;
	const Num<NUM_LEN> activ_valid_imre = w_re ? in_im : 0;
	const Num<NUM_LEN> activ_valid_imim = w_im ? in_im : 0;

	//case(mac_shift[1:0]) shift right 0-3bit
	const int rightshift = mac_shift.get(1, 0);
	Num<NUM_LEN> activ_valid_re, activ_valid_im;

	if (w_re_sign && w_im_sign)
	{
		activ_valid_re = activ_valid_rere.signKeepSub(activ_valid_imim, rightshift);
		activ_valid_im = activ_valid_reim.signKeepAdd(activ_valid_imre, rightshift);
	}
	else if (w_re_sign && !w_im_sign)
	{
		activ_valid_re = activ_valid_rere.signKeepAdd(activ_valid_imim, rightshift);
		activ_valid_im = activ_valid_imre.signKeepSub(activ_valid_reim, rightshift);
	}
	else if (!w_re_sign && w_im_sign)
	{
		activ_valid_re = -activ_valid_rere.signKeepAdd(activ_valid_imim, rightshift);
		activ_valid_im = activ_valid_reim.signKeepSub(activ_valid_imre, rightshift);
	}
	else //if(!w_re_sign && !w_im_sign)
	{
		activ_valid_re = activ_valid_imim.signKeepSub(activ_valid_rere, rightshift);
		activ_valid_im = -activ_valid_reim.signKeepAdd(activ_valid_imre, rightshift);
	}

	//always posedge clk块
	if (fft_clear)
	{
		mul_result_re = activ_valid_re.changeBit<NUM_LEN + 4>();
		mul_result_im = activ_valid_im.changeBit<NUM_LEN + 4>();
	}
	else
	{
		mul_result_re = (mul_result_re << 1).signKeepAdd(
			activ_valid_re.changeBit<NUM_LEN + 4>()
		);
		mul_result_im = (mul_result_im << 1).signKeepAdd(
			activ_valid_im.changeBit<NUM_LEN + 4>()
		);
	}
	/////

	//case(mac_shift[4:2])  shift right 0-7bit
	const int rightshift2 = mac_shift.get(4, 2);
	BUMUL_out_re = (mul_result_re >> rightshift2).changeBit<NUM_LEN>();
	BUMUL_out_im = (mul_result_im >> rightshift2).changeBit<NUM_LEN>();
}