void onUdpReceive(UdpConnection& con, char *data, int size, IPAddress remoteIp, uint16_t remotePort);
const char *onTpm2Receive(uint8_t *packet);
const char *onArtnetReceive(uint8_t *packet, IPAddress *remoteIp);
void startTpmServer();
void init();
void paintBuffer();
