#define APA102_WORD_HEADER 0b11100000
#define APA102_MAX_BRIGHTNESS 0b11111

class APA102 {
public:
  uint8_t brightness;
  APA102();
  ~APA102();
  void init(uint16_t num_leds);
  void show(uint8_t *buffer);
  uint16_t numLeds();
private:
  uint16_t length;
};

