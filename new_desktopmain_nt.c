#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gtypes.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkdnd.h>
#include <gdk/gdkevents.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <gst/gst.h>
#include <json-glib/json-glib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

// all libraries needed to function, if anything is missing from the install script please include it
// main function at line 583

#define APP_DIRECTORY "./ZoraNT/Programs"  // Path to applications directory
#define USER_DIRECTORY "./ZoraNT/User"     // Path to user directory

// Function declarations
GtkWidget *load_icon(const char *icon_path);
void show_start_menu(GtkWidget *widget, gpointer menu);
GtkWidget *create_start_menu(GtkWidget *window);
void load_applications(GtkWidget *desktop);
gboolean on_icon_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);
gboolean launch_application(GtkWidget *widget, gpointer data);
gboolean on_drag_motion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer data);
void on_drag_begin(GtkWidget *widget, GdkDragContext *context, gpointer data);
void on_drag_end(GtkWidget *widget, GdkDragContext *context, gpointer data);
void open_file_manager(GtkWidget *widget, gpointer data);
void on_file_activated(GtkFileChooser *file_chooser, gpointer data);
void show_programs(GtkWidget *widget, gpointer data);
void show_run_dialog(GtkWidget *widget, gpointer data);
void shutdown_desktop(GtkWidget *widget, gpointer data);
void show_tray(GtkWidget *widget, gpointer data);
void show_settings(GtkWidget *widget, gpointer data);
void play_sound(const char *sound_path);
void show_shutdown_splash();
static void play_shutdown_sound(const char *base_path);
void add_taskbar_button(const char *app_name, const char *icon_path);
void open_network_settings(GtkWidget *widget, gpointer data);
void load_profiles(GtkWidget *profiles_menu);
void show_profile(GtkWidget *widget, gpointer profile_path);
void create_new_profile(GtkWidget *widget, gpointer data);
void save_profile(const char *profile_name, const char *bio, const char *image_path);

// Function to load an icon from a file
GtkWidget *load_icon(const char *icon_path) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(icon_path, 48, 48, TRUE, NULL);
    if (!pixbuf) {
        g_warning("Failed to load icon: %s", icon_path);
        return gtk_image_new(); // Return an empty image widget if loading fails
    }
    return gtk_image_new_from_pixbuf(pixbuf);
}

// Callback function to show the start menu
void show_start_menu(GtkWidget *widget, gpointer menu) {
    gtk_menu_popup_at_widget(GTK_MENU(menu), widget, GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, NULL);
}

// Function to create the start menu
GtkWidget *create_start_menu(GtkWidget *window) {
    GtkWidget *start_menu = gtk_menu_new();

    // Programs submenu
    GtkWidget *programs_menu = gtk_menu_new();
    GtkWidget *programs_item = gtk_menu_item_new_with_label("Programs");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(programs_item), programs_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), programs_item);

    // Dynamically add programs to the Programs submenu
    DIR *dir = opendir(APP_DIRECTORY);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_name[0] == '.') continue; // Skip hidden files

            // Load the application icon
            char app_path[256];
            snprintf(app_path, sizeof(app_path), "%s/%s", APP_DIRECTORY, entry->d_name);

            // Construct the icon path
            char icon_path[256];
            snprintf(icon_path, sizeof(icon_path), "%s/%s.ico", app_path, entry->d_name);

            GtkWidget *icon = load_icon(icon_path);
            GtkWidget *program_item = gtk_image_menu_item_new_with_label(entry->d_name);
            gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(program_item), icon);
            g_signal_connect(program_item, "activate", G_CALLBACK(launch_application), g_strdup(app_path));
            gtk_menu_shell_append(GTK_MENU_SHELL(programs_menu), program_item);
        }
        closedir(dir);
    }

    // Settings submenu
    GtkWidget *settings_menu = gtk_menu_new();
    GtkWidget *settings_item = gtk_menu_item_new_with_label("Settings");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_item), settings_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), settings_item);

    // Add some example settings
    GtkWidget *settings_item1 = gtk_menu_item_new_with_label("Display Settings");
    g_signal_connect(settings_item1, "activate", G_CALLBACK(show_settings), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), settings_item1);

    GtkWidget *settings_item2 = gtk_menu_item_new_with_label("Network Settings");
    g_signal_connect(settings_item2, "activate", G_CALLBACK(open_network_settings), NULL); // Updated line
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), settings_item2);

    // Profiles submenu
    GtkWidget *profiles_menu = gtk_menu_new();
    GtkWidget *profiles_item = gtk_menu_item_new_with_label("Profiles");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(profiles_item), profiles_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), profiles_item);

    // Load profiles dynamically
    load_profiles(profiles_menu);

    // Run dialog
    GtkWidget *run_item = gtk_menu_item_new_with_label("Run...");
    g_signal_connect(run_item, "activate", G_CALLBACK(show_run_dialog), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), run_item);

    // Separator
    GtkWidget *separator = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), separator);

    // Shutdown
    GtkWidget *shutdown_item = gtk_menu_item_new_with_label("Shutdown");
    g_signal_connect(shutdown_item, "activate", G_CALLBACK(shutdown_desktop), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(start_menu), shutdown_item);

    gtk_widget_show_all(start_menu);
    return start_menu;
}

