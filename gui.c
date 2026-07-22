#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EVENT_FILE "events.csv"
#define REG_FILE "registrations.csv"
#define MAX_STR 100
#define MAX_EVENTS 200
#define MAX_REGS 1000

typedef struct {
    int id; char title[MAX_STR]; char date[MAX_STR]; char time[MAX_STR];
    char venue[MAX_STR]; char category[MAX_STR]; int capacity; int registered; int attended;
} Event;

typedef struct {
    int event_id; char participant_name[MAX_STR]; int is_present; char feedback[MAX_STR];
} Registration;

Event events[MAX_EVENTS]; int event_count = 0;
Registration regs[MAX_REGS]; int reg_count = 0;

GtkWidget *main_stack;
GtkWidget *login_view, *org_view, *part_view, *vol_view;
GtkListStore *list_store_org, *list_store_part;
GtkWidget *reports_text_view;
GtkWidget *tree_view_org, *tree_view_part;

void loadData() {
    event_count = 0; reg_count = 0;
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
    if(fe) {
        for (int i = 0; i < event_count; i++)
            fprintf(fe, "%d,%s,%s,%s,%s,%s,%d,%d,%d\n", events[i].id, events[i].title, events[i].date, events[i].time, events[i].venue, events[i].category, events[i].capacity, events[i].registered, events[i].attended);
        fclose(fe);
    }
    FILE *fr = fopen(REG_FILE, "w");
    if(fr) {
        for (int i = 0; i < reg_count; i++)
            fprintf(fr, "%d,%s,%d,%s\n", regs[i].event_id, regs[i].participant_name, regs[i].is_present, regs[i].feedback);
        fclose(fr);
    }
}

void refresh_tree_view(GtkListStore *store) {
    gtk_list_store_clear(store);
    GtkTreeIter iter;
    for(int i=0; i<event_count; i++) {
        char slots[32]; sprintf(slots, "%d/%d", events[i].capacity - events[i].registered, events[i].capacity);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, events[i].id, 1, events[i].title, 2, events[i].date, 3, events[i].time, 4, events[i].venue, 5, slots, -1);
    }
}

void refresh_reports() {
    char output[10000] = "";
    for (int i = 0; i < event_count; i++) {
        char line[256];
        sprintf(line, "📅 %s (ID: %d) | Cap: %d | Reg: %d | Attended: %d\n  Participants: ", events[i].title, events[i].id, events[i].capacity, events[i].registered, events[i].attended);
        strcat(output, line);
        int found = 0;
        for (int j = 0; j < reg_count; j++) {
            if (regs[j].event_id == events[i].id) {
                char p[128]; sprintf(p, "%s (%s), ", regs[j].participant_name, regs[j].is_present ? "Present" : "Absent");
                strcat(output, p); found = 1;
            }
        }
        if(!found) strcat(output, "None");
        strcat(output, "\n\n");
    }
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(reports_text_view));
    gtk_text_buffer_set_text(buffer, output, -1);
}

void show_message(GtkWindow *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int prompt_input(GtkWindow *parent, const char *title, const char *label_text, char *output_str, gboolean is_password) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(title, parent, GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "OK", GTK_RESPONSE_OK, NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    if(is_password) gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_container_add(GTK_CONTAINER(content_area), gtk_label_new(label_text));
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    gtk_widget_show_all(dialog);
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) strcpy(output_str, gtk_entry_get_text(GTK_ENTRY(entry)));
    gtk_widget_destroy(dialog);
    return response == GTK_RESPONSE_OK;
}

// Navigation Actions
void on_logout_clicked(GtkWidget *w, gpointer data) { gtk_stack_set_visible_child(GTK_STACK(main_stack), login_view); }

void on_login_org(GtkWidget *w, gpointer window) {
    char pass[100] = "";
    if (prompt_input(GTK_WINDOW(window), "Organizer Login", "Enter Organizer Password (admin):", pass, TRUE)) {
        if (strcmp(pass, "admin") == 0) {
            refresh_tree_view(list_store_org); refresh_reports();
            gtk_stack_set_visible_child(GTK_STACK(main_stack), org_view);
        } else {
            show_message(GTK_WINDOW(window), "Authentication Failed!");
        }
    }
}
void on_login_part(GtkWidget *w, gpointer data) { refresh_tree_view(list_store_part); gtk_stack_set_visible_child(GTK_STACK(main_stack), part_view); }
void on_login_vol(GtkWidget *w, gpointer data) { gtk_stack_set_visible_child(GTK_STACK(main_stack), vol_view); }

