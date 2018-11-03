/*
 * A mesh network implementation for handshake and connection 
 * Utilizes the EsbRadio driver
 * 
 *
 *
 *
 *
 * 2018 David Hummel
 *
 */

#include "esb_mesh.h"

//ListeningJoinManager
void ListeningJoinManager::start() {

  uint8_t txdata[SEARCH_PACKET_SIZE];
  encodeDeviceId(txdata, deviceId);
  p_data[4] = SearchPacketType::PING_RESPONSE;
  p_data[5] = 1;

  Radio.transmit(0, SEARCH_PACKET_SIZE, txdata);
}

void ListeningJoinManager::joinPacketReceived(const uint8_t *p_data) {


}

//PingingSearcher
void PingingSearcher::start() {

  app_timer_stop(timer_id);

  targetListeningPipe = 0;
  retryCount = 0;
  state = PingState::PINGING;

  MeshAddress.setSearch(parms.rxPipeId = 1);

  Radio.switchModes(NRF_ESB_MODE_PTX);

  app_timer_start(timer_id, APP_TIMER_TICKS(SEARCH_PING_DUR_MS), NULL);
}

void PingingSearcher::onTimerEvent() {

  if (state == PingState::WAITING_COMPLETE) {
    if (retryCount == 0) {
      uint8_t txdata[SEARCH_PACKET_SIZE];
      encodeDeviceId(txdata, deviceId);
      txdata[4] = SearchPacketType::PING_CONFIRM;
      if (Radio.transmit(0, SEARCH_PACKET_SIZE, txdata) != NRF_SUCCESS) {
        NRF_LOG_ERROR("Failed to ping back on listener response");
        return;
      }
      retryCount = 1;
      return;
    } else {
      NRF_LOG_ERROR("Stuck in waiting for complete response, giving up.");
      app_timer_stop(timer_id);
      parms.success = false;
      Mesh.internalConnectionHandler(parms);
      return;
    }
  }
  if (state != PingState::PINGING) {
    if (retryCount == 99) {
      NRF_LOG_ERROR("Stuck in connect process, giving up.");
      app_timer_stop(timer_id);
      parms.success = false;
      Mesh.internalConnectionHandler(parms);
      return;
    }
    retryCount = 99;
    return;
  }
  if (targetListeningPipe == RX_MAX_DEVICES) {
    // Time to increment
    retryCount++;

    if (retryCount == SEARCH_PING_RETRY_COUNT) {
      NRF_LOG_ERROR("Unable to find any listeners, giving up.");
      app_timer_stop(timer_id);
      parms.success = false;
      Mesh.internalConnectionHandler(parms);
      return;
    }
    targetListeningPipe = 1;
  } else {
    targetListeningPipe++;
  }

  NRF_LOG_INFO("Pinging %i on retry%i", targetListeningPipe, retryCount);

  MeshAddress.setSearch(targetListeningPipe);
  uint8_t txdata[SEARCH_PACKET_SIZE];
  encodeDeviceId(txdata, deviceId);
  txdata[4] = SearchPacketType::PING;
  (void)nrf_esb_flush_tx();
  Radio.transmit(0, SEARCH_PACKET_SIZE, txdata);
}

void PingingSearcher::packetReceived(const uint8_t p_pipe, const uint8_t *p_data) {
  switch (state) {
  case PingState::PINGING: {
    if (p_data[4] == SearchPacketType::PING_RESPONSE) {
      parms.rxPipeId = targetListeningPipe;
      parms.txPipeId = p_data[5];
      parms.friendId = decodeDeviceId(p_data);

      NRF_LOG_INFO("Received ping response from deviceId %" PRIu32 " on Pipe %" PRIu8 " to tx on %" PRIu8, parms.friendId, parms.rxPipeId, parms.txPipeId);

      MeshAddress.setTx(parms.rxPipeId, parms.txPipeId);

      state = PingState::WAITING_COMPLETE;

      retryCount = 0;
    }
  } break;
  case PingState::WAITING_COMPLETE: {
    if (p_data[4] == SearchPacketType::PING_COMPLETE) {
      uint32_t rx_device = decodeDeviceId(p_data);
      NRF_LOG_INFO("Received ping conf from deviceId %" PRIu32, rx_device);

      if (rx_device != parms.friendId) {
        NRF_LOG_ERROR("Received conf from deviceId %" PRIu32 " but expected %" PRIu32, rx_device, parms.friendId);
        app_timer_stop(timer_id);
        parms.success = false;
        Mesh.internalConnectionHandler(parms);
        return;
      }
      app_timer_stop(timer_id);
      parms.success = true;
      Mesh.internalConnectionHandler(parms);
      return;
    }
  } break;
  }
}