// Dynamic loading of programs onto desktop
void load_applications(GtkWidget *desktop) {
    DIR *dir = opendir(APP_DIRECTORY);
    if (!dir) {
        perror("Failed to open the programs directory");
        return;
    }

    struct dirent *entry;
    int x = 20, y = 20; // Starting position for the icons
    const int icon_spacing = 80; // Spacing between icons
    const int max_icons_per_column = 5; // Max number of icons per column

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files

        // Load the application icon
        char app_path[256];
        snprintf(app_path, sizeof(app_path), "%s/%s", APP_DIRECTORY, entry->d_name);

        // Construct the icon path
        char icon_path[256];
        snprintf(icon_path, sizeof(icon_path), "%s/%s.ico", app_path, entry->d_name);

        GtkWidget *icon = load_icon(icon_path);
        GtkWidget *label = gtk_label_new(entry->d_name);

        GtkWidget *box = gtk_event_box_new();
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_box_pack_start(GTK_BOX(vbox), icon, FALSE, FALSE, 5);
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 5);
        gtk_container_add(GTK_CONTAINER(box), vbox);

        gtk_fixed_put(GTK_FIXED(desktop), box, x, y);

        // Set the box to receive button press events
        gtk_widget_set_events(box, GDK_BUTTON_PRESS_MASK);

        // Connect the button press event to handle double-clicks
        g_signal_connect(box, "button-press-event", G_CALLBACK(on_icon_button_press), g_strdup(app_path));

        // Enable drag-and-drop
        gtk_drag_source_set(box, GDK_BUTTON1_MASK, NULL, 0, GDK_ACTION_MOVE);
        g_signal_connect(box, "drag-begin", G_CALLBACK(on_drag_begin), NULL);
        g_signal_connect(box, "drag-motion", G_CALLBACK(on_drag_motion), desktop);
        g_signal_connect(box, "drag-end", G_CALLBACK(on_drag_end), NULL);

        y += icon_spacing;
        if (y >= icon_spacing * max_icons_per_column) {
            y = 20;
            x += icon_spacing;
        }
    }

    closedir(dir);
}

// Function to handle button press events on icons
gboolean on_icon_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    if (event->type == GDK_2BUTTON_PRESS) {
        launch_application(widget, data);
    }
    return TRUE; // Return TRUE to indicate the event has been handled
}

