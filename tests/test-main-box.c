#include <stdlib.h>

#include <gtk/gtk.h>

#include <libgd/gd-main-box.h>
#include <libgd/gd-main-box-item.h>

struct _TestMainBoxItem
{
  GObject parent_instance;

  gchar *id;
  gchar *uri;
  gchar *primary_text;
  gchar *secondary_text;
  cairo_surface_t *icon;
  gint64 mtime;
  gboolean pulse;
};

static void gd_main_box_item_iface_init (gpointer g_iface,
                                         gpointer iface_data);

G_DECLARE_FINAL_TYPE (TestMainBoxItem, test_main_box_item, TEST, MAIN_BOX_ITEM, GObject)
G_DEFINE_TYPE_WITH_CODE (TestMainBoxItem, test_main_box_item, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GD_TYPE_MAIN_BOX_ITEM,
                                                gd_main_box_item_iface_init))

static const gchar *
test_main_box_item_get_id (GdMainBoxItem *main_box_item)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (main_box_item);

  return self->id;
}

static const gchar *
test_main_box_item_get_uri (GdMainBoxItem *main_box_item)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (main_box_item);

  return self->uri;
}

static const gchar *
test_main_box_item_get_primary_text (GdMainBoxItem *main_box_item)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (main_box_item);

  return self->primary_text;
}

static const gchar *
test_main_box_item_get_secondary_text (GdMainBoxItem *main_box_item)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (main_box_item);

  return self->secondary_text;
}

static cairo_surface_t *
test_main_box_item_get_icon (GdMainBoxItem *main_box_item)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (main_box_item);

  return self->icon;
}

static void
gd_main_box_item_iface_init (gpointer g_iface,
                             gpointer iface_data)
{
  GdMainBoxItemInterface *iface;

  iface = g_iface;

  iface->get_id = test_main_box_item_get_id;
  iface->get_uri = test_main_box_item_get_uri;
  iface->get_primary_text = test_main_box_item_get_primary_text;
  iface->get_secondary_text = test_main_box_item_get_secondary_text;
  iface->get_icon = test_main_box_item_get_icon;
}

enum
{
  PROP_ID = 1,
  PROP_URI,
  PROP_PRIMARY_TEXT,
  PROP_SECONDARY_TEXT,
  PROP_ICON,
  PROP_MTIME,
  PROP_PULSE
};

