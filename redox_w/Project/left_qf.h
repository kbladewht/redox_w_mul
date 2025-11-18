


#define CHANNEL_QF  {4, 42, 77}


#define PIPE_NUMBER 0


#define C01 0
#define C02 1
#define C03 2
#define C04 3
#define C05 4
#define C06 5
#define C07 6

#define R01 21
#define R02 22
#define R03 23
#define R04 24


#define COL_QF { C01, C02, C03, C04, C05, C06, C07 }
#define ROW_QF {R01, R02, R03, R04}

#define COLUMNS 7
#define ROWS 4
#define USE_APP_CONFIG
#define NRF_LOG_ENABLED 1

#define USR_LED 9
#define QF_LOG

//对应左上角

#define MASK_COL_OTA	(1 << (0))		// 第 0 列掩码
#define MASK_COL_CHANNEL0	(1 << (1))	
#define MASK_COL_CHANNEL1	(1 << (2))	
