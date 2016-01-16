#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "apa102.cpp"
#include "tpm2_net.c"
#include "application.hpp"

uint8_t brightness = 0xFF;
UdpConnection udp(onUdpReceive);

void init()
{
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

void onUdpReceive(UdpConnection& con, char *data, int size, IPAddress remoteIp, uint16_t remotePort) {
  uint8_t *packet = (uint8_t *) data;
  Serial.println("Got packet!");

  if (tpm2_packet_is_tpm2(packet)) {
    if (tpm2_packet_type(packet) == PACKET_DATA ) {
      showColorBuffer(
        tpm2_packet_payload(packet),
        tpm2_packet_payload_size(packet),
        brightness
      );
    }
  }
  con.sendStringTo(remoteIp, TPM2_ACK_PORT, TPM2_CLIENT_RESPONSE);
}
