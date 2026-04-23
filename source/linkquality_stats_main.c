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

/*
 * linkquality_stats – standalone daemon that receives link-quality events
 * from OneWifi via rbus and drives the quality-manager (qmgr) library.
 *
 * Daemon pattern follows rdkb_daemonize() in
 *   ccsp-one-wifi/source/platform/rdkb/misc.c:
 *   - Named POSIX semaphore synchronises parent exit with child init.
 *   - fork() → child calls setsid(), redirects stdio → /dev/null.
 *   - Child signals semaphore after successful rbus init so the parent
 *     (waited on by systemd Type=forking) can exit cleanly.
 *   - PID file written by child for systemd PIDFile= tracking.
 */

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "linkquality_stats_rbus.h"
#include "lq_log.h"
#include "run_qmgr.h"

#define COMPONENT_NAME          "linkquality_stats"
#define LQ_PID_FILE             "/var/tmp/linkquality_stats.pid"
#define LQ_SEM_NAME             "pSemLQStats"
#define SSP_LOOP_TIMEOUT_SEC    30

static volatile sig_atomic_t g_running = 1;
static pthread_mutex_t       g_loop_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t        g_loop_cond  = PTHREAD_COND_INITIALIZER;
static sem_t                *g_sem        = SEM_FAILED;

/* ------------------------------------------------------------------ */
/*  Signal handler                                                     */
/* ------------------------------------------------------------------ */

static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
    pthread_cond_signal(&g_loop_cond);
}

/* ------------------------------------------------------------------ */
/*  PID file                                                           */
/* ------------------------------------------------------------------ */

static void lq_write_pid_file(void)
{
    FILE *fp = fopen(LQ_PID_FILE, "w");
    if (!fp) {
        wifi_util_error_print(WIFI_LQ, "%s:%d failed to open pid file %s: %s\n",
                              __func__, __LINE__, LQ_PID_FILE, strerror(errno));
        return;
    }
    fprintf(fp, "%d\n", getpid());
    fclose(fp);
}

/* ------------------------------------------------------------------ */
/*  Daemonize (rdkb_daemonize pattern from misc.c)                    */
/* ------------------------------------------------------------------ */

