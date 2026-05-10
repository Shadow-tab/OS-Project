CC      = gcc
TARGET  = registration
SRCS    = main.c registration.c priority.c logger.c
OBJS    = $(SRCS:.c=.o)
CFLAGS  = -Wall -Wextra -pthread -g -std=c11 -D_GNU_SOURCE

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "\033[0;32m✓ Build successful → ./$(TARGET)\033[0m"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

demo: all
	./$(TARGET) demo

clean:
	rm -f $(OBJS) $(TARGET) *.log
	@echo "\033[0;32m✓ Clean complete\033[0m"

.PHONY: all run demo clean