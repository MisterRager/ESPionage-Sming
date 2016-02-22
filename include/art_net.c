#include "art_net.h"

int artnet_packet_is_artnet(uint8_t *buffer) {
  return artnet_packet_is_artnet((Artnet_DmxHeader *) buffer);
}

char artnet_preamble[] = ARTNET_PREAMBLE;

int artnet_packet_is_artnet(Artnet_DmxHeader *buffer) {
  return memcmp((const char *) buffer->id, artnet_preamble, sizeof artnet_preamble) == 0;
}

size_t artnet_packet_payload_size(uint8_t *buffer) {
  return artnet_packet_payload_size((Artnet_DmxHeader *) buffer);
}

size_t artnet_packet_payload_size(Artnet_DmxHeader *buffer) {
  return (buffer->lengthHi << 8) + buffer->length;
}

uint16_t artnet_packet_universe(Artnet_DmxHeader *buffer){
  return (buffer->universeHi << 8) + buffer->universe;
}

uint8_t *artnet_packet_payload(uint8_t *buffer) {
  return &buffer[18];
}
