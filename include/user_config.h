#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "user_config_local.h"

#ifdef __cplusplus
extern "C" {
#endif

	// UART config
	#define SERIAL_BAUD_RATE 500000

	// ESP SDK config
	#define LWIP_OPEN_SRC
	#define USE_US_TIMER

	// Default types
	#define __CORRECT_ISO_CPP_STDLIB_H_PROTO
	#include <limits.h>
	#include <stdint.h>

	// Override c_types.h include and remove buggy espconn
	#define _C_TYPES_H_
	#define _NO_ESPCON_

	// Updated, compatible version of c_types.h
	// Just removed types declared in <stdint.h>
	#include <espinc/c_types_compatible.h>

	// System API declarations
	#include <esp_systemapi.h>

	// C++ Support
	#include <esp_cplusplus.h>
	// Extended string conversion for compatibility
	#include <stringconversion.h>
	// Network base API
	#include <espinc/lwip_includes.h>

	// Beta boards
	#define BOARD_ESP01

#ifndef WIFI_SSID
#define WIFI_SSID "ESPionage"
#endif

#ifndef WIFI_PWD
#define WIFI_PWD ""
#endif

#ifdef __cplusplus
}
#endif

#endif
