#include "wireless.h"

// Data and acknowledgement payloads
static uint8_t ack_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH]; ///< Placeholder for
                                                               ///< received ACK payloads

// ACK received callback (Device mode only).
// This callback is made when the Device receives an ACK (acknowledgement)
// packet.
void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info) {
    uint32_t ack_payload_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    if (tx_info.payload_received_in_ack) {
        // Pop packet and write first byte of the payload to the GPIO port.
        nrf_gzll_fetch_packet_from_rx_fifo(pipe, ack_payload, &ack_payload_length);
        //	NRF_LOG_INFO("Redox call backed received...");
    }
}

/*****************************************************************************/
/** Gazell callback function definitions  */
/*****************************************************************************/

// no action is taken when a packet fails to send, this might need to change
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info) {
    // printf("gzll device tx failed. pipe number: %d. error code: %d.\r\n", pipe,
    // nrf_gzll_get_error_code());
}

// Callbacks not needed
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info) {
}
void nrf_gzll_disabled() {
}
