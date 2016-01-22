#include <user_config.h>
#include <SmingCore/SmingCore.h>

#include "apa102.cpp"
//#include "nzt_out.cpp"
#include "tpm2_net.c"
#include "art_net.c"
#include "application.hpp"
#include "http_server.cpp"
#include "config.cpp"
//#include <WS2812/WS2812.h>
//#include "ws2812_i2s/src/ws2812_i2s.cpp"
//#include "ws2812i2s/ws2812_i2s.c"
//#include <Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include "ws2812_i2s/ws2812_i2s.cpp"


#define MAX_LEN 2048

#define MUTEX_LOCKED 1
#define MUTEX_UNLOCKED 0

uint8_t mutex = MUTEX_UNLOCKED;

uint8_t brightness = 0xFF;
WifiCredentials wifi_credentials;
LedType output_mode;

uint8_t buffer[MAX_LEN * 3];
size_t len = 0;
size_t packet_size = 0;

uint8_t *payload;
size_t packet_number = 0;
size_t packet_count = 0;
size_t payload_size = 0;
uint8_t packet_type = 0;

Artnet_ReplyPacket artnet_reply;

UdpConnection tpm2(onUdpReceive);
UdpConnection artnet(onUdpReceive);

//Apa104Writer apa104_writer(15);
//WS2812 *ws2812_writer = NULL;
WS2812 ws2812_writer;
APA102 apa102_writer;

Timer testTimer;
//Adafruit_NeoPixel *neopixel_writer = NULL;

void init()
{
  Serial.begin(500000);
  Serial.systemDebugOutput(true); // Allow debug print to serial

  // More dakka
  system_update_cpu_freq(160);

  memset(buffer, 0, MAX_LEN * 3);

  // SPIFFS filesystem
#ifdef RBOOT_SPIFFS_0
  int slot = rboot_get_current_rom();
  if (slot == 0) {
    //debugf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
    spiffs_mount_manual(RBOOT_SPIFFS_0 + 0x40200000, SPIFF_SIZE);
  } else {
    //debugf("trying to mount spiffs at %x, length %d", RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
    spiffs_mount_manual(RBOOT_SPIFFS_1 + 0x40200000, SPIFF_SIZE);
  }
#else
  spiffs_mount();
#endif


  // Grab credentials from spiffs
  read_wifi_credentials(wifi_credentials);

  if (!wifi_credentials.ssid || !wifi_credentials.ssid.length()) {
    wifi_credentials.ssid = WIFI_SSID;
    wifi_credentials.password = WIFI_PWD;
    save_wifi_credentials(wifi_credentials);
  }

  brightness = read_brightness();
  output_mode = read_output_mode();

  init_leds();

  debugf("Brightness %d, OutputMode %d\n", brightness, output_mode);

  // Wifi init
  WifiAccessPoint.enable(false);
  WifiStation.enable(true);
  WifiStation.config(wifi_credentials.ssid, wifi_credentials.password);

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

  // network daemons
  WifiStation.waitConnection(start_servers, 30, wifi_failed);

  testTimer.setIntervalMs(10);
  testTimer.setCallback(paintBuffer);
  testTimer.start(true);
}

void init_leds() {
  switch (output_mode) {
  case apa104:
  case ws2812:
  case ws2811:
    break;
  case apa102:
    SPI.begin(3, 4);
    break;
  }
}

void retry_wifi_client () {
  WifiAccessPoint.enable(false);
  WifiStation.enable(true);
  WifiStation.config(wifi_credentials.ssid, wifi_credentials.password);
  WifiStation.waitConnection(start_servers, 30, wifi_failed);
}

int servers_have_init = 0;

void wifi_failed() {
  WifiStation.enable(false);
  WifiAccessPoint.enable(true);
  start_servers();
}

void start_servers() {
  if (servers_have_init) {
    return;
  }
  servers_have_init = 1;

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
    new_len = ((packet_count - 1)  * packet_size + payload_size) / 3;
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

    switch (output_mode) {
    case apa102:
      //showColorBuffer(buffer, len * 3, brightness);
      if(apa102_writer.numLeds() != len) apa102_writer.init(len);
      apa102_writer.show(buffer);
      break;
    case apa104:
    case ws2811:
    case ws2812:
      if (ws2812_writer.numLeds() != len) ws2812_writer.init(len);
      ws2812_writer.show(buffer);
      break;
    }

    mutex = MUTEX_UNLOCKED;
  } else {
    debugf("Skipping buffer paint for mutex lock\n");
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
    save_brightness(brightness);
  }
  response_obj["brightness"] = brightness;

  response.sendJsonObject(stream);
}

void http_wifi_auth (HttpRequest &request, HttpResponse &response) {
  JsonObjectStream* stream = new JsonObjectStream();
  JsonObject& response_obj = stream->getRoot();

  String method = request.getRequestMethod();
  const char * c_method = method.c_str();

  if (memcmp(c_method, METHOD_POST, 5) == 0) {
    response_obj["old_ssid"] = wifi_credentials.ssid;
    wifi_credentials.ssid = request.getPostParameter("wifi_ssid");
    wifi_credentials.password = request.getPostParameter("wifi_password");
    save_wifi_credentials(wifi_credentials);
  }
  response_obj["ssid"] = wifi_credentials.ssid;

  response.sendJsonObject(stream);
}

void http_output_led_type (HttpRequest &request, HttpResponse &response) {
  JsonObjectStream* stream = new JsonObjectStream();
  JsonObject& response_obj = stream->getRoot();
  String method = request.getRequestMethod();
  String param_input;

  if (method.equals("METHOD_POST")) {
    param_input = request.getPostParameter("output_type");

    if (param_input.equals("apa102")) {
      output_mode = apa102;
      save_output_mode(output_mode);
    } else if (param_input.equals("apa104")) {
      output_mode = apa104;
      save_output_mode(output_mode);
    }
  }

  response_obj["type"] = output_mode;
  response.sendJsonObject(stream);
}

void ajax_init() {
  http_add_route("brightness", http_brightness);
  http_add_route("wifi_credentials", http_wifi_auth);
  http_add_route("led_type", http_output_led_type);
}
