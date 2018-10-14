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

#ifndef Esb_Mesh_h
#define Esb_Mesh_h

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#include "app_timer.h"
#include "nrf.h"
#include "nrf_error.h"
#include "nrf_esb_error_codes.h"
#include "nrf_log.h"

#include "esb_radio.h"

#define RX_SEARCH_RETRY_COUNT 3
#define RX_MAX_DEVICES 16

#define TX_SEARCH_RESPONSE_PACKET_SIZE 8

#define TX_SEARCH_DURATION 10000

#define MESH_ERROR_BASE (0xC100)
#define MESH_ERROR_MISC (MESH_ERROR_BASE + 0x0000)         ///<
#define MESH_ERROR_INVALID_ROOT (MESH_ERROR_BASE + 0x0001) ///< Root address rejected by NRF sdk
#define MESH_ERROR_INVALID_ID (MESH_ERROR_BASE + 0x0002)   ///< Id is invalid (0 is not allowed)

#define SEARCH_RATE_MS 50

static void MESH_TIMER_HANDLER(void *p_context);
static void MESH_RX_HANDLER(nrf_esb_evt_t const *p_event);
static void MESH_TX_HANDLER(nrf_esb_evt_t const *p_event);

APP_TIMER_DEF(timer_id);

typedef enum{
  MESH_PACKET_TYPE_TX_SEARCH,
  MESH_PACKET_TYPE_TX_SEARCH_CONFIRM,
  MESH_PACKET_TYPE_RX_SEARCH_RESPONSE,
  MESH_PACKET_TYPE_RX_SEARCH_CONFIRM,


} esb_mesh_packet_type_t;

class MeshMap {
public:
private:
};

class MeshAddress {
public:
  void setRoot(uint8_t const *p_root);

  uint32_t setSearch(uint8_t p_rxId) {
    uint32_t status = NRF_SUCCESS;
    if (p_rxId == 0) {
      NRF_LOG_ERROR("Mesh rxId invalid, cant be 0");
      return MESH_ERROR_INVALID_ID;
    }

    uint8_t addr[4];
    memcpy(addr, root, 3);
    addr[3] = 0;

    status = Radio.setPrimaryAddr(addr);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
      return MESH_ERROR_INVALID_ROOT;
    }

    uint8_t prefix[1] = {p_rxId};

    status = Radio.setAllAddrPrefix(1, prefix);

    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh set prefix failed on radio prefix update:%" PRIu32, status);
      return MESH_ERROR_MISC;
    }

    return status;
  }

  uint32_t setRx(uint8_t p_rxId) {

    if (p_rxId == 0) {
      NRF_LOG_ERROR("Mesh rxId invalid, cant be 0");
      return MESH_ERROR_INVALID_ID;
    }
    uint32_t status = NRF_SUCCESS;
    uint8_t addr[4];
    memcpy(addr, root, 3);
    addr[3] = 0;

    status = Radio.setPrimaryAddr(addr);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
      return MESH_ERROR_INVALID_ROOT;
    }

    addr[3] = p_rxId;

    status = Radio.setSecondaryAddr(addr);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
      return MESH_ERROR_INVALID_ROOT;
    }

    uint8_t prefix[8] = {p_rxId, 1, 2, 3, 4, 5, 6, 7};

    status = Radio.setAllAddrPrefix(8, prefix);

    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh set prefix failed on radio update:%" PRIu32, status);
      return MESH_ERROR_MISC;
    }
    return status;
  }

  uint32_t setTx(uint8_t p_rxId, uint8_t p_txId, uint8_t p_rxId2 = 0, uint8_t p_txId2 = 0) {
    if (p_rxId == 0) {
      NRF_LOG_ERROR("Mesh rxId invalid, cant be 0");
      return MESH_ERROR_INVALID_ID;
    }

    if (p_txId == 0) {
      NRF_LOG_ERROR("Mesh txId invalid, [1-7]");
      return MESH_ERROR_INVALID_ID;
    }

    uint32_t status = NRF_SUCCESS;

    uint8_t addr[4];
    memcpy(addr, root, 3);
    addr[3] = p_rxId;

    status = Radio.setPrimaryAddr(addr);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
      return MESH_ERROR_INVALID_ROOT;
    }

    if (p_rxId2 != 0) {
      uint8_t addr[4];
      memcpy(addr, root, 3);
      addr[3] = p_rxId2;

      status = Radio.setSecondaryAddr(addr);
      if (status != NRF_SUCCESS) {
        NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
        return MESH_ERROR_INVALID_ROOT;
      }
    }

    uint8_t prefix[8] = {p_txId, p_txId2};

    status = Radio.setAllAddrPrefix((p_rxId2 == 0) ? 1 : 2, prefix);

    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh set prefix failed on radio update:%" PRIu32, status);
      return MESH_ERROR_MISC;
    }
    return status;
  }

