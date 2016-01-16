#include "tpm2_net.h"

int tpm2_packet_is_tpm2(uint8_t *buffer) {
  return buffer[0] == 0xC9;
}

uint8_t tpm2_packet_type(uint8_t *buffer) {
  //return only known valid types
  switch (buffer[1]) {
  case PACKET_DATA:
  case PACKET_COMMAND:
  case PACKET_REQUESTED_RESPONSE:
    return buffer[1];
  default:
    return 0;
  }
}

size_t tpm2_packet_payload_size(uint8_t *buffer) {
  uint8_t payload_size_upper = buffer[2];
  uint8_t payload_size_lower = buffer[3];
  return  (((size_t) payload_size_upper) << 8) + payload_size_lower;
}

uint8_t *tpm2_packet_payload(uint8_t *buffer) {
  return &buffer[4];
}
