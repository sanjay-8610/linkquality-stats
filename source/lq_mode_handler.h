/************************************************************************************
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

#ifndef LQ_MODE_HANDLER_H
#define LQ_MODE_HANDLER_H

/*
 * lq_mode_handler — Polymorphic GW vs EXT event dispatcher.
 *
 * Replaces the function-pointer descriptor pattern from OneWifi's
 * wifi_linkquality_libs.c with C++ virtual dispatch.
 *
 * GW mode  : processes stats locally via qmgr (add_stats_metrics, etc.)
 * EXT mode : TODO — forward stats to GW (mechanism TBD)
 *
 * Usage:
 *   // Once at startup (in main):
 *   lq_mode_handler_init();
 *
 *   // On each IPC event (in receiver):
 *   lq_mode_handler *h = lq_mode_handler::get_instance();
 *   h->handle_periodic_stats(entries, count);
 */

#include "run_qmgr.h"

typedef enum {
    LQ_MODE_GW,
    LQ_MODE_EXT
} lq_mode_t;

#ifdef __cplusplus

class lq_mode_handler {
public:
    virtual ~lq_mode_handler() {}

    /* Periodic stats batch from assoc_client polling */
    virtual void handle_periodic_stats(stats_arg_t *stats, int count) = 0;

    /* Client permanently removed from sta_map */
    virtual void handle_disconnect(stats_arg_t *stats) = 0;

    /* Rapid disconnect detection */
    virtual void handle_rapid_disconnect(stats_arg_t *stats) = 0;

    /* Single HAL/DHCP caffinity event (auth, assoc, deauth, etc.) */
    virtual void handle_caffinity_event(stats_arg_t *stats) = 0;

    /* Start/stop link-quality metrics collection */
    virtual void handle_start_metrics() = 0;
    virtual void handle_stop_metrics() = 0;

    /* Ignite station registration */
    virtual void handle_register_station(const char *mac_str) = 0;
    virtual void handle_unregister_station(const char *mac_str) = 0;

    /* Reinitialize metrics parameters (reporting interval, threshold) */
    virtual void handle_reinit_metrics(server_arg_t *arg) = 0;

    /* Set per-radio max observed SNR */
    virtual void handle_set_max_snr(radio_max_snr_t *snr) = 0;

    /* Query current mode */
    virtual lq_mode_t get_mode() const = 0;

    /* Singleton accessor — returns GW or EXT handler based on init */
    static lq_mode_handler *get_instance();
};

/*
 * GW handler — processes everything locally via qmgr.
 */
class lq_gw_handler : public lq_mode_handler {
public:
    void handle_periodic_stats(stats_arg_t *stats, int count);
    void handle_disconnect(stats_arg_t *stats);
    void handle_rapid_disconnect(stats_arg_t *stats);
    void handle_caffinity_event(stats_arg_t *stats);
    void handle_start_metrics();
    void handle_stop_metrics();
    void handle_register_station(const char *mac_str);
    void handle_unregister_station(const char *mac_str);
    void handle_reinit_metrics(server_arg_t *arg);
    void handle_set_max_snr(radio_max_snr_t *snr);
    lq_mode_t get_mode() const { return LQ_MODE_GW; }
};

/*
 * EXT handler — TODO: forward stats to GW.
 */
class lq_ext_handler : public lq_mode_handler {
public:
    void handle_periodic_stats(stats_arg_t *stats, int count);
    void handle_disconnect(stats_arg_t *stats);
    void handle_rapid_disconnect(stats_arg_t *stats);
    void handle_caffinity_event(stats_arg_t *stats);
    void handle_start_metrics();
    void handle_stop_metrics();
    void handle_register_station(const char *mac_str);
    void handle_unregister_station(const char *mac_str);
    void handle_reinit_metrics(server_arg_t *arg);
    void handle_set_max_snr(radio_max_snr_t *snr);
    lq_mode_t get_mode() const { return LQ_MODE_EXT; }
};

extern "C" {
#endif

/*
 * C-callable init — reads /nvram/config.txt to determine mode,
 * creates the appropriate handler singleton.
 * Call once from main() before lq_ipc_receiver_start().
 */
void lq_mode_handler_init(void);

/*
 * C-callable dispatchers used by lq_ipc_receiver.c
 */
void lq_dispatch_periodic_stats(stats_arg_t *stats, int count);
void lq_dispatch_disconnect(stats_arg_t *stats);
void lq_dispatch_rapid_disconnect(stats_arg_t *stats);
void lq_dispatch_caffinity_event(stats_arg_t *stats);
void lq_dispatch_start_metrics(void);
void lq_dispatch_stop_metrics(void);
void lq_dispatch_register_station(const char *mac_str);
void lq_dispatch_unregister_station(const char *mac_str);
void lq_dispatch_reinit_metrics(server_arg_t *arg);
void lq_dispatch_set_max_snr(radio_max_snr_t *snr);

#ifdef __cplusplus
}
#endif

#endif /* LQ_MODE_HANDLER_H */
