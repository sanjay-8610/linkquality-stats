/************************************************************************************
  Stub wifi_util.h for linkquality-stats.
  Redirects to the standalone lq_log.h logging shim and adds the WIFI_LIB
  module tag needed by the math_utils source files.
 **************************************************************************/

#ifndef LQ_WIFI_UTIL_H
#define LQ_WIFI_UTIL_H

#include "lq_log.h"

/*
 * math_utils sources (number.cpp, vector.cpp, sequence.cpp) use
 * wifi_util_dbg_print(WIFI_LIB, ...) which is not defined in lq_log.h.
 * We add WIFI_LIB here as a value that the existing lq_log_print handles
 * via its default case.
 */
#ifndef WIFI_LIB
#define WIFI_LIB  ((lq_dbg_type_t)99)
#endif

#endif /* LQ_WIFI_UTIL_H */
