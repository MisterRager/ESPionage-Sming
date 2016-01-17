# ESPionage-Sming
ESP8266 application for controlling APA102 (dotstar) lights

This uses the esp8266 as a bridge for lighting control data sent over the Art-Net or tpm2.net protocols. Configure the wifi settings, flash the controllers, and simply push unicast data at them with your favorite lighting desk software.

Build requirements:
- Fairly recent Sming (and all its dependencies) at $SMING_HOME
- esp_open_sdk at $ESP_HOME
- SPI speed turned down to 0.5mbit or so:
- a config file at `include/user_config_local.h` with wifi credentials

To actually install:
```
make
make flash
```


To set wifi credentialls, add the wifi credentials to `include/user_config_local.h`:
```
#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_SSID SomeNetwork
#define WIFI_PWD SomeNetworkPass

#ifdef __cplusplus
}
#endif
```


The SPI clock frequency is in `$SMING_HOME/Sming/SmingCore/SPI.cpp`. See method `SPIClass::begin`. The variables `predivider` and `divider` must be adjusted so that their product is `16`. I used `4` and `4` for the new values. Setting the divider too small runs the lights faster than the strip can actually stably drive the lights at the end.
