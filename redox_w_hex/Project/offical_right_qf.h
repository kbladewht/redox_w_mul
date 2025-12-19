
#define CHANNEL_QF  {25, 63, 33};

#define PIPE_NUMBER 1

#define COL_QF {30, 0,2,3,4,5,6 }
#define ROW_QF {21,22,23,28,29}

#define COLUMNS 7
#define ROWS 5

#define USE_APP_CONFIG
#define NRF_LOG_ENABLED 0

#define USR_LED 9
#define QF_LOG


//对应右上角 // 第 6 列掩码
#define MASK_COL_OTA	(1 << (COLUMNS-1))		
#define MASK_COL_CHANNEL0	(1 << (COLUMNS-2))	
#define MASK_COL_CHANNEL1	(1 << (COLUMNS-3))	

