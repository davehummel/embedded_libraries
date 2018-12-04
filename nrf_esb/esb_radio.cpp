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

#include "esb_radio.h"

EsbRadio::EsbRadio() {
  esb_config.event_handler = RADIO_HANDLER;
}

void EsbRadio::setBitrate(nrf_esb_bitrate_t p_bitrate) { esb_config.bitrate = p_bitrate; }

void EsbRadio::setRetransmitDelay(uint16_t p_delay) { esb_config.retransmit_delay = p_delay; }

void EsbRadio::setRetransmitCount(uint16_t p_retry) { esb_config.retransmit_count = p_retry; }

void EsbRadio::setOutputPower(nrf_esb_tx_power_t p_output_power) { esb_config.tx_output_power = p_output_power; }

void EsbRadio::setCRCMode(nrf_esb_crc_t p_crc_mode) { esb_config.crc = p_crc_mode; }

uint32_t EsbRadio::setPrimaryAddr(uint8_t const *p_addr) { return nrf_esb_set_base_address_0(p_addr); }

uint32_t EsbRadio::setSecondaryAddr(uint8_t const *p_addr) { return nrf_esb_set_base_address_1(p_addr); }

uint32_t EsbRadio::setAllAddrPrefix(uint8_t count, uint8_t const *p_addr) { return nrf_esb_set_prefixes(p_addr, count); }

uint32_t EsbRadio::init(nrf_esb_mode_t p_mode) {
  uint32_t status = NRF_SUCCESS;
  esb_config.mode = p_mode;
  status = nrf_esb_init(&esb_config);
  return status;
}

uint32_t EsbRadio::switchModes(nrf_esb_mode_t p_mode) {
  uint32_t status = NRF_SUCCESS;

  if (p_mode == esb_config.mode) {
    return status;
  }

  if (p_mode == NRF_ESB_MODE_PTX) {
    nrf_esb_stop_rx();
    (void)nrf_esb_flush_tx();
    (void)nrf_esb_start_tx();
  }

  status = nrf_esb_disable();
  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Failed esb disable on mode switch");
    return status;
  }

  NRF_LOG_DEBUG("Reinitializing on mode switch");
  status = init(p_mode);
  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Failed esb init on mode switch");
    return status;
  }
}

uint32_t EsbRadio::enableRX() {
  return nrf_esb_start_rx();
}

uint32_t EsbRadio::disableRX() {
  return nrf_esb_stop_rx();
}

bool EsbRadio::isIdle() {
  return nrf_esb_is_idle();
}

uint32_t EsbRadio::transmit(uint8_t p_pipenum, uint8_t p_length, uint8_t *p_data, bool p_noack) {

  tx_payload.pipe = p_pipenum;
  tx_payload.noack = p_noack;
  tx_payload.length = p_length;

  memcpy(tx_payload.data, p_data, p_length);

  uint32_t err_code = 0;

  err_code = nrf_esb_write_payload(&tx_payload);

  return err_code;
}

void EsbRadio::flushTxQueue() {
  nrf_esb_flush_tx();
}

void EsbRadio::setReceivedListener(void (*p_listener)(nrf_esb_evt_t const *p_event)) { rxListener = p_listener; }

void EsbRadio::setTransmittedListener(void (*p_listener)(nrf_esb_evt_t const *p_event)) { txListener = p_listener; }

void EsbRadio::handleEvent(nrf_esb_evt_t const *p_event) {
  switch (p_event->evt_id) {
  case NRF_ESB_EVENT_TX_SUCCESS: {
    NRF_LOG_DEBUG("TX SUCCESS EVENT");
    if (txListener != 0)
      txListener(p_event);
  } break;
  case NRF_ESB_EVENT_TX_FAILED:
    NRF_LOG_DEBUG("TX FAILED EVENT");
    if (txListener != 0) {
      txListener(p_event);
    } else {
      (void)nrf_esb_flush_tx();
      (void)nrf_esb_start_tx();
    }
    break;
  case NRF_ESB_EVENT_RX_RECEIVED:
    NRF_LOG_DEBUG("RX RECEIVED EVENT");
    if (rxListener != 0)
      rxListener(p_event);
    break;
  }
}