// Feature Actions
void on_add_event_clicked(GtkWidget *w, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add New Event", GTK_WINDOW(window), GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "Save Event", GTK_RESPONSE_OK, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(grid), 10); gtk_grid_set_column_spacing(GTK_GRID(grid), 10); gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    GtkWidget *e_title = gtk_entry_new(); GtkWidget *e_date = gtk_entry_new();
    GtkWidget *e_time = gtk_entry_new(); GtkWidget *e_venue = gtk_entry_new();
    GtkWidget *e_cat = gtk_entry_new(); GtkWidget *e_cap = gtk_entry_new();
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Title:"), 0, 0, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_title, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Date (DD/MM/YY):"), 0, 1, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_date, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Time (HH:MM):"), 0, 2, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_time, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Venue:"), 0, 3, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_venue, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Category:"), 0, 4, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_cat, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Capacity:"), 0, 5, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_cap, 1, 5, 1, 1);
    
    gtk_container_add(GTK_CONTAINER(content), grid); gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        Event e; e.id = (event_count > 0) ? events[event_count-1].id + 1 : 101;
        strcpy(e.title, gtk_entry_get_text(GTK_ENTRY(e_title))); strcpy(e.date, gtk_entry_get_text(GTK_ENTRY(e_date)));
        strcpy(e.time, gtk_entry_get_text(GTK_ENTRY(e_time))); strcpy(e.venue, gtk_entry_get_text(GTK_ENTRY(e_venue)));
        strcpy(e.category, gtk_entry_get_text(GTK_ENTRY(e_cat))); e.capacity = atoi(gtk_entry_get_text(GTK_ENTRY(e_cap)));
        e.registered = 0; e.attended = 0; events[event_count++] = e; saveData();
        show_message(GTK_WINDOW(window), "Event Added Successfully!"); refresh_tree_view(list_store_org); refresh_reports();
    }
    gtk_widget_destroy(dialog);
}

void on_register_clicked(GtkWidget *w, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Register", GTK_WINDOW(window), GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "Register", GTK_RESPONSE_OK, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(grid), 10); gtk_grid_set_column_spacing(GTK_GRID(grid), 10); gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    GtkWidget *e_id = gtk_entry_new(); GtkWidget *e_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Event ID:"), 0, 0, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_id, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Your Name:"), 0, 1, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_name, 1, 1, 1, 1);
    
    gtk_container_add(GTK_CONTAINER(content), grid); gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(e_id))); const char *name = gtk_entry_get_text(GTK_ENTRY(e_name));
        int idx = -1; for(int i=0; i<event_count; i++) if(events[i].id == id) idx = i;
        if (idx == -1) show_message(GTK_WINDOW(window), "Event not found!");
        else if (events[idx].registered >= events[idx].capacity) show_message(GTK_WINDOW(window), "Event is full!");
        else {
            regs[reg_count].event_id = id; strcpy(regs[reg_count].participant_name, name);
            regs[reg_count].is_present = 0; strcpy(regs[reg_count].feedback, "N/A");
            reg_count++; events[idx].registered++; saveData();
            show_message(GTK_WINDOW(window), "Registration Successful!"); refresh_tree_view(list_store_part);
        }
    }
    gtk_widget_destroy(dialog);
}

void on_attendance_clicked(GtkWidget *w, gpointer window) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Mark Attendance", GTK_WINDOW(window), GTK_DIALOG_MODAL, "Cancel", GTK_RESPONSE_CANCEL, "Mark Present", GTK_RESPONSE_OK, NULL);
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new(); gtk_grid_set_row_spacing(GTK_GRID(grid), 10); gtk_grid_set_column_spacing(GTK_GRID(grid), 10); gtk_container_set_border_width(GTK_CONTAINER(grid), 20);
    
    GtkWidget *e_id = gtk_entry_new(); GtkWidget *e_name = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Event ID:"), 0, 0, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_id, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Participant Name:"), 0, 1, 1, 1); gtk_grid_attach(GTK_GRID(grid), e_name, 1, 1, 1, 1);
    
    gtk_container_add(GTK_CONTAINER(content), grid); gtk_widget_show_all(dialog);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        int id = atoi(gtk_entry_get_text(GTK_ENTRY(e_id))); const char *name = gtk_entry_get_text(GTK_ENTRY(e_name));
        int found = 0;
        for (int i = 0; i < reg_count; i++) {
            if (regs[i].event_id == id && strcmp(regs[i].participant_name, name) == 0) {
                if (!regs[i].is_present) {
                    regs[i].is_present = 1;
                    for(int j=0; j<event_count; j++) if(events[j].id == id) events[j].attended++;
                    saveData(); show_message(GTK_WINDOW(window), "Participant marked PRESENT."); refresh_reports();
                } else show_message(GTK_WINDOW(window), "Already marked present.");
                found = 1; break;
            }
        }
        if(!found) show_message(GTK_WINDOW(window), "Registration not found!");
    }
    gtk_widget_destroy(dialog);
}

