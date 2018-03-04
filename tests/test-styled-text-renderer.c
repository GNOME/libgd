#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgd/gd-styled-text-renderer.h>

static const gchar *test_data = "Lorem ipsum dolor sit amet, consectetur "
                                "adipiscing elit, sed do eiusmod tempor "
                                "incididunt ut labore et dolore magna aliqua. "
                                "Ut enim ad minim veniam, quis nostrud "
                                "exercitation ullamco laboris nisi ut aliquip "
                                "ex ea commodo consequat. Duis aute irure "
                                "dolor in reprehenderit in voluptate velit "
                                "esse cillum dolore eu fugiat nulla pariatur. "
                                "Excepteur sint occaecat cupidatat non "
                                "proident, sunt in culpa qui officia deserunt "
                                "mollit anim id est laborum.";

static const gchar *test_css = ".test-class {"
                               "  color: mix(@theme_fg_color, @theme_bg_color, 0.50);"
                               "}"
                               ".test-class:selected {"
                               "  color: rgb(0, 255, 0);"
                               "}";

static void
on_parsing_error (GtkCssProvider *provider,
                  GtkCssSection  *section,
                  GError         *error,
                  gpointer        user_data)
{
  g_error ("Parsing CSS failed at %u:%u – %u:%u: %s",
           gtk_css_section_get_start_line (section),
           gtk_css_section_get_start_position (section),
           gtk_css_section_get_end_line (section),
           gtk_css_section_get_end_position (section),
           error->message);
}

static void
populate_model (GtkListStore *list_store)
{
  g_auto (GStrv) data = NULL;

  data = g_strsplit (test_data, " ", -1);

  for (int i = 0; data[i] != NULL; i++)
    {
      GtkTreeIter iter;

      gtk_list_store_append (list_store, &iter);

      gtk_list_store_set (list_store, &iter, 0, data[i], -1);
    }
}

static void
on_destroy (GtkWidget *widget,
            gpointer   user_data)
{
  gtk_main_quit ();
}

gint
main (gint  argc,
      GStrv argv)
{
  g_autoptr (GtkCssProvider) css_provider = NULL;
  GdkDisplay *display;
  GtkCellRenderer *cell_renderer;
  GtkTreeViewColumn *column;
  g_autoptr (GtkListStore) list_store = NULL;
  GtkWidget *tree_view;
  GtkWidget *scrolled_window;
  GtkWidget *window;

  gtk_init ();

  css_provider = gtk_css_provider_new ();
  g_signal_connect (css_provider, "parsing-error", G_CALLBACK (on_parsing_error), NULL);
  gtk_css_provider_load_from_data (css_provider, test_css, -1);

  display = gdk_display_get_default ();
  gtk_style_context_add_provider_for_display (display, GTK_STYLE_PROVIDER (css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);

  cell_renderer = gd_styled_text_renderer_new ();
  gd_styled_text_renderer_add_class (GD_STYLED_TEXT_RENDERER (cell_renderer),
                                     "test-class");

  column = gtk_tree_view_column_new_with_attributes ("Text", cell_renderer,
                                                     "text", 0,
                                                     NULL);

  list_store = gtk_list_store_new (1, G_TYPE_STRING);
  populate_model (list_store);

  tree_view = gtk_tree_view_new ();
  gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view), column);
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (list_store));

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_window), tree_view);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (on_destroy), NULL);
  gtk_container_add (GTK_CONTAINER (window), scrolled_window);
  gtk_widget_set_size_request (window, 640, 480);
  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}