// Function to launch an application
gboolean launch_application(GtkWidget *widget, gpointer data) {
    const char *app_path = (const char *)data;
    g_print("Launching application from path: %s\n", app_path);

    // Construct the command to launch the application
    char command[256];
    snprintf(command, sizeof(command), "%s/%s", app_path, strrchr(app_path, '/') + 1);

    // Check if the application is a CLI program
    gboolean is_cli = g_str_has_suffix(command, "SeaDrive");

    // Launch the application asynchronously
    GError *error = NULL;
    if (is_cli) {
#ifdef _WIN32
        // Launch CLI program in a separate PowerShell window on Windows
        char ps_command[512];
        snprintf(ps_command, sizeof(ps_command), "Start-Process powershell -ArgumentList '-NoExit', '-Command', '%s'", command);
        if (!g_spawn_async(NULL, (gchar *[]){"powershell.exe", "-NoExit", "-Command", ps_command, NULL}, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Failed to launch CLI application: %s\n", error->message);
            g_error_free(error);
        }
#else
        // Launch CLI program in a terminal on Linux
        char *term_command[] = {"gnome-terminal", "--", "bash", "-c", command, NULL};
        if (!g_spawn_async(NULL, term_command, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Failed to launch CLI application: %s\n", error->message);
            g_error_free(error);
        }
#endif
    } else {
        // Launch GUI program
        char *gui_command[] = {command, NULL};
        if (!g_spawn_async(NULL, gui_command, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Failed to launch GUI application: %s\n", error->message);
            g_error_free(error);
        }
    }

    g_free(data); // Free the memory allocated by g_strdup
    return TRUE; // Return TRUE to indicate the event has been handled
}

// Function to handle drag motion
gboolean on_drag_motion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer data) {
    GtkWidget *desktop = GTK_WIDGET(data);
    gtk_fixed_move(GTK_FIXED(desktop), widget, x, y);
    return TRUE;
}

// Function to handle drag begin
void on_drag_begin(GtkWidget *widget, GdkDragContext *context, gpointer data) {
    g_print("Drag started\n");
}

// Function to handle drag end
void on_drag_end(GtkWidget *widget, GdkDragContext *context, gpointer data) {
    g_print("Drag ended\n");
}

// Function to open file manager
void open_file_manager(GtkWidget *widget, gpointer data) {
    GtkWidget *file_manager_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(file_manager_window), "File Manager");
    gtk_window_set_default_size(GTK_WINDOW(file_manager_window), 600, 400);

    GtkWidget *file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(file_chooser), USER_DIRECTORY);

    g_signal_connect(file_chooser, "file-activated", G_CALLBACK(on_file_activated), NULL);

    gtk_container_add(GTK_CONTAINER(file_manager_window), file_chooser);

    gtk_widget_show_all(file_manager_window);
}

void on_file_activated(GtkFileChooser *file_chooser, gpointer data) {
    char *filename = gtk_file_chooser_get_filename(file_chooser);
    if (filename) {
        g_print("Opening file: %s\n", filename);

#ifdef _WIN32
        // Use ShellExecute to open the file on Windows
        HINSTANCE result = ShellExecute(NULL, "open", filename, NULL, NULL, SW_SHOWNORMAL);
        if ((int)result <= 32) {
            g_printerr("Failed to open file: %d\n", (int)result);
        }
#else
        // Use xdg-open to open the file on Linux
        char command[512];
        snprintf(command, sizeof(command), "xdg-open \"%s\"", filename);
        int ret = system(command);
        if (ret != 0) {
            g_printerr("Failed to open file: %s\n", filename);
        }
#endif

        g_free(filename);
    }
}

// Function to show available programs
void show_programs(GtkWidget *widget, gpointer data) {
    GtkWidget *programs_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(programs_window), "Programs");
    gtk_window_set_default_size(GTK_WINDOW(programs_window), 400, 300);

    GtkWidget *programs_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(programs_window), programs_list);

    DIR *dir = opendir(APP_DIRECTORY);
    if (!dir) {
        perror("Failed to open the programs directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files

        char app_path[256];
        snprintf(app_path, sizeof(app_path), "%s/%s", APP_DIRECTORY, entry->d_name);

        GtkWidget *program_button = gtk_button_new_with_label(entry->d_name);
        g_signal_connect(program_button, "clicked", G_CALLBACK(launch_application), g_strdup(app_path));
        gtk_box_pack_start(GTK_BOX(programs_list), program_button, FALSE, FALSE, 5);
    }

    closedir(dir);

    gtk_widget_show_all(programs_window);
}

