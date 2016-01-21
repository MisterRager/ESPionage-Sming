# ESPionage-Sming
ESP8266 application for controlling APA102 (dotstar) lights

This uses the esp8266 as a bridge for lighting control data sent over the [Art-Net](https://en.wikipedia.org/wiki/Art-Net) or [TPM2.net](https://gist.github.com/jblang/89e24e2655be6c463c56) protocols. Configure the wifi settings, flash the controllers, and simply push unicast data at them with your favorite lighting desk software.

Build requirements:
- Sming with SPI clocks exposed [Sming](https://github.com/MisterRager/Sming) at $SMING_HOME. This is a fork I made to do this.
- [esp_open_sdk](https://github.com/pfalcon/esp-open-sdk) at $ESP_HOME
- a config file at `include/user_config_local.h` with wifi credentials

To actually install onto the hardware:
```
make
make flash
```

Note that you may need to configure `Makefile-user.mk`


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

The firmware boots up and connects to the configured WiFi network. On failure, it starts an unsecured network. Once wifi settings are figured out, it starts UDP servers listening for Art-Net and TPM2.net packet data and an http server for adjusting a few of the onboard settings.

The http server is a combination of files in the `web` folder and some control endpoints called from the javascript. There, you can set a whole-strip global `brightness` value with a slider as well as the wifi credentials. The brightness should update as soon as you stop dragging the slider. The wifi settings *should* take hold after submitted. Else, just reboot the esp8266 module, and the new settings will be read on boot.

The firmware is ready to receive lighting data from software like [Glediator](http://www.solderlab.de/index.php/software/glediator), [Pixelcontroller](http://pixelinvaders.ch/?page_id=160), or [https://github.com/heronarts/LX](LX) as well as professional stuff that I can't afford to test. Currently, the TPM2.net data is just forwarded directly to the lights with the `brightness` value interpolated in from the global val. Art-Net data is mangled a bit - the controller assumes unicast communication and puts the first 512 "lights" (red, green, and blue lights in each LED module) on the first universe, the next 512 on the second universe, etc. Eventually, this layout should be editable in the web interface. 
