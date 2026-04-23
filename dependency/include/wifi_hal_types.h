/************************************************************************************
  Minimal WiFi HAL type definitions for linkquality-stats.
  Provides only the types this component actually uses, eliminating the
  dependency on the full ccsp-one-wifi / halinterface header chain.
 **************************************************************************/

#ifndef LQ_WIFI_HAL_TYPES_H
#define LQ_WIFI_HAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDR_LEN        6
#define MAC_ADDRESS_LENGTH  13

typedef unsigned char mac_addr_t[MAC_ADDR_LEN];

/* "AA:BB:CC:DD:EE:FF\0" — 17 chars + NUL */
typedef char mac_addr_str_t[18];

#ifdef __cplusplus
}
#endif

#endif /* LQ_WIFI_HAL_TYPES_H */
