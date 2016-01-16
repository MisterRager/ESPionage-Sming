#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "apa102.cpp"
#include "tpm2_net.c"
#include "art_net.c"
#include "application.hpp"
#include "http_server.cpp"

#define MAX_LEN 2048

#define MUTEX_LOCKED 1
#define MUTEX_UNLOCKED 0

uint8_t mutex = MUTEX_UNLOCKED;
uint8_t brightness = 0xFF;
UdpConnection tpm2(onUdpReceive);
UdpConnection artnet(onUdpReceive);
uint8_t buffer[MAX_LEN * 3];
size_t len = 0;
size_t packet_size = 0;

uint8_t *payload;
size_t packet_number = 0;
size_t packet_count = 0;
size_t payload_size = 0;
uint8_t packet_type = 0;

Artnet_ReplyPacket artnet_reply;

void init()
{
  memset(buffer, 0, MAX_LEN * 3);
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.systemDebugOutput(false); // Allow debug print to serial
  SPI.begin();

  //UDP server
  WifiStation.enable(true);
  WifiStation.config(WIFI_SSID, WIFI_PWD);
  WifiAccessPoint.enable(false);

  memcpy(&buffer[(packet_number - 1) * packet_size], payload, packet_size);
  memset(&artnet_reply.port, 0, sizeof(Artnet_ReplyPacket));
  artnet_reply.port = ARTNET_PORT_L;
  artnet_reply.portH = ARTNET_PORT_H;
  artnet_reply.verH = 0;
  artnet_reply.ver = ARTNET_VERSION;
  artnet_reply.subH = 0;
  artnet_reply.sub = 0;
  artnet_reply.oem = ARTNET_OEM_L;
  artnet_reply.oemH = ARTNET_OEM_H;
  artnet_reply.ubea = 0;
  artnet_reply.status = 0;
  strcpy((char *) artnet_reply.etsaman, ARTNET_ETSA_MAN);
  strcpy((char *) artnet_reply.shortname, ARTNET_SHORT_NAME);
  strcpy((char *) artnet_reply.longname, ARTNET_LONG_NAME);
  artnet_reply.numbports = 1;
  artnet_reply.numbportsH = 0;
  artnet_reply.porttypes[0] = 0x80;
  artnet_reply.swout[0] = 0;

  WifiStation.waitConnection(start_servers);
}

void start_servers() {
  tpm2.listen(TPM2_CLIENT_PORT);
  artnet.listen(ARTNET_PORT);
  http_server_init();
  ajax_init();
}

void onUdpReceive(UdpConnection& con, char *data, int size, IPAddress remoteIp, uint16_t remotePort) {
  const char *response = NULL;
  uint8_t *packet = (uint8_t *) data;
  uint16_t port;

  if (tpm2_packet_is_tpm2(packet)) {
    response = onTpm2Receive(packet);
    port = TPM2_ACK_PORT;
  } else if (artnet_packet_is_artnet(packet)) {
    response = onArtnetReceive(packet, &remoteIp);
    port = ARTNET_PORT;
  }

  if (response != NULL) {
    con.sendStringTo(remoteIp, ARTNET_PORT, response);
  } else {
    con.sendStringTo(remoteIp, ARTNET_PORT, "");
  }

  paintBuffer();
}

const char *onTpm2Receive(uint8_t *packet) {
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

const char *onArtnetReceive(uint8_t *packet, IPAddress *remoteIp) {
  Artnet_DmxHeader *header = (Artnet_DmxHeader *) packet;
  size_t new_len;
  size_t buffer_offset = (header->universe - 1) * 512;

  if (header->opCode == ARTNET_DMX) {
    payload_size = artnet_packet_payload_size(header);
    if (header->universe >= packet_count) {
      packet_count = (uint8_t) (header->universe & 0xFF);
    }
    new_len = (packet_count - 1)  * packet_size + payload_size;
    len = (len < new_len) ? new_len : len;
    memcpy(
      &buffer[buffer_offset],
      artnet_packet_payload(packet),
      payload_size
    );
  } else if (header->opCode == ARTNET_POLL) {
    memcpy(artnet_reply.ip, (uint8_t **) remoteIp, 4);
    strcpy((char *) artnet_reply.nodereport, "All peachy!");
    return (char *) &artnet_reply;
  }
  return NULL;
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

const char *METHOD_GET = "GET";
const char *METHOD_POST = "POST";

void http_brightness (HttpRequest &request, HttpResponse &response) {
  JsonObjectStream* stream = new JsonObjectStream();
  JsonObject& response_obj = stream->getRoot();

  String method = request.getRequestMethod();
  const char * c_method = method.c_str();

  if (memcmp(c_method, METHOD_POST, 5) == 0) {
    response_obj["old_brightness"] = brightness;
    brightness = request.getPostParameter("brightness").toInt();
  }
  response_obj["brightness"] = brightness;

  response.sendJsonObject(stream);
}

void ajax_init() {
  http_add_route("brightness", http_brightness);
}
