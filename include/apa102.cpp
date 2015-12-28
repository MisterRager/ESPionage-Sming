#include <SmingCore/SPI.h>

void showColor(size_t len, uint8_t brightness, uint8_t b, uint8_t g, uint8_t r)
{
  Serial.println("Go!");
  char message[32] = "";
  int k, l;
  int count = 0;
  uint8_t singleLight[4] = {brightness, b, g, r};

  for (k = 0;k < 4; k++) {
    SPI.transfer(0);
  }

  for (k = 0; k < len; k++) {
    for (l = 0; l < 4; l++) {
      SPI.transfer(singleLight[l]);
    }
    count++;
  }

  for (k = 0; k < (len + 14) / 16; k++) {
    SPI.transfer(1);
  }

  sprintf(message, "%d lights sent", count);
  Serial.println(message);
}