// Function to show the run dialog
void show_run_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Run", GTK_WINDOW(data), GTK_DIALOG_MODAL,
                                                    "_Run", GTK_RESPONSE_ACCEPT,
                                                    "_Cancel", GTK_RESPONSE_REJECT,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(content_area), entry, FALSE, FALSE, 5);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const gchar *command = gtk_entry_get_text(GTK_ENTRY(entry));
        GError *error = NULL;
        if (!g_spawn_async(NULL, (gchar *[]){(gchar *)command, NULL}, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
            g_print("Failed to run command: %s\n", error->message);
            g_error_free(error);
        }
    }

    gtk_widget_destroy(dialog);
}

// Function to shutdown the desktop
void shutdown_desktop(GtkWidget *widget, gpointer data) {
    show_shutdown_splash();
    gtk_main_quit();
}

// Function to update the clock
gboolean update_clock(GtkLabel *clock_label) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[10];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%I:%M %p", timeinfo);

    gtk_label_set_text(clock_label, buffer);
    return TRUE; // Keep the timeout running
}

// Function to close the splash screen and launch the desktop
static gboolean close_splash(GtkWidget *window) {
    gtk_widget_destroy(window);
    gtk_main_quit(); // Quit the splash screen GTK loop
    return FALSE; // Don't repeat
}

// Function to handle GStreamer bus messages
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;
            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);
            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

// Function to play the startup sound
static void play_startup_music(const char *base_path) {
    gchar *base_path_fixed = g_strdup(base_path);
    for (gchar *p = base_path_fixed; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    char *uri = g_strdup_printf("file:///%s/ZoraNT/Sys404/startup_sound.wav", base_path_fixed);
    g_print("Playing startup music from: %s\n", uri);
    GstElement *pipeline = gst_parse_launch(g_strdup_printf("playbin uri=%s", uri), NULL);
    if (pipeline) {
        GstBus *bus = gst_element_get_bus(pipeline);
        gst_bus_add_watch(bus, bus_call, NULL);
        gst_object_unref(bus);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        g_print("GStreamer pipeline created and playing.\n");
    } else {
        g_printerr("Failed to create GStreamer pipeline.\n");
    }
    g_free(uri);
    g_free(base_path_fixed);
}

void show_tray(GtkWidget *widget, gpointer data) {
    GtkWidget *tray_popup = gtk_menu_new();

    // Add some example tray items
    GtkWidget *tray_item1 = gtk_menu_item_new_with_label("Tray Item 1");
    gtk_menu_shell_append(GTK_MENU_SHELL(tray_popup), tray_item1);

    GtkWidget *tray_item2 = gtk_menu_item_new_with_label("Tray Item 2");
    gtk_menu_shell_append(GTK_MENU_SHELL(tray_popup), tray_item2);

    GtkWidget *tray_item3 = gtk_menu_item_new_with_label("Tray Item 3");
    gtk_menu_shell_append(GTK_MENU_SHELL(tray_popup), tray_item3);

    gtk_widget_show_all(tray_popup);

    // Position the tray popup near the taskbar
    gtk_menu_popup_at_widget(GTK_MENU(tray_popup), widget, GDK_GRAVITY_SOUTH_EAST, GDK_GRAVITY_NORTH_EAST, NULL);
}

void show_settings(GtkWidget *widget, gpointer data) {
    GtkWidget *settings_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(settings_window), "Settings");
    gtk_window_set_default_size(GTK_WINDOW(settings_window), 400, 300);

    GtkWidget *label = gtk_label_new("Settings window (functionality not implemented yet)");
    gtk_container_add(GTK_CONTAINER(settings_window), label);

    gtk_widget_show_all(settings_window);
}

