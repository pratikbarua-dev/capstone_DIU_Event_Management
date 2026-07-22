#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EVENT_FILE "events.csv"
#define REG_FILE "registrations.csv"
#define MAX_STR 100
#define MAX_EVENTS 200
#define MAX_REGS 1000

// --- Data Structures ---
typedef struct {
    int id;
    char title[MAX_STR];
    char date[MAX_STR];
    char time[MAX_STR];
    char venue[MAX_STR];
    char category[MAX_STR];
    int capacity;
    int registered;
    int attended;
} Event;

typedef struct {
    int event_id;
    char participant_name[MAX_STR];
    int is_present; // 0=No, 1=Yes
    char feedback[MAX_STR];
} Registration;

// --- Global Memory ---
Event events[MAX_EVENTS];
int event_count = 0;
Registration regs[MAX_REGS];
int reg_count = 0;

