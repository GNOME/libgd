#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgd/gd-notification.h>

static void
on_dismissed (GdNotification *notification,
              gpointer        user_data)
{
  gtk_widget_set_sensitive (GTK_WIDGET (user_data), TRUE);
}

static void
on_clicked (GtkButton *button,
            gpointer   user_data)
{
  GtkWidget *label;
  GtkWidget *notification;

  label = gtk_label_new ("This is a test.");

  notification = gd_notification_new ();
  gd_notification_set_timeout (GD_NOTIFICATION (notification), 3);
  gtk_container_add (GTK_CONTAINER (notification), label);
  gtk_widget_show (notification);

  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  gtk_overlay_add_overlay (GTK_OVERLAY (user_data), notification);

  g_signal_connect (notification, "dismissed",
                    G_CALLBACK (on_dismissed), button);
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
  GtkWidget *text_view;
  GtkWidget *show_notification_button;
  GtkWidget *button_box;
  GtkWidget *grid;
  GtkWidget *overlay;
  GtkWidget *window;

  gtk_init ();

  text_view = gtk_text_view_new ();
  gtk_widget_set_hexpand (text_view, TRUE);
  gtk_widget_set_vexpand (text_view, TRUE);

  show_notification_button = gtk_button_new_with_label ("Show Notification");

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (button_box), show_notification_button);

  grid = gtk_grid_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
  gtk_container_add (GTK_CONTAINER (grid), text_view);
  gtk_container_add (GTK_CONTAINER (grid), button_box);

  overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (overlay), grid);

  g_signal_connect (show_notification_button, "clicked", G_CALLBACK (on_clicked), overlay);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (on_destroy), NULL);
  gtk_container_add (GTK_CONTAINER (window), overlay);
  gtk_widget_set_size_request (window, 640, 480);
  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}
