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

#include <stdio.h>
#include <string.h>
#include "lq_mode_handler.h"
#include "linkquality_util.h"
#include "run_qmgr.h"

/* ------------------------------------------------------------------ */
/*  Mode detection — mirrors OneWifi wifi_linkquality_libs.c logic     */
/* ------------------------------------------------------------------ */

static lq_mode_t detect_mode(void)
{
    char role[50] = {0};
    char ip[50]   = {0};

    FILE *fp = fopen("/nvram/config.txt", "r");
    if (fp) {
        char line[100];
        if (fgets(line, sizeof(line), fp)) {
            sscanf(line, "%49[^,],%49s", role, ip);
            ip[strcspn(ip, "\n")] = 0;
        }
        fclose(fp);
    }

    if (strcmp(role, "Extender") == 0) {
        lq_util_info_print(LQ_LQTY,
            "%s:%d detected EXT mode from /nvram/config.txt (ip=%s)\n",
            __func__, __LINE__, ip);
        return LQ_MODE_EXT;
    }

    /*
     * TODO: Also check rbus/TR-181 for rdk_dev_mode_type_ext if needed.
     * For now only /nvram/config.txt is used (same as OneWifi lab setup).
     */

    lq_util_info_print(LQ_LQTY,
        "%s:%d detected GW mode (default)\n", __func__, __LINE__);
    return LQ_MODE_GW;
}

/* ------------------------------------------------------------------ */
/*  Singleton                                                          */
/* ------------------------------------------------------------------ */

static lq_mode_handler *g_handler = nullptr;

lq_mode_handler *lq_mode_handler::get_instance()
{
    return g_handler;
}

/* ------------------------------------------------------------------ */
/*  GW handler — local qmgr processing                                */
/* ------------------------------------------------------------------ */

void lq_gw_handler::handle_periodic_stats(stats_arg_t *stats, int count)
{
    lq_util_dbg_print(LQ_LQTY,
        "%s:%d GW: periodic_stats count=%d\n", __func__, __LINE__, count);
    add_stats_metrics(stats, count);
    periodic_caffinity_stats_update(stats, count);
}

void lq_gw_handler::handle_disconnect(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: disconnect MAC=%s\n", __func__, __LINE__, stats->mac_str);
    remove_link_stats(stats);
}

void lq_gw_handler::handle_rapid_disconnect(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: rapid_disconnect MAC=%s\n", __func__, __LINE__, stats->mac_str);
    disconnect_link_stats(stats);
}

void lq_gw_handler::handle_caffinity_event(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "[linkstatus] %s:%d GW: caffinity_event MAC=%s event=%d status=%u ap_mac=%s\n",
        __func__, __LINE__, stats->mac_str, stats->event, stats->status_code, stats->ap_mac_str);
    periodic_caffinity_stats_update(stats, 1);
}

void lq_gw_handler::handle_start_metrics()
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: start_metrics\n", __func__, __LINE__);
    start_link_metrics();
}

void lq_gw_handler::handle_stop_metrics()
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: stop_metrics\n", __func__, __LINE__);
    stop_link_metrics();
}

void lq_gw_handler::handle_register_station(const char *mac_str)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: register_station mac=%s\n", __func__, __LINE__, mac_str);
    register_station_mac(mac_str);
}

void lq_gw_handler::handle_unregister_station(const char *mac_str)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: unregister_station mac=%s\n", __func__, __LINE__, mac_str);
    unregister_station_mac(mac_str);
}

void lq_gw_handler::handle_reinit_metrics(server_arg_t *arg)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: reinit_metrics reporting=%u threshold=%f\n",
        __func__, __LINE__, arg->reporting, arg->threshold);
    reinit_link_metrics(arg);
}

void lq_gw_handler::handle_set_max_snr(radio_max_snr_t *snr)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d GW: set_max_snr 2g=%d 5g=%d 6g=%d\n",
        __func__, __LINE__, snr->radio_2g_max_snr,
        snr->radio_5g_max_snr, snr->radio_6g_max_snr);
    set_max_snr_radios(snr);
}

/* ------------------------------------------------------------------ */
/*  EXT handler — placeholders, forward to GW (TBD)                   */
/* ------------------------------------------------------------------ */

