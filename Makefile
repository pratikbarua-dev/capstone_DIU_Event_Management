CC = gcc
CFLAGS = -Wall -Wextra -O2

CLI_TARGET = main

.PHONY: all clean

all: $(CLI_TARGET)

$(CLI_TARGET): main.c
	$(CC) $(CFLAGS) main.c -o $(CLI_TARGET)

clean:
	rm -f $(CLI_TARGET) a.out *.o events_backup.csv regs_backup.csv
