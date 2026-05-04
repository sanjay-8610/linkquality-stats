/************************************************************************************
  If not stated otherwise in this file or this component's LICENSE file the
  following copyright and licenses apply:

  Copyright 2018 RDK Management

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 **************************************************************************/

#ifndef _LQ_EVENTS_H_
#define _LQ_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif
/*
 * These values MUST match the wifi_event_t enum in ccsp-one-wifi/include/wifi_events.h.
 * The sender stores the raw wifi_event_t sub_type in stats_arg_t::event before sending
 * over IPC, so the receiver must use the same numeric values.
 *
 * Base calculation from wifi_events.h:
 *   wifi_event_type_base    = 0x1
 *   wifi_event_type_hal_ind = 2
 *   wifi_event_hal_unknown_frame = 1 << (2 + 6) = 256
 */
typedef enum {
    wifi_event_hal_auth_frame           = 260,
    wifi_event_hal_deauth_frame         = 261,
    wifi_event_hal_assoc_req_frame      = 262,
    wifi_event_hal_assoc_rsp_frame      = 263,
    wifi_event_hal_reassoc_req_frame    = 264,
    wifi_event_hal_reassoc_rsp_frame    = 265,
    wifi_event_hal_sta_conn_status      = 269,
    wifi_event_hal_disassoc_device      = 271,
} lq_event_t;
#ifdef __cplusplus
}   
#endif
    
#endif
