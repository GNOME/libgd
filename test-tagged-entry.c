#include <gtk/gtk.h>
#include <libgd/gd-tagged-entry.h>

gint
main (gint argc,
      gchar ** argv)
{
  GtkWidget *window, *box, *entry;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 300, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), box);

  entry = GTK_WIDGET (gd_tagged_entry_new ());
  gtk_container_add (GTK_CONTAINER (box), entry);

  gd_tagged_entry_add_tag (GD_TAGGED_ENTRY (entry), "tag1", "Blah1", "documents-entry-tag", TRUE);
  gd_tagged_entry_insert_tag (GD_TAGGED_ENTRY (entry), "tag2", "Blah2", "documents-entry-tag", FALSE, -1);
  gd_tagged_entry_insert_tag (GD_TAGGED_ENTRY (entry), "tag3", "Blah3", "documents-entry-tag", FALSE, 0);

  gtk_widget_show_all (window);
  gtk_main ();

  gtk_widget_destroy (window);

  return 0;
}
