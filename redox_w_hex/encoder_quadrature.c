// Copyright 2018 Jack Humbert <jack.humb@gmail.com>
// Copyright 2018-2023 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "encoder.h"
//#include "gpio.h"
//#include "keyboard.h"
//#include "action.h"
//#include "keycodes.h"
//#include "wait.h"

#ifdef SPLIT_KEYBOARD
#    include "split_util.h"
#endif

// for memcpy
#include <string.h>

#if !defined(ENCODER_RESOLUTIONS) && !defined(ENCODER_RESOLUTION)
#    define ENCODER_RESOLUTION 4
#endif

#undef ENCODER_DEFAULT_PIN_API_IMPL
#if defined(ENCODER_A_PINS) && defined(ENCODER_B_PINS)
// Inform the quadrature driver that it needs to implement pin init/read functions
#    define ENCODER_DEFAULT_PIN_API_IMPL
#endif

extern volatile bool isLeftHand;

__attribute__((weak)) void    encoder_quadrature_init_pin(uint8_t index, bool pad_b);
__attribute__((weak)) uint8_t encoder_quadrature_read_pin(uint8_t index, bool pad_b);

#ifdef ENCODER_DEFAULT_PIN_API_IMPL

static pin_t encoders_pad_a[NUM_ENCODERS_MAX_PER_SIDE] = ENCODER_A_PINS;
static pin_t encoders_pad_b[NUM_ENCODERS_MAX_PER_SIDE] = ENCODER_B_PINS;

void encoder_wait_pullup_charge(void) {
    nrf_delay_us(100);
}

void encoder_quadrature_init_pin(uint8_t index, bool pad_b) {
    pin_t pin = pad_b ? encoders_pad_b[index] : encoders_pad_a[index];
    if (pin != NO_PIN) {
        nrf_gpio_cfg_input(pin, NRF_GPIO_PIN_PULLUP);
    }
}

static uint32_t call_count = 0;
__attribute__((weak)) uint8_t encoder_quadrature_read_pin(uint8_t index, bool pad_b) {
    
    call_count++;
    
    pin_t pin = pad_b ? encoders_pad_b[index] : encoders_pad_a[index];
    
    if (pin != NO_PIN) {
        uint8_t value = nrf_gpio_pin_read(pin) ? 1 : 0;
			
//        // 每1000次调用打印一次
//        if (call_count % 5000 == 0) {
//					
//            NRF_LOG_INFO("[Encoder Pin] 22222  Pin:%d, Value:%d pad_pin %d is_padb %d\n",  pin, value,pin,pad_b);
//            NRF_LOG_INFO("33333 Pin:%d, Value:%d pad_pin %d is_padb %d\n",  pin, value,encoders_pad_b[0],pad_b);
//					
//        }
        
        return value;
    }
    

    
    return 0;
}

#endif // ENCODER_DEFAULT_PIN_API_IMPL

#ifdef ENCODER_RESOLUTIONS
static uint8_t encoder_resolutions[NUM_ENCODERS] = ENCODER_RESOLUTIONS;
#endif

