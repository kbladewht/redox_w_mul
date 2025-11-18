
#ifndef BSP_H__
#define BSP_H__

#include <stdint.h>
#include <stdbool.h>
#include "boards.h"

#define BSP_INIT_NONE    0        /**< This define specifies the type of initialization without support for LEDs and buttons (@ref bsp_init).*/
#define BSP_INIT_LED     (1 << 0) /**< This bit enables LEDs during initialization (@ref bsp_init).*/

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t bsp_button_action_t; /**< The different actions that can be performed on a button. */

#define BSP_INDICATIONS_LIST {                \
        "BSP_INDICATE_IDLE",                  \
        "BSP_INDICATE_SCANNING",              \
        "BSP_INDICATE_ADVERTISING",           \
        "BSP_INDICATE_ADVERTISING_WHITELIST", \
        "BSP_INDICATE_ADVERTISING_SLOW",      \
        "BSP_INDICATE_ADVERTISING_DIRECTED",  \
        "BSP_INDICATE_BONDING",               \
        "BSP_INDICATE_CONNECTED",             \
        "BSP_INDICATE_SENT_OK",               \
        "BSP_INDICATE_SEND_ERROR",            \
        "BSP_INDICATE_RCV_OK",                \
        "BSP_INDICATE_RCV_ERROR",             \
        "BSP_INDICATE_FATAL_ERROR",           \
        "BSP_INDICATE_ALERT_0",               \
        "BSP_INDICATE_ALERT_1",               \
        "BSP_INDICATE_ALERT_2",               \
        "BSP_INDICATE_ALERT_3",               \
        "BSP_INDICATE_ALERT_OFF",             \
        "BSP_INDICATE_USER_STATE_OFF",        \
        "BSP_INDICATE_USER_STATE_0",          \
        "BSP_INDICATE_USER_STATE_1",          \
        "BSP_INDICATE_USER_STATE_2",          \
        "BSP_INDICATE_USER_STATE_3",          \
        "BSP_INDICATE_USER_STATE_ON"          \
} /**< See @ref examples_bsp_states for a list of how these states are indicated for the PCA10028/PCA10040 board and the PCA10031 dongle.*/


/**@brief BSP indication states.
 *
 * @details See @ref examples_bsp_states for a list of how these states are indicated for the PCA10028/PCA10040 board and the PCA10031 dongle.
 */
typedef enum
{
    BSP_INDICATE_FIRST = 0,
    BSP_INDICATE_IDLE  = BSP_INDICATE_FIRST, /**< See \ref BSP_INDICATE_IDLE.*/
    BSP_INDICATE_SCANNING,                   /**< See \ref BSP_INDICATE_SCANNING.*/
    BSP_INDICATE_ADVERTISING,                /**< See \ref BSP_INDICATE_ADVERTISING.*/
    BSP_INDICATE_ADVERTISING_WHITELIST,      /**< See \ref BSP_INDICATE_ADVERTISING_WHITELIST.*/
    BSP_INDICATE_ADVERTISING_SLOW,           /**< See \ref BSP_INDICATE_ADVERTISING_SLOW.*/
    BSP_INDICATE_ADVERTISING_DIRECTED,       /**< See \ref BSP_INDICATE_ADVERTISING_DIRECTED.*/
    BSP_INDICATE_BONDING,                    /**< See \ref BSP_INDICATE_BONDING.*/
    BSP_INDICATE_CONNECTED,                  /**< See \ref BSP_INDICATE_CONNECTED.*/
    BSP_INDICATE_SENT_OK,                    /**< See \ref BSP_INDICATE_SENT_OK.*/
    BSP_INDICATE_SEND_ERROR,                 /**< See \ref BSP_INDICATE_SEND_ERROR.*/
    BSP_INDICATE_RCV_OK,                     /**< See \ref BSP_INDICATE_RCV_OK.*/
    BSP_INDICATE_RCV_ERROR,                  /**< See \ref BSP_INDICATE_RCV_ERROR.*/
    BSP_INDICATE_FATAL_ERROR,                /**< See \ref BSP_INDICATE_FATAL_ERROR.*/
    BSP_INDICATE_ALERT_0,                    /**< See \ref BSP_INDICATE_ALERT_0.*/
    BSP_INDICATE_ALERT_OFF,                  /**< See \ref BSP_INDICATE_ALERT_OFF.*/
    BSP_INDICATE_USER_STATE_OFF,             /**< See \ref BSP_INDICATE_USER_STATE_OFF.*/
    BSP_INDICATE_USER_STATE_ON,              /**< See \ref BSP_INDICATE_USER_STATE_ON.*/
    BSP_INDICATE_LAST = BSP_INDICATE_USER_STATE_ON
} bsp_indication_t;

/**@brief BSP events.
 *
 * @note Events from BSP_EVENT_KEY_0 to BSP_EVENT_KEY_LAST are by default assigned to buttons.
 */
