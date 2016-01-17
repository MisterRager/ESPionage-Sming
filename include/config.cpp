#include <SmingCore/FileSystem.h>
#include <string>

#define WIFI_CONFIG_FILE ".wifi_credentials"

typedef struct {
  String ssid;
  String password;
} WifiCredentials;

void save_wifi_credentials(WifiCredentials &config_credentials);
void read_wifi_credentials(WifiCredentials &config_credentials);

void save_wifi_credentials(WifiCredentials &config_credentials) {
  String config_string = config_credentials.ssid + "\n" + config_credentials.password;
  fileSetContent((const String) WIFI_CONFIG_FILE, config_string);
}

void read_wifi_credentials(WifiCredentials &config_credentials) {
  String config_string;
  size_t newline_pos, second_newline_pos;

  if (!fileExist(WIFI_CONFIG_FILE)) {
    config_credentials.ssid = "";
    config_credentials.password = "";
    save_wifi_credentials(config_credentials);
  } else {
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
