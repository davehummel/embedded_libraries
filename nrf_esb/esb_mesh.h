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

#define SEARCH_PING_RETRY_COUNT 1
#define RX_MAX_DEVICES 16

#define SEARCH_PACKET_SIZE 8

#define SEARCH_PING_DUR_MS 100
#define SEARCH_PING_DELAY_MS 10000
#define SEARCH_LISTEN_DUR_MS 2000

#define MESH_ROOT \
  { 0x44, 0x41, 0xA0 }

#define MESH_ERROR_BASE (0xC100)
#define MESH_ERROR_MISC (MESH_ERROR_BASE + 0x0000)         ///<
#define MESH_ERROR_INVALID_ROOT (MESH_ERROR_BASE + 0x0001) ///< Root address rejected by NRF sdk
#define MESH_ERROR_INVALID_ID (MESH_ERROR_BASE + 0x0002)   ///< Id is invalid (0 is not allowed)

static void MESH_TIMER_HANDLER(void *p_context);
static void MESH_RX_HANDLER(nrf_esb_evt_t const *p_event);
static void MESH_TX_HANDLER(nrf_esb_evt_t const *p_event);

APP_TIMER_DEF(timer_id);

struct connection_parms_t {
  bool success = false;
  uint32_t friendId = 0;
  uint8_t rxPipeId = 0;
  uint8_t txPipeId = 0;
};

class JoinManager {
public:
  virtual void start() = 0;
  virtual void joinPacketReceived(const uint8_t *p_data) = 0;
  void setConnectedDevices(int32_t *p_connectedDevices) {
    connectedDevices = p_connectedDevices;
  }

  void setDeviceId(const uint32_t p_id) { // If state is null when called, connection failed
    deviceId = p_id;
  }

protected:
  uint32_t connectedDevices[];
  uint32_t deviceId;
};

class ListeningJoinManager : public JoinManager {
  void start();
  void joinPacketReceived(const uint8_t *p_data);
};

class Searcher {
public:
  virtual void start() = 0;
  virtual void packetReceived(const uint8_t p_pipe, const uint8_t *p_data) = 0;
  virtual void onTimerEvent() = 0;

  void setDeviceId(const uint32_t p_id) { // If state is null when called, connection failed
    deviceId = p_id;
  }

  void stop() {
    app_timer_stop(timer_id);
  }

protected:
  enum SearchPacketType : uint8_t {
    PING = 1,
    PING_RESPONSE,
    PING_CONFIRM,
    PING_COMPLETE
  };

  connection_parms_t parms;
  uint32_t deviceId = 0;
};

class PingingSearcher : public Searcher {
  void start();
  void packetReceived(const uint8_t p_pipe, const uint8_t *p_data);
  void onTimerEvent();

private:
  void buildSearchPingPacket(uint8_t *p_data);
  void buildSearchPingConfirmPacket(uint8_t *p_data);

  uint8_t targetListeningPipe;
  uint8_t retryCount;

  enum struct PingState {
    PINGING,
    WAITING_COMPLETE,
    CONNECTED
  };

  PingState state;
};

class ListeningSearcher : public Searcher {
  void start();
  void packetReceived(const uint8_t p_pipe, const uint8_t *p_data);
  void onTimerEvent();

private:
  void buildSearchPingResponsePacket(uint8_t *p_data);
  void buildSearchPingCompletePacket(uint8_t *p_data);

  enum struct ListenState {
    LISTENING,
    WAITING_CONF,
    CONNECTED
  };

  ListenState state;
};

class MeshAddress {
public:
  void setRoot(uint8_t const *p_root);

  uint32_t setSearch(uint8_t p_rxId);
  uint32_t setRx(uint8_t p_rxId);
  uint32_t setTx(uint8_t p_rxId, uint8_t p_txId, uint8_t p_rxId2 = 0, uint8_t p_txId2 = 0);

private:
  uint8_t root[3] = MESH_ROOT;
};

class EsbMesh {
public:
  /**@brief Mesh role describing behavior of device on connect . */
  enum RoleType {
    ROLE_LOW_POWER,  /**< Low Power TX centric role     */
    ROLE_HIGH_POWER, /**< High Power TX/RX optional role     */
                     // ROLE_CENTRAL,    /**< High Power central RX only role     */
  };

  enum ConnectionEvent {
    CONNECT_FAILED,
    CONNECT_SUCCESS,    /**< Pinging central RX channels in search     */
    CONNECT_NEW_JOINER, /**< Pinging central RX channels in search     */
    CONENCT_LOST,       /**< Listening to central RX channels for search     */
  };

  EsbMesh();

  uint32_t join(uint32_t p_id, RoleType p_role);

  void handleTimerEvent(void *p_context);

  void handleRXEvent(nrf_esb_evt_t const *p_event);
  void handleTXEvent(nrf_esb_evt_t const *p_event);

  void setConnectionEventHandler(void (*p_listener)(ConnectionEvent));

  void internalConnectionHandler(connection_parms_t p_parms);

private:
  enum struct MeshState {
    DEACTIVE,
    SEARCH_PING,
    SEARCH_LISTEN,
    SEARCH_WAIT,
    ACTIVE
  };

  void connectionEvent(ConnectionEvent p_event);

  void startPingingSearcher();

  void startListeningSearcher();

  uint32_t createTimer();

  bool timerCreated = false;

  RoleType role;

  uint32_t deviceId;

  uint32_t connectedDevices[7] = {0};

  void (*connectionEventHandler)(ConnectionEvent p_event) = 0;

  Searcher *searcher = NULL;

  JoinManager *joinManager = NULL;

  MeshState state = MeshState::DEACTIVE;

  nrf_esb_payload_t rx_payload;
};

extern EsbMesh Mesh;

static MeshAddress MeshAddress;

static void MESH_TIMER_HANDLER(void *p_context) {
  Mesh.handleTimerEvent(p_context);
}

static void MESH_RX_HANDLER(nrf_esb_evt_t const *p_event) {
  Mesh.handleRXEvent(p_event);
}

static void MESH_TX_HANDLER(nrf_esb_evt_t const *p_event) {
  Mesh.handleTXEvent(p_event);
}

static uint32_t decodeDeviceId(const uint8_t *p_data) {
  return (p_data[0] | (p_data[1] << 8) | (p_data[2] << 16) | (p_data[3] << 24));
}

static void encodeDeviceId(uint8_t *p_data, uint32_t p_id) {
  p_data[0] = p_id & 0xFF;
  p_data[1] = (p_id >> 8) & 0xFF;
  p_data[2] = (p_id >> 16) & 0xFF;
  p_data[3] = (p_id >> 24) & 0xFF;
}

#endif