typedef enum
{
    BSP_EVENT_NOTHING = 0,                  /**< Assign this event to an action to prevent the action from generating an event (disable the action). */
    BSP_EVENT_DEFAULT,                      /**< Assign this event to an action to assign the default event to the action. */
    BSP_EVENT_CLEAR_BONDING_DATA,           /**< Persistent bonding data should be erased. */
    BSP_EVENT_CLEAR_ALERT,                  /**< An alert should be cleared. */
    BSP_EVENT_DISCONNECT,                   /**< A link should be disconnected. */
    BSP_EVENT_ADVERTISING_START,            /**< The device should start advertising. */
    BSP_EVENT_ADVERTISING_STOP,             /**< The device should stop advertising. */
    BSP_EVENT_WHITELIST_OFF,                /**< The device should remove its advertising whitelist. */
    BSP_EVENT_BOND,                         /**< The device should bond to the currently connected peer. */
    BSP_EVENT_RESET,                        /**< The device should reset. */
    BSP_EVENT_SLEEP,                        /**< The device should enter sleep mode. */
    BSP_EVENT_WAKEUP,                       /**< The device should wake up from sleep mode. */
    BSP_EVENT_SYSOFF,                       /**< The device should enter system off mode (without wakeup). */
    BSP_EVENT_DFU,                          /**< The device should enter DFU mode. */
    BSP_EVENT_KEY_0,                        /**< Default event of the push action of BSP_BUTTON_0 (only if this button is present). */
    BSP_EVENT_KEY_1,                        /**< Default event of the push action of BSP_BUTTON_1 (only if this button is present). */
    BSP_EVENT_KEY_2,                        /**< Default event of the push action of BSP_BUTTON_2 (only if this button is present). */
    BSP_EVENT_KEY_3,                        /**< Default event of the push action of BSP_BUTTON_3 (only if this button is present). */
    BSP_EVENT_KEY_4,                        /**< Default event of the push action of BSP_BUTTON_4 (only if this button is present). */
    BSP_EVENT_KEY_5,                        /**< Default event of the push action of BSP_BUTTON_5 (only if this button is present). */
    BSP_EVENT_KEY_6,                        /**< Default event of the push action of BSP_BUTTON_6 (only if this button is present). */
    BSP_EVENT_KEY_7,                        /**< Default event of the push action of BSP_BUTTON_7 (only if this button is present). */
    BSP_EVENT_KEY_LAST = BSP_EVENT_KEY_7,
} bsp_event_t;



uint32_t bsp_init_qf(uint32_t type, uint32_t ticks_per_100ms);





/**@brief       Function for configuring indicators to required state.
 *
 * @details     This function indicates the required state by means of LEDs (if enabled).
 *
 * @note        Alerts are indicated independently.
 *
 * @param[in]   indicate   State to be indicated.
 *
 * @retval      NRF_SUCCESS               If the state was successfully indicated.
 * @retval      NRF_ERROR_NO_MEM          If the internal timer operations queue was full.
 * @retval      NRF_ERROR_INVALID_STATE   If the application timer module has not been initialized,
 *                                        or internal timer has not been created.
 */
uint32_t bsp_indication_set(bsp_indication_t indicate);


/**@brief       Function for enabling all buttons.
 *
 * @details     After calling this function, all buttons will generate events when pressed, and
 *              all buttons will be able to wake the system up from sleep mode.
 *
 * @retval      NRF_SUCCESS              If the buttons were successfully enabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or BSP_SIMPLE is defined.
 * @return      A propagated error.
 */
uint32_t bsp_buttons_enable(void);


/**@brief       Function for disabling all buttons.
 *
 * @details     After calling this function, no buttons will generate events when pressed, and
 *              no buttons will be able to wake the system up from sleep mode.
 *
 * @retval      NRF_SUCCESS              If the buttons were successfully disabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or BSP_SIMPLE is defined.
 * @return      A propagated error.
 */
uint32_t bsp_buttons_disable(void);


/**@brief       Function for enabling wakeup from SYSTEM OFF for given button.
 *
 * @details     After calling this function, button can be used to wake up the chip.
 *              This function should only be called immediately before going into sleep.
 *
 * @param[in]   button_idx  Index of the button.
 *
 * @retval      NRF_SUCCESS              If the button was successfully enabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or BSP_SIMPLE is defined.
 */
uint32_t bsp_wakeup_button_enable(uint32_t button_idx);


/**@brief       Function for disabling wakeup for the given button.
 *
 * @param[in]   button_idx  index of the button.
 *
 * @retval      NRF_SUCCESS              If the button was successfully disabled.
 * @retval      NRF_ERROR_NOT_SUPPORTED  If the board has no buttons or BSP_SIMPLE is defined.
 */
uint32_t bsp_wakeup_button_disable(uint32_t button_idx);


#ifdef __cplusplus
}
#endif

#endif // BSP_H__

/** @} */