//Listening Searcher
void ListeningSearcher::start() {

  app_timer_stop(timer_id);

  state = ListenState::LISTENING;

  Radio.switchModes(NRF_ESB_MODE_PRX);

  MeshAddress.setSearch(parms.rxPipeId = 1);

  uint8_t txdata[SEARCH_PACKET_SIZE];
  encodeDeviceId(txdata, deviceId);
  p_data[4] = SearchPacketType::PING_RESPONSE;
  p_data[5] = 1;

  Radio.transmit(0, SEARCH_PACKET_SIZE, txdata);

  app_timer_start(timer_id, APP_TIMER_TICKS(SEARCH_LISTEN_DUR_MS), NULL);

  Radio.enableRX();
}

void ListeningSearcher::packetReceived(const uint8_t p_pipe, const uint8_t *p_data) {
  switch (state) {
  case ListenState::LISTENING: {
    Radio.disableRX();
    Radio.flushTxQueue();
    if (p_data[4] == SearchPacketType::PING) {
      parms.friendId = decodeDeviceId(p_data);
      parms.txPipeId = 1;
      parms.rxPipeId = p_pipe + parms.rxPipeId;

      MeshAddress.setRx(parms.rxPipeId);

      uint8_t txdata[SEARCH_PACKET_SIZE];

      encodeDeviceId(txdata, deviceId);
      buildSearchPingCompletePacket(txdata);
      // The Tx device is now transmitting on the first Tx/Rx pipe
      // Not the discovery (0) pipe

      Radio.transmit(1, SEARCH_PACKET_SIZE, txdata);
      state = ListenState::WAITING_CONF;
    }
    Radio.enableRX();
  } break;
  case ListenState::WAITING_CONF: {
    Radio.disableRX();
    Radio.flushTxQueue();
    if (p_data[4] == SearchPacketType::PING_CONFIRM) {
      if (p_pipe != 1) {
        NRF_LOG_ERROR("Received confirmation but not on pipe 1 : pipe %" PRIu8, p_pipe);
        app_timer_stop(timer_id);
        parms.success = false;
        Mesh.internalConnectionHandler(parms);
        return;
      }
      uint32_t tx_device = decodeDeviceId(&p_data[0]);
      if (tx_device != parms.friendId) {
        NRF_LOG_ERROR("Received conf from deviceId %" PRIu32 " but expected %" PRIu32, tx_device, parms.friendId);
        app_timer_stop(timer_id);
        parms.success = false;
        Mesh.internalConnectionHandler(parms);
        return;
      }
      state = ListenState::CONNECTED;
      parms.success = true;
      app_timer_stop(timer_id);
      Mesh.internalConnectionHandler(parms);
      return;
    }
    Radio.enableRX();
  } break;
  }
}

void ListeningSearcher::onTimerEvent() {
  Radio.disableRX();
  if (state != ListenState::CONNECTED) {
    NRF_LOG_INFO("Listening completed on range %" PRIu8 " and starting next 8", parms.rxPipeId);
    parms.rxPipeId += 8;
    if (parms.rxPipeId > RX_MAX_DEVICES) {
      NRF_LOG_INFO("Listening completed without activity, stopping");
      app_timer_stop(timer_id);
      parms.success = false;
      Mesh.internalConnectionHandler(parms);
      return;
    }
    MeshAddress.setSearch(parms.rxPipeId);
  }
  Radio.enableRX();
}

