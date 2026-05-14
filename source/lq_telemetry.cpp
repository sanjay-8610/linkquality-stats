#ifdef ENABLE_T2_TELEMETRY

#include "lq_telemetry.h"
#include "utils/linkquality_util.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

extern "C" {
#ifdef LINKQUALITY_RDKB_T2_SUPPORT
    // Real T2 API — resolved from libtelemetry_msgsender at link time
    int t2_event_s(char *marker, char *value);
    int t2_event_d(char *marker, int value);
#else
    // No-op stubs when telemetry library is not available
    static int t2_event_s(char *marker, char *value)
    {
        (void)marker; (void)value;
        return 0;
    }
    static int t2_event_d(char *marker, int value)
    {
        (void)marker; (void)value;
        return 0;
    }
#endif
}

void lq_publish_t2_events(const std::vector<std::string> &station_metrics,
                           double avg_lq_score, double avg_caff_score,
                           double avg_ucaff_score)
{
    char marker[64] = {0};
    char value[128] = {0};
    int ret = 0;

    lq_util_error_print(LQ_LQTY, "%s:%d lq_publish_t2_events called: stations=%zu, avg_lq=%.2f, avg_caff=%.2f, avg_ucaff=%.2f\n",
           __func__, __LINE__, station_metrics.size(), avg_lq_score, avg_caff_score, avg_ucaff_score);

    // Per-station LQ scores
    for (const auto &metric : station_metrics) {
        snprintf(marker, sizeof(marker), "WIFI_LQ_SCORE");
        ret = t2_event_s(marker, const_cast<char *>(metric.c_str()));
        lq_util_error_print(LQ_LQTY, "%s:%d t2_event_s(%s, %s) returned %d\n", __func__, __LINE__, marker, metric.c_str(), ret);
    }

    // Average Home LQ Score
    snprintf(marker, sizeof(marker), "WIFI_HOMELQ_SCORE");
    snprintf(value, sizeof(value), "%f", avg_lq_score);
    ret = t2_event_s(marker, value);
    lq_util_error_print(LQ_LQTY, "%s:%d t2_event_s(%s, %s) returned %d\n", __func__, __LINE__, marker, value, ret);

    // Average Connected Affinity Score
    std::memset(marker, 0, sizeof(marker));
    std::memset(value, 0, sizeof(value));
    snprintf(marker, sizeof(marker), "WIFI_HOMECONNECTED_SCORE");
    snprintf(value, sizeof(value), "%f", avg_caff_score);
    ret = t2_event_s(marker, value);
    lq_util_error_print(LQ_LQTY, "%s:%d t2_event_s(%s, %s) returned %d\n", __func__, __LINE__, marker, value, ret);

    // Average Unconnected Affinity Score
    std::memset(marker, 0, sizeof(marker));
    std::memset(value, 0, sizeof(value));
    snprintf(marker, sizeof(marker), "WIFI_HOMEUNCONNECTED_SCORE");
    snprintf(value, sizeof(value), "%f", avg_ucaff_score);
    ret = t2_event_s(marker, value);
    lq_util_error_print(LQ_LQTY, "%s:%d t2_event_s(%s, %s) returned %d\n", __func__, __LINE__, marker, value, ret);
}

#endif // ENABLE_T2_TELEMETRY