#ifndef ENCODER_DIRECTION_FLIP
#    define ENCODER_CLOCKWISE true
#    define ENCODER_COUNTER_CLOCKWISE false
#else
#    define ENCODER_CLOCKWISE false
#    define ENCODER_COUNTER_CLOCKWISE true
#endif
static int8_t encoder_LUT[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

static uint8_t encoder_state[NUM_ENCODERS]  = {0};
static int8_t  encoder_pulses[NUM_ENCODERS] = {0};

// encoder counts
static uint8_t thisCount;


__attribute__((weak)) void encoder_quadrature_post_init_kb(void) {
    extern void encoder_quadrature_handle_read(uint8_t index, uint8_t pin_a_state, uint8_t pin_b_state);
    // Unused normally, but can be used for things like setting up pin-change interrupts in keyboard code.
    // During the interrupt, read the pins then call `encoder_handle_read()` with the pin states and it'll queue up an encoder event if needed.
}

void encoder_quadrature_post_init(void) {


    for (uint8_t i = 0; i < thisCount; i++) {
        encoder_quadrature_init_pin(i, false);
        encoder_quadrature_init_pin(i, true);
    }
    encoder_wait_pullup_charge();
    for (uint8_t i = 0; i < thisCount; i++) {
        encoder_state[i] = (encoder_quadrature_read_pin(i, false) << 0) | (encoder_quadrature_read_pin(i, true) << 1);
    }


    encoder_quadrature_post_init_kb();
}

void encoder_driver_init(void) {

    thisCount                = NUM_ENCODERS;

#ifdef ENCODER_TESTS
    // Annoying that we have to clear out values during initialisation here, but
    // because all the arrays are static locals, rerunning tests in the same
    // executable doesn't reset any of these. Kinda crappy having test-only code
    // here, but it's the simplest solution.
    memset(encoder_state, 0, sizeof(encoder_state));
    memset(encoder_pulses, 0, sizeof(encoder_pulses));
    const pin_t encoders_pad_a_left[] = ENCODER_A_PINS;
    const pin_t encoders_pad_b_left[] = ENCODER_B_PINS;
    for (uint8_t i = 0; i < thisCount; i++) {
        encoders_pad_a[i] = encoders_pad_a_left[i];
        encoders_pad_b[i] = encoders_pad_b_left[i];
    }
#endif

#if defined(SPLIT_KEYBOARD) && defined(ENCODER_A_PINS_RIGHT) && defined(ENCODER_B_PINS_RIGHT)
    // Re-initialise the pads if it's the right-hand side
    if (!isLeftHand) {
        const pin_t encoders_pad_a_right[] = ENCODER_A_PINS_RIGHT;
        const pin_t encoders_pad_b_right[] = ENCODER_B_PINS_RIGHT;
        for (uint8_t i = 0; i < thisCount; i++) {
            encoders_pad_a[i] = encoders_pad_a_right[i];
            encoders_pad_b[i] = encoders_pad_b_right[i];
        }
    }
#endif // defined(SPLIT_KEYBOARD) && defined(ENCODER_A_PINS_RIGHT) && defined(ENCODER_B_PINS_RIGHT)

    // Encoder resolutions is defined differently in config.h, so concatenate
#if defined(SPLIT_KEYBOARD) && defined(ENCODER_RESOLUTIONS)
#    if defined(ENCODER_RESOLUTIONS_RIGHT)
    static const uint8_t encoder_resolutions_right[NUM_ENCODERS_RIGHT] = ENCODER_RESOLUTIONS_RIGHT;
#    else  // defined(ENCODER_RESOLUTIONS_RIGHT)
    static const uint8_t encoder_resolutions_right[NUM_ENCODERS_RIGHT] = ENCODER_RESOLUTIONS;
#    endif // defined(ENCODER_RESOLUTIONS_RIGHT)
    for (uint8_t i = 0; i < NUM_ENCODERS_RIGHT; i++) {
        encoder_resolutions[NUM_ENCODERS_LEFT + i] = encoder_resolutions_right[i];
    }
#endif // defined(SPLIT_KEYBOARD) && defined(ENCODER_RESOLUTIONS)

    encoder_quadrature_post_init();
}

static void encoder_handle_state_change(uint8_t index, uint8_t state) {
	
	
		
    uint8_t i = index;

    const uint8_t resolution = ENCODER_RESOLUTION;

//NRF_LOG_INFO("encoder_handle_state_change %d**....\r\n",state);
//				NRF_LOG_FLUSH();
    encoder_pulses[i] += encoder_LUT[state & 0xF];
		//NRF_LOG_INFO("encoder_pulses[i] qqqq %d**********....\r\n",encoder_pulses[i]);
				//NRF_LOG_FLUSH();

    if (encoder_pulses[i] >= resolution) {
//				NRF_LOG_INFO("encoder_handle_state_change qqqq %d**********....\r\n");
//				NRF_LOG_FLUSH();
				encoder_queue_event(index, ENCODER_COUNTER_CLOCKWISE);
		}


    if (encoder_pulses[i] <= -resolution) { // direction is arbitrary here, but this clockwise

				encoder_queue_event(index, ENCODER_CLOCKWISE);
		}
		encoder_pulses[i] %= resolution;

}

void encoder_quadrature_handle_read(uint8_t index, uint8_t pin_a_state, uint8_t pin_b_state) {
    uint8_t state = pin_a_state | (pin_b_state << 1);
    if ((encoder_state[index] & 0x3) != state) {
        encoder_state[index] <<= 2;
        encoder_state[index] |= state;
//				NRF_LOG_INFO("encoder_handle_state_change pin_a %d pin_b %d \n",pin_a_state,pin_b_state);
//			  NRF_LOG_FLUSH();
        encoder_handle_state_change(index, encoder_state[index]);
    }
}

void encoder_driver_task(void) {
    //for (uint8_t i = 0; i < thisCount; i++) {
        encoder_quadrature_handle_read(0, encoder_quadrature_read_pin(0, false), encoder_quadrature_read_pin(0, true));
    //}
}


