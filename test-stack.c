#include <gtk/gtk.h>
#include <libgd/gd-stack.h>
#include <libgd/gd-stack-switcher.h>

GtkWidget *stack;
GtkWidget *switcher;
GtkWidget *b1;

static void
set_visible_child (GtkWidget *button, gpointer data)
{
  gd_stack_set_visible_child (GD_STACK (stack), GTK_WIDGET (data));
}

static void
set_visible_child_name (GtkWidget *button, gpointer data)
{
  gd_stack_set_visible_child_name (GD_STACK (stack), (const char *)data);
}

static void
toggle_homogeneous (GtkWidget *button, gpointer data)
{
  gboolean active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  g_print ("active %d\n", active);
  gd_stack_set_homogeneous (GD_STACK (stack), active);
}

static void
toggle_icon_name (GtkWidget *button, gpointer data)
{
  gboolean active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
  gtk_container_child_set (GTK_CONTAINER (stack), b1,
			   "symbolic-icon-name", active ? "edit-find-symbolic" : NULL,
			   NULL);
}

gint
main (gint argc,
      gchar ** argv)
{
  GtkWidget *window, *box, *button, *hbox;
  GtkWidget *b2, *b3;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 300, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), box);

  switcher = gd_stack_switcher_new ();
  gtk_box_pack_start (GTK_BOX (box), switcher, FALSE, FALSE, 0);

  stack = gd_stack_new ();
  gtk_widget_set_halign (stack, GTK_ALIGN_START);
  gtk_container_add (GTK_CONTAINER (box), stack);

  gd_stack_switcher_set_stack (GD_STACK_SWITCHER (switcher), GD_STACK (stack));

  b1 = gtk_button_new_with_label ("Blah");
  gtk_container_add_with_properties (GTK_CONTAINER (stack), b1,
				     "name", "1",
				     "title", "1",
				     NULL);

  b2 = gtk_button_new_with_label ("Gazoooooooooooooooonk");
  gtk_container_add (GTK_CONTAINER (stack), b2);
  gtk_container_child_set (GTK_CONTAINER (stack), b2,
			   "name", "2",
			   "title", "2",
			   NULL);

  b3 = gtk_button_new_with_label ("Foo\nBar");
  gd_stack_add_titled (GD_STACK (stack), b3, "3", "3");

  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_container_add (GTK_CONTAINER (box), hbox);

  button = gtk_button_new_with_label ("1");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child, b1);

  button = gtk_button_new_with_label ("2");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child, b2);

  button = gtk_button_new_with_label ("3");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child, b3);

  button = gtk_button_new_with_label ("1");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child_name, (gpointer) "1");

  button = gtk_button_new_with_label ("2");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child_name, (gpointer) "2");

  button = gtk_button_new_with_label ("3");
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) set_visible_child_name, (gpointer) "3");

  button = gtk_check_button_new_with_label ("homogeneous");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button),
				gd_stack_get_homogeneous (GD_STACK (stack)));
  gtk_container_add (GTK_CONTAINER (hbox), button);
  g_signal_connect (button, "clicked", (GCallback) toggle_homogeneous, NULL);

  button = gtk_toggle_button_new_with_label ("Add symbolic icon");
  g_signal_connect (button, "toggled", (GCallback) toggle_icon_name, NULL);
  gtk_container_add (GTK_CONTAINER (hbox), button);

  gtk_widget_show_all (window);
  gtk_main ();

  gtk_widget_destroy (window);

  return 0;
}