void show_shutdown_splash() {
    g_print("Showing shutdown splash screen...\n");

    GtkWidget *shutdown_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(shutdown_window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(shutdown_window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(shutdown_window), "Shutdown Screen");
    gtk_window_set_position(GTK_WINDOW(shutdown_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_app_paintable(shutdown_window, TRUE);
    gtk_widget_realize(shutdown_window);
    GdkScreen *screen = gtk_widget_get_screen(shutdown_window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual) {
        gtk_widget_set_visual(shutdown_window, visual);
    }

    char *image_path = g_strdup_printf("%s/ZoraNT/Sys404/zorant_splash_shutdown.png", g_get_current_dir());
    g_print("Loading image from: %s\n", image_path);
    GtkWidget *image = gtk_image_new_from_file(image_path);
    g_free(image_path);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
    gtk_container_add(GTK_CONTAINER(shutdown_window), box);
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
    gtk_widget_show_all(shutdown_window);

    char *base_path = g_get_current_dir();
    play_shutdown_sound(base_path);
    g_free(base_path);

    g_timeout_add_seconds(4, (GSourceFunc)gtk_main_quit, NULL);
    gtk_main();
}

static void play_shutdown_sound(const char *base_path) {
    gchar *base_path_fixed = g_strdup(base_path);
    for (gchar *p = base_path_fixed; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    char *uri = g_strdup_printf("file:///%s/ZoraNT/Sys404/shutdown_sound.wav", base_path_fixed);
    g_print("Playing shutdown music from: %s\n", uri);
    GstElement *pipeline = gst_parse_launch(g_strdup_printf("playbin uri=%s", uri), NULL);
    if (pipeline) {
        GstBus *bus = gst_element_get_bus(pipeline);
        gst_bus_add_watch(bus, bus_call, NULL);
        gst_object_unref(bus);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        g_print("GStreamer pipeline created and playing.\n");
    } else {
        g_printerr("Failed to create GStreamer pipeline.\n");
    }
    g_free(uri);
    g_free(base_path_fixed);
}

// Function to add a button to the taskbar
void add_taskbar_button(const char *app_name, const char *icon_path) {
    GtkWidget *taskbar = g_object_get_data(G_OBJECT(gtk_widget_get_toplevel(GTK_WIDGET(taskbar))), "taskbar");

    GtkWidget *button = gtk_button_new();
    gtk_widget_set_name(button, "taskbar_button");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *icon = load_icon(icon_path);
    GtkWidget *label = gtk_label_new(app_name);

    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(button), box);

    gtk_box_pack_start(GTK_BOX(taskbar), button, FALSE, FALSE, 5);
    gtk_widget_show_all(button);
}

void show_desktop() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *desktop = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), desktop);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "style.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    load_applications(desktop);

    GtkWidget *taskbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(taskbar, 800, 40);
    gtk_fixed_put(GTK_FIXED(desktop), taskbar, 0, 560);
    gtk_widget_set_name(taskbar, "taskbar");
    g_object_set_data(G_OBJECT(window), "taskbar", taskbar);

    GtkWidget *start_button = gtk_button_new_with_label("Start");
    gtk_widget_set_name(start_button, "start_button");
    gtk_box_pack_start(GTK_BOX(taskbar), start_button, FALSE, FALSE, 5);

    GtkWidget *start_menu = create_start_menu(window);
    g_signal_connect(start_button, "clicked", G_CALLBACK(show_start_menu), start_menu);

    GtkWidget *clock_label = gtk_label_new("00:00 AM");
    gtk_widget_set_name(clock_label, "clock_label");
    gtk_box_pack_end(GTK_BOX(taskbar), clock_label, FALSE, FALSE, 5);
    g_timeout_add(1000, (GSourceFunc)update_clock, GTK_LABEL(clock_label));

    GtkWidget *tray_button = gtk_button_new_with_label("Tray");
    gtk_box_pack_end(GTK_BOX(taskbar), tray_button, FALSE, FALSE, 5);
    g_signal_connect(tray_button, "clicked", G_CALLBACK(show_tray), NULL);

    GtkWidget *file_manager_button = gtk_button_new_with_label("File Manager");
    gtk_box_pack_start(GTK_BOX(taskbar), file_manager_button, FALSE, FALSE, 5);
    g_signal_connect(file_manager_button, "clicked", G_CALLBACK(open_file_manager), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}

// Main function start here

int main(int argc, char *argv[]) {
    // Set environment variables for GDK-Pixbuf loaders
    char *base_path = g_get_current_dir();
    char *loaders_cache_path = g_strdup_printf("%s/ZoraNT/Sys404/lib/loaders/loaders.cache", base_path);
    g_setenv("GDK_PIXBUF_MODULE_FILE", loaders_cache_path, TRUE);
    g_free(loaders_cache_path);

    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    g_print("Current working directory: %s\n", base_path);

#ifndef _WIN32
    // Set the GTK theme to Windows-95-master on Linux
    GtkSettings *settings = gtk_settings_get_default();
    if (settings) {
        g_object_set(settings, "gtk-theme-name", "Windows-95-master", NULL);
    }
#endif

    // Create splash screen
    GtkWidget *splash_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(splash_window), FALSE);
    gtk_window_set_default_size(GTK_WINDOW(splash_window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(splash_window), "Startup Screen");
    gtk_window_set_position(GTK_WINDOW(splash_window), GTK_WIN_POS_CENTER_ALWAYS);
    gtk_widget_set_app_paintable(splash_window, TRUE);
    gtk_widget_realize(splash_window);
    GdkScreen *screen = gtk_widget_get_screen(splash_window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual) {
        gtk_widget_set_visual(splash_window, visual);
    }
    char *image_path = g_strdup_printf("%s/ZoraNT/Sys404/zorant_splash.png", base_path);
    GtkWidget *image = gtk_image_new_from_file(image_path);
    g_free(image_path);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
    gtk_container_add(GTK_CONTAINER(splash_window), box);
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    gtk_widget_set_halign(image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(image, GTK_ALIGN_CENTER);
    gtk_widget_show_all(splash_window);
    play_startup_music(base_path);
    g_timeout_add_seconds(5, (GSourceFunc)close_splash, splash_window);
    gtk_main(); // Run the GTK loop for the splash screen

    // Initialize the desktop after the splash screen is closed
    show_desktop();

    g_free(base_path);
    return 0;
}

// Function to open NetDig.exe
void open_network_settings(GtkWidget *widget, gpointer data) {
    const char *netdig_path = "./ZoraNT/Programs(legacy)/NetDig.exe";
    g_print("Opening Network Settings: %s\n", netdig_path);

    GError *error = NULL;
#ifdef _WIN32
    // Launch NetDig.exe in a separate window on Windows
    if (!g_spawn_async(NULL, (gchar *[]){"cmd.exe", "/c", "start", netdig_path, NULL}, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
        g_print("Failed to open Network Settings: %s\n", error->message);
        g_error_free(error);
    }
#else
    // Launch NetDig.exe in a separate terminal on Linux
    if (!g_spawn_async(NULL, (gchar *[]){"xterm", "-e", netdig_path, NULL}, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, NULL, &error)) {
        g_print("Failed to open Network Settings: %s\n", error->message);
        g_error_free(error);
    }
#endif
}

// Function to load profiles from the Profile directory
void load_profiles(GtkWidget *profiles_menu) {
    DIR *dir = opendir("./ZoraNT/Profile");
    if (!dir) {
        perror("Failed to open the profiles directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files

        // Construct the profile path
        char profile_path[256];
        snprintf(profile_path, sizeof(profile_path), "./ZoraNT/Profile/%s", entry->d_name);

        // Load the profile image (default image for now)
        char icon_path[256];
        snprintf(icon_path, sizeof(icon_path), "./ZoraNT/Sys404/default_profile_image.png");

        GtkWidget *icon = load_icon(icon_path);
        GtkWidget *profile_item = gtk_image_menu_item_new_with_label(entry->d_name);
        gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(profile_item), icon);
        g_signal_connect(profile_item, "activate", G_CALLBACK(show_profile), g_strdup(profile_path));
        gtk_menu_shell_append(GTK_MENU_SHELL(profiles_menu), profile_item);
    }
    closedir(dir);

    // Add "Create New Profile" item
    GtkWidget *new_profile_item = gtk_menu_item_new_with_label("Create New Profile");
    g_signal_connect(new_profile_item, "activate", G_CALLBACK(create_new_profile), profiles_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(profiles_menu), new_profile_item);
}

// Function to show the profile window
void show_profile(GtkWidget *widget, gpointer profile_path) {
    const char *path = (const char *)profile_path;
    g_print("Opening profile from path: %s\n", path);

    // Create a new window for the profile
    GtkWidget *profile_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(profile_window), "Profile");
    gtk_window_set_default_size(GTK_WINDOW(profile_window), 400, 300);

    // Load profile information from JSON file
    char json_path[256];
    snprintf(json_path, sizeof(json_path), "%s/profile.json", path);
    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    if (!json_parser_load_from_file(parser, json_path, &error)) {
        g_printerr("Failed to load profile JSON: %s\n", error->message);
        g_error_free(error);
        g_object_unref(parser);
        gtk_widget_destroy(profile_window);
        return;
    }

    JsonNode *root = json_parser_get_root(parser);
    JsonObject *object = json_node_get_object(root);

    const char *name = json_object_get_string_member(object, "name");
    const char *bio = json_object_get_string_member(object, "bio");
    const char *image_path = json_object_get_string_member(object, "image");

    // Create widgets to display profile information
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(profile_window), vbox);

    GtkWidget *image = load_icon(image_path);
    gtk_box_pack_start(GTK_BOX(vbox), image, FALSE, FALSE, 5);

    GtkWidget *name_label = gtk_label_new(name);
    gtk_box_pack_start(GTK_BOX(vbox), name_label, FALSE, FALSE, 5);

    GtkWidget *bio_label = gtk_label_new(bio);
    gtk_box_pack_start(GTK_BOX(vbox), bio_label, FALSE, FALSE, 5);

    gtk_widget_show_all(profile_window);
    g_object_unref(parser);
    g_free(profile_path);
}

// Function to create a new profile
void create_new_profile(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create New Profile", NULL, GTK_DIALOG_MODAL,
                                                    "_Create", GTK_RESPONSE_ACCEPT,
                                                    "_Cancel", GTK_RESPONSE_REJECT,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "Profile Name");
    gtk_box_pack_start(GTK_BOX(content_area), name_entry, FALSE, FALSE, 5);

    GtkWidget *bio_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(bio_entry), "Bio");
    gtk_box_pack_start(GTK_BOX(content_area), bio_entry, FALSE, FALSE, 5);

    GtkWidget *file_chooser_button = gtk_file_chooser_button_new("Select Profile Image", GTK_FILE_CHOOSER_ACTION_OPEN);
    gtk_box_pack_start(GTK_BOX(content_area), file_chooser_button, FALSE, FALSE, 5);

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const gchar *profile_name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const gchar *bio = gtk_entry_get_text(GTK_ENTRY(bio_entry));
        gchar *image_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser_button));

        save_profile(profile_name, bio, image_path);

        // Reload profiles menu
        GtkWidget *profiles_menu = GTK_WIDGET(data);
        GList *children = gtk_container_get_children(GTK_CONTAINER(profiles_menu));
        for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);
        load_profiles(profiles_menu);

        g_free(image_path);
    }

    gtk_widget_destroy(dialog);
}

