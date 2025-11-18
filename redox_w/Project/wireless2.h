

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "app_error.h"
#include "app_config.h"
#include "app_util_platform.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_rtc.h"
#include "nrf_gpio.h"
#include "nrf_gzll.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// Debounce time (dependent on tick frequency)
#define DEBOUNCE 5

#if defined(COMPILE_LEFT) && defined(COMPILE_RIGHT)
#error "Only one of COMPILE_LEFT and COMPILE_RIGHT can be defined at once."
#endif

//#ifdef QF_CHANNEL0
#define BASE_ADDR0_PART1	0x01020304
#define BASE_ADDR0_PART2	0x05060708

#define BASE_ADDR1_PART1	0x02020202
#define BASE_ADDR1_PART2	0x03030303

#define BASE_ADDR2_PART1	0x04040404
#define BASE_ADDR2_PART2	0x05050505


