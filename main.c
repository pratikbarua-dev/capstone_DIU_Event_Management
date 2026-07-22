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

// --- File Handling Functions ---
void loadData() {
    FILE *fe = fopen(EVENT_FILE, "r");
    if (fe) {
        char line[512];
        while (fgets(line, sizeof(line), fe)) {
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%d", 
                &events[event_count].id, events[event_count].title, events[event_count].date, 
                events[event_count].time, events[event_count].venue, events[event_count].category, 
                &events[event_count].capacity, &events[event_count].registered, &events[event_count].attended);
            event_count++;
        }
        fclose(fe);
    }
    
    FILE *fr = fopen(REG_FILE, "r");
    if (fr) {
        char line[512];
        while (fgets(line, sizeof(line), fr)) {
            sscanf(line, "%d,%[^,],%d,%[^\n]", 
                &regs[reg_count].event_id, regs[reg_count].participant_name, 
                &regs[reg_count].is_present, regs[reg_count].feedback);
            reg_count++;
        }
        fclose(fr);
    }
}

void saveData() {
    FILE *fe = fopen(EVENT_FILE, "w");
    for (int i = 0; i < event_count; i++) {
        fprintf(fe, "%d,%s,%s,%s,%s,%s,%d,%d,%d\n", events[i].id, events[i].title, events[i].date, 
                events[i].time, events[i].venue, events[i].category, events[i].capacity, 
                events[i].registered, events[i].attended);
    }
    if (fe) fclose(fe);

    FILE *fr = fopen(REG_FILE, "w");
    for (int i = 0; i < reg_count; i++) {
        fprintf(fr, "%d,%s,%d,%s\n", regs[i].event_id, regs[i].participant_name, 
                regs[i].is_present, regs[i].feedback);
    }
    if (fr) fclose(fr);
}

