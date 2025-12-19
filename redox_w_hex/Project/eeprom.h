#include <stdint.h>
#include <string.h>

#include "nrf.h"
#include <stdbool.h>
#include "app_error.h"
#include "nordic_common.h"

void eeprom_write(uint32_t channel_value);
uint32_t eeprom_read(void);

//void m_flash_write(void);
//void m_flash_erase(void);

//256 -40 ,自定义存储其实地址
#define EE_MODE_PAGE 216