void ListeningSearcher::buildSearchPingCompletePacket(uint8_t *p_data) {
  p_data[4] = SearchPacketType::PING_COMPLETE;
}

//MeshAddr
void MeshAddress::setRoot(uint8_t const *p_root) {
  memcpy(root, p_root, 3);
}

uint32_t MeshAddress::setSearch(uint8_t p_base_rxId) {
  uint32_t status = NRF_SUCCESS;

  if (p_base_rxId == 0) {
    NRF_LOG_ERROR("Mesh rxId invalid, cant be 0");
    return MESH_ERROR_INVALID_ID;
  }

  uint8_t addr[4] = {root[0], root[1], root[2], 0};

  status = Radio.setPrimaryAddr(addr);
  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
    return MESH_ERROR_INVALID_ROOT;
  }

  status = Radio.setSecondaryAddr(addr);
  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
    return MESH_ERROR_INVALID_ROOT;
  }

  uint8_t prefix[8] = {p_base_rxId, p_base_rxId + 1, p_base_rxId + 2, p_base_rxId + 3, p_base_rxId + 4, p_base_rxId + 5, p_base_rxId + 6, p_base_rxId + 7};

  status = Radio.setAllAddrPrefix(8, prefix);

  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh set prefix failed on radio prefix update:%" PRIu32, status);
    return MESH_ERROR_MISC;
  }

  return status;
}

uint32_t MeshAddress::setRx(uint8_t p_rxId) {

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

uint32_t MeshAddress::setTx(uint8_t p_rxId, uint8_t p_txId, uint8_t p_rxId2, uint8_t p_txId2) {
  if (p_rxId == 0) {
    NRF_LOG_ERROR("Mesh rxId invalid, cant be 0");
    return MESH_ERROR_INVALID_ID;
  }

  if (p_txId == 0) {
    NRF_LOG_ERROR("Mesh txId invalid, [1-7]");
    return MESH_ERROR_INVALID_ID;
  }

  uint32_t status = NRF_SUCCESS;

  uint8_t addr[4] = {root[0], root[1], root[2], p_rxId};

  status = Radio.setPrimaryAddr(addr);
  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
    return MESH_ERROR_INVALID_ROOT;
  }

  if (p_rxId2 != 0) {

    uint8_t addr[4] = {root[0], root[1], root[2], p_rxId2};

    status = Radio.setSecondaryAddr(addr);
    if (status != NRF_SUCCESS) {
      NRF_LOG_ERROR("Mesh Addr failed on radio update:%" PRIu32, status);
      return MESH_ERROR_INVALID_ROOT;
    }
  }

  if (p_txId2 == 0) {
    uint8_t prefix[1] = {p_txId};
    status = Radio.setAllAddrPrefix(1, prefix);
  } else {
    uint8_t prefix[2] = {p_txId, p_txId2};
    status = Radio.setAllAddrPrefix(2, prefix);
  }

  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh set prefix failed on radio update:%" PRIu32, status);
    return MESH_ERROR_MISC;
  }
  return status;
}

//EsbMesh

EsbMesh::EsbMesh() {
  Radio.setReceivedListener(MESH_RX_HANDLER);
  // Radio.setTransmittedListener(MESH_TX_HANDLER);
}

uint32_t EsbMesh::join(uint32_t p_id, RoleType p_role) {

  deviceId = p_id;
  role = p_role;
  uint32_t status = createTimer();

  if (status != NRF_SUCCESS) {
    NRF_LOG_ERROR("Mesh failed to create timer:%" PRIu32, status);
    return status;
  }

  app_timer_stop(timer_id);

  Radio.init(NRF_ESB_MODE_PTX);

  startPingingSearcher();

  return NRF_SUCCESS;
}