// Function to save a new profile
void save_profile(const char *profile_name, const char *bio, const char *image_path) {
    char profile_dir[256];
    snprintf(profile_dir, sizeof(profile_dir), "./ZoraNT/Profile/%s", profile_name);
    #ifdef _WIN32
        mkdir(profile_dir); // Windows version of mkdir
    #else
        mkdir(profile_dir, 0755); // Linux version of mkdir with permissions
    #endif

    char json_path[256];
    snprintf(json_path, sizeof(json_path), "%s/profile.json", profile_dir);

    JsonBuilder *builder = json_builder_new();
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "name");
    json_builder_add_string_value(builder, profile_name);
    json_builder_set_member_name(builder, "bio");
    json_builder_add_string_value(builder, bio);
    json_builder_set_member_name(builder, "image");
    json_builder_add_string_value(builder, image_path);
    json_builder_end_object(builder);

    JsonGenerator *gen = json_generator_new();
    JsonNode *root = json_builder_get_root(builder);
    json_generator_set_root(gen, root);
    json_generator_to_file(gen, json_path, NULL);

    g_object_unref(gen);
    json_node_free(root);
    g_object_unref(builder);

    // Copy the selected image to the profile directory
    char dest_image_path[256];
    snprintf(dest_image_path, sizeof(dest_image_path), "%s/profile_image.png", profile_dir);
    g_file_copy(g_file_new_for_path(image_path), g_file_new_for_path(dest_image_path), G_FILE_COPY_NONE, NULL, NULL, NULL, NULL);
}