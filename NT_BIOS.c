#include <gtk/gtk.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

#ifdef _WIN32
#include <windows.h>
#include <sysinfoapi.h>
#else
#include <sys/sysinfo.h>
#endif

// Function prototypes for the tabs
static void show_main_tab(GtkWidget *widget, gpointer data);
static void show_advanced_tab(GtkWidget *widget, gpointer data);
static void show_security_tab(GtkWidget *widget, gpointer data);
static void show_boot_tab(GtkWidget *widget, gpointer data);
static void exit_program(GtkWidget *widget, gpointer data);

// Function prototypes for boot tab buttons
static void setup(GtkWidget *widget, gpointer data);
static void setup_without_compiler(GtkWidget *widget, gpointer data);
static void install_extra_libraries(GtkWidget *widget, gpointer data);
static void build(GtkWidget *widget, gpointer data);

// Global variable to keep track of the active tab
static int active_tab = 0;

static void update_system_info(GtkTextBuffer *buffer) {
    // Get current time and date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

#ifdef _WIN32
    // Windows-specific memory info
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    long total_memory = statex.ullTotalPhys / (1024 * 1024); // Convert to MB
    long free_memory = statex.ullAvailPhys / (1024 * 1024);  // Convert to MB
#else
    // Linux-specific memory info
    struct sysinfo info;
    sysinfo(&info);
    long total_memory = info.totalram / (1024 * 1024); // Convert to MB
    long free_memory = info.freeram / (1024 * 1024);   // Convert to MB
#endif

    // Get current directory
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    // Check for Zora executables
    const char *status;
    const char *legacy_diskette = "None";
    const char *primary_master = cwd;
    const char *primary_slave = "None";

    if (access("ZoraNT", F_OK) == 0) {
        status = "ZoraNT detected";
        primary_slave = "ZoraNT";
    } else if (access("Zora95", F_OK) == 0) {
        status = "Zora95 detected";
        legacy_diskette = "Zora95";
    } else if (access("ZoraDS", F_OK) == 0) {
        status = "ZoraDS detected";
        legacy_diskette = "ZoraDS";
    } else {
        status = "No Zora executable detected";
    }

    // Update text buffer with live data
    char info_text[1024];
    snprintf(info_text, sizeof(info_text),
             "System Time:  %02d:%02d:%02d\n"
             "System Date:  %02d/%02d/%04d\n\n"
             "Legacy Diskette:  %s\n\n"
             "Primary Master:  %s\n"
             "Primary Slave:  %s\n\n"
             "Secondary Master:  CD-ROM\n"
             "Secondary Slave:  None\n\n"
             "Total Memory:  %ld MB\n"
             "Free Memory:  %ld MB\n"
             "Status: %s\n",
             tm.tm_hour, tm.tm_min, tm.tm_sec,
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
             legacy_diskette,
             primary_master,
             primary_slave,
             total_memory,
             free_memory,
             status);

    gtk_text_buffer_set_text(buffer, info_text, -1);
}

static gboolean refresh_system_info(gpointer data) {
    if (active_tab == 0) { // Only update if the main tab is active
        GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
        update_system_info(buffer);
    }
    return TRUE; // Continue calling this function
}

static void show_main_tab(GtkWidget *widget, gpointer data) {
    active_tab = 0;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gtk_text_buffer_set_text(buffer, "Main tab content\n", -1);
}

static void show_advanced_tab(GtkWidget *widget, gpointer data) {
    active_tab = 1;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gtk_text_buffer_set_text(buffer, "Credits:\nDeveloped by Tomoko\n", -1);
}

static void show_security_tab(GtkWidget *widget, gpointer data) {
    active_tab = 2;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    const char *security_status;
    if (access("./ZoraNT/Sys404/runtime/Stolaris", F_OK) == 0) {
        security_status = "Stolaris executable or script found.";
    } else {
        security_status = "Stolaris executable or script not found.";
    }
    gtk_text_buffer_set_text(buffer, security_status, -1);
}

