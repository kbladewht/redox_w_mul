

//#undef NRF_LOG_ENABLED
//#define NRF_LOG_ENABLED 1


#ifndef _BOARD_H_
#define _BOARD_H_

#define DEBUG


#ifdef COMPILE_LEFT
#include "left_qf.h"
#endif

#ifdef COMPILE_RIGHT
#include "right_qf.h"
#endif


#undef FDS_VIRTUAL_PAGES
#define FDS_VIRTUAL_PAGES 12


#define RTC_ENABLED 1
#define RTC1_ENABLED 1
#define APP_TIMER_ENABLED 0
#define FSTORAGE_ENABLED 0
#define CLOCK_CONFIG_LF_SRC 0
#define BOOTLOADER_DFU_START 0xB1
#define QF_APP_MAGIC_START 0x24

// Low frequency clock source to be used by the SoftDevice
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC,            \
                                 .rc_ctiv       = 16,                                \
                                 .rc_temp_ctiv  = 2,                                \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_250_PPM}

#endif	/*_BOARD_H_*/