private:
  uint8_t root[3] = {0x44, 0x41, 0xA0};
};

class EsbMesh {
public:
  /**@brief Mesh role describing behavior of device on connect . */
  typedef enum {
    ROLE_LOW_POWER,  /**< Low Power TX centric role     */
    ROLE_HIGH_POWER, /**< High Power TX/RX optional role     */
                     // ROLE_CENTRAL,    /**< High Power central RX only role     */
  } esb_mesh_role_t;

  typedef enum {
    STATE_NONE,
    STATE_RX_SEARCH,    /**< Pinging central RX channels in search     */
    STATE_TX_SEARCH,    /**< Listening to central RX channels for search     */
    STATE_TX_ACTIVE,    /**< Joining as a TX device  */
    STATE_RX_ACTIVE,    /**< Joining as a RX device  */
    STATE_RX_BROADCAST, /**< Send a ping for signal strength awareness to all listeners  */
    STATE_TX_SEARCH_RETRY_DELAY,
  } esb_mesh_state_t;

  typedef enum {
    CONNECT_FAILED,
    CONNECT_SUCCESS,    /**< Pinging central RX channels in search     */
    CONNECT_NEW_JOINER, /**< Pinging central RX channels in search     */
    CONENCT_LOST,       /**< Listening to central RX channels for search     */
  } esb_mesh_connection_event_t;

  EsbMesh() {
  }

  uint32_t join(uint32_t p_id, esb_mesh_role_t p_role) {

    deviceId = p_id;
    role = p_role;
    uint32_t status = NRF_SUCCESS;

    Radio.setReceivedListener(MESH_RX_HANDLER);
    Radio.setTXCompleteListener(MESH_TX_HANDLER);

    status = createTimer();

    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh join failed to create timer");
      return status;
    }

    // if timer is already running
    app_timer_stop(timer_id);

    state = STATE_TX_SEARCH;
    txFailedCount = 0;

    Radio.init(NRF_ESB_MODE_PTX);

    // TODO offset search rate with a +-25 random number to avoid perfect sync
    status = app_timer_start(timer_id, APP_TIMER_TICKS(SEARCH_RATE_MS), NULL);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh join failed to start timer");
      return status;
    }
    return status;
  }

  void handleTimerEvent(void *p_context) {
    NRF_LOG_DEBUG("timer called");
    switch (state) {
    case STATE_TX_SEARCH: {
      uint8_t rxSearchId = 1 + (txFailedCount / RX_SEARCH_RETRY_COUNT);
      NRF_LOG_DEBUG("Pinging %i.", rxSearchId);
      if (rxSearchId > RX_MAX_DEVICES) {
        NRF_LOG_DEBUG("Failed to find listener");
        if (role == ROLE_LOW_POWER) {
          state = STATE_TX_SEARCH_RETRY_DELAY;
          txFailedCount = 0;
          break;
        }
        app_timer_stop(timer_id);
        txFailedCount = 0;

        state = STATE_RX_SEARCH;
        meshAddress.setSearch(1);

        Radio.switchModes(NRF_ESB_MODE_PRX);

        uint8_t txdata[TX_SEARCH_RESPONSE_PACKET_SIZE];
        buildTxSearchResponsePacket(txdata);
        Radio.transmit(0, TX_SEARCH_RESPONSE_PACKET_SIZE, txdata);

        app_timer_start(timer_id, APP_TIMER_TICKS(TX_SEARCH_DURATION), NULL);
        break;
      }

      meshAddress.setSearch(rxSearchId);
      sendTXSearchPing();

    } break;
    case STATE_RX_SEARCH: {
      app_timer_stop(timer_id);
      txFailedCount = 0;
      (void)nrf_esb_flush_tx();
      (void)nrf_esb_start_tx();
      state = STATE_TX_SEARCH;
      txFailedCount = 0;

      Radio.init(NRF_ESB_MODE_PTX);

      // TODO offset search rate with a +- random number to avoid perfect sync
      uint32_t status = app_timer_start(timer_id, APP_TIMER_TICKS(SEARCH_RATE_MS), NULL);
      if (status != NRF_SUCCESS) {
        NRF_LOG_ERROR("Mesh join failed to start timer");
      }
    } break;
    case STATE_TX_SEARCH_RETRY_DELAY: {
      if (txFailedCount < RX_SEARCH_RETRY_COUNT) {
        txFailedCount++;
      } else {
        txFailedCount = 0;
        state = STATE_TX_SEARCH;
      }
    } break;
    case STATE_NONE: {
      app_timer_stop(timer_id);
      connectionEvent(CONNECT_FAILED);
    } break;
    }
  }

  void handleRXEvent(nrf_esb_evt_t const *p_event) {
    nrf_esb_payload_t rx_payload;
    if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) {
      NRF_LOG_DEBUG("Receiving packet: %02x", rx_payload.data[0]);
      switch (state) {
      case STATE_TX_SEARCH: //Received a response from tx search
        processTxSearchResponsePacket(rx_payload.data);
        break;
      case STATE_RX_SEARCH: //Received a tx searcber
        if (rx_payload[0]
        processTxSearchResponsePacket(rx_payload.data);
        break;
      }
    }
  }

  void handleTXEvent(nrf_esb_evt_t const *p_event) {
    switch (p_event->evt_id) {
    case NRF_ESB_EVENT_TX_SUCCESS:
      txFailedCount = 0;
      break;
    case NRF_ESB_EVENT_TX_FAILED:
      txFailedCount++;
      (void)nrf_esb_flush_tx();
      (void)nrf_esb_start_tx();
      break;
    }
  }

  void setConnectionEventHandler(void (*p_listener)(esb_mesh_connection_event_t)) {
    connectionEventHandler = p_listener;
  }

