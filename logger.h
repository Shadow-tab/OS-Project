#pragma once

#include "structs.h"

void log_attempt(int student_id, int course_id, int priority, int result);
void log_banner(const char *title);
void log_summary(void);
void logger_init(void);
void logger_close(void);

