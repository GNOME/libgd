#include <gtk/gtk.h>
#include <libgd/gd-bubble-button.h>

gint
main (gint argc,
      gchar ** argv)
{
  GtkWidget *window, *box, *button, *widget;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 300, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), box);

  widget = gtk_label_new ("hello");
  gtk_widget_show (widget);

  button = gd_bubble_button_new ();
  gd_bubble_button_set_widget (GD_BUBBLE_BUTTON (button), widget);

  widget = gtk_label_new ("BUTTON");
  gtk_container_add (GTK_CONTAINER (button), widget);

  gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 0);

  gtk_widget_show_all (window);
  gtk_main ();

  gtk_widget_destroy (window);

  return 0;
}
