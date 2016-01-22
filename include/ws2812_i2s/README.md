This is the lovely [esp8266_ws2812_i2s](https://github.com/JoDaNl/esp8266_ws2812_i2s) library from JoDaNl, a refactor of the work by [cnlohr](https://github.com/cnlohr/esp8266ws2812i2s). Here, dark magic from Espressif originally intended to run an i2s peripheral with DMA buffers is used to instead push data to ws281x lights, including the apa104.

The code here is mostly as I found it except for some dependency tweaking and a truly ugly hack to get the function rom_i2s_writeReg to work.
