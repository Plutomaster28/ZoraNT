#include <gtk/gtk.h>
#include <cairo.h>

// Function declarations
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_save_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_color_picker_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_clear_activate(GtkMenuItem *menuitem, gpointer user_data);
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);

// Global variables
GtkWidget *window;
GtkWidget *drawing_area;
GdkRGBA current_color = {0, 0, 0, 1}; // Default color is black
gboolean drawing = FALSE;
gdouble last_x = 0, last_y = 0;

void on_open_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open Image",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        if (pixbuf) {
            GdkWindow *window = gtk_widget_get_window(drawing_area);
            cairo_region_t *region = cairo_region_create();
            GdkDrawingContext *context = gdk_window_begin_draw_frame(window, region);
            cairo_t *cr = gdk_drawing_context_get_cairo_context(context);

            gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
            cairo_paint(cr);

            gdk_window_end_draw_frame(window, context);
            cairo_region_destroy(region);
            g_object_unref(pixbuf);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_save_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save Image",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Save", GTK_RESPONSE_ACCEPT,
                                         NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                              gtk_widget_get_allocated_width(drawing_area),
                                                              gtk_widget_get_allocated_height(drawing_area));
        cairo_t *cr = cairo_create(surface);
        gtk_widget_draw(drawing_area, cr);
        cairo_surface_write_to_png(surface, filename);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_main_quit();
}

void on_color_picker_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog = gtk_color_chooser_dialog_new("Select Color", GTK_WINDOW(window));
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(dialog), &current_color);
    }
    gtk_widget_destroy(dialog);
}

void on_clear_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GdkWindow *window = gtk_widget_get_window(drawing_area);
    cairo_region_t *region = cairo_region_create();
    GdkDrawingContext *context = gdk_window_begin_draw_frame(window, region);
    cairo_t *cr = gdk_drawing_context_get_cairo_context(context);

    cairo_set_source_rgb(cr, 1, 1, 1); // White background
    cairo_paint(cr);

    gdk_window_end_draw_frame(window, context);
    cairo_region_destroy(region);
}

gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    // Draw the current state of the drawing area
    return FALSE;
}

gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->button == GDK_BUTTON_PRIMARY) {
        drawing = TRUE;
        last_x = event->x;
        last_y = event->y;
    }
    return TRUE;
}

gboolean on_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
    if (drawing) {
        GdkWindow *window = gtk_widget_get_window(widget);
        cairo_region_t *region = cairo_region_create();
        GdkDrawingContext *context = gdk_window_begin_draw_frame(window, region);
        cairo_t *cr = gdk_drawing_context_get_cairo_context(context);

        cairo_set_source_rgba(cr, current_color.red, current_color.green, current_color.blue, current_color.alpha);
        cairo_set_line_width(cr, 2.0);
        cairo_move_to(cr, last_x, last_y);
        cairo_line_to(cr, event->x, event->y);
        cairo_stroke(cr);

        gdk_window_end_draw_frame(window, context);
        cairo_region_destroy(region);

        last_x = event->x;
        last_y = event->y;
    }
    return TRUE;
}

gboolean on_button_release_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    if (event->button == GDK_BUTTON_PRIMARY) {
        drawing = FALSE;
    }
    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Paint");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu bar and drawing area
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

    GtkWidget *save_item = gtk_menu_item_new_with_label("Save");
    g_signal_connect(save_item, "activate", G_CALLBACK(on_save_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);

    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    GtkWidget *edit_menu = gtk_menu_new();
    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit_item);

    GtkWidget *color_picker_item = gtk_menu_item_new_with_label("Color Picker");
    g_signal_connect(color_picker_item, "activate", G_CALLBACK(on_color_picker_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), color_picker_item);

    GtkWidget *clear_item = gtk_menu_item_new_with_label("Clear");
    g_signal_connect(clear_item, "activate", G_CALLBACK(on_clear_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), clear_item);

    // Create the drawing area
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 800, 600);
    gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(drawing_area, "motion-notify-event", G_CALLBACK(on_motion_notify_event), NULL);
    g_signal_connect(drawing_area, "button-release-event", G_CALLBACK(on_button_release_event), NULL);

    gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area)
                                        | GDK_BUTTON_PRESS_MASK
                                        | GDK_POINTER_MOTION_MASK
                                        | GDK_BUTTON_RELEASE_MASK);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}