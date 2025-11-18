
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
#include "redox-w.h"
#include <string.h>


// 定义行号索引，增强可读性


extern  FLASH_CTRL flash_t;

/** @brief Function for erasing a page in flash.
 *
 * @param page_address Address of the first word in the page to be erased.
 */
static void flash_page_erase(uint32_t *page_address) {
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

/** @brief Function for filling a page in flash with a value.
 *
 * @param[in] address Address of the first word in the page to be filled.
 * @param[in] value Value to be written to flash.
 */
static void flash_word_write(uint32_t *address, uint32_t value) {
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


static void pipe_addr_store(uint32_t addr0, uint32_t addr1) {
  if (flash_t.used_size < flash_t.pg_size) {
    // Erase page:
    flash_page_erase(flash_t.page_start_addr);
    // Write to flash
    if (flash_t.patold_0 != addr0) {
      flash_t.patold_0 = addr0;
      flash_word_write(flash_t.pipe0_store_addr, (uint32_t)addr0);
      flash_t.used_size += 4;
    }
    if (flash_t.patold_1 != addr1) {
      flash_t.patold_1 = addr1;
      flash_word_write(flash_t.pipe1_store_addr, (uint32_t)addr1);
      flash_t.used_size += 4;
#ifdef NRF_LOG_ENABLED
      // printf("addr1:%p was write to flash\n\r", (void *)addr1);
#endif
    }
  }
}
#define ROW0 0
#define ROW1 1
/**
 * @brief 检查特定按键组合是否按下，用于切换 Gazell 地址通道。
 *
 * @param keys_buffer 按键矩阵状态缓存，每个字节对应一行，每个 bit
 * 表示该行某列是否被按下
 * @return true 如果检测到匹配的切换组合并完成切换
 * @return false 没有匹配任何切换条件
 */
 bool process_switch(const uint8_t *keys_buffer) {
  // ⚠️ 检查是否按下了 ROW1 + COL0
  // 这是整个地址切换的前置条件（即启动键），没有这个按下，不执行后续切换逻辑
  if (!(keys_buffer[ROW1] & MASK_COL0)) {
    return false;
  }

  // ✅ 检查是否按下了 ROW0 + COL1（与前置键同时按下）
  // 如果是，则切换到地址组 BASE_ADDR0 + BASE_ADDR1
  if (keys_buffer[ROW0] & MASK_COL1) {
    pipe_addr_store(BASE_ADDR0, BASE_ADDR1);
    return true;
  }

  // ✅ 检查是否按下了 ROW0 + COL2（与前置键同时按下）
  // 如果是，则切换到地址组 BASE_ADDR2 + BASE_ADDR3
  if (keys_buffer[ROW0] & MASK_COL2) {
    pipe_addr_store(BASE_ADDR2, BASE_ADDR3);
    return true;
  }

  // ✅ 检查是否按下了 ROW0 + COL3（与前置键同时按下）
  // 如果是，则切换到地址组 BASE_ADDR4 + BASE_ADDR5
  if (keys_buffer[ROW0] & MASK_COL3) {
    pipe_addr_store(BASE_ADDR4, BASE_ADDR5);
    return true;
  }

  // 没有匹配任意切换组合，返回 false
  return false;
}
 
