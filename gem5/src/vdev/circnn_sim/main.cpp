#include "main.h"



using namespace std;


const int NOT_USED = -1;
int batch_num, bit_num, fft_level, shift_mode, mac_shift42, mac_shift10, out_switch;
int mac_num, FFTx_addr_step_inst, FFTx_addr_step_sim;
bool FFTx_choose, tf_en;
int x_addr, FFTx_addr, bit_num_pe;
int FFTx_addr_inst, FFTw_addr, FFTx_addr_sim, FFTx_part, FFTx_addr_numberStep;
int rightshift, bias_size;
				
std::pair<double,double> CirCNN::Run()
{
	if(state == HighEnergy)
	{
		cycleLen = cycleLenHigh;
	}
	else 
	{
		cycleLen = cycleLenLow;
	}
	
	
	double timeused = 1, energyused = 1;
	
	int batch, ins, layer;
	ins = counter % 23;
	layer = counter / 23 % 3;
	batch = counter / 23 / 3 % 62;
	
		
	if(ins == 0)
	{
		sim.Mem_in(batch * 8, layer == 0 ? 8 : -1, layer);	
			sim.Parameter(
				true,
				batch_num = 16,
				bit_num = 12,
				shift_mode = 0,
				mac_shift42 = W_BITNUM - 1,
				mac_shift10 = 0,
				out_switch = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED
			);			
			int cycles = sim.FFT("", FFTx_choose = 0, tf_en = 1, x_addr = 0, fft_level = 6, FFTx_addr = 0);
			timeused = cycles * cycleLen;
			energyused = cycles * (energy_FFT + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 1)
	{
		int cycles = sim.FFT("data/fft_log1.txt", FFTx_choose = 0, tf_en = 0, x_addr = 12, fft_level = 6, FFTx_addr = 0);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_FFT + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 2)
	{
		int cycles = sim.FFT("data/fft_log2.txt", FFTx_choose = 0, tf_en = 0, x_addr = 204, fft_level = 6, FFTx_addr = 16 * 12);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_FFT + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 3)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
			
		int cycles = sim.PE("data/pe_log1.txt",
				FFTx_addr_inst = 0,					//inst结尾的参数是写进data_rom里的
				FFTw_addr = 0,
				FFTx_addr_sim = 0,					//sim结尾的参数是模拟器用的
				FFTx_part = 0,						//这个参蔵反应进GG运算的蔵是128位中的哪个part(0~3)
				FFTx_addr_numberStep = 12			//这个参数反应实际硬件上的FFTx中会被同时取出的四组数
													//在模拟器的FFTx地址空间中的距离
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 4)
	{
		int cycles = sim.Output_PE(x_addr = 12, FFTx_part = 0, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 5)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log2.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 1,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 6)
	{
		int cycles = sim.Output_PE(x_addr = 12, FFTx_part = 1, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 7)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log3.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12 * 2,
				FFTx_addr_sim,
				FFTx_part = 2,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 8)
	{
		int cycles = sim.Output_PE(x_addr = 12, FFTx_part = 2, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 9)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log4.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12 * 3,
				FFTx_addr_sim,
				FFTx_part = 3,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 10)
	{
		int cycles = sim.Output_PE(x_addr = 12, FFTx_part = 3, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 11)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log5.txt",
				FFTx_addr_inst = 0,					
				FFTw_addr = 8 * 12 * 4,
				FFTx_addr_sim = 0,				
				FFTx_part = 0,						
				FFTx_addr_numberStep = 12				
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 12)
	{
		int cycles = sim.Output_PE(x_addr = 204, FFTx_part = 0, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 13)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log6.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12 * 5,
				FFTx_addr_sim,
				FFTx_part = 1,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 14)
	{
		int cycles = sim.Output_PE(x_addr = 204, FFTx_part = 1, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 15)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log7.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12 * 6,
				FFTx_addr_sim,
				FFTx_part = 2,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 16)
	{
		int cycles = sim.Output_PE(x_addr = 204, FFTx_part = 2, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 17)
	{
		sim.Parameter(
				false,
				NOT_USED,
				bit_num_pe = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
		int cycles = sim.PE("data/pe_log8.txt",
				FFTx_addr_inst = 0,
				FFTw_addr = 8 * 12 * 7,
				FFTx_addr_sim,
				FFTx_part = 3,
				FFTx_addr_numberStep = 12	
			);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_PE + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 18)
	{
		int cycles = sim.Output_PE(x_addr = 204, FFTx_part = 3, rightshift = bit_num_pe - 6);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
	}
	if(ins == 19)
	{
		sim.Parameter(
				true,
				batch_num = 16,
				bit_num = 12,
				shift_mode = 0,
				mac_shift42 = 4,
				mac_shift10 = 1,
				out_switch = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED
			);
	}
	if(ins == 20)
	{
		int cycles = sim.FFT("data/ifft_log1.txt", FFTx_choose = 0, tf_en = 0, x_addr = 12, fft_level = 6, FFTx_addr = 0);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_FFT + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 21)
	{
		int cycles = sim.FFT("data/ifft_log2.txt", FFTx_choose = 0, tf_en = 0, x_addr = 204, fft_level = 6, FFTx_addr = 192);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_FFT + energy_other) /1000.; //pJ->nJ
	}
	if(ins == 22)
	{
		int cycles = sim.Output_IFFT(x_addr = 12, FFTx_addr_sim = 0, batch_num = 32, rightshift = 0);
		timeused = cycles * cycleLen;
		energyused = cycles * (energy_other) /1000.; //pJ->nJ
		
		cycles = sim.Bias_Relu(x_addr = 12, FFTw_addr = 8 * 12 * 8, bias_size = 512, batch_num = 4);
		timeused += cycles * cycleLen;
		energyused += cycles * (energy_BiasReLU + energy_other) /1000.; //pJ->nJ
	}
	
	counter++;	
	finished = (counter % 23 == 0) && (counter / 23 % 3 == 0);	//回到了第0层，说明算完了。
	if(finished)
	{
		Result = sim.GetLayer3();
	}
	return std::make_pair(timeused,energyused);
}

void CirCNN::Rewind()
{
	counter--;
}