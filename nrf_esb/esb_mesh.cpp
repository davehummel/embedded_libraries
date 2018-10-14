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

 void MeshAddress::setRoot(uint8_t const *p_root) {
    memcpy(root, p_root, 3);
  }
