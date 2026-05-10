#pragma once


#include <pthread.h>
#include <time.h>
#include "config.h"

typedef struct {
    int             course_id;
    char            name[64];
    int             total_seats;
    int             available_seats;
    int             enrolled_count;
    pthread_mutex_t lock;
} Course;

typedef struct {
    int  student_id;
    int  priority;
    int  num_requested;
    int  requested_courses[MAX_COURSES_PER_STUDENT];
    int  registered[NUM_COURSES];
    int  success_count;
    int  fail_count;
} Student;

extern Course         courses[NUM_COURSES];
extern pthread_mutex_t log_mutex;
extern int            total_success;
extern int            total_fail;
extern FILE          *log_fp;

