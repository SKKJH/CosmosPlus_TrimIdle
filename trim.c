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
		if (((*(ptr)<0)||(*(ptr)>0xfff0))||((*(ptr+1)<0)||(*(ptr+1)>0xfff0))||((*(ptr+2)< 0)||(*(ptr+2)>0xfff0))||((*(ptr+3)<0)||(*(ptr+3)>0xfff0)))
		{
            dmRangePtr->dmRange[i].ContextAttributes.value = 0;
            dmRangePtr->dmRange[i].lengthInLogicalBlocks = 1;
            dmRangePtr->dmRange[i].startingLBA[0] = 0;
            dmRangePtr->dmRange[i].startingLBA[1] = 0;
        }
        else {
            dmRangePtr->dmRange[i].ContextAttributes.value = *(ptr);
            dmRangePtr->dmRange[i].lengthInLogicalBlocks = *(ptr+1);
            dmRangePtr->dmRange[i].startingLBA[0] = *(ptr + 2);
            dmRangePtr->dmRange[i].startingLBA[1] = *(ptr + 3);
        }
        //xil_printf(" %dth TRIM CMD : sLBA %d and %d blks\n",i, dmRangePtr->dmRange[i].startingLBA[0] ,dmRangePtr->dmRange[i].lengthInLogicalBlocks );
        tempAddr += sizeof(unsigned int)*4;
    }
    //XTime_GetTime(&tEtime);
    //dma_time += (tEtime - tStime)*3;

	trim_cnt += nr;
	if (t_time == 0xffffffff) {
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
			int lsa = (p_lba_save / 4);
			TRIM(lsa, 1, 1, 1, 1, i);
			check_cmdValid = check_nvme_cmd_come();
			if (check_cmdValid == 1) {
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
	dw_time = 0;
	trim_cnt = 0;
	t_time = 4294967290;
	p_trim_cnt = 0;
	p_lba_save = 0;
	
	//Initialize delayWritebuffer
	for (unsigned int i = 0; i < AVAILABLE_DATA_BUFFER_ENTRY_COUNT; i++)
	{
		delayBufHashTablePtr->delayBufHash[i].headEntry = DATA_BUF_NONE;
		delayBufHashTablePtr->delayBufHash[i].tailEntry = DATA_BUF_NONE;
	}

	//xil_printf("trim done\r\n");
	return 0;
}


void TRIM(unsigned int LPN, unsigned int BLK0, unsigned int BLK1, unsigned int BLK2, unsigned int BLK3, unsigned int trim_time){
	//Considering Delayed Write Buffer
   	if (dw_time >= trim_time){
		unsigned int delayEntry = CheckDelayBufTime(LPN,trim_time);
		if (delayEntry != DATA_BUF_FAIL) {
			if ((BLK0 == 1) && (delayBufMapPtr->delayWriteBuf[delayEntry].blk0 == 0)) {
				BLK0 = 1;
			}
			else {
				BLK0 = 0;
			}
			if ((BLK1 == 1) && (delayBufMapPtr->delayWriteBuf[delayEntry].blk1 == 0)) {
				BLK1 = 1;
			}
			else {
				BLK1 = 0;
			}
			if ((BLK2 == 1) && (delayBufMapPtr->delayWriteBuf[delayEntry].blk1 == 0)) {
				BLK2 = 1;
			}
			else {
				BLK2 = 0;
			}
			if ((BLK3 == 1) && (delayBufMapPtr->delayWriteBuf[delayEntry].blk1 == 0)) {
				BLK3 = 1;
			}
			else {
				BLK3 = 0;
			}
		}
    }

	// write buffer
	unsigned int bufEntry = CheckDataBufHitByLSA(LPN);
	if (bufEntry != DATA_BUF_FAIL){
	    if (BLK0 == 1){
	    	dataBufMapPtr->dataBuf[bufEntry].blk0 = 0;
	    }
		if (BLK1 == 1) {
			dataBufMapPtr->dataBuf[bufEntry].blk1 = 0;
		}
		if (BLK2 == 1) {
			dataBufMapPtr->dataBuf[bufEntry].blk2 = 0;
		}
		if (BLK3 == 1) {
	    	dataBufMapPtr->dataBuf[bufEntry].blk3 = 0;
		}
	    if((dataBufMapPtr->dataBuf[bufEntry].blk0 == 0) && (dataBufMapPtr->dataBuf[bufEntry].blk1 == 0) && (dataBufMapPtr->dataBuf[bufEntry].blk2 == 0) && (dataBufMapPtr->dataBuf[bufEntry].blk3 == 0))
		{
			unsigned int prevBufEntry, nextBufEntry;
			prevBufEntry = dataBufMapPtr->dataBuf[bufEntry].prevEntry;
			nextBufEntry = dataBufMapPtr->dataBuf[bufEntry].nextEntry;

			if (prevBufEntry != DATA_BUF_NONE && nextBufEntry != DATA_BUF_NONE)
          	{
				dataBufMapPtr->dataBuf[prevBufEntry].nextEntry = nextBufEntry;
				dataBufMapPtr->dataBuf[nextBufEntry].prevEntry = prevBufEntry;
				nextBufEntry = DATA_BUF_NONE;
				prevBufEntry = dataBufLruList.tailEntry;
				dataBufMapPtr->dataBuf[dataBufLruList.tailEntry].nextEntry = bufEntry;
				dataBufLruList.tailEntry = bufEntry;
			}
			else if (prevBufEntry != DATA_BUF_NONE && nextBufEntry == DATA_BUF_NONE)
			{
				dataBufLruList.tailEntry = bufEntry;
			}
			else if(prevBufEntry == DATA_BUF_NONE && nextBufEntry != DATA_BUF_NONE)
			{
				dataBufMapPtr->dataBuf[nextBufEntry].prevEntry = DATA_BUF_NONE;
				dataBufLruList.headEntry = nextBufEntry;
				prevBufEntry = dataBufLruList.tailEntry;
				dataBufMapPtr->dataBuf[dataBufLruList.tailEntry].nextEntry = bufEntry;
				dataBufLruList.tailEntry = bufEntry;
			}
			else
			{
				prevBufEntry = DATA_BUF_NONE;
				nextBufEntry = DATA_BUF_NONE;
				dataBufLruList.headEntry = bufEntry;
				dataBufLruList.tailEntry = bufEntry;
			}
			SelectiveGetFromDataBufHashList(bufEntry);
			dataBufMapPtr->dataBuf[bufEntry].blockingReqTail = REQ_SLOT_TAG_NONE;
			dataBufMapPtr->dataBuf[bufEntry].dirty = DATA_BUF_CLEAN;
			dataBufMapPtr->dataBuf[bufEntry].reserved0 = 0;
	    }
	}

	// L2P
	unsigned int virtualSliceAddr = logicalSliceMapPtr->logicalSlice[LPN].virtualSliceAddr;
	if(virtualSliceAddr != VSA_NONE) {
		if (BLK0 == 1) {
		    logicalSliceMapPtr->logicalSlice[LPN].blk0 = 0;
		}
		if (BLK1 == 1) {
		    logicalSliceMapPtr->logicalSlice[LPN].blk1 = 0;
		}
		if (BLK2 == 1) {
		    logicalSliceMapPtr->logicalSlice[LPN].blk2 = 0;
		}
		if (BLK3 == 1) {
		    logicalSliceMapPtr->logicalSlice[LPN].blk3 = 0;
		}
		if((logicalSliceMapPtr->logicalSlice[LPN].blk0 == 0)&&(logicalSliceMapPtr->logicalSlice[LPN].blk1 == 0)&&(logicalSliceMapPtr->logicalSlice[LPN].blk2 == 0)&&(logicalSliceMapPtr->logicalSlice[LPN].blk3 == 0)) {
		    InvalidateOldVsa(LPN);
		}
	}
}