private:
  void buildTxSearchPacket(const uint8_t *p_data) {
  p_data[5] = MESH_PACKET_TYPE_TX_SEARCH;
  }

  void processTxSearchPacket(const uint8_t *p_data) {
    uint32_t txDeviceID = p_data[0];
  }

  void buildTxSearchResponsePacket(const uint8_t *p_data) {
    p_data[5] = MESH_PACKET_TYPE_RX_SEARCH_RESPONSE;
    p_data[6] = freePipe;
  }

  void processTxSearchResponsePacket(const uint8_t *p_data) {
   txPipeId = p_data[6];
  }

    void buildTxSearchResponsePacket(const uint8_t *p_data) {
    p_data[5] = MESH_PACKET_TYPE_RX_SEARCH_RESPONSE;
    p_data[6] = freePipe;
  }


  bool processTxSearchConfirmPacket(const uint8_t *p_data) {
    return p_data[6] == deviceId;
  }

  void processTxChangePacket(const uint8_t *p_data) {
  }

  void connectionEvent(esb_mesh_connection_event_t p_event) {
    if (connectionEventHandler != 0)
      connectionEventHandler(p_event);
  }

  uint32_t createTimer() {
    uint32_t status = NRF_SUCCESS;
    if (timerCreated)
      return status;

    status = app_timer_init();

    if (status != NRF_SUCCESS) {
      return status;
    }

    status = app_timer_create(&timer_id,
        APP_TIMER_MODE_REPEATED,
        MESH_TIMER_HANDLER);
  }

  uint32_t sendTXSearchPing() {

    uint8_t data[4];
    data[2] = deviceId;

    data[0] = role;

    uint32_t status = Radio.transmit(0, 4, data);

    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("TX Send failed :%" PRIu32, status);
    }
    return status;
  }

  MeshAddress meshAddress;

  esb_mesh_role_t role;

  volatile esb_mesh_state_t state = STATE_NONE;

  uint16_t txFailedCount = 0;

  uint8_t rxId = 0;

  uint8_t rxId2 = 0;

  uint8_t txPipeId = 0;

  uint8_t txPipeId2 = 0;

  uint32_t deviceId = 0;

  uint32_t connecteDevices[7] = {0};

  uint8_t freePipe = 0;

  bool timerCreated = false;

  void (*connectionEventHandler)(esb_mesh_connection_event_t p_event) = 0;
};

extern EsbMesh Mesh;

static void MESH_TIMER_HANDLER(void *p_context) {
  Mesh.handleTimerEvent(p_context);
}

static void MESH_RX_HANDLER(nrf_esb_evt_t const *p_event) {
  Mesh.handleRXEvent(p_event);
}

static void MESH_TX_HANDLER(nrf_esb_evt_t const *p_event) {
  Mesh.handleTXEvent(p_event);
}

#endif