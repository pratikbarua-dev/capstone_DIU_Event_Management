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

