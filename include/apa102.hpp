void showColor(size_t len, uint8_t brightness, uint8_t b, uint8_t g, uint8_t r);
void showColorBuffer(uint8_t *buffer, size_t len, uint8_t brightness);

#define APA102_WORD_HEADER 0xE0
#define APA102_MAX_BRIGHTNESS 31

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

