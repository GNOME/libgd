#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgd/gd-two-lines-renderer.h>

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

      if (data[i + 1] != NULL)
        {
          i++;

          gtk_list_store_set (list_store, &iter, 1, data[i], -1);
        }
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
  GtkCellRenderer *cell_renderer;
  GtkTreeViewColumn *column;
  g_autoptr (GtkListStore) list_store = NULL;
  GtkWidget *tree_view;
  GtkWidget *scrolled_window;
  GtkWidget *window;

  gtk_init ();

  cell_renderer = gd_two_lines_renderer_new ();

  column = gtk_tree_view_column_new_with_attributes ("Text", cell_renderer,
                                                     "text", 0,
                                                     "line-two", 1,
                                                     NULL);

  list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
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