static void lq_daemonize(void)
{
    int fd;

    /*
     * Named semaphore: parent blocks here until child signals that rbus
     * init has completed.  Unlinked immediately so a crash does not leave
     * a stale named semaphore blocking the next start.
     */
    g_sem = sem_open(LQ_SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (g_sem == SEM_FAILED) {
        wifi_util_error_print(WIFI_LQ, "%s:%d sem_open failed: %d - %s\n",
                              __func__, __LINE__, errno, strerror(errno));
        _exit(1);
    }
    sem_unlink(LQ_SEM_NAME);

    switch (fork()) {
    case 0:
        /* child — fall through and continue daemon execution */
        break;
    case -1:
        wifi_util_error_print(WIFI_LQ, "%s:%d fork failed: %d - %s\n",
                              __func__, __LINE__, errno, strerror(errno));
        sem_close(g_sem);
        _exit(1);
    default:
        /* parent — wait for child to confirm init, then exit */
        sem_wait(g_sem);
        sem_close(g_sem);
        _exit(0);
    }

    /* Child: create new session, detach from controlling terminal */
    if (setsid() < 0) {
        wifi_util_error_print(WIFI_LQ, "%s:%d setsid failed: %d - %s\n",
                              __func__, __LINE__, errno, strerror(errno));
        sem_post(g_sem);
        sem_close(g_sem);
        _exit(1);
    }

    /* Redirect stdio to /dev/null */
    fd = open("/dev/null", O_RDONLY);
    if (fd != -1 && fd != STDIN_FILENO)  { dup2(fd, STDIN_FILENO);  close(fd); }
    fd = open("/dev/null", O_WRONLY);
    if (fd != -1 && fd != STDOUT_FILENO) { dup2(fd, STDOUT_FILENO); close(fd); }
    fd = open("/dev/null", O_WRONLY);
    if (fd != -1 && fd != STDERR_FILENO) { dup2(fd, STDERR_FILENO); close(fd); }
}

/* ------------------------------------------------------------------ */
/*  SSP-style event loop                                               */
/*                                                                     */
/*  Uses a condition variable with a timed wait so the loop can:       */
/*    - wake immediately on SIGINT/SIGTERM (via sig_handler)           */
/*    - wake periodically every SSP_LOOP_TIMEOUT_SEC seconds for       */
/*      health-checks or future periodic work                          */
/*    - wake on-demand when an internal event is posted (future use)   */
/*                                                                     */
/*  rc from pthread_cond_timedwait:                                    */
/*    ETIMEDOUT — periodic tick, no condition posted                   */
/*    0         — condition signaled while still running:              */
/*                either shutdown (g_running==0) or a future event     */
/*    EINTR     — interrupted by a signal, re-evaluate g_running      */
/*    other     — unexpected error, log and continue                   */
/* ------------------------------------------------------------------ */

static void lq_loop(void)
{
    struct timespec ts;
    int rc;

    pthread_mutex_lock(&g_loop_mutex);

    while (g_running) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += SSP_LOOP_TIMEOUT_SEC;

        rc = pthread_cond_timedwait(&g_loop_cond, &g_loop_mutex, &ts);

        /* Check shutdown first — takes priority over everything else */
        if (!g_running) {
            break;
        }

        switch (rc) {
        case ETIMEDOUT:
            /*
             * Periodic wakeup — placeholder for future periodic tasks:
             * e.g. health-check, telemetry flush, rbus reconnect probe.
             */
            wifi_util_dbg_print(WIFI_LQ,
                "%s:%d ssp_loop: periodic tick (every %ds)\n",
                __func__, __LINE__, SSP_LOOP_TIMEOUT_SEC);
            /* TODO: dispatch periodic work here */
            break;

        case 0:
            /*
             * Condition was signaled while g_running is still true.
             * Placeholder for future in-process event dispatch:
             * e.g. dequeue from a work-item ring and process.
             */
            wifi_util_dbg_print(WIFI_LQ,
                "%s:%d ssp_loop: event wakeup\n", __func__, __LINE__);
            /* TODO: dequeue and handle event from work queue here */
            break;

        case EINTR:
            /* Signal interrupted the wait — re-evaluate g_running */
            break;

        default:
            wifi_util_error_print(WIFI_LQ,
                "%s:%d ssp_loop: pthread_cond_timedwait error rc=%d (%s)\n",
                __func__, __LINE__, rc, strerror(rc));
            break;
        }
    }

    pthread_mutex_unlock(&g_loop_mutex);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    /*
     * Daemonize first: fork(), setsid(), stdio→/dev/null.
     * Parent blocks on semaphore until child signals after rbus init.
     * From this point on we are running in the child process.
     */
    lq_daemonize();

    /* Write PID file before anything else so systemd can track us */
    lq_write_pid_file();

    wifi_util_info_print(WIFI_LQ, "%s:%d starting linkquality_stats daemon (pid=%d)\n",
                         __func__, __LINE__, getpid());

    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, SIG_IGN);

    /*if (lq_stats_rbus_init() != 0) {
        wifi_util_error_print(WIFI_LQ, "%s:%d rbus init failed — signalling parent and exiting\n",
                              __func__, __LINE__);
        
         * Signal parent even on failure so it does not hang waiting
         * on the semaphore.  Parent will see our exit code via waitpid
         * through systemd's Type=forking tracking.
         
        sem_post(g_sem);
        sem_close(g_sem);
        unlink(LQ_PID_FILE);
        return EXIT_FAILURE;
    }
*/

    sem_post(g_sem);
    sem_close(g_sem);
    g_sem = SEM_FAILED;

    /* Event-driven: rbus dispatches callbacks on its own threads.
     * lq_loop() keeps the daemon alive with periodic + on-demand wakeups. */
    lq_loop();

    wifi_util_info_print(WIFI_LQ, "%s:%d shutting down linkquality_stats\n",
                         __func__, __LINE__);
    //lq_stats_rbus_deinit();
    unlink(LQ_PID_FILE);

    return EXIT_SUCCESS;
}
