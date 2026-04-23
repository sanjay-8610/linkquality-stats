##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2018 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################

ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = -D_ANSC_LINUX -D_ANSC_USER -D_ANSC_LITTLE_ENDIAN_ -Wall -Werror

# Include paths:
#   - dependency stubs first (override sysroot wifi_*.h)
#   - our own include/ (lq_log.h, linkquality_stats_rbus.h)
#   - rbus sysroot headers
AM_CFLAGS += \
	-I$(top_srcdir)/dependency/include \
	-I$(top_srcdir)/include \
	-I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus

AM_CXXFLAGS = $(AM_CFLAGS)

bin_PROGRAMS = linkquality_stats

linkquality_stats_SOURCES = \
	$(top_srcdir)/source/linkquality_stats_main.c \
	$(top_srcdir)/source/linkquality_stats_rbus.c \
	$(top_srcdir)/source/run_qmgr.cpp \
	$(top_srcdir)/source/qmgr.cpp \
	$(top_srcdir)/source/linkq.cpp \
	$(top_srcdir)/source/caffinty.cpp \
	$(top_srcdir)/dependency/source/collection.c \
	$(top_srcdir)/dependency/source/number.cpp \
	$(top_srcdir)/dependency/source/vector.cpp \
	$(top_srcdir)/dependency/source/sequence.cpp

linkquality_stats_LDFLAGS = \
	-lrbus \
	-lcjson \
	-lstdc++ \
	-lpthread \
	-lm \
	-Wl,--no-as-needed \
	-Wl,--as-needed
