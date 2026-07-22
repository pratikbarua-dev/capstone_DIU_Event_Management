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

// --- Utility Functions ---
void strip_nl(char *str) { str[strcspn(str, "\n")] = 0; }
int findEventIndex(int id) {
    for (int i = 0; i < event_count; i++) if (events[i].id == id) return i;
    return -1;
}

// FR8: Conflict Detection [cite: 29]
int checkConflict(char* date, char* time, char* venue, int ignore_id) {
    for (int i = 0; i < event_count; i++) {
        if (events[i].id != ignore_id && strcmp(events[i].date, date) == 0 && 
            strcmp(events[i].time, time) == 0 && strcmp(events[i].venue, venue) == 0) {
            return 1; 
        }
    }
    return 0;
}

// --- Core Functionalities ---
// FR6: View Events [cite: 29]
void displayEvents(int filter_mode, char* keyword) {
    printf("\n%-5s | %-15s | %-10s | %-10s | %-15s | %-10s | %s\n", "ID", "Title", "Date", "Time", "Venue", "Category", "Slots");
    printf("--------------------------------------------------------------------------------------\n");
    for (int i = 0; i < event_count; i++) {
        int match = 1;
        if (filter_mode == 1) match = (strstr(events[i].title, keyword) || strstr(events[i].date, keyword)); // FR10: Search [cite: 30]
        if (filter_mode == 2) match = (strcmp(events[i].category, keyword) == 0 || strcmp(events[i].venue, keyword) == 0); // FR14: Filter [cite: 30]
        
        if (match) {
            printf("%-5d | %-15s | %-10s | %-10s | %-15s | %-10s | %d/%d\n", 
                   events[i].id, events[i].title, events[i].date, events[i].time, 
                   events[i].venue, events[i].category, events[i].capacity - events[i].registered, events[i].capacity);
        }
    }
}

// FR3: Add Event [cite: 29]
void addEvent() {
    Event e;
    e.id = (event_count > 0) ? events[event_count-1].id + 1 : 101;
    printf("Title: "); fgets(e.title, MAX_STR, stdin); strip_nl(e.title);
    printf("Date (DD/MM/YY): "); fgets(e.date, MAX_STR, stdin); strip_nl(e.date);
    printf("Time (HH:MM): "); fgets(e.time, MAX_STR, stdin); strip_nl(e.time);
    printf("Venue: "); fgets(e.venue, MAX_STR, stdin); strip_nl(e.venue);
    printf("Category: "); fgets(e.category, MAX_STR, stdin); strip_nl(e.category);
    printf("Capacity: "); scanf("%d", &e.capacity); getchar();
    
    if (checkConflict(e.date, e.time, e.venue, -1)) {
        printf("ERROR: Venue conflict detected!\n"); return;
    }
    e.registered = 0; e.attended = 0;
    events[event_count++] = e;
    saveData();
    printf("Event added successfully! (ID: %d)\n", e.id);
}

// FR4: Update Event [cite: 29]
void updateEvent() {
    int id, idx;
    printf("Enter Event ID to update: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }
    
    printf("New Title (or press enter to skip): "); char buffer[MAX_STR]; fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(events[idx].title, buffer);
    
    printf("New Time (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) {
        if(checkConflict(events[idx].date, buffer, events[idx].venue, id)) { printf("Conflict! Time not updated.\n"); } 
        else { strcpy(events[idx].time, buffer); }
    }
    saveData();
    printf("Event updated.\n");
}

// FR5: Delete Event [cite: 29]
void deleteEvent() {
    int id, idx;
    printf("Enter Event ID to delete: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }
    
    for (int i = idx; i < event_count - 1; i++) events[i] = events[i+1];
    event_count--;
    saveData();
    printf("Event deleted.\n");
}

