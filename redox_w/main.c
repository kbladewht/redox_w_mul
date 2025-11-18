#include "wireless.h"
#include "eeprom.h"


// Mark as inactive after a number of ticks:
#define INACTIVITY_THRESHOLD 500 // 0.5sec

static uint8_t channel_table[3] = CHANNEL_QF;

const nrf_drv_rtc_t rtc1 = NRF_DRV_RTC_INSTANCE(1);

uint16_t hand_count = 0;
int rows[] = ROW_QF;
int columns[] = COL_QF;

static void lfclk_config(void) {
  ret_code_t err_code =
  nrf_drv_clock_init();
  APP_ERROR_CHECK(err_code);

  nrf_drv_clock_lfclk_request(NULL);
}

static const uint32_t COL_PINS[] = COL_QF;
// Return the key states
static void read_keys(uint8_t *row_stat) {
  unsigned short c;
  uint32_t input = 0;
  // static const uint32_t COL_PINS[] = { C01, C02, C03, C04, C05, C06, C07 };


  // scan matrix by columns
  for (c = 0; c < COLUMNS; ++c) {

    nrf_gpio_pin_set(COL_PINS[c]);
   asm volatile("nop");
    input = NRF_GPIO->IN;
    for (int i = 0; i < ROWS; i++) {
      row_stat[i] = (row_stat[i] << 1) | ((input >> (rows[i])) & 1);
    }
    nrf_gpio_pin_clear(COL_PINS[c]);
  }
}

 bool compare_keys(const uint8_t *first, const uint8_t *second, uint32_t size) {
  for (int i = 0; i < size; i++) {
    if (first[i] != second[i]) {
      return false;
    }
  }

  return true;
}

static bool empty_keys(const uint8_t *keys_buffer) {
  for (int i = 0; i < ROWS; i++) {
    if (keys_buffer[i]) {
      return false;
    }
  }
  return true;
}


static void handle_inactivity(const uint8_t *keys_buffer) {
  static uint32_t inactivity_ticks = 0;

  // looking for 500 ticks of no keys pressed, to go back to deep sleep
  if (empty_keys(keys_buffer)) {
    inactivity_ticks++;
    if (inactivity_ticks > INACTIVITY_THRESHOLD) {
      nrf_drv_rtc_disable(&rtc1);
      nrf_gpio_pin_clear(USR_LED);

      for (int i = 0; i < COLUMNS; i++) {
        nrf_gpio_pin_set(columns[i]);
      }

      inactivity_ticks = 0;
			        // Bare-metal mode (no SoftDevice)
        NRF_POWER->GPREGRET = QF_APP_MAGIC_START;

      NRF_LOG_INFO("SYSTEMOFF \n");
      NRF_POWER->SYSTEMOFF = 1;
    }
  } else {
    inactivity_ticks = 0;
  }
}
extern  void handle_send(const uint8_t *keys_buffer);
void check_predef_action(const uint8_t *keys_buffer);
void tick(nrf_drv_rtc_int_type_t int_type) {
  
  uint8_t keys_buffer[ROWS];
  memset(keys_buffer, 0, ROWS);
	
  read_keys(keys_buffer);
	//check it for ota/switch channel purpose
	check_predef_action(keys_buffer);
  handle_inactivity(keys_buffer);
  handle_send(keys_buffer);
}
// Setup switch pins with pullups
static void gpio_config(void) {

  for (int i = 0; i < ROWS; i++) {
    nrf_gpio_cfg_sense_input(rows[i], NRF_GPIO_PIN_PULLDOWN,NRF_GPIO_PIN_SENSE_HIGH);
  }

  for (int i = 0; i < COLUMNS; i++) {
    nrf_gpio_cfg_output(columns[i]);
    nrf_gpio_pin_clear(columns[i]);
  }

#if USR_LED
  nrf_gpio_cfg_output(USR_LED);
  nrf_gpio_pin_set(USR_LED);
#endif
}
static void rtc_config(void) {

  // Initialize RTC instance
  nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
  config.prescaler = RTC_FREQ_TO_PRESCALER(1000);

  nrf_drv_rtc_init(&rtc1, &config, tick);
  // APP_ERROR_CHECK(err_code);

  // Enable tick event & interrupt
  nrf_drv_rtc_tick_enable(&rtc1, true);

  // Power on RTC instance
  nrf_drv_rtc_enable(&rtc1);


}


uint8_t channelC=0;
int main(void) {
	uint32_t err_code;

	err_code = NRF_LOG_INIT(NULL);
	APP_ERROR_CHECK(err_code);

	channelC = eeprom_read();
	NRF_LOG_INFO("Addr is  *************is %d\r\n", channelC);
	
	  // Initialize Gazell
  nrf_gzll_init(NRF_GZLL_MODE_DEVICE);

  // Attempt sending every packet up to 100 times
  nrf_gzll_set_max_tx_attempts(100);
  nrf_gzll_set_timeslots_per_channel(4);
  nrf_gzll_set_channel_table(channel_table, 3);
  nrf_gzll_set_datarate(NRF_GZLL_DATARATE_1MBIT);
  nrf_gzll_set_timeslot_period(900);

	if(channelC == 0){
			  // Set Addressing
		nrf_gzll_set_base_address_0(BASE_ADDR0_PART1);
		nrf_gzll_set_base_address_1(BASE_ADDR0_PART2);
	}else if(channelC == 1){
		  // Set Addressing
		NRF_LOG_INFO("Taking addr 11**********....\r\n");
		nrf_gzll_set_base_address_0(BASE_ADDR1_PART1);
		nrf_gzll_set_base_address_1(BASE_ADDR1_PART2);
	}else{
		NRF_LOG_INFO("Taking addr 000**********....\r\n");
		nrf_gzll_set_base_address_0(BASE_ADDR0_PART1);
		nrf_gzll_set_base_address_1(BASE_ADDR0_PART2);
	}

// Enable Gazell to start sending over the air
  nrf_gzll_enable();

  // Configure 32kHz xtal oscillator
  lfclk_config();

  // Configure RTC peripherals with ticks
  rtc_config();

  // Configure all keys as inputs with pullups
  gpio_config();

  NRF_LOG_INFO(" xxxx Redox Client Started ***\r\n");
  NRF_LOG_FLUSH();

	uint32_t m_counter =0;
		for (;;) {
	
			m_counter++;
			if(m_counter % 5000 ==0){
				  NRF_LOG_INFO("11 m_counter -> %d\r\n",m_counter/5000);
					NRF_LOG_FLUSH();
			}
			
			__WFE();
			__SEV();
			__WFE();
		}
}

//void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
//    NVIC_SystemReset();
//}