static void show_boot_tab(GtkWidget *widget, gpointer data) {
    active_tab = 3;
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gtk_text_buffer_set_text(buffer, "Boot tab content\n", -1);

    // Create a vertical box to hold the buttons
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    // Create the buttons
    GtkWidget *setup_button = gtk_button_new_with_label("Setup");
    GtkWidget *setup_without_compiler_button = gtk_button_new_with_label("Setup without Compiler");
    GtkWidget *install_extra_libraries_button = gtk_button_new_with_label("Install Extra Libraries");
    GtkWidget *build_button = gtk_button_new_with_label("Build");

    // Connect the buttons to their respective functions
    g_signal_connect(setup_button, "clicked", G_CALLBACK(setup), NULL);
    g_signal_connect(setup_without_compiler_button, "clicked", G_CALLBACK(setup_without_compiler), NULL);
    g_signal_connect(install_extra_libraries_button, "clicked", G_CALLBACK(install_extra_libraries), NULL);
    g_signal_connect(build_button, "clicked", G_CALLBACK(build), NULL);

    // Clear the content area and add the vertical box
    GtkWidget *content_area = gtk_widget_get_parent(widget);
    GList *children = gtk_container_get_children(GTK_CONTAINER(content_area));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    gtk_widget_show_all(content_area);
}

static void setup(GtkWidget *widget, gpointer data) {
    g_print("Setup button clicked\n");
    // Implement the setup functionality here
}

static void setup_without_compiler(GtkWidget *widget, gpointer data) {
    g_print("Setup without Compiler button clicked\n");
    // Implement the setup without compiler functionality here
}

static void install_extra_libraries(GtkWidget *widget, gpointer data) {
    g_print("Install Extra Libraries button clicked\n");
    // Implement the install extra libraries functionality here
}

static void build(GtkWidget *widget, gpointer data) {
    g_print("Build button clicked\n");
    // Implement the build functionality here
}

static void exit_program(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ZoraNT BIOS Setup Utility");
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 400);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Header with tabs
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 0);
    const char *tabs[] = {"Main", "Advanced", "Security", "Boot", "Exit"};
    GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);
    for (int i = 0; i < 5; i++) {
        GtkWidget *button = gtk_button_new_with_label(tabs[i]);
        gtk_box_pack_start(GTK_BOX(header), button, FALSE, FALSE, 0);
        if (i == 0) {
            g_signal_connect(button, "clicked", G_CALLBACK(show_main_tab), buffer);
        } else if (i == 1) {
            g_signal_connect(button, "clicked", G_CALLBACK(show_advanced_tab), buffer);
        } else if (i == 2) {
            g_signal_connect(button, "clicked", G_CALLBACK(show_security_tab), buffer);
        } else if (i == 3) {
            g_signal_connect(button, "clicked", G_CALLBACK(show_boot_tab), buffer);
        } else if (i == 4) {
            g_signal_connect(button, "clicked", G_CALLBACK(exit_program), NULL);
        }
    }

    // Main content area
    GtkWidget *content_area = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), content_area, TRUE, TRUE, 0);

    // Left panel with BIOS text
    GtkWidget *left_panel = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(left_panel), FALSE);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(left_panel), buffer);
    update_system_info(buffer);
    gtk_box_pack_start(GTK_BOX(content_area), left_panel, TRUE, TRUE, 0);

    // Right panel with help text
    GtkWidget *right_panel = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(right_panel), FALSE);
    GtkTextBuffer *help_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(right_panel));
    gtk_text_buffer_set_text(help_buffer,
        "Item Specific Help\n"
        "--------------------\n"
        "<Tab>, <Shift-Tab>, or <Enter> selects field.\n", -1);
    gtk_box_pack_start(GTK_BOX(content_area), right_panel, FALSE, FALSE, 0);

    // Footer
    GtkWidget *footer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), footer, FALSE, FALSE, 0);
    GtkWidget *f1_label = gtk_label_new("F1 Help");
    GtkWidget *esc_label = gtk_label_new("Esc Exit");
    GtkWidget *f9_label = gtk_label_new("F9 Setup Defaults");
    GtkWidget *f10_label = gtk_label_new("F10 Save and Exit");
    gtk_box_pack_start(GTK_BOX(footer), f1_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(footer), esc_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(footer), f9_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(footer), f10_label, FALSE, FALSE, 5);

    gtk_widget_show_all(window);

    // Refresh system info every second
    g_timeout_add_seconds(1, refresh_system_info, buffer);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.zorant.bios", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
