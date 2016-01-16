#include "tpm2_net.h"

uint8_t tpm2_byte = TPM2_FIRST_BYTE;

int tpm2_packet_is_tpm2(uint8_t *buffer) {
  return buffer[0] == tpm2_byte;
}

const uint8_t tpm2_byte_pd = PACKET_DATA;
const uint8_t tpm2_byte_pc = PACKET_COMMAND;
const uint8_t tpm2_byte_prr = PACKET_REQUESTED_RESPONSE;

uint8_t tpm2_packet_type(uint8_t *buffer) {
  //return only known valid types
  switch (buffer[1]) {
  case tpm2_byte_pd:
  case tpm2_byte_pc:
  case tpm2_byte_prr:
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

size_t tpm2_packet_number(uint8_t *buffer) {
  return buffer[4];
}

size_t tpm2_packet_count(uint8_t *buffer) {
  return buffer[5];
}

uint8_t *tpm2_packet_payload(uint8_t *buffer) {
  return &buffer[6];
}
