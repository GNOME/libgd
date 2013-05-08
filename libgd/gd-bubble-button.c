/*
 * gd-bubble-button.c
 * This file is part of libgd
 *
 * Copyright (C) 2013 - Ignacio Casal Quinteiro
 *
 * libgd is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libgd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgd. If not, see <http://www.gnu.org/licenses/>.
 */


#include "gd-bubble-button.h"
#include "gd-bubble-window.h"


struct _GdBubbleButtonPrivate
{
  GtkWidget *bubble;
  GtkArrowType arrow_type;
  GtkWidget *arrow_widget;
};

enum {
  PROP_0,
  PROP_WIDGET,
  PROP_DIRECTION
};

G_DEFINE_TYPE (GdBubbleButton, gd_bubble_button, GTK_TYPE_TOGGLE_BUTTON)

static void
gd_bubble_button_finalize (GObject *object)
{
  G_OBJECT_CLASS (gd_bubble_button_parent_class)->finalize (object);
}

static void
gtk_bubble_button_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GdBubbleButton *button = GD_BUBBLE_BUTTON (object);

  switch (property_id)
    {
      case PROP_WIDGET:
        gd_bubble_button_set_widget (button, g_value_get_object (value));
        break;
      case PROP_DIRECTION:
        gd_bubble_button_set_direction (button, g_value_get_enum (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
gtk_bubble_button_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GdBubbleButtonPrivate *priv = GD_BUBBLE_BUTTON (object)->priv;

  switch (property_id)
    {
      case PROP_WIDGET:
        g_value_set_object (value, gtk_bin_get_child (GTK_BIN (priv->bubble)));
        break;
      case PROP_DIRECTION:
        g_value_set_enum (value, priv->arrow_type);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
gd_bubble_button_add (GtkContainer *container,
                      GtkWidget    *child)
{
  GdBubbleButton *button = GD_BUBBLE_BUTTON (container);

  if (button->priv->arrow_widget)
    gtk_container_remove (container, button->priv->arrow_widget);

  GTK_CONTAINER_CLASS (gd_bubble_button_parent_class)->add (container, child);
}

static void
gd_bubble_button_remove (GtkContainer *container,
                         GtkWidget    *child)
{
  GdBubbleButton *button = GD_BUBBLE_BUTTON (container);

  if (child == button->priv->arrow_widget)
    button->priv->arrow_widget = NULL;

  GTK_CONTAINER_CLASS (gd_bubble_button_parent_class)->remove (container, child);
}

static void
gd_bubble_button_toggled (GtkToggleButton *button)
{
  GdBubbleButton *bubble_button = GD_BUBBLE_BUTTON (button);
  GdBubbleButtonPrivate *priv = bubble_button->priv;

  if (!gtk_bin_get_child (GTK_BIN (priv->bubble)))
    return;

  if (gtk_toggle_button_get_active (button) &&
      !gtk_widget_get_visible (GTK_WIDGET (priv->bubble)))
    {
      cairo_rectangle_int_t pointing_to = {0,};
      GtkAllocation alloc;
      GtkPositionType pos;
      GdkDevice *device;

      gtk_widget_get_allocation (GTK_WIDGET (button), &alloc);

      switch (priv->arrow_type)
        {
        case GTK_ARROW_UP:
          pointing_to.x = alloc.width / 2;
          pos = GTK_POS_TOP;
          break;
        case GTK_ARROW_DOWN:
          pointing_to.x = alloc.width / 2;
          pointing_to.y = alloc.height;
          pos = GTK_POS_BOTTOM;
          break;
        default:
          break;
        }

      device = gtk_get_current_event_device ();
      if (device == NULL)
        {
          GdkDisplay *display;
          GdkDeviceManager *device_manager;
          GList *devices;

          display = gtk_widget_get_display (GTK_WIDGET (button));
          device_manager = gdk_display_get_device_manager (display);
          devices = gdk_device_manager_list_devices (device_manager, GDK_DEVICE_TYPE_MASTER);

          device = devices->data;

          g_list_free (devices);
        }

      _gd_bubble_window_popup (GD_BUBBLE_WINDOW (priv->bubble),
                               gtk_widget_get_window (GTK_WIDGET (button)),
                               &pointing_to,
                               pos);

      _gd_bubble_window_grab (GD_BUBBLE_WINDOW (priv->bubble), device, 0);
    }
  else if (!gtk_toggle_button_get_active (button))
    _gd_bubble_window_popdown (GD_BUBBLE_WINDOW (priv->bubble));
}

static void
gd_bubble_button_class_init (GdBubbleButtonClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  GtkToggleButtonClass *toggle_button_class = GTK_TOGGLE_BUTTON_CLASS (klass);

  gobject_class->set_property = gtk_bubble_button_set_property;
  gobject_class->get_property = gtk_bubble_button_get_property;
  gobject_class->finalize = gd_bubble_button_finalize;

  container_class->add = gd_bubble_button_add;
  container_class->remove = gd_bubble_button_remove;

  toggle_button_class->toggled = gd_bubble_button_toggled;

  /**
   * GtkBubbleButton:widget:
   *
   * The #GtkWidget that will be popped up inside a bubble window.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_WIDGET,
                                   g_param_spec_object ("widget",
                                                        "Widget",
                                                        "The bubble widget.",
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READWRITE));

  /**
   * GdBubbleButton:direction:
   *
   * The #GtkArrowType representing the direction in which the
   * menu will be popped out.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_DIRECTION,
                                   g_param_spec_enum ("direction",
                                                      "direction",
                                                      "The direction the arrow should point.",
                                                      GTK_TYPE_ARROW_TYPE,
                                                      GTK_ARROW_DOWN,
                                                      G_PARAM_READWRITE));

  g_type_class_add_private (gobject_class, sizeof (GdBubbleButtonPrivate));
}

static void
add_arrow (GdBubbleButton *button)
{
  GtkWidget *arrow;

  arrow = gtk_arrow_new (button->priv->arrow_type, GTK_SHADOW_NONE);
  gtk_container_add (GTK_CONTAINER (button), arrow);
  gtk_widget_show (arrow);
  button->priv->arrow_widget = arrow;
}

static void
on_bubble_hidden (GtkWidget      *bubble,
                  GdBubbleButton *button)
{
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
}

static void
gd_bubble_button_init (GdBubbleButton *button)
{
  GdBubbleButtonPrivate *priv;

  button->priv = G_TYPE_INSTANCE_GET_PRIVATE (button,
                                              GD_TYPE_BUBBLE_BUTTON,
                                              GdBubbleButtonPrivate);
  priv = button->priv;

  priv->arrow_type = GTK_ARROW_DOWN;
  priv->bubble = _gd_bubble_window_new ();

  /* Attach bubble to button */
  gtk_window_set_attached_to (GTK_WINDOW (priv->bubble), GTK_WIDGET (button));

  add_arrow (button);

  g_signal_connect_after (priv->bubble, "hide",
                          G_CALLBACK (on_bubble_hidden), button);
}

GtkWidget *
gd_bubble_button_new (void)
{
  return g_object_new (GD_TYPE_BUBBLE_BUTTON, NULL);
}

/**
 * gd_bubble_button_set_direction:
 * @menu_button: a #GtkMenuButton
 * @direction: a #GtkArrowType
 *
 * Sets the direction in which the menu will be popped up, as
 * well as changing the arrow's direction. The child will not
 * be changed to an arrow if it was customized.
 *
 * If the bubble when popped out would have collided with screen edges,
 * we will do our best to keep it inside the screen and fully visible.
 *
 * If you pass %GTK_ARROW_NONE for a @direction, the menu will behave
 * as if you passed %GTK_ARROW_DOWN (although you won't see any arrows).
 */
void
gd_bubble_button_set_direction (GdBubbleButton *button,
                                GtkArrowType    direction)
{
  GdBubbleButtonPrivate *priv = button->priv;
  GtkWidget *child;

  g_return_if_fail (GD_IS_BUBBLE_BUTTON (button));

  if (priv->arrow_type == direction)
    return;

  priv->arrow_type = direction;

  /* Is it custom content? We don't change that */
  child = gtk_bin_get_child (GTK_BIN (button));
  if (priv->arrow_widget != child)
    return;

  gtk_arrow_set (GTK_ARROW (child), priv->arrow_type, GTK_SHADOW_NONE);
}

/**
 * gtk_menu_button_get_direction:
 * @menu_button: a #GdBubbleButton
 *
 * Returns the direction the menu will be pointing at when popped up.
 *
 * Returns: a #GtkArrowType value.
 */
GtkArrowType
gd_bubble_button_get_direction (GdBubbleButton *button)
{
  g_return_val_if_fail (GD_IS_BUBBLE_BUTTON (button), GTK_ARROW_DOWN);

  return button->priv->arrow_type;
}

/**
 * gd_bubble_button_get_widget:
 *
 * Returns: (transfer none):
 */
GtkWidget *
gd_bubble_button_get_widget (GdBubbleButton *button)
{
  g_return_val_if_fail (GD_IS_BUBBLE_BUTTON (button), NULL);

  return gtk_bin_get_child (GTK_BIN (button->priv->bubble));
}

void
gd_bubble_button_set_widget (GdBubbleButton *button,
                             GtkWidget      *widget)
{
  GdBubbleButtonPrivate *priv;
  GtkWidget *child;

  g_return_if_fail (GD_IS_BUBBLE_BUTTON (button));

  priv = button->priv;
  child = gtk_bin_get_child (GTK_BIN (priv->bubble));

  if (child)
    gtk_widget_destroy (child);

  gtk_container_add (GTK_CONTAINER (priv->bubble), widget);
}

/* ex:set ts=2 et: */
