## Local build configuration
## Parameters configured here will override default and ENV values.
## Uncomment and change examples:

#Add your source directories here separated by space
MODULES = app

## ESP_HOME sets the path where ESP tools and SDK are located.
## Windows:
# ESP_HOME = c:/Espressif

## MacOS / Linux:
ESP_HOME = /opt/esp-open-sdk

## SMING_HOME sets the path where Sming framework is located.
## Windows:
# SMING_HOME = c:/tools/sming/Sming 

# MacOS / Linux
SMING_HOME = /opt/sming/Sming

## COM port parameter is reqruied to flash firmware correctly.
## Windows: 
# COM_PORT = COM3

# MacOS / Linux:
# COM_PORT = /dev/tty.usbserial
COM_PORT = /dev/ttyUSB0

# Com port speed
COM_SPEED = 500000

SPIFF_FILES = web

# rBoot - bootloader-specific settings
RBOOT_ENABLED   = 0
#RBOOT_BIG_FLASH = 1
#RBOOT_TWO_ROMS  = 1
SPI_SIZE        = 4M
SPI_MODE        = qio
