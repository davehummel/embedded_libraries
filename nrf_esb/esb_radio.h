/*
 * Driver for direct control of Nordic ESB 
 * Wraps functionality and initialization of nrf_esb
 * library
 *
 *
 *
 *
 * 2018 David Hummel
 *
 */

#ifndef Esb_Radio_h
#define Esb_Radio_h

#include "nrf_esb.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#define NRF_ESB_MAX_PAYLOAD_LENGTH 252

#include "nrf.h"
#include "nrf_error.h"
#include "nrf_esb_error_codes.h"
#include "nrf_log.h"

static void RADIO_HANDLER(nrf_esb_evt_t const *p_event);

class EsbRadio {
public:
  EsbRadio();

  void setBitrate(nrf_esb_bitrate_t p_bitrate);

  void setRetransmitDelay(uint16_t p_delay);

  void setRetransmitCount(uint16_t p_retry);

  void setOutputPower(nrf_esb_tx_power_t p_output_power);

  void setCRCMode(nrf_esb_crc_t p_crc_mode);

  uint32_t setPrimaryAddr(uint8_t const *p_addr);

  uint32_t setSecondaryAddr(uint8_t const *p_addr);

  uint32_t setAllAddrPrefix(uint8_t count, uint8_t const *p_addr);

  uint32_t init(nrf_esb_mode_t p_mode);

  uint32_t switchModes(nrf_esb_mode_t p_mode);

  uint32_t transmit(uint8_t p_pipenum, uint8_t p_length, uint8_t *p_data, bool p_noack = false);

  void setReceivedListener(void (*p_listener)(nrf_esb_evt_t const *p_event));

  void setTXCompleteListener(void (*p_listener)(nrf_esb_evt_t const *p_event));

  void handleEvent(nrf_esb_evt_t const *p_event);

private:
  nrf_esb_config_t esb_config = NRF_ESB_DEFAULT_CONFIG;
  nrf_esb_payload_t tx_payload;
  void (*rxListener)(nrf_esb_evt_t const *p_event) = 0;
  void (*txListener)(nrf_esb_evt_t const *p_event) = 0;
};

extern EsbRadio Radio;

static void RADIO_HANDLER(nrf_esb_evt_t const *p_event) {
  Radio.handleEvent(p_event);
}

#endif
