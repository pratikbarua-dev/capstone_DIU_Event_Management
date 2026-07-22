#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EVENT_FILE "events.csv"
#define REG_FILE "registrations.csv"
#define VENUE_FILE "venues.csv"
#define AUDIT_FILE "system_audit.log"
#define REPORT_EXPORT_FILE "event_summary_report.txt"

#define MAX_STR 100
#define MAX_EVENTS 200
#define MAX_REGS 1000
#define MAX_VENUES 50

// ============================================================================
// --- Implemented by Pratik Barua: Core Data Structures ---
// ============================================================================
typedef struct {
    int id;
    char title[MAX_STR];
    char description[MAX_STR];
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

// ============================================================================
// --- Implemented by Umme Habiba Ayshi: Venue Allocation Subsystem ---
// ============================================================================
typedef struct {
    int id;
    char name[MAX_STR];
    char location[MAX_STR];
    int max_capacity;
    char facilities[MAX_STR];
} Venue;

// --- Global Memory ---
Event events[MAX_EVENTS];
int event_count = 0;
Registration regs[MAX_REGS];
int reg_count = 0;
Venue venues[MAX_VENUES];
int venue_count = 0;

// --- Utility Functions ---
void strip_nl(char *str) { str[strcspn(str, "\n")] = 0; }

int findEventIndex(int id) {
    for (int i = 0; i < event_count; i++) if (events[i].id == id) return i;
    return -1;
}

// ============================================================================
// --- Implemented by Moshiur Rahman: System Audit Logger ---
// ============================================================================
void logAuditEvent(const char* action, const char* details) {
    FILE *fa = fopen(AUDIT_FILE, "a");
    if (fa) {
        time_t now = time(NULL);
        char time_str[64];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(fa, "[%s] ACTION: %-25s | DETAILS: %s\n", time_str, action, details);
        fclose(fa);
    }
}

void viewAuditLogs() {
    FILE *fa = fopen(AUDIT_FILE, "r");
    printf("\n=== SYSTEM AUDIT LOGS ===\n");
    if (!fa) {
        printf("No audit logs recorded yet.\n");
        return;
    }
    char line[512];
    while (fgets(line, sizeof(line), fa)) {
        printf("%s", line);
    }
    fclose(fa);
}

// ============================================================================
// --- Implemented by Umme Habiba Ayshi: Venue Data Management ---
// ============================================================================
void loadVenues() {
    venue_count = 0;
    FILE *fv = fopen(VENUE_FILE, "r");
    if (fv) {
        char line[512];
        while (fgets(line, sizeof(line), fv)) {
            sscanf(line, "%d,%[^,],%[^,],%d,%[^\n]", 
                &venues[venue_count].id, venues[venue_count].name, 
                venues[venue_count].location, &venues[venue_count].max_capacity, 
                venues[venue_count].facilities);
            venue_count++;
        }
        fclose(fv);
    }
}

void saveVenues() {
    FILE *fv = fopen(VENUE_FILE, "w");
    if (fv) {
        for (int i = 0; i < venue_count; i++) {
            fprintf(fv, "%d,%s,%s,%d,%s\n", venues[i].id, venues[i].name, 
                    venues[i].location, venues[i].max_capacity, venues[i].facilities);
        }
        fclose(fv);
    }
}

void displayVenues() {
    printf("\n=== DIU VENUE DIRECTORY & ALLOCATION ===\n");
    printf("%-5s | %-22s | %-20s | %-12s | %s\n", "ID", "Venue Name", "Location", "Capacity Cap", "Facilities");
    printf("-----------------------------------------------------------------------------------------\n");
    for (int i = 0; i < venue_count; i++) {
        printf("%-5d | %-22s | %-20s | %-12d | %s\n", 
               venues[i].id, venues[i].name, venues[i].location, venues[i].max_capacity, venues[i].facilities);
    }
}

void addVenue() {
    Venue v;
    v.id = (venue_count > 0) ? venues[venue_count-1].id + 1 : 201;
    printf("Enter Venue Name: "); fgets(v.name, MAX_STR, stdin); strip_nl(v.name);
    printf("Enter Location (Campus/Building): "); fgets(v.location, MAX_STR, stdin); strip_nl(v.location);
    printf("Enter Max Capacity Cap: "); scanf("%d", &v.max_capacity); getchar();
    printf("Enter Facilities (e.g., AC, Projector, Sound): "); fgets(v.facilities, MAX_STR, stdin); strip_nl(v.facilities);
    
    venues[venue_count++] = v;
    saveVenues();
    printf("Venue added successfully to DIU directory! (ID: %d)\n", v.id);
    logAuditEvent("ADD_VENUE", v.name);
}

char* getOccupancyIndicator(int registered, int capacity) {
    if (capacity <= 0) return "[N/A]";
    float ratio = (float)registered / capacity;
    if (ratio >= 1.0f) return "[FULL]";
    if (ratio >= 0.8f) return "[FEW SLOTS LEFT]";
    return "[AVAILABLE]";
}

// ============================================================================
// --- Implemented by Pratik Barua: File Handling & Core Operations ---
// ============================================================================
void loadData() {
    event_count = 0; reg_count = 0;
    FILE *fe = fopen(EVENT_FILE, "r");
    if (fe) {
        char line[512];
        while (fgets(line, sizeof(line), fe)) {
            sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%d,%d,%d", 
                &events[event_count].id, events[event_count].title, events[event_count].description,
                events[event_count].date, events[event_count].time, events[event_count].venue, 
                events[event_count].category, &events[event_count].capacity, 
                &events[event_count].registered, &events[event_count].attended);
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
    loadVenues();
}

void saveData() {
    FILE *fe = fopen(EVENT_FILE, "w");
    for (int i = 0; i < event_count; i++) {
        fprintf(fe, "%d,%s,%s,%s,%s,%s,%s,%d,%d,%d\n", events[i].id, events[i].title, events[i].description,
                events[i].date, events[i].time, events[i].venue, events[i].category, 
                events[i].capacity, events[i].registered, events[i].attended);
    }
    if (fe) fclose(fe);

    FILE *fr = fopen(REG_FILE, "w");
    for (int i = 0; i < reg_count; i++) {
        fprintf(fr, "%d,%s,%d,%s\n", regs[i].event_id, regs[i].participant_name, 
                regs[i].is_present, regs[i].feedback);
    }
    if (fr) fclose(fr);
}

// ============================================================================
// --- Implemented by Moshiur Rahman: Backup & Recovery Subsystem ---
// ============================================================================
void manageBackups(int restore) {
    if (restore) {
        rename("events_backup.csv", EVENT_FILE);
        rename("regs_backup.csv", REG_FILE);
        event_count = 0; reg_count = 0;
        loadData();
        printf("Data restored from backup successfully.\n");
        logAuditEvent("RESTORE_DATA", "Restored events and registrations from backup");
    } else {
        saveData();
        FILE *src1 = fopen(EVENT_FILE, "r"), *dst1 = fopen("events_backup.csv", "w");
        FILE *src2 = fopen(REG_FILE, "r"), *dst2 = fopen("regs_backup.csv", "w");
        char ch;
        if (src1 && dst1) while ((ch = fgetc(src1)) != EOF) fputc(ch, dst1);
        if (src2 && dst2) while ((ch = fgetc(src2)) != EOF) fputc(ch, dst2);
        if(src1) fclose(src1); 
        if(dst1) fclose(dst1);
        if(src2) fclose(src2); 
        if(dst2) fclose(dst2);
        printf("Data backup created successfully.\n");
        logAuditEvent("BACKUP_DATA", "Created events_backup.csv and regs_backup.csv");
    }
}

// ============================================================================
// --- Implemented by Moshiur Rahman: Report Generation & Export ---
// ============================================================================
void exportReportToFile() {
    FILE *fr = fopen(REPORT_EXPORT_FILE, "w");
    if (!fr) {
        printf("ERROR: Unable to create report export file.\n");
        return;
    }

    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(fr, "=======================================================================\n");
    fprintf(fr, " DAFFODIL INTERNATIONAL UNIVERSITY (DIU) - EVENT SUMMARY REPORT\n");
    fprintf(fr, " Generated On: %s\n", time_str);
    fprintf(fr, "=======================================================================\n\n");

    for (int i = 0; i < event_count; i++) {
        float att_rate = (events[i].registered > 0) ? ((float)events[i].attended / events[i].registered) * 100.0f : 0.0f;
        fprintf(fr, "Event ID: %d | Title: %s\n", events[i].id, events[i].title);
        fprintf(fr, "  Description : %s\n", events[i].description);
        fprintf(fr, "  Venue/Timing: %s (%s at %s)\n", events[i].venue, events[i].date, events[i].time);
        fprintf(fr, "  Statistics  : Capacity: %d | Registered: %d | Attended: %d (%.1f%% Attendance Rate)\n",
                events[i].capacity, events[i].registered, events[i].attended, att_rate);
        fprintf(fr, "  Participants:\n");
        
        int found = 0;
        for (int j = 0; j < reg_count; j++) {
            if (regs[j].event_id == events[i].id) {
                fprintf(fr, "    - %-25s | Status: %-7s | Feedback: %s\n", 
                        regs[j].participant_name, regs[j].is_present ? "PRESENT" : "ABSENT", regs[j].feedback);
                found = 1;
            }
        }
        if (!found) fprintf(fr, "    - None registered yet.\n");
        fprintf(fr, "-----------------------------------------------------------------------\n\n");
    }
    fclose(fr);
    printf("Report successfully exported to '%s'!\n", REPORT_EXPORT_FILE);
    logAuditEvent("EXPORT_REPORT", "Generated file event_summary_report.txt");
}

void generateReports() {
    printf("\n--- SUMMARY REPORT ---\n");
    for (int i = 0; i < event_count; i++) {
        float att_rate = (events[i].registered > 0) ? ((float)events[i].attended / events[i].registered) * 100.0f : 0.0f;
        printf("Event: %s | Cap: %d | Reg: %d | Attended: %d (%.1f%% Attended)\n", 
               events[i].title, events[i].capacity, events[i].registered, events[i].attended, att_rate);
        printf("  Participants: ");
        int found = 0;
        for (int j = 0; j < reg_count; j++) {
            if (regs[j].event_id == events[i].id) {
                printf("%s (%s), ", regs[j].participant_name, regs[j].is_present ? "Present" : "Absent");
                found = 1;
            }
        }
        if(!found) printf("None");
        printf("\n");
    }
    logAuditEvent("GENERATE_REPORT", "Viewed summary reports in console");
}

// ============================================================================
// --- Implemented by Pratik Barua: Conflict Detection Engine ---
// ============================================================================
int checkConflict(char* date, char* time, char* venue, int ignore_id) {
    for (int i = 0; i < event_count; i++) {
        if (events[i].id != ignore_id && strcmp(events[i].date, date) == 0 && 
            strcmp(events[i].time, time) == 0 && strcmp(events[i].venue, venue) == 0) {
            return 1; 
        }
    }
    return 0;
}

// ============================================================================
// --- Implemented by Pratik Barua: Core Functionalities (CRUD, Search, Filter) ---
// ============================================================================
void displayEvents(int filter_mode, char* keyword) {
    printf("\n%-5s | %-15s | %-20s | %-10s | %-8s | %-12s | %-10s | %-8s | %s\n", 
           "ID", "Title", "Description", "Date", "Time", "Venue", "Category", "Slots", "Status");
    printf("------------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < event_count; i++) {
        int match = 1;
        if (filter_mode == 1) match = (strstr(events[i].title, keyword) || strstr(events[i].date, keyword) || strstr(events[i].description, keyword));
        if (filter_mode == 2) match = (strcmp(events[i].category, keyword) == 0 || strcmp(events[i].venue, keyword) == 0);
        
        if (match) {
            char slots[32];
            sprintf(slots, "%d/%d", events[i].capacity - events[i].registered, events[i].capacity);
            printf("%-5d | %-15s | %-20s | %-10s | %-8s | %-12s | %-10s | %-8s | %s\n", 
                   events[i].id, events[i].title, events[i].description, events[i].date, events[i].time, 
                   events[i].venue, events[i].category, slots, getOccupancyIndicator(events[i].registered, events[i].capacity));
        }
    }
}

void addEvent() {
    Event e;
    e.id = (event_count > 0) ? events[event_count-1].id + 1 : 101;
    printf("Title: "); fgets(e.title, MAX_STR, stdin); strip_nl(e.title);
    printf("Description: "); fgets(e.description, MAX_STR, stdin); strip_nl(e.description);
    printf("Date (DD/MM/YY): "); fgets(e.date, MAX_STR, stdin); strip_nl(e.date);
    printf("Time (HH:MM): "); fgets(e.time, MAX_STR, stdin); strip_nl(e.time);
    
    // Display venue directory to assist allocation
    if (venue_count > 0) {
        printf("\nAvailable Venues: ");
        for(int k=0; k<venue_count; k++) printf("[%s (Cap: %d)] ", venues[k].name, venues[k].max_capacity);
        printf("\n");
    }
    printf("Venue: "); fgets(e.venue, MAX_STR, stdin); strip_nl(e.venue);
    printf("Category: "); fgets(e.category, MAX_STR, stdin); strip_nl(e.category);
    printf("Capacity: "); scanf("%d", &e.capacity); getchar();
    
    if (checkConflict(e.date, e.time, e.venue, -1)) {
        printf("ERROR: Venue/Time conflict detected!\n"); 
        logAuditEvent("ADD_EVENT_FAILED", "Conflict detected");
        return;
    }
    e.registered = 0; e.attended = 0;
    events[event_count++] = e;
    saveData();
    printf("Event added successfully! (ID: %d)\n", e.id);
    logAuditEvent("ADD_EVENT", e.title);
}

void updateEvent() {
    int id, idx;
    printf("Enter Event ID to update: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }
    
    char buffer[MAX_STR];
    printf("New Title (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(events[idx].title, buffer);
    
    printf("New Description (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(events[idx].description, buffer);

    char new_date[MAX_STR], new_time[MAX_STR], new_venue[MAX_STR];
    strcpy(new_date, events[idx].date);
    strcpy(new_time, events[idx].time);
    strcpy(new_venue, events[idx].venue);

    printf("New Date (DD/MM/YY) (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(new_date, buffer);

    printf("New Time (HH:MM) (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(new_time, buffer);

    printf("New Venue (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(new_venue, buffer);

    if(checkConflict(new_date, new_time, new_venue, id)) {
        printf("ERROR: Venue/Time conflict detected! Schedule details reverted.\n");
    } else {
        strcpy(events[idx].date, new_date);
        strcpy(events[idx].time, new_time);
        strcpy(events[idx].venue, new_venue);
    }

    printf("New Category (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) strcpy(events[idx].category, buffer);

    printf("New Capacity (or press enter to skip): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
    if(strlen(buffer) > 0) {
        int cap = atoi(buffer);
        if (cap > 0) {
            if (cap < events[idx].registered) {
                printf("WARNING: New capacity (%d) is lower than current registrations (%d)!\n", cap, events[idx].registered);
            }
            events[idx].capacity = cap;
        }
    }

    saveData();
    printf("Event updated successfully.\n");
    logAuditEvent("UPDATE_EVENT", events[idx].title);
}

void deleteEvent() {
    int id, idx;
    printf("Enter Event ID to delete: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }
    
    char del_title[MAX_STR];
    strcpy(del_title, events[idx].title);
    for (int i = idx; i < event_count - 1; i++) events[i] = events[i+1];
    event_count--;
    saveData();
    printf("Event deleted successfully.\n");
    logAuditEvent("DELETE_EVENT", del_title);
}

// ============================================================================
// --- Implemented by Pratik Barua: Participant Registration & Feedback ---
// ============================================================================
void registerParticipant() {
    int id, idx; char name[MAX_STR];
    printf("Enter Event ID: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }
    if (events[idx].registered >= events[idx].capacity) { printf("ERROR: Event is at full capacity.\n"); return; }
    
    printf("Enter Your Name: "); fgets(name, MAX_STR, stdin); strip_nl(name);
    regs[reg_count].event_id = id;
    strcpy(regs[reg_count].participant_name, name);
    regs[reg_count].is_present = 0;
    strcpy(regs[reg_count].feedback, "N/A");
    reg_count++;
    events[idx].registered++;
    saveData();
    printf("Registration Successful! Confirmation generated.\n");
    logAuditEvent("PARTICIPANT_REGISTERED", name);
}

void cancelRegistration() {
    int id; char name[MAX_STR];
    printf("Enter Event ID: "); scanf("%d", &id); getchar();
    printf("Enter Your Name: "); fgets(name, MAX_STR, stdin); strip_nl(name);
    
    for (int i = 0; i < reg_count; i++) {
        if (regs[i].event_id == id && strcmp(regs[i].participant_name, name) == 0) {
            for (int j = i; j < reg_count - 1; j++) regs[j] = regs[j+1];
            reg_count--;
            int idx = findEventIndex(id);
            if(idx != -1) events[idx].registered--;
            saveData();
            printf("Registration cancelled successfully.\n");
            logAuditEvent("CANCEL_REGISTRATION", name);
            return;
        }
    }
    printf("Registration record not found.\n");
}

void leaveFeedback() {
    int id; char name[MAX_STR];
    printf("Enter Event ID: "); scanf("%d", &id); getchar();
    printf("Enter Your Name: "); fgets(name, MAX_STR, stdin); strip_nl(name);
    
    for (int i = 0; i < reg_count; i++) {
        if (regs[i].event_id == id && strcmp(regs[i].participant_name, name) == 0) {
            printf("Enter your feedback: ");
            fgets(regs[i].feedback, MAX_STR, stdin); strip_nl(regs[i].feedback);
            saveData();
            printf("Feedback saved. Thank you!\n");
            logAuditEvent("LEAVE_FEEDBACK", name);
            return;
        }
    }
    printf("Registration record not found.\n");
}

void markAttendance() {
    int id; char name[MAX_STR];
    printf("Enter Event ID: "); scanf("%d", &id); getchar();
    printf("Enter Participant Name: "); fgets(name, MAX_STR, stdin); strip_nl(name);
    
    for (int i = 0; i < reg_count; i++) {
        if (regs[i].event_id == id && strcmp(regs[i].participant_name, name) == 0) {
            if (!regs[i].is_present) {
                regs[i].is_present = 1;
                int idx = findEventIndex(id);
                if(idx != -1) events[idx].attended++;
                saveData();
                printf("Participant marked as PRESENT.\n");
                logAuditEvent("MARK_ATTENDANCE", name);
            } else {
                printf("Participant already marked present.\n");
            }
            return;
        }
    }
    printf("Participant not found.\n");
}

void viewParticipantList() {
    int id, idx;
    printf("Enter Event ID to view participants: "); scanf("%d", &id); getchar();
    if ((idx = findEventIndex(id)) == -1) { printf("Event not found.\n"); return; }

    printf("\n=======================================================\n");
    printf(" Participant List for Event: %s (ID: %d)\n", events[idx].title, events[idx].id);
    printf(" Description: %s\n", events[idx].description);
    printf(" Venue: %s | Date: %s | Time: %s\n", events[idx].venue, events[idx].date, events[idx].time);
    printf(" Total Registrations: %d / Capacity: %d\n", events[idx].registered, events[idx].capacity);
    printf("=======================================================\n");
    printf("%-5s | %-25s | %-10s | %s\n", "#", "Participant Name", "Attendance", "Feedback");
    printf("-------------------------------------------------------\n");
    
    int count = 0;
    for (int i = 0; i < reg_count; i++) {
        if (regs[i].event_id == id) {
            count++;
            printf("%-5d | %-25s | %-10s | %s\n", count, regs[i].participant_name, 
                   regs[i].is_present ? "PRESENT" : "ABSENT", regs[i].feedback);
        }
    }
    if (count == 0) printf("No registered participants found for this event.\n");
    printf("-------------------------------------------------------\n");
    logAuditEvent("VIEW_PARTICIPANT_LIST", events[idx].title);
}

// ============================================================================
// --- Menus & Subsystem Interfaces ---
// ============================================================================

// --- Implemented by Pratik Barua (Core Organizers Interface) & Extended by Moshiur & Ayshi ---
void organizerMenu() {
    int choice; char buffer[MAX_STR];
    while(1) {
        printf("\n=== ORGANIZER DASHBOARD ===\n");
        printf("1. Add Event       2. Update Event       3. Delete Event\n");
        printf("4. View All        5. Search/Filter      6. Mark Attendance\n");
        printf("7. View Reports    8. Export Report File 9. View Venues\n");
        printf("10. Add Venue      11. Backup Data       12. Restore Data\n");
        printf("13. Audit Logs     14. Participant List  0. Logout\nChoice: ");
        scanf("%d", &choice); getchar();
        switch(choice) {
            case 1: addEvent(); break;
            case 2: updateEvent(); break;
            case 3: deleteEvent(); break;
            case 4: displayEvents(0, ""); break;
            case 5: 
                printf("Filter by (1) Title/Date/Desc or (2) Category/Venue? ");
                int fm; scanf("%d", &fm); getchar();
                printf("Enter keyword: "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
                displayEvents(fm, buffer); break;
            case 6: markAttendance(); break;
            case 7: generateReports(); break;
            case 8: exportReportToFile(); break;
            case 9: displayVenues(); break;
            case 10: addVenue(); break;
            case 11: manageBackups(0); break;
            case 12: manageBackups(1); break;
            case 13: viewAuditLogs(); break;
            case 14: viewParticipantList(); break;
            case 0: 
                manageBackups(0); // Auto-backup on exit
                return;
        }
    }
}

// --- Implemented by Pratik Barua: Participant Dashboard ---
void participantMenu() {
    int choice; char buffer[MAX_STR];
    while(1) {
        printf("\n=== PARTICIPANT DASHBOARD ===\n");
        printf("1. View Schedule  2. Search Events  3. Register for Event\n");
        printf("4. Cancel Reg     5. Leave Feedback 0. Logout\nChoice: ");
        scanf("%d", &choice); getchar();
        switch(choice) {
            case 1: displayEvents(0, ""); break;
            case 2: 
                printf("Search Keyword (Title/Date/Desc): "); fgets(buffer, MAX_STR, stdin); strip_nl(buffer);
                displayEvents(1, buffer); break;
            case 3: registerParticipant(); break;
            case 4: cancelRegistration(); break;
            case 5: leaveFeedback(); break;
            case 0: return;
        }
    }
}

// --- Implemented by Pratik Barua: Volunteer Dashboard ---
void volunteerMenu() {
    int choice;
    while(1) {
        printf("\n=== VOLUNTEER DASHBOARD ===\n");
        printf("1. View Events  2. Mark Attendance  3. View Participant List  4. View Venues\n0. Logout\nChoice: ");
        scanf("%d", &choice); getchar();
        switch(choice) {
            case 1: displayEvents(0, ""); break;
            case 2: markAttendance(); break;
            case 3: viewParticipantList(); break;
            case 4: displayVenues(); break;
            case 0: return;
        }
    }
}

// --- Implemented by Pratik Barua: Main Entry Point & Role Authentication ---
int main() {
    loadData();
    logAuditEvent("SYSTEM_START", "Event Management System launched");
    int role; char pass[20];
    char expected[] = "admin";
    
    while(1) {
        printf("\n=================================\n");
        printf("    EVENT MANAGEMENT SYSTEM\n");
        printf("=================================\n");
        printf("1. Organizer\n2. Participant\n3. Volunteer\n0. Exit\nLogin Role: ");
        scanf("%d", &role); getchar();
        
        if (role == 0) {
            manageBackups(0); // Auto-backup on exit
            logAuditEvent("SYSTEM_EXIT", "System terminated cleanly");
            printf("Exiting system. Data saved cleanly. Goodbye!\n");
            break;
        } else if (role == 1) { 
            printf("Enter Organizer Password: ");
            fgets(pass, 20, stdin); strip_nl(pass);
            int valid = 1;
            for(int i=0; i<5; i++) {
                if (pass[i] != expected[i]) valid = 0;
            }
            if (valid) {
                logAuditEvent("LOGIN_SUCCESS", "Organizer logged in");
                organizerMenu();
            } else {
                logAuditEvent("LOGIN_FAILED", "Invalid organizer password");
                printf("Authentication Failed.\n");
            }
        } else if (role == 2) {
            logAuditEvent("ROLE_SELECT", "Participant menu accessed");
            participantMenu();
        } else if (role == 3) {
            logAuditEvent("ROLE_SELECT", "Volunteer menu accessed");
            volunteerMenu();
        }
    }
    return 0;
}
