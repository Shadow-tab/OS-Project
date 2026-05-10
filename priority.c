#include <unistd.h>
#include <stdlib.h>
#include "priority.h"
#include "config.h"

/* Delays low-priority threads to give high-priority students a head start. */
void apply_priority_delay(int priority, int student_id) {
    if (priority == HIGH_PRIORITY) {
        return;
    }

    unsigned int seed = (unsigned int)student_id;

    int delay_range = LOW_PRIORITY_DELAY_MAX - LOW_PRIORITY_DELAY_MIN;
    int delay_us    = LOW_PRIORITY_DELAY_MIN + (rand_r(&seed) % delay_range);

    usleep((useconds_t)delay_us);
}