#include <gtk/gtk.h>
#include <libgd/gd-header-bar.h>

gint
main (gint argc,
      gchar ** argv)
{
  GtkWidget *window, *bar, *box, *button;

  gtk_init (&argc, &argv);

  if (g_getenv ("RTL"))
    gtk_widget_set_default_direction (GTK_TEXT_DIR_RTL);
  else
    gtk_widget_set_default_direction (GTK_TEXT_DIR_LTR);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 300, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), box);

  bar = gd_header_bar_new ("Shells Shells Shells Shells Shells");
  gtk_box_pack_start (GTK_BOX (box), bar, FALSE, TRUE, 0);

  button = gtk_button_new_with_label ("Forget");
  gtk_widget_set_valign (button, GTK_ALIGN_CENTER);
  gtk_style_context_add_class (gtk_widget_get_style_context (button), GTK_STYLE_CLASS_RAISED);
  gd_header_bar_pack_start (GD_HEADER_BAR (bar), button);

  button = gtk_button_new_with_label ("Done");
  gtk_widget_set_valign (button, GTK_ALIGN_CENTER);
  gtk_style_context_add_class (gtk_widget_get_style_context (button), "suggested-action");

  gd_header_bar_pack_end (GD_HEADER_BAR (bar), button);

  button = gtk_button_new_with_label ("Almost");
  gtk_widget_set_valign (button, GTK_ALIGN_CENTER);
  gtk_style_context_add_class (gtk_widget_get_style_context (button), GTK_STYLE_CLASS_RAISED);
  gd_header_bar_pack_end (GD_HEADER_BAR (bar), button);

  gtk_widget_show_all (window);
  gtk_main ();

  gtk_widget_destroy (window);

  return 0;
}
