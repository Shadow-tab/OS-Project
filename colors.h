#pragma once

#define RESET       "\033[0m"

#define RED         "\033[0;31m"
#define GREEN       "\033[0;32m"
#define YELLOW      "\033[0;33m"
#define BLUE        "\033[0;34m"
#define MAGENTA     "\033[0;35m"
#define CYAN        "\033[0;36m"
#define WHITE       "\033[0;37m"

#define BOLD_RED    "\033[1;31m"
#define BOLD_GREEN  "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_CYAN   "\033[1;36m"
#define BOLD_WHITE  "\033[1;37m"

#define BADGE_HIGH  BOLD_YELLOW "[HIGH]" RESET
#define BADGE_LOW   BLUE        "[LOW] " RESET

#define BADGE_OK    BOLD_GREEN  "✓ SUCCESS         " RESET
#define BADGE_FAIL  BOLD_RED    "✗ FAILED-No Seats " RESET
#define BADGE_DUP   BOLD_RED    "✗ FAILED-Duplicate" RESET
