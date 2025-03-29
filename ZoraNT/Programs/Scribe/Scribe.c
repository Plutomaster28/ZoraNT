#include <gtk/gtk.h>

// Function declarations
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_save_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_bold_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_italic_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_underline_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_font_size_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_cut_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_copy_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_paste_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_about_activate(GtkMenuItem *menuitem, gpointer user_data);

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

void on_bold_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(text_buffer, &start, &end);
    gtk_text_buffer_apply_tag_by_name(text_buffer, "bold", &start, &end);
}

void on_italic_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(text_buffer, &start, &end);
    gtk_text_buffer_apply_tag_by_name(text_buffer, "italic", &start, &end);
}

void on_underline_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(text_buffer, &start, &end);
    gtk_text_buffer_apply_tag_by_name(text_buffer, "underline", &start, &end);
}

void on_font_size_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkTextIter start, end;
    gtk_text_buffer_get_selection_bounds(text_buffer, &start, &end);
    gtk_text_buffer_apply_tag_by_name(text_buffer, "font_size", &start, &end);
}

void on_cut_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_cut_clipboard(text_buffer, clipboard, TRUE);
}

void on_copy_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_copy_clipboard(text_buffer, clipboard);
}

void on_paste_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_paste_clipboard(text_buffer, clipboard, NULL, TRUE);
}

void on_about_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "WordPad\nVersion 1.0\n\nA simple text editor.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Document - WordPad");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu bar, toolbar, text view, and status bar
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

    GtkWidget *cut_item = gtk_menu_item_new_with_label("Cut");
    g_signal_connect(cut_item, "activate", G_CALLBACK(on_cut_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_item);

    GtkWidget *copy_item = gtk_menu_item_new_with_label("Copy");
    g_signal_connect(copy_item, "activate", G_CALLBACK(on_copy_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_item);

    GtkWidget *paste_item = gtk_menu_item_new_with_label("Paste");
    g_signal_connect(paste_item, "activate", G_CALLBACK(on_paste_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_item);

    GtkWidget *view_menu = gtk_menu_new();
    GtkWidget *view_item = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_item), view_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view_item);

    GtkWidget *insert_menu = gtk_menu_new();
    GtkWidget *insert_item = gtk_menu_item_new_with_label("Insert");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(insert_item), insert_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), insert_item);

    GtkWidget *format_menu = gtk_menu_new();
    GtkWidget *format_item = gtk_menu_item_new_with_label("Format");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format_item), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), format_item);

    GtkWidget *bold_item = gtk_menu_item_new_with_label("Bold");
    g_signal_connect(bold_item, "activate", G_CALLBACK(on_bold_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), bold_item);

    GtkWidget *italic_item = gtk_menu_item_new_with_label("Italic");
    g_signal_connect(italic_item, "activate", G_CALLBACK(on_italic_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), italic_item);

    GtkWidget *underline_item = gtk_menu_item_new_with_label("Underline");
    g_signal_connect(underline_item, "activate", G_CALLBACK(on_underline_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), underline_item);

    GtkWidget *font_size_item = gtk_menu_item_new_with_label("Font Size");
    g_signal_connect(font_size_item, "activate", G_CALLBACK(on_font_size_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), font_size_item);

    GtkWidget *help_menu = gtk_menu_new();
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help_item);

    GtkWidget *about_item = gtk_menu_item_new_with_label("About");
    g_signal_connect(about_item, "activate", G_CALLBACK(on_about_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), about_item);

    // Create the toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    GtkToolItem *new_button = gtk_tool_button_new(NULL, "New");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(new_button), "document-new");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_button, -1);

    GtkToolItem *open_button = gtk_tool_button_new(NULL, "Open");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(open_button), "document-open");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open_button, -1);

    GtkToolItem *save_button = gtk_tool_button_new(NULL, "Save");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save_button), "document-save");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save_button, -1);

    GtkToolItem *cut_button = gtk_tool_button_new(NULL, "Cut");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(cut_button), "edit-cut");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), cut_button, -1);

    GtkToolItem *copy_button = gtk_tool_button_new(NULL, "Copy");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(copy_button), "edit-copy");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), copy_button, -1);

    GtkToolItem *paste_button = gtk_tool_button_new(NULL, "Paste");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(paste_button), "edit-paste");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), paste_button, -1);

    GtkToolItem *bold_button = gtk_toggle_tool_button_new();
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(bold_button), "format-text-bold");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), bold_button, -1);

    GtkToolItem *italic_button = gtk_toggle_tool_button_new();
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(italic_button), "format-text-italic");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), italic_button, -1);

    GtkToolItem *underline_button = gtk_toggle_tool_button_new();
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(underline_button), "format-text-underline");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), underline_button, -1);

    GtkWidget *font_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Sans");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Serif");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(font_combo), "Monospace");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_tool_item_new(), -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_tool_item_new(), -1);

    GtkWidget *font_size_spin = gtk_spin_button_new_with_range(8, 72, 1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_tool_item_new(), -1);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), gtk_tool_item_new(), -1);

    // Create the text view
    text_view = gtk_text_view_new();
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Create text tags for formatting
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(text_buffer);
    gtk_text_tag_table_add(tag_table, gtk_text_tag_new("bold"));
    gtk_text_tag_table_add(tag_table, gtk_text_tag_new("italic"));
    gtk_text_tag_table_add(tag_table, gtk_text_tag_new("underline"));
    gtk_text_tag_table_add(tag_table, gtk_text_tag_new("font_size"));

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