#University Course Registration System

An OS lab project simulating a university course registration system using POSIX threads in C. Students are modeled as concurrent threads competing for limited course seats, protected using mutex synchronization.

---

## Project Structure

```
concurrent-registration-sim/
├── main.c            Entry point, initialization, thread spawning, summary
├── registration.c    Core registration logic and critical section
├── registration.h
├── priority.c        Priority delay logic
├── priority.h
├── logger.c          Thread-safe console and file logging
├── logger.h
├── structs.h         Course and Student struct definitions
├── config.h          All configurable constants
├── colors.h          ANSI terminal color macros
├── Makefile
└── README.md
```

---

## OS Concepts Implemented

- POSIX Threads (pthreads) — one thread per student
- Mutex locks — per-course mutex protecting seat updates
- Critical section — atomic seat check and decrement
- Priority handling — controlled delay for low-priority threads
- Deadlock prevention — single lock acquired at a time
- Starvation avoidance — one-time delay, not a permanent block

---

## Configuration

All simulation parameters are in `config.h`:

| Constant | Default | Description |
|---|---|---|
| `NUM_STUDENTS` | 100 | Total concurrent student threads |
| `NUM_COURSES` | 8 | Number of courses |
| `HIGH_PRIORITY_COUNT` | 30 | Number of high-priority students |
| `MIN_COURSES_PER_STUDENT` | 1 | Minimum courses a student requests |
| `MAX_COURSES_PER_STUDENT` | 3 | Maximum courses a student requests |
| `LOW_PRIORITY_DELAY_MIN` | 50000 | Min delay for low-priority threads (microseconds) |
| `LOW_PRIORITY_DELAY_MAX` | 200000 | Max delay for low-priority threads (microseconds) |

---

## Build

Requirements: `gcc`, `make`, Linux

```bash
make
```

Clean build artifacts:

```bash
make clean
```

---

## Running

Full simulation (100 students, 8 courses):

```bash
./registration
```

Mandatory demo scenario from spec section 13 (10 students, 3 courses, CS101:2 seats, CS102:1 seat, CS103:3 seats):

```bash
./registration demo
```

---

## Output

Each registration attempt is logged to the console with color and to `registration.log` in plain text:

```
[HH:MM:SS.mmm] Student S001 | Course CS101 | [HIGH] | SUCCESS
[HH:MM:SS.mmm] Student S047 | Course CS101 | [LOW]  | FAILED-No Seats
```

At program termination a summary table is printed showing remaining seats per course and total successful and failed registrations.

---

## Deadlock Prevention

Each student thread acquires only one course mutex at a time, performs the seat check and update, then releases the lock before moving to the next course. A thread never holds two locks simultaneously, making circular wait impossible.

---

## Log File

`registration.log` is generated on every run in plain text without color codes. It is excluded from version control via `.gitignore`.