static void
test_main_box_item_set_property (GObject      *object,
                                 guint         property_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (object);

  switch (property_id)
    {
    case PROP_ID:
      g_clear_pointer (&self->id, g_free);
      self->id = g_value_dup_string (value);
      break;

    case PROP_URI:
      g_clear_pointer (&self->uri, g_free);
      self->uri = g_value_dup_string (value);
      break;

    case PROP_PRIMARY_TEXT:
      g_clear_pointer (&self->primary_text, g_free);
      self->primary_text = g_value_dup_string (value);
      break;

    case PROP_SECONDARY_TEXT:
      g_clear_pointer (&self->secondary_text, g_free);
      self->secondary_text = g_value_dup_string (value);
      break;

    case PROP_ICON:
      g_clear_pointer (&self->icon, cairo_surface_destroy);
      self->icon = g_value_dup_boxed (value);
      break;

    case PROP_MTIME:
      self->mtime = g_value_get_int64 (value);
      break;

    case PROP_PULSE:
      self->pulse = g_value_get_boolean (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
test_main_box_item_get_property (GObject    *object,
                                 guint       property_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  TestMainBoxItem *self;

  self = TEST_MAIN_BOX_ITEM (object);

  switch (property_id)
    {
    case PROP_ID:
      g_value_set_string (value, self->id);
      break;

    case PROP_URI:
      g_value_set_string (value, self->uri);
      break;

    case PROP_PRIMARY_TEXT:
      g_value_set_string (value, self->primary_text);
      break;

    case PROP_SECONDARY_TEXT:
      g_value_set_string (value, self->secondary_text);
      break;

    case PROP_ICON:
      g_value_set_boxed (value, self->icon);
      break;

    case PROP_MTIME:
      g_value_set_int64 (value, self->mtime);
      break;

    case PROP_PULSE:
      g_value_set_boolean (value, self->pulse);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
finalize (GObject *object)
{
  TestMainBoxItem *self;
  GObjectClass *object_class;

  self = TEST_MAIN_BOX_ITEM (object);
  object_class = G_OBJECT_CLASS (test_main_box_item_parent_class);

  g_clear_pointer (&self->id, g_free);
  g_clear_pointer (&self->uri, g_free);
  g_clear_pointer (&self->primary_text, g_free);
  g_clear_pointer (&self->secondary_text, g_free);
  g_clear_pointer (&self->icon, cairo_surface_destroy);

  object_class->finalize (object);
}

static void
test_main_box_item_class_init (TestMainBoxItemClass *klass)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (klass);

  object_class->set_property = test_main_box_item_set_property;
  object_class->get_property = test_main_box_item_get_property;
  object_class->finalize = finalize;

  g_object_class_override_property (object_class, PROP_ID, "id");
  g_object_class_override_property (object_class, PROP_URI, "uri");
  g_object_class_override_property (object_class, PROP_PRIMARY_TEXT, "primary-text");
  g_object_class_override_property (object_class, PROP_SECONDARY_TEXT, "secondary-text");
  g_object_class_override_property (object_class, PROP_ICON, "icon");
  g_object_class_override_property (object_class, PROP_MTIME, "mtime");
  g_object_class_override_property (object_class, PROP_PULSE, "pulse");
}

static void
test_main_box_item_init (TestMainBoxItem *self)
{
  g_autoptr (GtkIconInfo) icon_info = NULL;
  cairo_surface_t *surface;

  icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (),
                                          "text-x-generic", 48, 0);
  surface = gtk_icon_info_load_surface (icon_info, NULL, NULL);

  self->id = NULL;
  self->uri = g_strdup ("file:///dev/null");
  self->primary_text = g_strdup ("Primary text");
  self->secondary_text = g_strdup ("Secondary text");
  self->icon = surface;
  self->mtime = -1;
  self->pulse = TRUE;
}

static void
on_selection_mode_request (gpointer user_data)
{
  gd_main_box_set_selection_mode (user_data, TRUE);
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
  GListStore *model;
  GtkWidget *main_box;
  GtkWidget *scrolled_window;
  GtkWidget *primary_text_toggle_button;
  GtkWidget *secondary_text_toggle_button;
  GtkWidget *selection_mode_toggle_button;
  GtkWidget *button_box;
  GtkWidget *grid;
  GtkWidget *window;

  gtk_init ();

  model = g_list_store_new (test_main_box_item_get_type ());
  for (gint i = 0; i < 64; i++)
    {
      g_autoptr (TestMainBoxItem) item = NULL;

      item = g_object_new (test_main_box_item_get_type (), NULL);

      g_list_store_append (model, item);
    }

  main_box = gd_main_box_new (GD_MAIN_BOX_ICON);
  g_signal_connect (main_box, "selection-mode-request",
                    G_CALLBACK (on_selection_mode_request), main_box);
  gd_main_box_set_model (GD_MAIN_BOX (main_box), G_LIST_MODEL (model));
  gtk_widget_set_hexpand (main_box, TRUE);
  gtk_widget_set_vexpand (main_box, TRUE);

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (scrolled_window), main_box);

  primary_text_toggle_button = gtk_toggle_button_new_with_label ("Primary Text");
  g_object_bind_property (primary_text_toggle_button, "active",
                          main_box, "show-primary-text",
                          G_BINDING_BIDIRECTIONAL);

  secondary_text_toggle_button = gtk_toggle_button_new_with_label ("Secondary Text");
  g_object_bind_property (secondary_text_toggle_button, "active",
                          main_box, "show-secondary-text",
                          G_BINDING_BIDIRECTIONAL);

  selection_mode_toggle_button = gtk_toggle_button_new_with_label ("Selection Mode");
  g_object_bind_property (selection_mode_toggle_button, "active",
                          main_box, "selection-mode",
                          G_BINDING_BIDIRECTIONAL);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (button_box), primary_text_toggle_button);
  gtk_container_add (GTK_CONTAINER (button_box), secondary_text_toggle_button);
  gtk_container_add (GTK_CONTAINER (button_box), selection_mode_toggle_button);

  grid = gtk_grid_new ();
  gtk_orientable_set_orientation (GTK_ORIENTABLE (grid), GTK_ORIENTATION_VERTICAL);
  gtk_container_add (GTK_CONTAINER (grid), scrolled_window);
  gtk_container_add (GTK_CONTAINER (grid), button_box);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "destroy", G_CALLBACK (on_destroy), NULL);
  gtk_container_add (GTK_CONTAINER (window), grid);
  gtk_widget_show (window);

  gtk_main ();

  return EXIT_SUCCESS;
}
