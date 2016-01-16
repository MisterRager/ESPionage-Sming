#define TPM2_FIRST_BYTE 0x9C
#define PACKET_DATA 0xDA
#define PACKET_COMMAND 0xC0
#define PACKET_REQUESTED_RESPONSE 0xAA

#define TPM2_CLIENT_RESPONSE "¬"  // just one char: 0xAC

#define TPM2_CLIENT_PORT 0xFFE2
#define TPM2_ACK_PORT 0xFFA2

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int tpm2_packet_is_tpm2(uint8_t *buffer);
uint8_t tpm2_packet_type(uint8_t *buffer);
size_t tpm2_packet_payload_size(uint8_t *buffer);
uint8_t *tpm2_packet_payload(uint8_t *buffer);
