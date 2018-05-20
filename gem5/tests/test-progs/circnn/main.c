#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <stdint.h>

#include "peripheral.h"
#include "output_label.h"
#include "w3.h"
#include "b3.h"

#define BATCH 10

int main()
{
	int count,pic,i,j,index,correct = 0;
	//int acc_x, acc_y, acc_z, tmp, payload_size, i, count = 0;
	//uint8_t collection[1+2+6*ANum];
	//uint8_t * payload;
	volatile uint8_t * data;//, * acc_reg, * rf_reg;
	//uint8_t ImagIn[16]={0};
	//uint8_t ImagOut[16];
	//uint8_t RealOut[16];
	double layer3[8][64];
	uint8_t finalresult[BATCH*8];
	printf("Program Start.\n");

	// peripheral register
	//periRegister(TMP_SENSOR_ID, tmp_reg);
	//periRegister(ACC_SENSOR_ID, acc_reg);
	//periRegister(RF_ID, rf_reg);

	//tmp_reg = (uint8_t*) mmap(PERI_ADDR[TMP_SENSOR_ID], sizeof(uint8_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	//acc_reg = (uint8_t*) mmap(PERI_ADDR[ACC_SENSOR_ID], sizeof(uint8_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	//rf_reg = (uint8_t*) mmap(PERI_ADDR[RF_ID], sizeof(uint8_t), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	data = (uint8_t*) mmap(PERI_ADDR[0], sizeof(double)*64*8+1, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	printf("Peripherals Registered.\n");

	for(count = 0; count < BATCH; count++)
	{

		// initialization 
		periInit(data);         
		printf("Inited.\n");

		// data read 
		periRead(data, &(layer3[0][0]), sizeof(double)*64*8);
		//computation
		for (pic = 0; pic < 8; pic++)
		{
			double result[10];
			for (i = 0; i < 10; i++)
			{
				result[i] = b3[i];
				for (j = 0; j < 64; j++)
				{
					result[i] += w3[i][j] * layer3[pic][j];
				}
			}
			index = 0;
			for(i = 1; i < 10; i++)
			{
				if(result[i] > result[index])
					index = i;
			}

			printf("Got result: %d\n", index);
			finalresult[count*8 + pic] = index;
		}


		printf("Complete the %d-th packet.\n", count);
		
		//DPRINTF(EXECUTION, "Complete the %d-th packet.\n", ++count);

	}
	// peripheral logout
	periLogout(0);
	for(i = 0;i<BATCH*8;i++)
	{
		if(finalresult[i] == target_list[i])
			correct++;
	}
	printf("accu: %f\n", (double)correct / (BATCH*8));

	return 0;
}
