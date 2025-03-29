#include <gtk/gtk.h>
#include <gst/gst.h>

// Function declarations
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_play_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_pause_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_stop_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_volume_changed(GtkScaleButton *scale, gdouble value, gpointer user_data);
void on_seek_changed(GtkRange *range, gpointer user_data);

// Global variables
GtkWidget *window;
GtkWidget *video_area;
GtkWidget *statusbar;
GtkWidget *seek_bar;
guint context_id;
GstElement *pipeline;
GstElement *video_sink;
gboolean is_playing = FALSE;

void on_open_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        if (filename) {
            g_print("Selected file: %s\n", filename);

            gst_element_set_state(pipeline, GST_STATE_NULL);

            // Convert Windows file path to URI format
            gchar *uri = g_strdup_printf("file:///%s", filename);
            for (char *p = uri; *p; p++) {
                if (*p == '\\') {
                    *p = '/';
                }
            }

            g_print("URI: %s\n", uri);
            g_object_set(pipeline, "uri", uri, NULL);
            g_free(uri);

            gst_element_set_state(pipeline, GST_STATE_PLAYING);
            is_playing = TRUE;

            g_free(filename);
        } else {
            g_print("No file selected.\n");
        }
    }

    gtk_widget_destroy(dialog);
}

void on_play_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (!is_playing) {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        is_playing = TRUE;
    }
}

void on_pause_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (is_playing) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        is_playing = FALSE;
    }
}

void on_stop_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    is_playing = FALSE;
}

void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_main_quit();
}

void on_volume_changed(GtkScaleButton *scale, gdouble value, gpointer user_data) {
    g_object_set(pipeline, "volume", value, NULL);
}

