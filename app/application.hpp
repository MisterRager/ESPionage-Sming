enum LedType_enum {
  apa102,
  ws2811,
  ws2812,
  apa104
};

typedef LedType_enum LedType;

void onUdpReceive(UdpConnection& con, char *data, int size, IPAddress remoteIp, uint16_t remotePort);
const char *onTpm2Receive(uint8_t *packet);
const char *onArtnetReceive(uint8_t *packet, IPAddress *remoteIp);
void start_servers();
void wifi_failed();
void init();
void init_leds();
void ajax_init();
void paintBuffer();
void http_brightness (HttpRequest &request, HttpResponse &response);
void updateOutputBuffer(uint8_t *input, uint16_t strip_len, uint16_t leds_offset);
