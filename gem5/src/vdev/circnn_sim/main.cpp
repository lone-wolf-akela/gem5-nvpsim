#include "main.h"



using namespace std;


const int NOT_USED = -1;
int batch_num, bit_num, fft_level, shift_mode, mac_shift42, mac_shift10, out_switch;
int mac_num, FFTx_addr_step_inst, FFTx_addr_step_sim;
bool FFTx_choose, tf_en;
int x_addr, FFTx_addr;
int FFTx_addr_inst, FFTw_addr, FFTx_addr_sim, FFTx_part, FFTx_addr_numberStep;
int rightshift, bias_size;
				
std::pair<double,double> CirCNN::Run()
{
	double timeused = 100, energyused = 1;
	finished = false;
	
	if(ins == 0)
	{
		sim.Mem_in(batch, layer == 0 ? 8 : -1, layer);	
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
			sim.Parameter(
				false,
				NOT_USED,
				bit_num = 12,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				NOT_USED,
				mac_num = 8,
				FFTx_addr_step_inst = 0,
				FFTx_addr_step_sim = 12 * 4
			);
			sim.FFT("", FFTx_choose = 0, tf_en = 1, x_addr = 0, fft_level = 6, FFTx_addr = 0);
	}
	if(ins == 1)
	{
		sim.FFT("data/fft_log1.txt", FFTx_choose = 0, tf_en = 0, x_addr = 12, fft_level = 6, FFTx_addr = 0);
	}
	if(ins == 2)
	{
		sim.FFT("data/fft_log2.txt", FFTx_choose = 0, tf_en = 0, x_addr = 204, fft_level = 6, FFTx_addr = batch_num * bit_num);
	}
	if(ins == 3)
	{
		sim.PE("data/pe_log1.txt",
				FFTx_addr_inst = 0,					//inst结尾的参数是写进data_rom里的
				FFTw_addr = 0,
				FFTx_addr_sim = 0,					//sim结尾的参数是模拟器用的
				FFTx_part = 0,						//这个参数反应进行运算的数是128位中的哪个part(0~3)
				FFTx_addr_numberStep = 12			//这个参数反应实际硬件上的FFTx中会被同时取出的四组数
													//在模拟器的FFTx地址空间中的距离
			);
	}
	if(ins == 4)
	{
		sim.Output_PE(x_addr = 12, FFTx_part = 0, rightshift = 6);
	}
	if(ins == 5)
	{
		sim.PE("data/pe_log2.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 1,
				FFTx_addr_numberStep
			);
	}
	if(ins == 6)
	{
		sim.Output_PE(x_addr = 12, FFTx_part = 1, rightshift);
	}
	if(ins == 7)
	{
		sim.PE("data/pe_log3.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 2,
				FFTx_addr_numberStep
			);
	}
	if(ins == 8)
	{
		sim.Output_PE(x_addr = 12, FFTx_part = 2, rightshift);
	}
	if(ins == 9)
	{
		sim.PE("data/pe_log4.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 3,
				FFTx_addr_numberStep
			);
	}
	if(ins == 10)
	{
		sim.Output_PE(x_addr = 12, FFTx_part = 3, rightshift);
	}
	if(ins == 11)
	{
		sim.PE("data/pe_log5.txt",
				FFTx_addr_inst = 0,					//inst结尾的参数是写进data_rom里的
				FFTw_addr += 8 * 12,
				FFTx_addr_sim = 0,					//sim结尾的参数是模拟器用的
				FFTx_part = 0,						//这个参数反应进行运算的数是128位中的哪个part(0~3)
				FFTx_addr_numberStep				//这个参数反应实际硬件上的FFTx中会被同时取出的四组数
													//在模拟器的FFTx地址空间中的距离
			);
	}
	if(ins == 12)
	{
		sim.Output_PE(x_addr = 204, FFTx_part = 0, rightshift);
	}
	if(ins == 13)
	{
		sim.PE("data/pe_log6.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 1,
				FFTx_addr_numberStep
			);
	}
	if(ins == 14)
	{
		sim.Output_PE(x_addr = 204, FFTx_part = 1, rightshift);
	}
	if(ins == 15)
	{
		sim.PE("data/pe_log7.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 2,
				FFTx_addr_numberStep
			);
	}
	if(ins == 16)
	{
		sim.Output_PE(x_addr = 204, FFTx_part = 2, rightshift);
	}
	if(ins == 17)
	{
		sim.PE("data/pe_log8.txt",
				FFTx_addr_inst = 0,
				FFTw_addr += 8 * 12,
				FFTx_addr_sim,
				FFTx_part = 3,
				FFTx_addr_numberStep
			);
	}
	if(ins == 18)
	{
		sim.Output_PE(x_addr = 204, FFTx_part = 3, rightshift);
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
		sim.FFT("data/ifft_log1.txt", FFTx_choose = 0, tf_en = 0, x_addr = 12, fft_level = 6, FFTx_addr = 0);
	}
	if(ins == 21)
	{
		sim.FFT("data/ifft_log2.txt", FFTx_choose = 0, tf_en = 0, x_addr = 204, fft_level = 6, FFTx_addr = 192);
	}
	if(ins == 22)
	{
		sim.Output_IFFT(x_addr = 12, FFTx_addr_sim = 0, batch_num = 32, rightshift = 0);
	}
	if(ins == 23)
	{
		sim.Bias_Relu(x_addr = 12, FFTw_addr += 8 * 12, bias_size = 512, batch_num = 4);
	}
	
	ins++;	
	if(ins>=24) 
	{
		layer++;
		ins = 0;
	}
	if(layer >= 3)
	{
		batch+=8;
		layer = 0;	
		Result = sim.GetLayer3();
		finished = true;	
	}
	if(batch>=496) 
	{
		batch = 0;
	}
	
	return std::make_pair(timeused,energyused);
}