#ifndef LQ_TELEMETRY_H
#define LQ_TELEMETRY_H

#include <string>
#include <vector>
#include <telemetry_busmessage_sender.h>

void lq_publish_t2_events(const std::vector<std::string> &station_metrics,
                           double avg_lq_score, double avg_caff_score,
                           double avg_ucaff_score);



#endif // LQ_TELEMETRY_H
