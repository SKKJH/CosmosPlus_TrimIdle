unsigned int get_data_flag, do_trim_flag, trimDevAddr, nr, trim_cnt, dw_cnt, t_time, p_trim_cnt, p_lba_save,dw_time, trimming_flag;
//nr means number of ranges => trim_cnt use it
//trim_cnt means sum of nr => dmRange use it
//dw_cnt means delayed write range cnt
//t_time means order of trim cmd => delayed write buffer use it
///p_trim_cnt means present trim cnt
void GetTrimData();
unsigned int do_trim();
void TRIM (unsigned int LPN, unsigned int BLK0, unsigned int BLK1, unsigned int BLK2, unsigned int BLK3, unsigned int trim_time);
