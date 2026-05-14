#ifndef LQ_TELEMETRY_H
#define LQ_TELEMETRY_H

#include <string>
#include <vector>

#ifdef ENABLE_T2_TELEMETRY

void lq_publish_t2_events(const std::vector<std::string> &station_metrics,
                           double avg_lq_score, double avg_caff_score,
                           double avg_ucaff_score);

#else

static inline void lq_publish_t2_events(const std::vector<std::string> &,
                                         double, double, double) {}

#endif // ENABLE_T2_TELEMETRY

#endif // LQ_TELEMETRY_H
