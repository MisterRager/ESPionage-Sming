#include <SmingCore/SPI.h>
#include "apa102.hpp"

APA102::APA102() {
  brightness = APA102_MAX_BRIGHTNESS;
}

APA102::~APA102() {
}

void APA102::show(uint8_t *buffer) {
  uint16_t k;
  uint8_t l;

  debugf("Outputting APA102 strip of len %d with brightness %d [%d]\n",
    length, brightness, brightness | APA102_WORD_HEADER);

  //Preamble
  for (k = 0; k < 4; k++) {
    SPI.transfer(0);
  }

  // Output data
  for (k = 0; k < length * 3; k += 3) {
    SPI.transfer(brightness | APA102_WORD_HEADER);
    SPI.transfer(buffer[k]);
    SPI.transfer(buffer[k + 1]);
    SPI.transfer(buffer[k + 2]);
  }

  // Number of bytes to push the data all out of shift registers
  l = (length + 14) / 16;
  for (k = 0; k < l; k++) {
    SPI.transfer(1);
  }
}

void APA102::init(uint16_t len) {
  length = len;
}

uint16_t APA102::numLeds() {
  return length;
}
