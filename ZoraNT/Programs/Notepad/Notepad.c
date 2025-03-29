#include <gtk/gtk.h>

// Function declarations
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_save_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_text_changed(GtkTextBuffer *textbuffer, gpointer user_data);

// Global variables
GtkWidget *window;
GtkWidget *text_view;
GtkTextBuffer *text_buffer;
GtkWidget *statusbar;
guint context_id;

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

        gchar *content;
        gsize length;
        if (g_file_get_contents(filename, &content, &length, NULL)) {
            gtk_text_buffer_set_text(text_buffer, content, length);
            g_free(content);
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_save_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Save File",
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

        GtkTextIter start, end;
        gtk_text_buffer_get_start_iter(text_buffer, &start);
        gtk_text_buffer_get_end_iter(text_buffer, &end);
        gchar *text = gtk_text_buffer_get_text(text_buffer, &start, &end, FALSE);

        g_file_set_contents(filename, text, -1, NULL);
        g_free(text);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_main_quit();
}

void on_text_changed(GtkTextBuffer *textbuffer, gpointer user_data) {
    gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id, "Text changed");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Notepad");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu bar, text view, and status bar
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

    // Create the text view
    text_view = gtk_text_view_new();
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    g_signal_connect(text_buffer, "changed", G_CALLBACK(on_text_changed), NULL);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    // Create the status bar
    statusbar = gtk_statusbar_new();
    context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Statusbar");
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}