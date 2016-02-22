#include <string.h>

#define ARTNET_PREAMBLE "Art-Net"

#define ARTNET_PORT 0x1936
#define ARTNET_PORT_L 0x36
#define ARTNET_PORT_H 0x19
#define ARTNET_VERSION 14
#define ARTNET_OEM 0x6666
#define ARTNET_OEM_L 0x66
#define ARTNET_OEM_H 0x66
#define ARTNET_ETSA_MAN "WO"
#define ARTNET_SHORT_NAME "ESPionage Relay"
#define ARTNET_LONG_NAME "esp8266-based apa102 control module"

#define ARTNET_SHORT_NAME_LENGTH 18
#define ARTNET_LONG_NAME_LENGTH 64
#define ARTNET_REPORT_LENGTH 64
#define ARTNET_MAX_PORTS 4
#define ARTNET_MAC_SIZE 6

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

struct	Artnet_PollPacket_Struct {
  uint8_t  id[8];
  uint16_t opCode;
  uint8_t  verH;
  uint8_t  ver;
  uint8_t  ttm;
  uint8_t  pad;
} PACKED;

typedef struct Artnet_PollPacket_Struct Artnet_PollPacket;

struct Artnet_ReplyPacket_Struct {
  uint8_t  id[8];
  uint16_t opCode;
  uint8_t  ip[4];
  uint8_t port;
  uint8_t portH;
  uint8_t  verH;
  uint8_t  ver;
  uint8_t  subH;
  uint8_t  sub;
  uint8_t  oemH;
  uint8_t  oem;
  uint8_t  ubea;
  uint8_t  status;
  uint8_t  etsaman[2];
  uint8_t  shortname[ARTNET_SHORT_NAME_LENGTH];
  uint8_t  longname[ARTNET_LONG_NAME_LENGTH];
  uint8_t  nodereport[ARTNET_REPORT_LENGTH];
  uint8_t  numbportsH;
  uint8_t  numbports;
  uint8_t  porttypes[ARTNET_MAX_PORTS];
  uint8_t  goodinput[ARTNET_MAX_PORTS];
  uint8_t  goodoutput[ARTNET_MAX_PORTS];
  uint8_t  swin[ARTNET_MAX_PORTS];
  uint8_t  swout[ARTNET_MAX_PORTS];
  uint8_t  swvideo;
  uint8_t  swmacro;
  uint8_t  swremote;
  uint8_t  sp1;
  uint8_t  sp2;
  uint8_t  sp3;
  uint8_t  style;
  uint8_t  mac[ARTNET_MAC_SIZE];
  uint8_t  filler[32];
} PACKED;

typedef struct Artnet_ReplyPacket_Struct Artnet_ReplyPacket;

enum Artnet_PacketType_Enum {
  ARTNET_POLL = 0x2000,
  ARTNET_REPLY = 0x2100,
  ARTNET_DMX = 0x5000,
  ARTNET_ADDRESS = 0x6000,
  ARTNET_INPUT = 0x7000,
  ARTNET_TODREQUEST = 0x8000,
  ARTNET_TODDATA = 0x8100,
  ARTNET_TODCONTROL = 0x8200,
  ARTNET_RDM = 0x8300,
  ARTNET_VIDEOSTEUP = 0xa010,
  ARTNET_VIDEOPALETTE = 0xa020,
  ARTNET_VIDEODATA = 0xa040,
  ARTNET_MACMASTER = 0xf000,
  ARTNET_MACSLAVE = 0xf100,
  ARTNET_FIRMWAREMASTER = 0xf200,
  ARTNET_FIRMWAREREPLY = 0xf300,
  ARTNET_IPPROG = 0xf800,
  ARTNET_IPREPLY = 0xf900,
  ARTNET_MEDIA = 0x9000,
  ARTNET_MEDIAPATCH = 0x9200,
  ARTNET_MEDIACONTROLREPLY = 0x9300
} PACKED;

typedef enum Artnet_PacketType_Enum Artnet_PacketType;

struct Artnet_DmxHeader_Struct {
  uint8_t  id[8];
  Artnet_PacketType opCode;
  uint8_t  verH;
  uint8_t  ver;
  uint8_t  sequence;
  uint8_t  physical;
  uint8_t universe;
  uint8_t universeHi;
  uint8_t  lengthHi;
  uint8_t  length;
} PACKED;

typedef struct Artnet_DmxHeader_Struct Artnet_DmxHeader;

int artnet_packet_is_artnet(uint8_t *buffer);
int artnet_packet_is_artnet(Artnet_DmxHeader *buffer);
uint8_t *artnet_packet_payload(uint8_t *buffer);
size_t artnet_packet_payload_size(uint8_t *buffer);
uint16_t artnet_packet_universe(Artnet_DmxHeader *buffer);
size_t artnet_packet_payload_size(Artnet_DmxHeader *buffer);
