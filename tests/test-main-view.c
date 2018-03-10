#include <stdlib.h>

#include <gtk/gtk.h>

#include <cairo-gobject.h>
#include <libgd/gd-main-view.h>
#include <libgd/gd-main-view-generic.h>

static void
on_selection_mode_request (gpointer user_data)
{
  gd_main_view_set_selection_mode (user_data, TRUE);
}

static void
on_view_button_clicked (GtkButton *button,
                        gpointer   user_data)
{
  GdMainView *main_view;

  main_view = user_data;

  if (gd_main_view_get_view_type (main_view) == GD_MAIN_VIEW_ICON)
    {
      gd_main_view_set_view_type (main_view, GD_MAIN_VIEW_LIST);
      gtk_button_set_label (button, "Icon View");
    }
  else
    {
      gd_main_view_set_view_type (main_view, GD_MAIN_VIEW_ICON);
      gtk_button_set_label (button, "List View");
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
  GType column_types[] =
    {
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      CAIRO_GOBJECT_TYPE_SURFACE,
      G_TYPE_INT64,
      G_TYPE_BOOLEAN,
      G_TYPE_UINT
    };
  g_autoptr (GtkListStore) list_store = NULL;
  g_autoptr (GtkIconInfo) icon_info = NULL;
  cairo_surface_t *surface;
  GdMainView *main_view;
  GtkWidget *selection_mode_toggle_button;
  GtkWidget *view_type_button;
  GtkWidget *button_box;
  GtkWidget *grid;
  GtkWidget *window;

  gtk_init ();

  list_store = gtk_list_store_new (1, G_TYPE_STRING);
  gtk_list_store_set_column_types (list_store, G_N_ELEMENTS (column_types), column_types);

  icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (),
                                          "text-x-generic", 48, 0);
  surface = gtk_icon_info_load_surface (icon_info, NULL, NULL);

  for (int i = 0; i < 64; i++)
    {
      GtkTreeIter iter;

      gtk_list_store_append (list_store, &iter);

      gtk_list_store_set (list_store, &iter,
                          GD_MAIN_COLUMN_ID, NULL,
                          GD_MAIN_COLUMN_URI, "file:///dev/null",
                          GD_MAIN_COLUMN_PRIMARY_TEXT, "Primary text",
                          GD_MAIN_COLUMN_SECONDARY_TEXT, "Secondary text",
                          GD_MAIN_COLUMN_ICON, surface,
                          GD_MAIN_COLUMN_MTIME, -1,
                          GD_MAIN_COLUMN_SELECTED, FALSE,
                          GD_MAIN_COLUMN_PULSE, i,
                          -1);
    }

  main_view = gd_main_view_new (GD_MAIN_VIEW_ICON);
  g_signal_connect (main_view, "selection-mode-request",
                    G_CALLBACK (on_selection_mode_request), main_view);
  gd_main_view_set_model (main_view, GTK_TREE_MODEL (list_store));

  selection_mode_toggle_button = gtk_toggle_button_new_with_label ("Selection Mode");
  g_object_bind_property (selection_mode_toggle_button, "active",
                          main_view, "selection-mode",
                          G_BINDING_BIDIRECTIONAL);

  view_type_button = gtk_button_new_with_label ("List View");
  g_signal_connect (view_type_button, "clicked",
                    G_CALLBACK (on_view_button_clicked), main_view);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (button_box), selection_mode_toggle_button);
  gtk_container_add (GTK_CONTAINER (button_box), view_type_button);

  grid = gtk_grid_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
  gtk_container_add (GTK_CONTAINER (grid), GTK_WIDGET (main_view));
  gtk_container_add (GTK_CONTAINER (grid), button_box);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (on_destroy), NULL);
  gtk_container_add (GTK_CONTAINER (window), grid);
  gtk_widget_set_size_request (window, 640, 480);
  gtk_widget_show (window);

  gtk_main ();

  cairo_surface_destroy (surface);

  return EXIT_SUCCESS;
}
