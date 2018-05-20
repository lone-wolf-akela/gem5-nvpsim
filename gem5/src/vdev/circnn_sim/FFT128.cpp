#include "FFT128.h"
void FFT128::run()
{
	//这个地方要倒着来，才能像实际硬件一样运行好几个周期之后才出结果。

	//level_last();

	for (int i = FFT_LEVEL - 1; i >= 0; i--)
	{
		level(i);
	}

	fft_out_re_full = out_re_s[fft_level.getRaw() - 1];
	fft_out_im_full = out_im_s[fft_level.getRaw() - 1];

	for (size_t i = 0; i < fft_out_re_full.size(); i++)
	{
		OUT fft_out_re[i] = fft_out_re_full[i].getBit(out_switch.getRaw());
		OUT fft_out_im[i] = fft_out_im_full[i].getBit(out_switch.getRaw());
	}

	/*
	assign fft_out_bit = {fft_out_im[63:48],fft_out_re[63:48],
	fft_out_im[47:32],fft_out_re[47:32],
	fft_out_im[31:16],fft_out_re[31:16],
	fft_out_im[15:0] ,fft_out_re[15:0]};
	*/
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 16; j++)
		{
			OUT fft_out_bit[i * 32 + j + 16] = fft_out_im[i * 16 + j];
			OUT fft_out_bit[i * 32 + j] = fft_out_re[i * 16 + j];
		}
	}

	in_re_buffer = in_re;
	in_im_buffer = in_im;
}

void FFT128::level(int lv)
{
	assert(lv >= 0);
	assert(lv < FFT_LEVEL);

	std::array<Num<NUM_LEN>, FFT_LEN> *lv_in_re;
	std::array<Num<NUM_LEN>, FFT_LEN> *lv_in_im;
	if (lv == 0)
	{
		lv_in_re = &in_re_buffer;
		lv_in_im = &in_im_buffer;
	}
	else
	{
		lv_in_re = &in_re_s[lv - 1];
		lv_in_im = &in_im_s[lv - 1];
	}

	//第一层blockLen=2，第二层blockLen=4......
	const int blockLen = 1 << (lv + 1);
	//Butterfly unit
	if (bfu_en.get(lv))
	{
		size_t index = 0;
		for (int i = 0; i < FFT_LEN / blockLen; i++)
		{
			for (int j = 0; j < blockLen / 2; j++)
			{
				bfu2[lv][index].shift_mode = shift_mode.get(lv);
				bfu2[lv][index].in_re1 = (*lv_in_re)[i*blockLen + j];
				bfu2[lv][index].in_im1 = (*lv_in_im)[i*blockLen + j];
				bfu2[lv][index].in_re2 = (*lv_in_re)[i*blockLen + j + blockLen / 2];
				bfu2[lv][index].in_im2 = (*lv_in_im)[i*blockLen + j + blockLen / 2];
				
				index++;
			}			
		}
	}

	if (mac_en.get(lv))
	{
		size_t index = 0;
		for (int i = 0; i < FFT_LEN / (blockLen * 2); i++)
		{
			//j=0被略过了，因为这一步的w=1
			for (int j = 1; j < blockLen; j++)
			{
				bfumul2[lv][index].fft_clear = fft_clear.get(lv);
				bfumul2[lv][index].mac_shift = mac_shift;

				bfumul2[lv][index].w_re = w_re[j*FFT_W_LEN / blockLen - 1];
				bfumul2[lv][index].w_im = w_im[j*FFT_W_LEN / blockLen - 1];

				bfumul2[lv][index].w_re_sign = w_re_sign[j*FFT_W_LEN / blockLen - 1];
				bfumul2[lv][index].w_im_sign = w_im_sign[j*FFT_W_LEN / blockLen - 1];


				bfumul2[lv][index].in_re = out_re_s[lv][i*blockLen * 2 + blockLen + j];
				bfumul2[lv][index].in_im = out_im_s[lv][i*blockLen * 2 + blockLen + j];
			
				
				bfumul2[lv][index].run();

				index++;
			}			
		}
	}

	//bfu2的run必须放在bfumul2后面，才能保证时序正确
	if (bfu_en.get(lv))
	{
		for (auto &bfu : bfu2[lv])
		{
			bfu.run();
		}
	}

	//最后取得结果
	//bfu结果
	{
		size_t index = 0;
		for (int i = 0; i < FFT_LEN / blockLen; i++)
		{
			for (int j = 0; j < blockLen / 2; j++)
			{
				out_re_s[lv][i*blockLen + j] = bfu2[lv][index].BU_out_re1;
				out_im_s[lv][i*blockLen + j] = bfu2[lv][index].BU_out_im1;

				out_re_s[lv][i*blockLen + j + blockLen / 2] = bfu2[lv][index].BU_out_re2;
				out_im_s[lv][i*blockLen + j + blockLen / 2] = bfu2[lv][index].BU_out_im2;
			
				index++;
			}			
		}
	}
	//mul结果
	{
		in_re_s[lv] = out_re_s[lv];
		in_im_s[lv] = out_im_s[lv];

		size_t index = 0;
		for (int i = 0; i < FFT_LEN / (blockLen * 2); i++)
		{
			//j=0被略过了，因为这一步的w=1
			for (int j = 1; j < blockLen; j++)
			{
				in_re_s[lv][i*blockLen * 2 + blockLen + j] = bfumul2[lv][index].BUMUL_out_re;
				in_im_s[lv][i*blockLen * 2 + blockLen + j] = bfumul2[lv][index].BUMUL_out_im;
			
				index++;
			}			
		}
	}
}

/*void FFT128::level_last()
{
	for (int i = 0; i < FFT_LEN / 2; i++)
	{
		bfu2[FFT_LEVEL - 1][i].in_re1 = in_re_s[FFT_LEVEL - 2][i];
		bfu2[FFT_LEVEL - 1][i].in_re2 = in_re_s[FFT_LEVEL - 2][i + FFT_LEN / 2];
		bfu2[FFT_LEVEL - 1][i].in_im1 = in_im_s[FFT_LEVEL - 2][i];
		bfu2[FFT_LEVEL - 1][i].in_im2 = in_im_s[FFT_LEVEL - 2][i + FFT_LEN / 2];
		bfu2[FFT_LEVEL - 1][i].shift_mode = shift_mode.get(FFT_LEVEL-1);

		if (bfu_en.get(FFT_LEVEL - 1))
		{
			bfu2[FFT_LEVEL - 1][i].run();
		}

		in_re_s[FFT_LEVEL - 1][i] = bfu2[FFT_LEVEL - 1][i].BU_out_re1;
		in_im_s[FFT_LEVEL - 1][i] = bfu2[FFT_LEVEL - 1][i].BU_out_im1;
		in_re_s[FFT_LEVEL - 1][i + FFT_LEN / 2] = bfu2[FFT_LEVEL - 1][i].BU_out_re2;
		in_im_s[FFT_LEVEL - 1][i + FFT_LEN / 2] = bfu2[FFT_LEVEL - 1][i].BU_out_im2;
	}
}*/
