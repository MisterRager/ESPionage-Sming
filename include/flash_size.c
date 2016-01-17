// we need to include these to be able to use the spi_flash_get_id() function
#ifdef __plusplus
extern "C" {
#endif
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
#ifdef __plusplus
}
#endif

size_t fs_size() { // returns the flash chip's size, in BYTES
  uint32_t id = spi_flash_get_id();  
  uint8_t mfgr_id = id & 0xff;
  uint8_t type_id = (id >> 8) & 0xff; // not relevant for size calculation
  uint8_t size_id = (id >> 16) & 0xff; // lucky for us, WinBond ID's their chips as a form that lets us calculate the size
  if(mfgr_id != 0xEF) // 0xEF is WinBond; that's all we care about (for now)
    return 0;
  return 1 << size_id;
}
