#include <SmingCore/SPI.h>
#include "apa102.hpp"

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

void showColorBuffer(uint8_t *buffer, size_t len, uint8_t brightness) {
  int k, l;
  int lights = len / 3;

  //Preamble
  for (k = 0; k < 4; k++) {
    SPI.transfer(0);
  }

  // Add global brightness to beginning of the triplets for color
  for (k = 0; k < lights; k += 3) {
    SPI.transfer(brightness);
    for (l = 0; l < 3; l++) {
      SPI.transfer(buffer[k + l]);
    }
  }

  // Number of bytes to push the data all out
  l = (lights + 14) / 16;
  for (k = 0; k < l; k++) {
    SPI.transfer(1);
  }
}
