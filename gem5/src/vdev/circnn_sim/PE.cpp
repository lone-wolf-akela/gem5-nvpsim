#include "PE.h"
void PE::run()
{
	if (accu_en)
	{
		const Num<NUM_LEN * 2> mul_result_re = 
			activ_re.getRawValue() * wre_full.getRawValue() -
			activ_im.getRawValue() * wim_full.getRawValue();
		const Num<NUM_LEN * 2> mul_result_im = 
			activ_re.getRawValue() * wim_full.getRawValue() +
			activ_im.getRawValue() * wre_full.getRawValue();

		accu_result_out_re = accu_clear ? 0 : (accu_result_out_re + mul_result_re);
		accu_result_out_im = accu_clear ? 0 : (accu_result_out_im + mul_result_im);
	}

	if (mac_en)
	{
		if(mac_clear)
		{
			w_bitcounter = NUM_LEN - 1;
			wre_full = 0;
			wim_full = 0;
		}

		assert(w_bitcounter >= 0);

		wre_full.setBit(w_bitcounter, weightbit_re);
		wim_full.setBit(w_bitcounter, weightbit_im);
		w_bitcounter--;
	}
}