void EsbMesh::internalConnectionHandler(connection_parms_t p_parms) {
  switch (state) {
  case MeshState::DEACTIVE:
    break;
  case MeshState::SEARCH_PING: {
    if (p_parms.success == false) {
      if (role == ROLE_LOW_POWER) {
        NRF_LOG_INFO("Low power device waiting between ping search");
        state = MeshState::SEARCH_WAIT;
        app_timer_stop(timer_id);

        if (searcher != NULL) {
          delete searcher;
          searcher = NULL;
        }

        app_timer_start(timer_id, APP_TIMER_TICKS(SEARCH_PING_DELAY_MS), NULL);
      } else {
        NRF_LOG_INFO("High power device switching from ping to listen search");
        app_timer_stop(timer_id);

        startListeningSearcher();
      }
    } else {
      NRF_LOG_INFO("Sucessfully connected via pinging search");
      state = MeshState::ACTIVE;
      app_timer_stop(timer_id);

      if (searcher != NULL) {
        delete searcher;
        searcher = NULL;
      }
      if (connectionEventHandler != NULL) {
        connectionEventHandler(ConnectionEvent::CONNECT_SUCCESS);
      }
    }
  } break;
  case MeshState::SEARCH_LISTEN: {
    if (p_parms.success == false) {
      NRF_LOG_INFO("High power device switching from listen to ping search");
      app_timer_stop(timer_id);

      startPingingSearcher();
    } else {
      NRF_LOG_INFO("Sucessfully connected via listening search");
      state = MeshState::ACTIVE;
      app_timer_stop(timer_id);

      if (searcher != NULL) {
        delete searcher;
        searcher = NULL;
      }
      if (connectionEventHandler != NULL) {
        connectionEventHandler(ConnectionEvent::CONNECT_SUCCESS);
      }
    }
  } break;
  }
}

void EsbMesh::handleTimerEvent(void *p_context) {
  switch (state) {
  case MeshState::SEARCH_WAIT: {
    app_timer_stop(timer_id);
    startPingingSearcher();
  } break;
  case MeshState::SEARCH_PING: {
    searcher->onTimerEvent();
  } break;
  case MeshState::SEARCH_LISTEN: {
    searcher->onTimerEvent();
  } break;
  }
}

void EsbMesh::handleRXEvent(nrf_esb_evt_t const *p_event) {
  while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS) {
    if (rx_payload.length > 0) {
      NRF_LOG_DEBUG("MESH RECEIVED PAYLOAD\n");
      switch (state) {
      case MeshState::SEARCH_PING:
      case MeshState::SEARCH_LISTEN: {
        if (rx_payload.length != SEARCH_PACKET_SIZE) {
          NRF_LOG_ERROR("Incorrectly sized search ping received : %i\n", rx_payload.length);
          return;
        }
        searcher->packetReceived(rx_payload.pipe, rx_payload.data);
      } break;
      case MeshState::ACTIVE:
        break;
      }
    }
  }
}

void EsbMesh::handleTXEvent(nrf_esb_evt_t const *p_event) {
  //TODO route event
  if (p_event->evt_id == NRF_ESB_EVENT_TX_FAILED) {
    (void)nrf_esb_flush_tx();
    (void)nrf_esb_start_tx();
  }
}

void EsbMesh::setConnectionEventHandler(void (*p_listener)(ConnectionEvent)) {
  connectionEventHandler = p_listener;
}

uint32_t EsbMesh::createTimer() {
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

  timerCreated = true;

  return status;
}

void EsbMesh::startPingingSearcher() {
  if (searcher != NULL) {
    searcher->stop();
    delete searcher;
  }
  searcher = new PingingSearcher();
  searcher->setDeviceId(deviceId);

  state = MeshState::SEARCH_PING;

  searcher->start();
}

void EsbMesh::startListeningSearcher() {
  if (searcher != NULL) {
    searcher->stop();
    delete searcher;
  }
  searcher = new ListeningSearcher();
  searcher->setDeviceId(deviceId);

  state = MeshState::SEARCH_LISTEN;
  searcher->start();
}