/**
 * Copyright (c) 2016 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "nrf_dfu.h"
#include "nrf_dfu_transport.h"
#include "nrf_dfu_utils.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_dfu_settings.h"
#include "nrf_gpio.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "nrf_log.h"
#include "boards.h"
#include "nrf_bootloader_info.h"
#include "nrf_dfu_req_handler.h"
#include "pca10028.h"

#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE, 0)                        /**< Maximum size of scheduler events. */

#define SCHED_QUEUE_SIZE                20                                                      /**< Maximum number of events in the scheduler queue. */

#define APP_TIMER_PRESCALER             0                                                       /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                                       /**< Size of timer operation queues. */

// Weak function implementation

/** @brief Weak implemenation of nrf_dfu_check_enter.
 *
 * @note    This function must be overridden to enable entering DFU mode at will.
 *          Default behaviour is to enter DFU when BOOTLOADER_BUTTON is pressed.
 */
__WEAK bool nrf_dfu_enter_check(void)
{
    if (nrf_gpio_pin_read(BOOTLOADER_BUTTON) == 0)
    {
        return true;
    }

    if (s_dfu_settings.enter_buttonless_dfu == 1)
    {
        s_dfu_settings.enter_buttonless_dfu = 0;
        APP_ERROR_CHECK(nrf_dfu_settings_write(NULL));
        return true;
    }
    return false;
}


// Internal Functions

/**@brief Function for initializing the timer handler module (app_timer).
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler.
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
}


/** @brief Function for event scheduler initialization.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}
static inline uint32_t get_system_time_ticks(void)
{
    /* The frequency configuration of RTc1 is 32768Hz, so we divide it by 32 */
    uint32_t systime = (app_timer_cnt_get() / 32);
    return systime;
}
uint32_t timer_read32(void)
{
    uint32_t ret = get_system_time_ticks();
    return ret;
}
#define TIMER_DIFF(a, b, max) ((max == UINT8_MAX) ? ((uint8_t)((a) - (b))) : ((max == UINT16_MAX) ? ((uint16_t)((a) - (b))) : ((max == UINT32_MAX) ? ((uint32_t)((a) - (b))) : ((a) >= (b) ? (a) - (b) : (max) + 1 - (b) + (a)))))

#define TIMER_DIFF_16(a, b) TIMER_DIFF(a, b, UINT16_MAX)
#define TIMER_DIFF_32(a, b) TIMER_DIFF(a, b, UINT32_MAX)
uint32_t timer_elapsed32(uint32_t last)
{
    return TIMER_DIFF_32(timer_read32(), last);
}

#include "app_timer.h"
//static void wait_for_event()
//{
//    // Transport is waiting for event?
//    while(true)
//    {
//        // Can't be emptied like this because of lack of static variables
//        app_sched_execute();
//    }
//}

bool dfu_is_in_progress(void)
{
    // 1. Init packet 已经开始传输
    if (s_dfu_settings.progress.command_size > 0)
    {
        return true;
    }

    // 2. 固件数据包正在写入
    if (s_dfu_settings.progress.firmware_image_offset > 0 ||
        s_dfu_settings.write_offset > 0)
    {
        return true;
    }

    // 否则说明还没进入升级，或者升级已经完成
    return false;
}
uint32_t aa = 1;
#include "nrf_delay.h"
static void wait_for_event()
{
    uint32_t start_ms = timer_read32();
    
    while(true)
    {
//			aa++;
//			nrf_delay_us(1000);
        app_sched_execute();
//        if (aa > 1000*30)
//        {
//            if (nrf_dfu_app_is_valid())
//						{
////								NRF_LOG_INFO("Jumping to: 0x%08x\r\n", MAIN_APPLICATION_START_ADDR);
////								nrf_bootloader_app_start(MAIN_APPLICATION_START_ADDR);
//									//if(dfu_is_in_progress()==false){
//										NRF_POWER->GPREGRET = 0x24;
//										NVIC_SystemReset();
//									//}
//							    //break;
//						}
//						start_ms = timer_read32();
//        }
    }
}

static void blink_timeout_handler(void *p_context) {
			if (nrf_dfu_app_is_valid())
			{
							NRF_POWER->GPREGRET = 0x24;
							NVIC_SystemReset();
			}
}

void nrf_dfu_wait()
{
    app_sched_execute();
}
APP_TIMER_DEF(led_blink_timer_id);

uint32_t nrf_dfu_init()
{
    uint32_t ret_val = NRF_SUCCESS;
    uint32_t enter_bootloader_mode = 0;

    NRF_LOG_INFO("In real nrf_dfu_init\r\n");

    nrf_dfu_settings_init();

    // Continue ongoing DFU operations
    // Note that this part does not rely on SoftDevice interaction
    ret_val = nrf_dfu_continue(&enter_bootloader_mode);
    if(ret_val != NRF_SUCCESS)
    {
        NRF_LOG_INFO("Could not continue DFU operation: 0x%08x\r\n");
        enter_bootloader_mode = 1;
    }

    // Check if there is a reason to enter DFU mode
    // besides the effect of the continuation
    if (nrf_dfu_enter_check())
    {
        NRF_LOG_INFO("Application sent bootloader request\n");
        enter_bootloader_mode = 1;
    }

    if(enter_bootloader_mode != 0 || !nrf_dfu_app_is_valid())
    {
        timers_init();
        scheduler_init();

		//ret_code_t err_code;
    //config as output
    //nrf_gpio_cfg_output(STATE_LED);
    //init blink timer
			/**
			ram 0x1480 start 0x20002C00
			flash 0x3AC00 size 5000
			*/
   ret_code_t err_code = app_timer_create(&led_blink_timer_id, APP_TIMER_MODE_REPEATED, blink_timeout_handler);
			app_timer_start(led_blink_timer_id, APP_TIMER_TICKS(1000*30,0), NULL);
//			
        // Initializing transports
        ret_val = nrf_dfu_transports_init();
        if (ret_val != NRF_SUCCESS)
        {
            NRF_LOG_INFO("Could not initalize DFU transport: 0x%08x\r\n");
            return ret_val;
        }

        (void)nrf_dfu_req_handler_init();

        // This function will never return
        NRF_LOG_INFO("Waiting for events\r\n");
        wait_for_event();
        NRF_LOG_INFO("After waiting for events\r\n");
    }

    if (nrf_dfu_app_is_valid())
    {
        NRF_LOG_INFO("Jumping to: 0x%08x\r\n", MAIN_APPLICATION_START_ADDR);
        nrf_bootloader_app_start(MAIN_APPLICATION_START_ADDR);
    }

    // Should not be reached!
    NRF_LOG_INFO("After real nrf_dfu_init\r\n");
    return NRF_SUCCESS;
}
