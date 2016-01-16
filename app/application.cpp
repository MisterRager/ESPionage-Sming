#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "apa102.cpp"
#include "tpm2_net.c"
#include "application.hpp"

#define MAX_LEN 2048

#define MUTEX_LOCKED 1
#define MUTEX_UNLOCKED 0

uint8_t mutex = MUTEX_UNLOCKED;
uint8_t brightness = 0xFF;
UdpConnection udp(onUdpReceive);
uint8_t buffer[MAX_LEN * 3];
size_t len = 0;
size_t packet_size = 0;
size_t highest_packet = 1;

void init()
{
  memset(buffer, 0, MAX_LEN * 3);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.systemDebugOutput(true); // Allow debug print to serial
  SPI.begin();

  //UDP server
  WifiStation.enable(true);
  WifiStation.config(WIFI_SSID, WIFI_PWD);
  WifiAccessPoint.enable(false);

  WifiStation.waitConnection(startTpmServer);
}

void startTpmServer() {
  udp.listen(TPM2_CLIENT_PORT);
}

uint8_t *payload;
size_t packet_number = 0;
size_t packet_count = 0;
size_t payload_size = 0;
uint8_t packet_type = 0;

const char *onTpm2Receive(UdpConnection& con, uint8_t *packet) {
  size_t new_len;
  packet_type = tpm2_packet_type(packet);

  if (packet_type == PACKET_DATA ) {
    packet_number = tpm2_packet_number(packet);
    packet_count = (packet_number > packet_count) ? packet_number : packet_count;
    payload_size = tpm2_packet_payload_size(packet);
    payload = tpm2_packet_payload(packet);

    if (packet_size == 0 || packet_number == 1) {
      packet_size = payload_size;
    }

    new_len = (packet_count - 1)  * packet_size + payload_size;
    len = (len < new_len) ? new_len : len;
    memcpy(&buffer[(packet_number - 1) * packet_size], payload, packet_size);
  }

  return TPM2_CLIENT_RESPONSE;
}

void onUdpReceive(UdpConnection& con, char *data, int size, IPAddress remoteIp, uint16_t remotePort) {
  const char *response;
  uint8_t *packet = (uint8_t *) data;

  if (tpm2_packet_is_tpm2(packet)) {
    response = onTpm2Receive(con, packet);
  } else {
    response = "";
  }

  con.sendStringTo(remoteIp, TPM2_ACK_PORT, response);
  paintBuffer();
}

void paintBuffer() {
  if (mutex == MUTEX_UNLOCKED) {
    mutex = MUTEX_LOCKED;
    showColorBuffer(buffer, len * 3, brightness);
    mutex = MUTEX_UNLOCKED;
  } else {
    Serial.printf("Skipping paint for mutex lock\n");
  }
}
