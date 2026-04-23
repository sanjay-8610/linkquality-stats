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

#ifndef LQ_RBUS_EVENTS_H
#define LQ_RBUS_EVENTS_H

#include <stdint.h>
#include "run_qmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * rbus event element names for onewifi → linkquality_stats IPC.
 * onewifi publishes these; linkquality_stats subscribes.
 */
#define LQ_RBUS_EVENT_PERIODIC_STATS      "Device.WiFi.LinkQuality.PeriodicStats"
#define LQ_RBUS_EVENT_RAPID_DISCONNECT    "Device.WiFi.LinkQuality.RapidDisconnect"
#define LQ_RBUS_EVENT_REMOVE              "Device.WiFi.LinkQuality.Remove"
#define LQ_RBUS_EVENT_HAL_INDICATION      "Device.WiFi.LinkQuality.HalIndication"
#define LQ_RBUS_EVENT_START               "Device.WiFi.LinkQuality.Start"
#define LQ_RBUS_EVENT_STOP                "Device.WiFi.LinkQuality.Stop"
#define LQ_RBUS_EVENT_GW_DISCOVERY        "Device.WiFi.LinkQuality.GwDiscovery"

/*
 * Wire-format message sent as raw bytes via bus_raw_event_publish_fn.
 * stats_arg_t is a flat struct (no pointers) — safe for raw memcpy serialization.
 */
typedef struct {
    uint32_t event_type;      /* wifi_event_type_t */
    uint32_t sub_type;        /* wifi_event_subtype_t */
    uint32_t num_entries;     /* count of stats_arg_t entries that follow */
    stats_arg_t entries[];    /* flexible array member */
} lq_rbus_event_msg_t;

/* Helper macro: total byte size for a message carrying n stats_arg_t entries */
#define LQ_RBUS_MSG_SIZE(n) \
    (sizeof(lq_rbus_event_msg_t) + (size_t)(n) * sizeof(stats_arg_t))

#ifdef __cplusplus
}
#endif

#endif /* LQ_RBUS_EVENTS_H */
