#include "PE_array.h"
void PE_array::run()
{
	//pe和accu_result_out都是 第一个下标决定是哪个输入，第二个下标决定是哪个w
	for (int input = 0; input < 4; input++)
	{
		for (int w = 0; w < 4; w++)
		{
			for (int i = 0; i < 16; i++)
			{
				auto &pe_this = pe[input][w][i];

				pe_this.mac_en = mac_en;
				pe_this.accu_en = accu_en;
				pe_this.mac_clear = mac_clear;
				pe_this.accu_clear = accu_clear;				
				pe_this.weightbit_re = weightbit_re[w][i];
				pe_this.weightbit_im = weightbit_im[w][i];

				pe_this.run();

				pe_this.activ_re = activ_re_buffer[input][i];
				pe_this.activ_im = activ_im_buffer[input][i];

				accu_result_out_re_full[input][w][i] = pe_this.accu_result_out_re;
				accu_result_out_im_full[input][w][i] = pe_this.accu_result_out_im;
			}
		}
	}

	if (data_in)
	{
		for (int i = 0; i < 4; i++)
		{
			activ_re_buffer[i] = activ_re[i];
			activ_im_buffer[i] = activ_im[i];
		}
	}
}