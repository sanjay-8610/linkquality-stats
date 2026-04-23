/************************************************************************************
  Minimal wifi_events.h for linkquality-stats.
  Contains only the HAL event enums that caffinty.cpp references.
  Values match ccsp-one-wifi/include/wifi_events.h exactly.
 **************************************************************************/

#ifndef LQ_WIFI_EVENTS_H
#define LQ_WIFI_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- event-type base and bitshift macros (same as onewifi) ---------- */
#define wifi_event_type_base  0x1

typedef enum {
    wifi_event_type_exec,       /* 0 */
    wifi_event_type_webconfig,  /* 1 */
    wifi_event_type_hal_ind,    /* 2 */
    wifi_event_type_command,
    wifi_event_type_monitor,
    wifi_event_type_net,
    wifi_event_type_wifiapi,
    wifi_event_type_analytic,
    wifi_event_type_csi,
    wifi_event_type_speed_test,
    wifi_event_type_max
} wifi_event_type_t;

/* ---------- HAL indication event subtypes ----------------------------------
 * Base value: wifi_event_type_base << (wifi_event_type_hal_ind + 6)
 *           = 0x1 << (2 + 6) = 0x1 << 8 = 256
 * Then sequential values from there.
 */
typedef enum {
    wifi_event_hal_unknown_frame = wifi_event_type_base
        << (wifi_event_type_hal_ind + 6),     /* 256 */
    wifi_event_hal_mgmt_frames,               /* 257 */
    wifi_event_hal_probe_req_frame,           /* 258 */
    wifi_event_hal_probe_rsp_frame,           /* 259 */
    wifi_event_hal_auth_frame,                /* 260 */
    wifi_event_hal_deauth_frame,              /* 261 */
    wifi_event_hal_assoc_req_frame,           /* 262 */
    wifi_event_hal_assoc_rsp_frame,           /* 263 */
    wifi_event_hal_reassoc_req_frame,         /* 264 */
    wifi_event_hal_reassoc_rsp_frame,         /* 265 */
    wifi_event_hal_dpp_public_action_frame,   /* 266 */
    wifi_event_hal_dpp_config_req_frame,      /* 267 */
    wifi_event_hal_anqp_gas_init_frame,       /* 268 */
    wifi_event_hal_sta_conn_status,           /* 269 */
    wifi_event_hal_assoc_device,              /* 270 */
    wifi_event_hal_disassoc_device            /* 271 */
} wifi_event_subtype_hal_t;

#ifdef __cplusplus
}
#endif

#endif /* LQ_WIFI_EVENTS_H */
