#include "BFU2.h"
void BFU2::run()
{
	if (shift_mode)
	{
		BU_out_re1 = in_re1.signKeepAdd(in_re2, 1);
		BU_out_re2 = in_re1.signKeepSub(in_re2, 1);
		BU_out_im1 = in_im1.signKeepAdd(in_im2, 1);
		BU_out_im2 = in_im1.signKeepSub(in_im2, 1);
	}
	else
	{
		BU_out_re1 = in_re1.signKeepAdd(in_re2);
		BU_out_re2 = in_re1.signKeepSub(in_re2);
		BU_out_im1 = in_im1.signKeepAdd(in_im2);
		BU_out_im2 = in_im1.signKeepSub(in_im2);
	}
}