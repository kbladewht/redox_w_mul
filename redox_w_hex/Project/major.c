#include "wireless.h"
#include "eeprom.h"

void print_bits_raw(uint8_t value) {
    for (int i = 7; i >= 0; i--) {
        NRF_LOG_RAW_INFO("%d", (value >> i) & 1);
    }
    NRF_LOG_RAW_INFO("\n");
    NRF_LOG_FLUSH();
}

uint8_t pre_flag = 0;

extern bool compare_keys(const uint8_t *first, const uint8_t *second, uint32_t size);

void handle_send(const uint8_t *keys_buffer) {
    static uint8_t keys_snapshot[ROWS] = { 0 };
    static uint32_t debounce_ticks = 0;

    const bool no_change = compare_keys(keys_buffer, keys_snapshot, ROWS);
    if (no_change) {
        debounce_ticks++;
        // debouncing - send only if the keys state has been stable
        // for DEBOUNCE ticks
        if (debounce_ticks == DEBOUNCE) {
            if (keys_snapshot[0] != pre_flag) {
                pre_flag = keys_snapshot[0];
                print_bits_raw(keys_snapshot[0]);
            }
            nrf_gzll_add_packet_to_tx_fifo(PIPE_NUMBER, keys_snapshot, ROWS);
            debounce_ticks = 0;
        }
    } else {
        // change detected, start over

        debounce_ticks = 0;
        for (int k = 0; k < ROWS; k++) {
            keys_snapshot[k] = keys_buffer[k];
        }
    }
}

void qf_enter_dfu(void) {
    // Bare-metal mode (no SoftDevice)
    NRF_POWER->GPREGRET = BOOTLOADER_DFU_START;

    NRF_LOG_INFO("qf_enter_dfu runing \n");
    NRF_LOG_FLUSH();
    NVIC_SystemReset();
}

uint32_t rtc_ota_counter;
bool check_ota(const uint8_t *keys_buffer) {
    bool corner_pressed = (keys_buffer[0] & MASK_COL_OTA) != 0;

    if (!corner_pressed) {
        return false;
    }

    //NRF_LOG_INFO("col 6 pressed = %d\n", corner_pressed);
    rtc_ota_counter++;

    if (rtc_ota_counter % 1000 == 0) {
        NRF_LOG_INFO("rtc_ota_counter in processing %d \n", rtc_ota_counter);
        NRF_LOG_FLUSH();
    }

    if (rtc_ota_counter > 10000) {
        NRF_LOG_INFO("triggered OTA button hodling more 5 secs %d \n", rtc_ota_counter);
        NRF_LOG_FLUSH();
        rtc_ota_counter = 0;
        qf_enter_dfu();
    }

    return false;
}

typedef struct {
    uint8_t mask;
    uint32_t counter;
    uint8_t channel;
} addr_checker_t;

addr_checker_t addr0 = { .mask = MASK_COL_CHANNEL0, .counter = 0, .channel = 0 };

addr_checker_t addr1 = { .mask = MASK_COL_CHANNEL1, .counter = 0, .channel = 1 };

uint32_t rtc_addr_counter0;
uint32_t rtc_addr_counter1;
extern uint8_t channelC;

//checker is either 0 or 1 channel struct
bool check_addr(const uint8_t *keys_buffer, addr_checker_t *checker) {
    // Hold Edge 10 secs for trigger setup button

    if (channelC == checker->channel) {
        return false;
    }

    bool corner_pressed = (keys_buffer[0] & checker->mask) != 0;

    if (!corner_pressed) {
        return false;
    }
    checker->counter++;

    if (checker->counter % 1000 == 0) {
        NRF_LOG_INFO(" rtc_addr_counter in processing %d target channel %d\n", checker->counter / 1000, checker->channel);
        NRF_LOG_FLUSH();
    }

    if (checker->counter > 10 * 1000) {
        NRF_LOG_INFO(" Rtc_addr_counter running > 10 secs %d for target channel %d\n", checker->counter / 1000, checker->channel);
        NRF_LOG_FLUSH();
        checker->counter = 0;
        eeprom_write(checker->channel);

        NVIC_SystemReset();
    }

    return false;
}

void check_predef_action(const uint8_t *keys_buffer) {
    check_addr(keys_buffer, &addr0);
    check_addr(keys_buffer, &addr1);
    check_ota(keys_buffer);
}
