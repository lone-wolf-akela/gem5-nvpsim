#include "PE_controller.h"

void PE_controller::run()
{
	//PE_ctrl    0010
	OUT en_B = inst_in.get(31, 28) == 0b0010;

	if (state == IDLE)
	{
		if (en_B)
		{
			OUT PE_ctrl_done = false;
			state = RUN;

			//本来这两个值是从inst_in里解析出来的，但是实际硬件和模拟器不一样所以还是分出来了
			//FFTx_addr_next = inst_in.get(25, 16);
			//FFTw_addr_next = inst_in.get(13, 0);
			FFTx_addr_next = FFTx_addr_init;
			FFTw_addr_next = FFTw_addr_init;

			pipeline_current = 0;
			cycle_current = 0;

			//要进行mac_num次运算。
			//收到的mac_num本来就是从0开始（少1）的。
			//最开始有一段读取数据的时间。
			pipeline_max = mac_num.getRaw() + 2;

			accu_has_cleared = false;
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
		if (pipeline_current == pipeline_max)
		{
			state = IDLE;

			PE_ctrl_done = true;
			mac_en = false;
			//最后停下来之前把最后一次accu做了
			accu_en = true;
			mac_clear = false;
			accu_clear = false;

			return;
		}
	}

	//最后一层不需要输入只需要计算
	const bool input = pipeline_current < pipeline_max - 1;
	//刚开始只需要输入不需要计算
	const bool compute = pipeline_current >= 1;


	if (cycle_current == int(bit_num.getRaw()))
	{
		PE_data_in = true;
	}
	else
	{
		PE_data_in = false;
	}

	if(input)
	{
		if (cycle_current == 0)
		{
			OUT FFTx_addr = decltype(FFTx_addr)::ValueType(FFTx_addr_next);
			FFTx_addr_next += FFTx_addr_step;
		}
		else
		{
			OUT FFTx_addr = FFTx_addr.getRaw() + 1;
		}

		OUT FFTx_sram_en = true;
	}
	else
	{
		FFTx_sram_en = false;
	}

	if(compute)
	{
		//只有在最开始的时候才进行accu_clear
		if (!accu_has_cleared)
		{		
			OUT accu_clear = true;
			accu_has_cleared = true;
		}
		else
		{
			OUT accu_clear = false;
		}

		if (cycle_current == 0)
		{
			//每次乘法算完之后，都需要进行一次累加
			accu_en = true;
			//还要清空乘法寄存器
			mac_clear = true;
		}
		else
		{
			accu_en = false;
			mac_clear = false;			
		}

		//计算期间mac_en始终为true
		mac_en = true;
		if (cycle_current == 0)
		{
			OUT FFTw_addr = decltype(FFTw_addr)::ValueType(FFTw_addr_next);
			FFTw_addr_next += NUM_LEN;
		}
		else
		{
			OUT FFTw_addr = FFTw_addr.getRaw() + 1;
		}
		//OUT FFTw_addr = decltype(FFTw_addr)::ValueType(FFTw_addr_next);
		//FFTw_addr_next++;
	}
	else
	{
		mac_en = false;
	}
}