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




