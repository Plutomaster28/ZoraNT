#include <gtk/gtk.h>
#include <glib.h>

// Function to get system specifications
void get_system_specs(GtkWidget *label) {
    gchar *specs = g_strdup_printf(
        "Operating System: %s\n"
        "GLib Version: %d.%d.%d\n",
        g_get_os_info(G_OS_INFO_KEY_NAME),
        GLIB_MAJOR_VERSION,
        GLIB_MINOR_VERSION,
        GLIB_MICRO_VERSION
    );

    gtk_label_set_text(GTK_LABEL(label), specs);
    g_free(specs);
}

// Function to show credits
void show_credits(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Credits:\n\n"
                                               "ZoraNT Development Team\n"
                                               "Lead Developer: Tomoko Saito\n"
                                               "Contributors: Adita, Nub\n"
                                               "Special Thanks: Nanomi Community");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "My Computer");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a label to display system specifications
    GtkWidget *specs_label = gtk_label_new(NULL);
    get_system_specs(specs_label);
    gtk_box_pack_start(GTK_BOX(vbox), specs_label, TRUE, TRUE, 5);

    // Create a label to display the current version of ZoraNT
    GtkWidget *version_label = gtk_label_new("ZoraNT Version: 1.0.0");
    gtk_box_pack_start(GTK_BOX(vbox), version_label, TRUE, TRUE, 5);

    // Create a button to show credits
    GtkWidget *credits_button = gtk_button_new_with_label("Credits");
    g_signal_connect(credits_button, "clicked", G_CALLBACK(show_credits), window);
    gtk_box_pack_start(GTK_BOX(vbox), credits_button, TRUE, TRUE, 5);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}