#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgd/gd-margin-container.h>

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
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;
  GdMarginContainer *margin_container;

  gtk_init ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  g_signal_connect (window, "destroy", G_CALLBACK (on_destroy), NULL);
  gtk_widget_show (window);

  grid = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (window), grid);

  gtk_widget_set_halign (grid, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (grid, GTK_ALIGN_CENTER);

  margin_container = gd_margin_container_new ();
  gtk_grid_attach (GTK_GRID (grid), GTK_WIDGET (margin_container), 0, 1, 1, 1);

  g_object_set (margin_container, "min-margin", 6, "max-margin", 64, NULL);

  button = gtk_button_new_with_label ("Left");
  gtk_container_add (GTK_CONTAINER (margin_container), button);

  button = gtk_button_new_with_label ("Top");
  gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label ("Right");
  gtk_grid_attach (GTK_GRID (grid), button, 2, 1, 1, 1);

  button = gtk_button_new_with_label ("Bottom");
  gtk_grid_attach (GTK_GRID (grid), button, 1, 2, 1, 1);

  margin_container = gd_margin_container_new ();
  gtk_grid_attach (GTK_GRID (grid), GTK_WIDGET (margin_container), 1, 1, 1, 1);

  g_object_set (margin_container,
                "min-margin", 6,
                "max-margin", 64,
                "orientation", GTK_ORIENTATION_VERTICAL,
                NULL);

  button = gtk_button_new_with_label ("Middle");
  gtk_container_add (GTK_CONTAINER (margin_container), button);

  gtk_main ();

  return EXIT_SUCCESS;
}