void on_seek_changed(GtkRange *range, gpointer user_data) {
    gdouble value = gtk_range_get_value(range);
    gst_element_seek_simple(pipeline, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, (gint64)(value * GST_SECOND));
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            gst_element_set_state(pipeline, GST_STATE_NULL);
            is_playing = FALSE;
            break;
        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            gst_element_set_state(pipeline, GST_STATE_NULL);
            is_playing = FALSE;
            break;
        }
        default:
            break;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Media Player");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu bar, toolbar, video area, seek bar, and status bar
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the menu bar
    GtkWidget *menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

    GtkWidget *open_item = gtk_menu_item_new_with_label("Open");
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);

    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    GtkWidget *control_menu = gtk_menu_new();
    GtkWidget *control_item = gtk_menu_item_new_with_label("Control");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(control_item), control_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), control_item);

    GtkWidget *play_item = gtk_menu_item_new_with_label("Play");
    g_signal_connect(play_item, "activate", G_CALLBACK(on_play_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), play_item);

    GtkWidget *pause_item = gtk_menu_item_new_with_label("Pause");
    g_signal_connect(pause_item, "activate", G_CALLBACK(on_pause_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), pause_item);

    GtkWidget *stop_item = gtk_menu_item_new_with_label("Stop");
    g_signal_connect(stop_item, "activate", G_CALLBACK(on_stop_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), stop_item);

    // Create the toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    GtkToolItem *back_button = gtk_tool_button_new(gtk_image_new_from_icon_name("go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR), "Back");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), back_button, -1);

    GtkToolItem *forward_button = gtk_tool_button_new(gtk_image_new_from_icon_name("go-next", GTK_ICON_SIZE_LARGE_TOOLBAR), "Forward");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), forward_button, -1);

    GtkToolItem *radio_button = gtk_tool_button_new(gtk_image_new_from_icon_name("media-playlist-repeat", GTK_ICON_SIZE_LARGE_TOOLBAR), "Radio");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), radio_button, -1);

    GtkToolItem *music_button = gtk_tool_button_new(gtk_image_new_from_icon_name("audio-x-generic", GTK_ICON_SIZE_LARGE_TOOLBAR), "Music");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), music_button, -1);

    GtkToolItem *media_guide_button = gtk_tool_button_new(gtk_image_new_from_icon_name("help-browser", GTK_ICON_SIZE_LARGE_TOOLBAR), "Media Guide");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), media_guide_button, -1);

    // Create the video area
    video_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(video_area, 800, 450);
    gtk_box_pack_start(GTK_BOX(vbox), video_area, TRUE, TRUE, 0);

    // Create the playback controls
    GtkWidget *hbox_controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_controls, FALSE, FALSE, 0);

    GtkWidget *stop_button = gtk_button_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop_activate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_controls), stop_button, FALSE, FALSE, 0);

    GtkWidget *prev_button = gtk_button_new_from_icon_name("media-skip-backward", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox_controls), prev_button, FALSE, FALSE, 0);

    GtkWidget *play_pause_button = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_BUTTON);
    g_signal_connect(play_pause_button, "clicked", G_CALLBACK(on_play_activate), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_controls), play_pause_button, FALSE, FALSE, 0);

    GtkWidget *next_button = gtk_button_new_from_icon_name("media-skip-forward", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox_controls), next_button, FALSE, FALSE, 0);

    GtkWidget *fast_forward_button = gtk_button_new_from_icon_name("media-seek-forward", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox_controls), fast_forward_button, FALSE, FALSE, 0);

    GtkWidget *rewind_button = gtk_button_new_from_icon_name("media-seek-backward", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox_controls), rewind_button, FALSE, FALSE, 0);

    GtkWidget *mute_button = gtk_button_new_from_icon_name("audio-volume-muted", GTK_ICON_SIZE_BUTTON);
    gtk_box_pack_start(GTK_BOX(hbox_controls), mute_button, FALSE, FALSE, 0);

    GtkWidget *volume_slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 1.0, 0.1);
    gtk_range_set_value(GTK_RANGE(volume_slider), 0.5);
    g_signal_connect(volume_slider, "value-changed", G_CALLBACK(on_volume_changed), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_controls), volume_slider, TRUE, TRUE, 0);

    // Create the seek bar
    seek_bar = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    g_signal_connect(seek_bar, "value-changed", G_CALLBACK(on_seek_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), seek_bar, FALSE, FALSE, 0);

    // Create the status bar
    statusbar = gtk_statusbar_new();
    context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Statusbar");
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    // Create the bottom information panel
    GtkWidget *info_grid = gtk_grid_new();
    gtk_box_pack_start(GTK_BOX(vbox), info_grid, FALSE, FALSE, 0);

    GtkWidget *show_label = gtk_label_new("Show:");
    gtk_grid_attach(GTK_GRID(info_grid), show_label, 0, 0, 1, 1);

    GtkWidget *clip_label = gtk_label_new("Clip:");
    gtk_grid_attach(GTK_GRID(info_grid), clip_label, 0, 1, 1, 1);

    GtkWidget *author_label = gtk_label_new("Author:");
    gtk_grid_attach(GTK_GRID(info_grid), author_label, 0, 2, 1, 1);

    GtkWidget *copyright_label = gtk_label_new("Copyright:");
    gtk_grid_attach(GTK_GRID(info_grid), copyright_label, 0, 3, 1, 1);

    GtkWidget *show_value = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(info_grid), show_value, 1, 0, 1, 1);

    GtkWidget *clip_value = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(info_grid), clip_value, 1, 1, 1, 1);

    GtkWidget *author_value = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(info_grid), author_value, 1, 2, 1, 1);

    GtkWidget *copyright_value = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(info_grid), copyright_value, 1, 3, 1, 1);

    // Initialize GStreamer
    pipeline = gst_element_factory_make("playbin", "player");
    video_sink = gst_element_factory_make("gtksink", "video_sink");
    g_object_set(pipeline, "video-sink", video_sink, NULL);

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, NULL);
    gst_object_unref(bus);

    gtk_widget_show_all(window);

    gtk_main();

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return 0;
}