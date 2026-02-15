/* Copyright 2023 Cheng Liren
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// #include "timer.h"
#include "app_timer.h"
// static uint32_t ticks_offset = 0;
// static uint32_t last_ticks = 0;
// static uint32_t ms_offset = 0;

#define TIMER_DIFF(a, b, max) ((max == UINT8_MAX) ? ((uint8_t)((a) - (b))) : ((max == UINT16_MAX) ? ((uint16_t)((a) - (b))) : ((max == UINT32_MAX) ? ((uint32_t)((a) - (b))) : ((a) >= (b) ? (a) - (b) : (max) + 1 - (b) + (a)))))

#define TIMER_DIFF_16(a, b) TIMER_DIFF(a, b, UINT16_MAX)
#define TIMER_DIFF_32(a, b) TIMER_DIFF(a, b, UINT32_MAX)
static __INLINE uint32_t rtc1_counter_get(void)
{
    return NRF_RTC1->COUNTER;
}

uint32_t app_timer_cnt_get(void)
{
    return rtc1_counter_get();
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

uint16_t timer_read(void)
{
    return (uint16_t)timer_read32();
}



uint16_t timer_elapsed(uint16_t last)
{
    return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last)
{
    return TIMER_DIFF_32(timer_read32(), last);
}
