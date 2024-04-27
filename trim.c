#include "xil_printf.h"
#include <assert.h>
#include "nvme/nvme.h"
#include "nvme/host_lld.h"
#include "memory_map.h"
#include "address_translation.h"
#include "ftl_config.h"
#include "data_buffer.h"
#include <time.h>
#include "trim.h"
#include "xparameters.h"
#include "xtime_l.h"

void GetTrimData(){
	
	get_data_flag = 0;
	unsigned int tempAddr = trimDevAddr;

	for (int i = trim_cnt; i < trim_cnt + nr; i++)
	{
	    unsigned int *ptr = (unsigned int*)tempAddr;
		dmRangePtr->dmRange[i].ContextAttributes.value = *(ptr);
		dmRangePtr->dmRange[i].lengthInLogicalBlocks = *(ptr + 1);
		dmRangePtr->dmRange[i].startingLBA[0] = *(ptr + 2);
		dmRangePtr->dmRange[i].startingLBA[1] = *(ptr + 3);
		//xil_printf(" %dth TRIM CMD : sLBA %d and %d blks\n",i, dmRangePtr->dmRange[i].startingLBA[0] ,dmRangePtr->dmRange[i].lengthInLogicalBlocks );
	}
	trim_cnt += nr;
	if (t_time == 13){
		t_time = 0;
	}
	else {
		t_time += 1;
	}
	do_trim_flag = 1;
}


unsigned int do_trim(){
	
	unsigned int check_cmdValid;
	unsigned int tempLsa, nvmeBlockOffset, startLba, nlb, BLK0, BLK1, BLK2, BLK3;
	int templength, tempsLBA;
	BLK0 = 0;
	BLK1 = 0;
	BLK2 = 0;
	BLK3 = 0;
	tempsLBA = 0;
	for (unsigned int i = p_trim_cnt; i < trim_cnt; i++) {

		nlb = dmRangePtr->dmRange[i].lengthInLogicalBlocks;
		startLba = dmRangePtr->dmRange[i].startingLBA[0];
		tempLsa = startLba / NVME_BLOCKS_PER_SLICE;
		//xil_printf("startLBA %d\n",startLba);
		nvmeBlockOffset = (startLba % NVME_BLOCKS_PER_SLICE);

		templength = nlb;
		tempsLBA = startLba;

		//Do Trim part
		if (trimming_flag != 1) {
			if (nlb == 1) {
				if (nvmeBlockOffset == 0) {
					BLK0 = 1;
					BLK1 = 0;
					BLK2 = 0;
					BLK3 = 0;
					//xil_printf("1\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
				}
				else if (nvmeBlockOffset == 1) {
					BLK0 = 0;
					BLK1 = 1;
					BLK2 = 0;
					BLK3 = 0;
					//xil_printf("2\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
				}
				else if (nvmeBlockOffset == 2) {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 1;
					BLK3 = 0;
					//xil_printf("3\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
				}
				else {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 0;
					BLK3 = 1;
					//xil_printf("4\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
				}
				tempsLBA += 1;
			} else if (nlb == 2) {
				if (nvmeBlockOffset == 0) {
					BLK0 = 1;
					BLK1 = 1;
					BLK2 = 0;
					BLK3 = 0;
					//xil_printf("5\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 2;
				}
				else if (nvmeBlockOffset == 1) {
					BLK0 = 0;
					BLK1 = 1;
					BLK2 = 1;
					BLK3 = 0;
					//xil_printf("6\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 2;
				}
				else if (nvmeBlockOffset == 2) {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("7\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 2;
				}
				else {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 0;
					BLK3 = 1;
					//xil_printf("8\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 1;
				}
			} else if (nlb == 3) {
				if (nvmeBlockOffset == 0) {
					BLK0 = 1;
					BLK1 = 1;
					BLK2 = 1;
					BLK3 = 0;
					//xil_printf("9\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 3;
				} else if (nvmeBlockOffset == 1) {
					BLK0 = 0;
					BLK1 = 1;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("10\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 3;
				} else if (nvmeBlockOffset == 2) {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("11\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 2;
				} else {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 0;
					BLK3 = 1;
					//xil_printf("12\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 1;
				}
			} else {
				if (nvmeBlockOffset == 0) {
					BLK0 = 1;
					BLK1 = 1;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("13\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 4;
				} else if (nvmeBlockOffset == 1) {
					BLK0 = 0;
					BLK1 = 1;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("14\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 3;
				} else if (nvmeBlockOffset == 2){
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 1;
					BLK3 = 1;
					//xil_printf("15\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 2;
				} else {
					BLK0 = 0;
					BLK1 = 0;
					BLK2 = 0;
					BLK3 = 1;
					//xil_printf("16\n");
					TRIM(tempLsa, BLK0, BLK1, BLK2, BLK3, i);
					tempsLBA += 1;
				}
			}
			trimming_flag = 0;
			p_lba_save = tempsLBA;
		}

		for (unsigned int j = p_lba_save; (j + 4) <= startLba + templength; j += 4) {
			//xil_printf("p_lba_save %d\n",p_lba_save);
			int lsa = (p_lba_save / 4);
			//xil_printf("17\n");
			TRIM(lsa, 1, 1, 1, 1, i);
			//check new cmd arrived
			sleep(5);
			//xil_printf("sleep end\r\n");
			check_cmdValid = check_nvme_cmd_come();
			if (check_cmdValid == 1) {
				//xil_printf("get new cmd \r\n");
				p_lba_save += 4;
				trimming_flag = 1;
				return 1;
			}
			p_lba_save += 4;
		}

		unsigned int delayNumOfNvmeBlock = (startLba + nlb) % NVME_BLOCKS_PER_SLICE;
		if(tempLsa != (p_lba_save / 4)) {
			if (delayNumOfNvmeBlock == 1){
				BLK0 = 1;
				BLK1 = 0;
				BLK2 = 0;
				BLK3 = 0;
				//xil_printf("18\n");
				TRIM(p_lba_save / 4, BLK0, BLK1, BLK2, BLK3, i);
			} else if (delayNumOfNvmeBlock == 2) {
				BLK0 = 1;
				BLK1 = 1;
				BLK2 = 0;
				BLK3 = 0;
				//xil_printf("19\n");
				TRIM(p_lba_save / 4, BLK0, BLK1, BLK2, BLK3, i);
			} else if (delayNumOfNvmeBlock == 3) {
				BLK0 = 1;
				BLK1 = 1;
				BLK2 = 1;
				BLK3 = 0;
				//xil_printf("20\n");
				TRIM(p_lba_save / 4, BLK0, BLK1, BLK2, BLK3, i);
			}
		}
		p_trim_cnt += 1;
		trimming_flag = 0;
	}

	do_trim_flag = 0;
	trimming_flag = 0;
	dw_cnt = 0;
	trim_cnt = 0;
	t_time = 0;
	p_trim_cnt = 0;
	p_lba_save = 0;
	xil_printf("trim done\r\n");
	return 0;
}


void TRIM(unsigned int LPN, unsigned int BLK0, unsigned int BLK1, unsigned int BLK2, unsigned int BLK3, unsigned int trim_time){
	xil_printf("LPN : %d, Trim Time : %d BIT : %d%d%d%d\n",LPN, trim_time, BLK0, BLK1, BLK2, BLK3);
}
