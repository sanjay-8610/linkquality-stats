/************************************************************************************
  Minimal wifi_base.h for linkquality-stats.
  Contains only the structures and macros that this component uses,
  copied from ccsp-one-wifi/include/wifi_base.h.
 **************************************************************************/

#ifndef LQ_WIFI_BASE_H
#define LQ_WIFI_BASE_H

#include "wifi_hal_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define wifi_sub_component_base  0x01

/* Lightweight device-stats struct — only the fields link-quality scoring needs */
typedef struct {
    unsigned long      cli_PacketsSent;
    unsigned long      cli_PacketsReceived;
    unsigned long      cli_RetransCount;
    unsigned long long cli_RxRetries;
    int                cli_SNR;
    unsigned int       cli_MaxDownlinkRate;
    unsigned int       cli_MaxUplinkRate;
    unsigned int       cli_LastDataDownlinkRate;
    unsigned int       cli_LastDataUplinkRate;
    bool               cli_PowerSaveMode;
} dev_stats_t;

/* Per-sample telemetry snapshot */
typedef struct {
    double score;
    double snr;
    double per;
    double phy;
    char   time[1024];
} sample_t;

/* Per-client link report for batch reporting */
typedef struct {
    char     mac[18];
    int      vap_index;
    double   threshold;
    int      alarm;
    char     reporting_time[32];
    size_t   sample_count;
    sample_t *samples;
} link_report_t;

/* Batch of link reports */
typedef struct {
    size_t         link_count;
    link_report_t *links;
} report_batch_t;

#ifdef __cplusplus
}
#endif

#endif /* LQ_WIFI_BASE_H */
