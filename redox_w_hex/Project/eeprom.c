
#include <stdint.h>
#include <string.h>

#include "nrf.h"
#include <stdbool.h>
#include "app_error.h"
#include "nordic_common.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"
#include "eeprom.h"

#include "nrf_soc.h"

uint32_t pg_size;
uint32_t pg_num;

uint32_t err_code;

void flash_page_erase(uint32_t *page_address) {
    // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }
    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }

    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }
}


void flash_word_write(uint32_t *address, uint32_t value) {
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }

    *address = value;

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }

    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);

    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {
        // Do nothing.
    }
}

void eeprom_write(uint32_t channel_value) {
    uint32_t *addr;

    NRF_LOG_INFO("Flashwrite example, %d \r\n", channel_value);
   
    pg_size = NRF_FICR->CODEPAGESIZE;
    pg_num = EE_MODE_PAGE; // 216K in flash ,256-40  Start address:
    addr = (uint32_t *)(pg_size * pg_num);
    // Erase page:
    NRF_LOG_INFO("pg_num taking, %d \r\n", pg_num);
    flash_page_erase(addr);
    flash_word_write(addr, channel_value);
}

uint32_t eeprom_read(void) {
    uint32_t *addr;

    pg_size = NRF_FICR->CODEPAGESIZE;
    // pg_num = NRF_FICR->CODESIZE - 40; // Use last page in flash,获取的是第254页面
		pg_num = EE_MODE_PAGE; // 216K in flash

    NRF_LOG_INFO("pg_size is ************, %d bytes \r\n", pg_size);
    NRF_LOG_INFO("pg_num is ************, %d k \r\n", pg_num);
    // Start address:
    addr = (uint32_t *)(pg_size * pg_num);

    uint32_t channel_value = 0;
    channel_value = *(uint32_t *)addr;

    NRF_LOG_INFO("Flash Read 51822 is  %d \r", channel_value);
    NRF_LOG_INFO("Flash Read 51822 addr is  %x \r", (pg_size * pg_num));

	//Give default value is invalid for first read
    if ( channel_value != 1 && channel_value != 0) {
        NRF_LOG_INFO("The value is init status %x, put to default channel_value   %d \r", channel_value,0);
        channel_value = 0;
    }

    NRF_LOG_INFO("return value is  from eeprom   %d \r", channel_value);
    return channel_value;
}

