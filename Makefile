CC = gcc
CFLAGS = -Wall -Wextra -O2
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null)

CLI_TARGET = main
GUI_TARGET = event_gui

.PHONY: all cli gui clean

all: cli gui

cli: main.c
	$(CC) $(CFLAGS) main.c -o $(CLI_TARGET)

gui: gui.c
	@if pkg-config --exists gtk+-3.0; then \
		$(CC) $(CFLAGS) gui.c -o $(GUI_TARGET) $(GTK_CFLAGS) $(GTK_LIBS); \
		echo "GUI built successfully: ./event_gui"; \
	else \
		echo "GTK+-3.0 not installed. Skipping GUI build."; \
	fi

clean:
	rm -f $(CLI_TARGET) $(GUI_TARGET) a.out *.o events_backup.csv regs_backup.csv
