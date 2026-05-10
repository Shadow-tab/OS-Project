#include <stdio.h>
#include "registration.h"
#include "priority.h"
#include "logger.h"
#include "structs.h"
#include "config.h"

#define RESULT_SUCCESS   0
#define RESULT_NO_SEATS  1
#define RESULT_DUPLICATE 2

/* Attempts to register a student for a course using mutex-protected critical section. */
int attempt_registration(Student *student, int course_index) {
    if (student->registered[course_index]) {
        return RESULT_DUPLICATE;
    }

    pthread_mutex_lock(&courses[course_index].lock);

    int result;

    if (courses[course_index].available_seats > 0) {
        courses[course_index].available_seats--;
        courses[course_index].enrolled_count++;
        student->registered[course_index] = 1;
        result = RESULT_SUCCESS;
    } else {
        result = RESULT_NO_SEATS;
    }

    pthread_mutex_unlock(&courses[course_index].lock);

    return result;
}

/* Thread function executed by each student — applies priority delay then attempts registration. */
void *student_thread(void *arg) {
    Student *student = (Student *)arg;

    apply_priority_delay(student->priority, student->student_id);

    for (int i = 0; i < student->num_requested; i++) {
        int course_idx = student->requested_courses[i];

        int result = attempt_registration(student, course_idx);

        log_attempt(student->student_id,
                    courses[course_idx].course_id,
                    student->priority,
                    result);

        if (result == RESULT_SUCCESS) student->success_count++;
        else                          student->fail_count++;
    }

    return NULL;
}