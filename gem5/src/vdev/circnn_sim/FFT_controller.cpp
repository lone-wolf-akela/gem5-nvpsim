#include "FFT_controller.h"
void FFT_controller::run()
{
	//FFT_ctrl    0001
	OUT en_A = inst_in.get(31, 28) == 0b0001;

	if (state == IDLE)
	{
		if (en_A)
		{
			state = RUN;
			x_addr_next = inst_in.get(25, 16);
			//x_addr_step = inst_in.get(15, 12);
			x_addr_step = bit_num.getRaw() + 1;
			FFTx_addr_next = inst_in.get(9, 0);
			
			OUT FFT_ctrl_done = false;

			pipeline_max = batch_num.getRaw() + fft_level.getRaw() + 1;
			pipeline_current = 0;
			cycle_current = 0;
		}
	}
	else //state == RUN
	{
		if (pipeline_current < pipeline_max)
		{
			cycle_current++;
			if (cycle_current > int(bit_num.getRaw()))
			{
				cycle_current = 0;
				pipeline_current++;
			}		
		}
		if(pipeline_current == pipeline_max)
		{
			state = IDLE;

			OUT FFT_ctrl_done = true;
			OUT mac_en = 0;
			OUT bfu_en = 0;
			OUT fft_clear = 0b11111;
			OUT FFTx_sram_en = false;

			return;
		}
	}
	 
	std::array<bool, FFT_LEVEL+1> level_en;
	for(int i=0;i<=FFT_LEVEL;i++)
	{
		level_en[i] = (pipeline_current - i <= int(batch_num.getRaw())) &&
			(pipeline_current >= i);
	}


	//根据cycle_num和level_en决定各层行为

	//输入层
	if(level_en[0])
	{
		//在12周期中的第0个周期增加x_addr_step
		if (cycle_current == 0)
		{
			OUT x_addr = decltype(x_addr)::ValueType(x_addr_next);
			x_addr_next += x_addr_step;
		}
		else
		{
			OUT x_addr = x_addr.getRaw() + 1;
		}
		OUT x_sram_en = true;		
	}
	else
	{
		OUT x_sram_en = false;
	}

	//FFT 1~fft_level 层
	for (int lv = 0; lv < int(fft_level.getRaw()) - 1; lv++)
	{
		//实质上每层只需要 1 + W_BITNUM 个周期
		//bfu
		if (level_en[lv + 1] && cycle_current == 0)
		{
			OUT fft_clear.setBit(lv, true);
			OUT mac_en.setBit(lv, false);
			OUT bfu_en.setBit(lv, true);
		}
		//first mul
		else if (level_en[lv + 1] && cycle_current == 1)
		{
			OUT fft_clear.setBit(lv, true);
			OUT mac_en.setBit(lv, true);
			OUT bfu_en.setBit(lv, false);
		}
		//mul
		else if (level_en[lv + 1] && cycle_current <= W_BITNUM)
		{
			OUT fft_clear.setBit(lv, false);
			OUT mac_en.setBit(lv, true);
			OUT bfu_en.setBit(lv, false);
		}
		//闲置
		else
		{
			OUT fft_clear.setBit(lv, true);
			OUT mac_en.setBit(lv, false);
			OUT bfu_en.setBit(lv, false);
		}
	}
	//twiddle_bitchoose比较特殊，因为每层的这个参数是相同的，所以放在循环外面
	if (cycle_current >= 1 && cycle_current <= W_BITNUM)
	{
		OUT twiddle_bitchoose = W_BITNUM - cycle_current;
	}
	else
	{
		OUT twiddle_bitchoose = 0;
	}

	//FFT 最后一层（只有bfu没有mul） 及输出
	if(level_en[fft_level.getRaw()])
	{
		if (cycle_current == 0)
		{
			OUT bfu_en.setBit(fft_level.getRaw() - 1, true);
		}
		else
		{
			OUT bfu_en.setBit(fft_level.getRaw() - 1, false);
		}

		OUT fft_clear.setBit(fft_level.getRaw() - 1, true);
		OUT mac_en.setBit(fft_level.getRaw() - 1, false);

		OUT FFTx_sram_en = true;	
		OUT out_switch = bit_num.getRaw() - cycle_current;

		OUT FFTx_addr = decltype(FFTx_addr)::ValueType(FFTx_addr_next);
		FFTx_addr_next++;
	}
	else
	{
		OUT FFTx_sram_en = false;
		OUT bfu_en.setBit(fft_level.getRaw() - 1, false);
	}

	//FFTx_sram_en = out_sram_en && !FFTx_choose;
	//tsram_en = out_sram_en && FFTx_choose;
	//x_sram_en = in_sram_en;
}
