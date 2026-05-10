#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "structs.h"
#include "config.h"
#include "colors.h"
#include "logger.h"
#include "registration.h"

Course          courses[NUM_COURSES];
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
int             total_success = 0;
int             total_fail    = 0;
FILE           *log_fp        = NULL;

/* Initializes 8 courses with names, seat counts and per-course mutexes. */
static void init_courses(void) {
    struct { int id; char name[64]; int seats; } data[NUM_COURSES] = {
        { 101, "Data Structures",         30 },
        { 102, "Algorithms",              25 },
        { 103, "Operating Systems",       20 },
        { 104, "Database Systems",        35 },
        { 105, "Computer Networks",       15 },
        { 106, "Software Engineering",    40 },
        { 107, "Artificial Intelligence", 10 },
        { 108, "Computer Architecture",   20 },
    };

    for (int i = 0; i < NUM_COURSES; i++) {
        courses[i].course_id       = data[i].id;
        strncpy(courses[i].name, data[i].name, 63);
        courses[i].total_seats     = data[i].seats;
        courses[i].available_seats = data[i].seats;
        courses[i].enrolled_count  = 0;

        if (pthread_mutex_init(&courses[i].lock, NULL) != 0) {
            fprintf(stderr, RED "FATAL: mutex init failed for course %d\n" RESET, data[i].id);
            exit(EXIT_FAILURE);
        }
    }

    printf(GREEN "✓ " RESET "%d courses initialized\n", NUM_COURSES);
}

/* Initializes 3 courses for the mandatory spec demo scenario. */
static void init_courses_demo(void) {
    struct { int id; char name[64]; int seats; } data[3] = {
        { 101, "Data Structures",   2 },
        { 102, "Algorithms",        1 },
        { 103, "Operating Systems", 3 },
    };
    for (int i = 0; i < 3; i++) {
        courses[i].course_id       = data[i].id;
        strncpy(courses[i].name, data[i].name, 63);
        courses[i].total_seats     = data[i].seats;
        courses[i].available_seats = data[i].seats;
        courses[i].enrolled_count  = 0;
        pthread_mutex_init(&courses[i].lock, NULL);
    }
    printf(GREEN "✓ " RESET "Demo courses initialized (3 courses, spec scenario)\n");
}

/* Generates student array with random course requests and assigned priorities. */
static void init_students(Student *students, int num_students,
                           int num_courses, int high_count) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < num_students; i++) {
        students[i].student_id    = i + 1;
        students[i].success_count = 0;
        students[i].fail_count    = 0;
        memset(students[i].registered, 0, sizeof(students[i].registered));

        students[i].priority = (i < high_count) ? HIGH_PRIORITY : LOW_PRIORITY;

        int num_req = MIN_COURSES_PER_STUDENT +
                      (rand() % (MAX_COURSES_PER_STUDENT - MIN_COURSES_PER_STUDENT + 1));
        students[i].num_requested = num_req;

        int picked = 0;
        while (picked < num_req) {
            int course_idx = rand() % num_courses;
            int already    = 0;

            for (int j = 0; j < picked; j++) {
                if (students[i].requested_courses[j] == course_idx) {
                    already = 1;
                    break;
                }
            }

            if (!already) {
                students[i].requested_courses[picked] = course_idx;
                picked++;
            }
        }
    }

    int low_count = num_students - high_count;
    printf(GREEN "✓ " RESET "%d students generated "
           "(" YELLOW "%d HIGH" RESET " / " BLUE "%d LOW" RESET " priority)\n",
           num_students, high_count, low_count);
}

/* Destroys all course mutexes and the log mutex after threads finish. */
static void destroy_mutexes(int num_courses) {
    for (int i = 0; i < num_courses; i++) {
        pthread_mutex_destroy(&courses[i].lock);
    }
    pthread_mutex_destroy(&log_mutex);
}

/* Entry point — initializes system, spawns student threads, joins them, prints summary. */
int main(int argc, char *argv[]) {
    int demo = (argc > 1 && strcmp(argv[1], "demo") == 0);

    int num_students  = demo ? DEMO_NUM_STUDENTS  : NUM_STUDENTS;
    int num_courses_n = demo ? DEMO_NUM_COURSES   : NUM_COURSES;
    int high_count    = demo ? DEMO_HIGH_PRIORITY : HIGH_PRIORITY_COUNT;

    log_banner(demo
        ? "Demo Mode — Spec Scenario (Section 13)"
        : "University Course Registration System");

    printf("\n" CYAN "  Mode     : " RESET "%s\n",
           demo ? "DEMO (10 students, 3 courses)" : "FULL (100 students, 8 courses)");
    printf(CYAN "  Threads  : " RESET "%d student threads\n", num_students);
    printf(CYAN "  Courses  : " RESET "%d courses\n\n", num_courses_n);

    if (demo) init_courses_demo();
    else      init_courses();

    Student students[num_students];
    init_students(students, num_students, num_courses_n, high_count);

    logger_init();
    printf(GREEN "✓ " RESET "Log file opened: %s\n", LOG_FILE);
    printf("\n" CYAN "  Spawning %d threads...\n\n" RESET, num_students);

    pthread_t threads[num_students];

    for (int i = 0; i < num_students; i++) {
        int rc = pthread_create(&threads[i], NULL, student_thread, &students[i]);
        if (rc != 0) {
            fprintf(stderr, RED "ERROR: pthread_create failed for student %d\n" RESET, i + 1);
        }
    }

    for (int i = 0; i < num_students; i++) {
        pthread_join(threads[i], NULL);
    }

    printf(GREEN "\n✓ All %d threads completed.\n" RESET, num_students);

    log_summary();

    logger_close();
    destroy_mutexes(num_courses_n);

    return EXIT_SUCCESS;
}