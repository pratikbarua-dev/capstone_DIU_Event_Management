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