void lq_ext_handler::handle_periodic_stats(stats_arg_t *stats, int count)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: periodic_stats count=%d — TODO forward to GW\n",
        __func__, __LINE__, count);

    /*
     * TODO: Forward stats to GW process.
     * Options under consideration:
     *   - AF_UNIX socket to GW's linkquality-stats
     *   - 1905.1 CMDU (like OneWifi wifi_linkquality_libs.c did)
     *   - rbus event publish
     * For now this is a no-op on extender.
     */
}

void lq_ext_handler::handle_disconnect(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: disconnect MAC=%s — TODO forward to GW\n",
        __func__, __LINE__, stats->mac_str);

    /* TODO: Forward disconnect event to GW */
}

void lq_ext_handler::handle_rapid_disconnect(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: rapid_disconnect MAC=%s — TODO forward to GW\n",
        __func__, __LINE__, stats->mac_str);

    /* TODO: Forward rapid disconnect event to GW */
}

void lq_ext_handler::handle_caffinity_event(stats_arg_t *stats)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: caffinity_event MAC=%s event=%d — TODO forward to GW\n",
        __func__, __LINE__, stats->mac_str, stats->event);

    /* TODO: Forward caffinity event to GW */
}

void lq_ext_handler::handle_start_metrics()
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: start_metrics — TODO forward to GW\n", __func__, __LINE__);
}

void lq_ext_handler::handle_stop_metrics()
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: stop_metrics — TODO forward to GW\n", __func__, __LINE__);
}

void lq_ext_handler::handle_register_station(const char *mac_str)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: register_station mac=%s — no-op on extender\n",
        __func__, __LINE__, mac_str);
}

void lq_ext_handler::handle_unregister_station(const char *mac_str)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: unregister_station mac=%s — no-op on extender\n",
        __func__, __LINE__, mac_str);
}

void lq_ext_handler::handle_reinit_metrics(server_arg_t *arg)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: reinit_metrics — no-op on extender\n", __func__, __LINE__);
    (void)arg;
}

void lq_ext_handler::handle_set_max_snr(radio_max_snr_t *snr)
{
    lq_util_info_print(LQ_LQTY,
        "%s:%d EXT: set_max_snr — no-op on extender\n", __func__, __LINE__);
    (void)snr;
}

/* ------------------------------------------------------------------ */
/*  C-callable init + dispatch wrappers                                */
/* ------------------------------------------------------------------ */

extern "C" void lq_mode_handler_init(void)
{
    if (g_handler != nullptr) {
        return;
    }

    lq_mode_t mode = detect_mode();
    if (mode == LQ_MODE_EXT) {
        g_handler = new lq_ext_handler();
    } else {
        g_handler = new lq_gw_handler();
    }

    lq_util_info_print(LQ_LQTY,
        "%s:%d mode handler initialized: %s\n",
        __func__, __LINE__,
        (mode == LQ_MODE_EXT) ? "EXTENDER" : "GATEWAY");
}

extern "C" void lq_dispatch_periodic_stats(stats_arg_t *stats, int count)
{
    if (g_handler) g_handler->handle_periodic_stats(stats, count);
}

extern "C" void lq_dispatch_disconnect(stats_arg_t *stats)
{
    if (g_handler) g_handler->handle_disconnect(stats);
}

extern "C" void lq_dispatch_rapid_disconnect(stats_arg_t *stats)
{
    if (g_handler) g_handler->handle_rapid_disconnect(stats);
}

extern "C" void lq_dispatch_caffinity_event(stats_arg_t *stats)
{
    if (g_handler) g_handler->handle_caffinity_event(stats);
}

extern "C" void lq_dispatch_start_metrics(void)
{
    if (g_handler) g_handler->handle_start_metrics();
}

extern "C" void lq_dispatch_stop_metrics(void)
{
    if (g_handler) g_handler->handle_stop_metrics();
}

extern "C" void lq_dispatch_register_station(const char *mac_str)
{
    if (g_handler) g_handler->handle_register_station(mac_str);
}

extern "C" void lq_dispatch_unregister_station(const char *mac_str)
{
    if (g_handler) g_handler->handle_unregister_station(mac_str);
}

extern "C" void lq_dispatch_reinit_metrics(server_arg_t *arg)
{
    if (g_handler) g_handler->handle_reinit_metrics(arg);
}

extern "C" void lq_dispatch_set_max_snr(radio_max_snr_t *snr)
{
    if (g_handler) g_handler->handle_set_max_snr(snr);
}
