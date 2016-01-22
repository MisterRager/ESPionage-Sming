#include <SmingCore/FileSystem.h>
#include <string>

#define WIFI_CONFIG_FILE ".wifi_credentials"
#define BRIGHTNESS_FILE ".brightness"
#define OUTPUT_MODE_FILE ".output"

typedef struct {
  String ssid;
  String password;
} WifiCredentials;

void save_wifi_credentials(WifiCredentials &config_credentials);
void read_wifi_credentials(WifiCredentials &config_credentials);
void save_brightness (uint8_t brightness);
uint8_t read_brightness();

void save_wifi_credentials(WifiCredentials &config_credentials) {
  String config_string = config_credentials.ssid + "\n" + config_credentials.password;
  debugf("Save!\n%s\n", config_string.c_str());
  fileSetContent((const String) WIFI_CONFIG_FILE, config_string);
}

void read_wifi_credentials(WifiCredentials &config_credentials) {
  String config_string;
  size_t newline_pos, second_newline_pos;

  if (fileExist(WIFI_CONFIG_FILE)) {
    config_string = fileGetContent(WIFI_CONFIG_FILE);
    newline_pos = config_string.indexOf('\n');
    second_newline_pos = newline_pos != std::string::npos ?
      config_string.indexOf('\n', newline_pos) : std::string::npos;

    if (newline_pos == std::string::npos) {
      config_credentials.ssid = "";
      config_credentials.password = "";
    } else {
      if (second_newline_pos == std::string::npos) {
        second_newline_pos = config_string.length() - 1;
      }

      config_credentials.ssid = config_string.substring(0, newline_pos);
      config_credentials.password = config_string.substring(newline_pos + 1, second_newline_pos - newline_pos - 1);
    }
  }
}

void save_brightness (uint8_t brightness) {
  String config_string;

  brightness = brightness < (255 - 31) ? (255 - 31): brightness;
  brightness = brightness > 255 ? 255 : brightness;

  config_string = String(brightness);
  fileSetContent((const String) BRIGHTNESS_FILE, config_string);
}

uint8_t read_brightness() {
  String config_string;

  if (fileExist(BRIGHTNESS_FILE)) {
    config_string = fileGetContent(BRIGHTNESS_FILE);
    return config_string.toInt();
  }

  return 255;
}

void save_output_mode(LedType mode) {
  String config_string = String((int) mode);
  fileSetContent((const String) OUTPUT_MODE_FILE, config_string);
}

LedType read_output_mode() {
  String config_string;
  if (fileExist(OUTPUT_MODE_FILE )) {
    config_string = fileGetContent(OUTPUT_MODE_FILE);
    return (LedType) config_string.toInt();
  }

  return apa102;
}