// UI Builders
GtkWidget* create_tree_view(GtkListStore **store_ptr) {
    *store_ptr = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(*store_ptr));
    g_object_unref(*store_ptr);
    const char *cols[] = {"ID", "Title", "Date", "Time", "Venue", "Slots"};
    for(int i=0; i<6; i++) gtk_tree_view_append_column(GTK_TREE_VIEW(tree), gtk_tree_view_column_new_with_attributes(cols[i], gtk_cell_renderer_text_new(), "text", i, NULL));
    return tree;
}

static void activate(GtkApplication* app, gpointer user_data) {
    loadData();
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 950, 650);

    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, 
        "window { background: #1e1e2e; color: #cdd6f4; }"
        "button { border-radius: 8px; padding: 12px; margin: 4px; font-weight: bold; background: #313244; color: #cdd6f4; border: none; font-size: 14px; }"
        "button.primary { background: #89b4fa; color: #11111b; }"
        "button:hover { background: #45475a; }"
        "button.primary:hover { background: #b4befe; }"
        "treeview { font-size: 15px; background: #181825; color: #cdd6f4; }"
        "treeview:selected { background: #89b4fa; color: #11111b; }"
        "label.title { font-size: 28px; font-weight: bold; color: #89b4fa; margin-bottom: 20px; }"
        "label.subtitle { font-size: 16px; color: #a6adc8; margin-bottom: 40px; }"
        "textview { font-size: 15px; background: #181825; color: #cdd6f4; padding: 20px; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Event Management System");
    gtk_window_set_titlebar(GTK_WINDOW(window), header);

    main_stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(main_stack), GTK_STACK_TRANSITION_TYPE_CROSSFADE);
    gtk_container_add(GTK_CONTAINER(window), main_stack);

    // --- 1. Login View ---
    login_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_valign(login_view, GTK_ALIGN_CENTER); gtk_widget_set_halign(login_view, GTK_ALIGN_CENTER);
    
    GtkWidget *l_title = gtk_label_new("Welcome to EventManager");
    GtkStyleContext *sc = gtk_widget_get_style_context(l_title); gtk_style_context_add_class(sc, "title");
    gtk_box_pack_start(GTK_BOX(login_view), l_title, FALSE, FALSE, 0);
    
    GtkWidget *l_sub = gtk_label_new("Select your role to continue");
    sc = gtk_widget_get_style_context(l_sub); gtk_style_context_add_class(sc, "subtitle");
    gtk_box_pack_start(GTK_BOX(login_view), l_sub, FALSE, FALSE, 0);
    
    GtkWidget *btn_org = gtk_button_new_with_label("Log in as Organizer");
    sc = gtk_widget_get_style_context(btn_org); gtk_style_context_add_class(sc, "primary");
    GtkWidget *btn_part = gtk_button_new_with_label("Log in as Participant");
    GtkWidget *btn_vol = gtk_button_new_with_label("Log in as Volunteer");
    
    gtk_box_pack_start(GTK_BOX(login_view), btn_org, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(login_view), btn_part, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(login_view), btn_vol, FALSE, FALSE, 5);
    
    g_signal_connect(btn_org, "clicked", G_CALLBACK(on_login_org), window);
    g_signal_connect(btn_part, "clicked", G_CALLBACK(on_login_part), NULL);
    g_signal_connect(btn_vol, "clicked", G_CALLBACK(on_login_vol), NULL);

    // --- 2. Organizer Dashboard ---
    org_view = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15); gtk_container_set_border_width(GTK_CONTAINER(org_view), 20);
    GtkWidget *org_sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); gtk_widget_set_size_request(org_sidebar, 200, -1);
    gtk_box_pack_start(GTK_BOX(org_view), org_sidebar, FALSE, FALSE, 0);
    
    GtkWidget *org_stack = gtk_stack_new(); gtk_box_pack_start(GTK_BOX(org_view), org_stack, TRUE, TRUE, 0);
    GtkWidget *org_stack_switcher = gtk_stack_sidebar_new(); gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(org_stack_switcher), GTK_STACK(org_stack));
    gtk_box_pack_start(GTK_BOX(org_sidebar), org_stack_switcher, TRUE, TRUE, 0);
    
    GtkWidget *btn_add_event = gtk_button_new_with_label("Add New Event"); sc = gtk_widget_get_style_context(btn_add_event); gtk_style_context_add_class(sc, "primary");
    g_signal_connect(btn_add_event, "clicked", G_CALLBACK(on_add_event_clicked), window);
    gtk_box_pack_start(GTK_BOX(org_sidebar), btn_add_event, FALSE, FALSE, 0);
    
    GtkWidget *btn_org_logout = gtk_button_new_with_label("Logout"); g_signal_connect(btn_org_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(org_sidebar), btn_org_logout, FALSE, FALSE, 0);
    
    // Manage Events Tab
    GtkWidget *org_events_scroll = gtk_scrolled_window_new(NULL, NULL); tree_view_org = create_tree_view(&list_store_org);
    gtk_container_add(GTK_CONTAINER(org_events_scroll), tree_view_org);
    gtk_stack_add_titled(GTK_STACK(org_stack), org_events_scroll, "manage_events", "📅 Manage Events");
    
    // Reports Tab
    GtkWidget *org_reports_scroll = gtk_scrolled_window_new(NULL, NULL);
    reports_text_view = gtk_text_view_new(); gtk_text_view_set_editable(GTK_TEXT_VIEW(reports_text_view), FALSE);
    gtk_container_add(GTK_CONTAINER(org_reports_scroll), reports_text_view);
    gtk_stack_add_titled(GTK_STACK(org_stack), org_reports_scroll, "reports", "📊 View Reports");
    
    // --- 3. Participant Dashboard ---
    part_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15); gtk_container_set_border_width(GTK_CONTAINER(part_view), 20);
    GtkWidget *part_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); gtk_box_pack_start(GTK_BOX(part_view), part_top, FALSE, FALSE, 0);
    
    GtkWidget *btn_part_reg = gtk_button_new_with_label("Register for Event"); sc = gtk_widget_get_style_context(btn_part_reg); gtk_style_context_add_class(sc, "primary");
    GtkWidget *btn_part_logout = gtk_button_new_with_label("Logout");
    gtk_box_pack_start(GTK_BOX(part_top), btn_part_reg, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(part_top), btn_part_logout, FALSE, FALSE, 0);
    
    g_signal_connect(btn_part_reg, "clicked", G_CALLBACK(on_register_clicked), window);
    g_signal_connect(btn_part_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    
    GtkWidget *part_scroll = gtk_scrolled_window_new(NULL, NULL); tree_view_part = create_tree_view(&list_store_part);
    gtk_container_add(GTK_CONTAINER(part_scroll), tree_view_part); gtk_box_pack_start(GTK_BOX(part_view), part_scroll, TRUE, TRUE, 0);

    // --- 4. Volunteer Dashboard ---
    vol_view = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15); gtk_container_set_border_width(GTK_CONTAINER(vol_view), 20);
    GtkWidget *vol_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); gtk_box_pack_start(GTK_BOX(vol_view), vol_top, FALSE, FALSE, 0);
    
    GtkWidget *btn_vol_att = gtk_button_new_with_label("Mark Attendance"); sc = gtk_widget_get_style_context(btn_vol_att); gtk_style_context_add_class(sc, "primary");
    GtkWidget *btn_vol_logout = gtk_button_new_with_label("Logout");
    gtk_box_pack_start(GTK_BOX(vol_top), btn_vol_att, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vol_top), btn_vol_logout, FALSE, FALSE, 0);
    
    g_signal_connect(btn_vol_att, "clicked", G_CALLBACK(on_attendance_clicked), window);
    g_signal_connect(btn_vol_logout, "clicked", G_CALLBACK(on_logout_clicked), NULL);

    GtkWidget *vol_lbl = gtk_label_new("Use 'Mark Attendance' to check-in participants."); gtk_box_pack_start(GTK_BOX(vol_view), vol_lbl, TRUE, TRUE, 0);

    // Add to main stack
    gtk_stack_add_named(GTK_STACK(main_stack), login_view, "login");
    gtk_stack_add_named(GTK_STACK(main_stack), org_view, "org");
    gtk_stack_add_named(GTK_STACK(main_stack), part_view, "part");
    gtk_stack_add_named(GTK_STACK(main_stack), vol_view, "vol");

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.capstone.eventmanagerfinal", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
