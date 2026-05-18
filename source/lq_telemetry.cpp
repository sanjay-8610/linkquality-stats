
#include "lq_telemetry.h"
#include "utils/linkquality_util.h"
#include <cstdio>
#include <cstring>
#include <string>

void lq_publish_t2_events(const std::string &t2_json)
{
    char marker[10] = {0};
    int ret = 0;

    lq_util_error_print(LQ_LQTY, "%s:%d lq_publish_t2_events called: json_len=%zu\n payload=%s\n",
           __func__, __LINE__, t2_json.size(), t2_json.c_str());

    snprintf(marker, sizeof(marker), "WEI_SCORE");
    ret = t2_event_s(marker, const_cast<char *>(t2_json.c_str()));
    lq_util_error_print(LQ_LQTY, "%s:%d t2_event_s(%s) returned %d\n", __func__, __LINE__, marker, ret);
}

