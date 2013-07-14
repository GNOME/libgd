#include <gtk/gtk.h>
#include <libgd/gd-tagged-entry.h>

static void
on_tag_clicked (GdTaggedEntry *entry,
                GdTaggedEntryTag *tag,
                gpointer useless)
{
  g_print ("tag clicked: %s\n", gd_tagged_entry_tag_get_label (tag));
}

static void
on_tag_button_clicked (GdTaggedEntry *entry,
                       GdTaggedEntryTag *tag,
                       gpointer useless)
{
  g_print ("tag button clicked: %s\n", gd_tagged_entry_tag_get_label (tag));
}

gint
main (gint argc,
      gchar ** argv)
{
  GtkWidget *window, *box, *entry;
  GdTaggedEntryTag *tag;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window, 300, 300);

  box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add (GTK_CONTAINER (window), box);

  entry = GTK_WIDGET (gd_tagged_entry_new ());
  g_signal_connect(entry, "tag-clicked",
                   G_CALLBACK (on_tag_clicked), NULL);
  g_signal_connect(entry, "tag-button-clicked",
                   G_CALLBACK (on_tag_button_clicked), NULL);
  gtk_container_add (GTK_CONTAINER (box), entry);

  tag = gd_tagged_entry_tag_new ("Blah1");
  gd_tagged_entry_add_tag (GD_TAGGED_ENTRY (entry), tag);
  g_object_unref (tag);

  tag = gd_tagged_entry_tag_new ("Blah2");
  gd_tagged_entry_tag_set_has_close_button (tag, FALSE);
  gd_tagged_entry_insert_tag (GD_TAGGED_ENTRY (entry), tag, -1);
  g_object_unref (tag);

  tag = gd_tagged_entry_tag_new ("Blah3");
  gd_tagged_entry_tag_set_has_close_button (tag, FALSE);
  gd_tagged_entry_insert_tag (GD_TAGGED_ENTRY (entry), tag, 0);
  g_object_unref (tag);

  gtk_widget_show_all (window);
  gtk_main ();

  gtk_widget_destroy (window);

  return 0;
}
