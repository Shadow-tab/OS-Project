#include <stdio.h>
#include <time.h>
#include "logger.h"
#include "colors.h"
#include "structs.h"
#include "config.h"

#define RESULT_SUCCESS   0
#define RESULT_NO_SEATS  1
#define RESULT_DUPLICATE 2

/* Formats current time as HH:MM:SS.mmm into buf. */
static void get_timestamp(char *buf, size_t size) {
    struct timespec ts;
    struct tm       tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &tm_info);

    snprintf(buf, size, "%02d:%02d:%02d.%03ld",
             tm_info.tm_hour,
             tm_info.tm_min,
             tm_info.tm_sec,
             ts.tv_nsec / 1000000L);
}

/* Opens the log file for writing. */
void logger_init(void) {
    log_fp = fopen(LOG_FILE, "w");
    if (log_fp == NULL) {
        fprintf(stderr, RED "ERROR: Cannot open log file %s\n" RESET, LOG_FILE);
    }
}

/* Closes the log file. */
void logger_close(void) {
    if (log_fp != NULL) {
        fclose(log_fp);
        log_fp = NULL;
    }
}

/* Prints a colored banner header to the console. */
void log_banner(const char *title) {
    printf("\n" BOLD_CYAN
           "╔══════════════════════════════════════════╗\n"
           "║  %-40s║\n"
           "╚══════════════════════════════════════════╝\n"
           RESET, title);
}

/* Logs a registration attempt with timestamp, priority and result. Thread-safe. */
void log_attempt(int student_id, int course_id, int priority, int result) {
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    const char *prio_badge   = (priority == HIGH_PRIORITY) ? BADGE_HIGH : BADGE_LOW;
    const char *result_badge;
    const char *result_plain;

    if      (result == RESULT_SUCCESS)  { result_badge = BADGE_OK;   result_plain = "SUCCESS"; }
    else if (result == RESULT_NO_SEATS) { result_badge = BADGE_FAIL; result_plain = "FAILED-No Seats"; }
    else                                { result_badge = BADGE_DUP;  result_plain = "FAILED-Duplicate"; }

    pthread_mutex_lock(&log_mutex);

    printf(CYAN "[%s]" RESET
           " Student " YELLOW "S%03d" RESET
           " │ Course " MAGENTA "CS%d" RESET
           " │ %s │ %s\n",
           timestamp,
           student_id,
           course_id,
           prio_badge,
           result_badge);

    if (log_fp != NULL) {
        fprintf(log_fp, "[%s] Student S%03d | Course CS%d | Priority: %s | Result: %s\n",
                timestamp,
                student_id,
                course_id,
                (priority == HIGH_PRIORITY) ? "HIGH" : "LOW",
                result_plain);
    }

    if (result == RESULT_SUCCESS) total_success++;
    else                          total_fail++;

    pthread_mutex_unlock(&log_mutex);
}

/* Prints final seat allocation table and registration statistics. */
void log_summary(void) {
    log_banner("Final Registration Summary");

    printf(BOLD_CYAN
           "\n  %-6s %-22s %6s %9s %8s\n"
           RESET,
           "Course", "Name", "Total", "Remaining", "Enrolled");

    printf(CYAN "  ──────────────────────────────────────────────────\n" RESET);

    for (int i = 0; i < NUM_COURSES; i++) {
        int remaining = courses[i].available_seats;
        int enrolled  = courses[i].enrolled_count;
        int total     = courses[i].total_seats;

        const char *seat_color = (remaining == 0) ? RED : GREEN;

        printf("  " MAGENTA "CS%-4d" RESET
               " %-22s"
               " %6d"
               " %s%9d" RESET
               " %8d",
               courses[i].course_id,
               courses[i].name,
               total,
               seat_color, remaining,
               enrolled);

        if (remaining == 0)
            printf(BOLD_RED "  [FULL]" RESET);

        printf("\n");
    }

    printf(CYAN "\n  ──────────────────────────────────────────────────\n" RESET);
    printf("  " BOLD_GREEN "Total Successful Registrations : %d\n" RESET, total_success);
    printf("  " BOLD_RED   "Total Failed Registrations     : %d\n" RESET, total_fail);
    printf("  " WHITE       "Total Attempts                 : %d\n" RESET,
           total_success + total_fail);
    printf(CYAN "\n  ══════════════════════════════════════════════════\n\n" RESET);

    if (log_fp != NULL) {
        fprintf(log_fp, "\n=== SUMMARY ===\n");
        fprintf(log_fp, "Total Successful: %d\n", total_success);
        fprintf(log_fp, "Total Failed    : %d\n", total_fail);
        for (int i = 0; i < NUM_COURSES; i++) {
            fprintf(log_fp, "CS%d - %s: %d/%d seats remaining\n",
                    courses[i].course_id,
                    courses[i].name,
                    courses[i].available_seats,
                    courses[i].total_seats);
        }
    }
}