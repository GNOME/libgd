/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * gd-notification
 * Based on gtk-notification from gnome-contacts:
 * http://git.gnome.org/browse/gnome-contacts/tree/src/gtk-notification.c?id=3.3.91
 *
 * Copyright (C) Erick Pérez Castellanos 2011 <erick.red@gmail.com>
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#include "gd-notification.h"

/**
 * SECTION:gdnotification
 * @short_description: Report notification messages to the user
 * @include: gtk/gtk.h
 * @see_also: #GtkStatusbar, #GtkMessageDialog, #GtkInfoBar
 *
 * #GdNotification is a widget made for showing notifications to
 * the user, allowing them to close the notification or wait for it
 * to time out.
 *
 * #GdNotification provides one signal (#GdNotification::dismissed), for when the notification
 * times out or is closed.
 *
 */

#define GTK_PARAM_READWRITE G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB
#define SHADOW_OFFSET_X 2
#define SHADOW_OFFSET_Y 3
#define ANIMATION_TIME 200 /* msec */
#define ANIMATION_STEP 20 /* msec */

enum {
  PROP_0,
  PROP_TIMEOUT,
  PROP_SHOW_CLOSE_BUTTON
};

struct _GdNotificationPrivate {
  GtkWidget *close_button;
  gboolean show_close_button;

  gdouble animate_y; /* ∈ [0.0; 1.0] */
  gboolean waiting_for_viewable;
  gboolean revealed;
  gboolean dismissed;
  gboolean sent_dismissed;
  guint animate_timeout;

  gint timeout;
  guint timeout_source_id;

  GtkEventController *motion_controller;
};

enum {
  DISMISSED,
  LAST_SIGNAL
};

static guint notification_signals[LAST_SIGNAL] = { 0 };

static void     gd_notification_measure                        (GtkWidget       *widget,
                                                                GtkOrientation   orientation,
                                                                gint             for_size,
                                                                gint            *minimum,
                                                                gint            *natural,
                                                                gint            *minimum_baseline,
                                                                gint            *natural_baseline);
static void     gd_notification_size_allocate                  (GtkWidget             *widget,
                                                                const GtkAllocation   *allocation,
                                                                gint                   baseline,
                                                                GtkAllocation         *out_clip);
static gboolean gd_notification_timeout_cb                     (gpointer         user_data);
static void     gd_notification_show                           (GtkWidget       *widget);

/* signals handlers */
static void     gd_notification_close_button_clicked_cb        (GtkWidget       *widget,
                                                                 gpointer         user_data);

G_DEFINE_TYPE(GdNotification, gd_notification, GTK_TYPE_BIN);

static void
unqueue_autohide (GdNotification *notification)
{
  GdNotificationPrivate *priv = notification->priv;

  if (priv->timeout_source_id)
    {
      g_source_remove (priv->timeout_source_id);
      priv->timeout_source_id = 0;
    }
}

static void
queue_autohide (GdNotification *notification)
{
  GdNotificationPrivate *priv = notification->priv;

  if (priv->timeout_source_id == 0 &&
      priv->timeout != -1)
    priv->timeout_source_id =
      g_timeout_add (priv->timeout * 1000, gd_notification_timeout_cb, notification);
}

static void
on_enter (GtkEventControllerMotion *controller,
          gdouble                   x,
          gdouble                   y,
          gpointer                  user_data)
{
  unqueue_autohide (user_data);
}

static void
on_leave (GtkEventControllerMotion *controller,
          gdouble                   x,
          gdouble                   y,
          gpointer                  user_data)
{
  queue_autohide (user_data);
}

static void
gd_notification_init (GdNotification *notification)
{
  GtkStyleContext *context;
  GdNotificationPrivate *priv;

  context = gtk_widget_get_style_context (GTK_WIDGET (notification));
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_FRAME);
  gtk_style_context_add_class (context, "app-notification");

  gtk_widget_set_halign (GTK_WIDGET (notification), GTK_ALIGN_CENTER);
  gtk_widget_set_valign (GTK_WIDGET (notification), GTK_ALIGN_START);

  gtk_widget_set_has_window (GTK_WIDGET (notification), FALSE);

  priv = notification->priv =
    G_TYPE_INSTANCE_GET_PRIVATE (notification,
                                 GD_TYPE_NOTIFICATION,
                                 GdNotificationPrivate);

  priv->animate_y = 0.0;
  priv->close_button = gtk_button_new_from_icon_name ("window-close-symbolic");
  gtk_widget_set_parent (priv->close_button, GTK_WIDGET (notification));
  gtk_widget_show (priv->close_button);
  g_object_set (priv->close_button,
                "relief", GTK_RELIEF_NONE,
                "focus-on-click", FALSE,
                NULL);
  g_signal_connect (priv->close_button,
                    "clicked",
                    G_CALLBACK (gd_notification_close_button_clicked_cb),
                    notification);

  priv->timeout_source_id = 0;

  priv->motion_controller = gtk_event_controller_motion_new (GTK_WIDGET (notification));

  g_signal_connect (priv->motion_controller, "enter", G_CALLBACK (on_enter), notification);
  g_signal_connect (priv->motion_controller, "leave", G_CALLBACK (on_leave), notification);
}

static void
gd_notification_finalize (GObject *object)
{
  GdNotification *notification;
  GdNotificationPrivate *priv;

  g_return_if_fail (GTK_IS_NOTIFICATION (object));

  notification = GD_NOTIFICATION (object);
  priv = notification->priv;

  if (priv->animate_timeout != 0)
    g_source_remove (priv->animate_timeout);

  if (priv->timeout_source_id != 0)
    g_source_remove (priv->timeout_source_id);

  G_OBJECT_CLASS (gd_notification_parent_class)->finalize (object);
}

static void
gd_notification_destroy (GtkWidget *widget)
{
  GdNotification *notification = GD_NOTIFICATION (widget);
  GdNotificationPrivate *priv = notification->priv;

  if (!priv->sent_dismissed)
    {
      g_signal_emit (notification, notification_signals[DISMISSED], 0);
      priv->sent_dismissed = TRUE;
    }

  if (priv->close_button)
    {
      gtk_widget_unparent (priv->close_button);
      priv->close_button = NULL;
    }

  GTK_WIDGET_CLASS (gd_notification_parent_class)->destroy (widget);
}

static gdouble
animation_target (GdNotification *notification)
{
  GdNotificationPrivate *priv = notification->priv;

  return priv->revealed? 1.0 : 0.0;
}

static gboolean
animation_timeout_cb (gpointer user_data)
{
  GdNotification *notification = GD_NOTIFICATION (user_data);
  GdNotificationPrivate *priv = notification->priv;
  GtkAllocation allocation;
  gdouble target, delta;

  target = animation_target (notification);

  if (priv->animate_y != target) {
    gtk_widget_get_allocation (GTK_WIDGET (notification), &allocation);

    delta = (gdouble) ANIMATION_STEP / (gdouble) ANIMATION_TIME;

    if (priv->revealed)
      priv->animate_y += delta;
    else
      priv->animate_y -= delta;

    priv->animate_y = CLAMP (priv->animate_y, 0.0, 1.0);

    gtk_widget_queue_draw (GTK_WIDGET (notification));

    return G_SOURCE_CONTINUE;
  }

  if (priv->dismissed && priv->animate_y == 0.0)
    gtk_widget_destroy (GTK_WIDGET (notification));

  priv->animate_timeout = 0;
  return G_SOURCE_REMOVE;
}

static void
start_animation (GdNotification *notification)
{
  GdNotificationPrivate *priv = notification->priv;
  int target;

  if (priv->animate_timeout != 0)
    return; /* Already running */

  target = animation_target (notification);
  if (priv->animate_y != target)
    priv->animate_timeout =
      g_timeout_add (ANIMATION_STEP, animation_timeout_cb, notification);
}

static void
gd_notification_show (GtkWidget *widget)
{
  GdNotification *notification = GD_NOTIFICATION (widget);
  GdNotificationPrivate *priv = notification->priv;

  GTK_WIDGET_CLASS (gd_notification_parent_class)->show (widget);
  priv->revealed = TRUE;
  priv->waiting_for_viewable = TRUE;
}

static void
gd_notification_hide (GtkWidget *widget)
{
  GdNotification *notification = GD_NOTIFICATION (widget);
  GdNotificationPrivate *priv = notification->priv;

  GTK_WIDGET_CLASS (gd_notification_parent_class)->hide (widget);
  priv->revealed = FALSE;
  priv->waiting_for_viewable = FALSE;
}

static void
gd_notification_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  GdNotification *notification = GD_NOTIFICATION (object);

  g_return_if_fail (GTK_IS_NOTIFICATION (object));

  switch (prop_id) {
  case PROP_TIMEOUT:
    gd_notification_set_timeout (notification,
                                 g_value_get_int (value));
    break;
  case PROP_SHOW_CLOSE_BUTTON:
    gd_notification_set_show_close_button (notification,
                                           g_value_get_boolean (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gd_notification_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  g_return_if_fail (GTK_IS_NOTIFICATION (object));
  GdNotification *notification = GD_NOTIFICATION (object);

  switch (prop_id) {
  case PROP_TIMEOUT:
    g_value_set_int (value, notification->priv->timeout);
    break;
  case PROP_SHOW_CLOSE_BUTTON:
    g_value_set_boolean (value,
                         notification->priv->show_close_button);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
gd_notification_forall (GtkContainer *container,
                         GtkCallback   callback,
                         gpointer      callback_data)
{
  GtkBin *bin = GTK_BIN (container);
  GdNotification *notification = GD_NOTIFICATION (container);
  GdNotificationPrivate *priv = notification->priv;
  GtkWidget *child;

  child = gtk_bin_get_child (bin);
  if (child)
    (* callback) (child, callback_data);

  if (priv->close_button)
    (* callback) (priv->close_button, callback_data);
}

static gboolean
gd_notification_visibility_notify_event (GtkWidget          *widget,
                                          GdkEventVisibility  *event)
{
  GdNotification *notification = GD_NOTIFICATION (widget);
  GdNotificationPrivate *priv = notification->priv;

  if (!gtk_widget_get_visible (widget))
    return FALSE;

  if (priv->waiting_for_viewable)
    {
      start_animation (notification);
      priv->waiting_for_viewable = FALSE;
    }

  queue_autohide (notification);

  return FALSE;
}

static void
gd_notification_snapshot (GtkWidget   *widget,
                          GtkSnapshot *snapshot)
{
  GdNotification *self;
  GdNotificationPrivate *priv;
  GtkAllocation allocation;
  graphene_rect_t bounds;

  self = GD_NOTIFICATION (widget);
  priv = self->priv;

  gtk_widget_get_allocation (widget, &allocation);

  bounds = GRAPHENE_RECT_INIT (0, 0,
                               allocation.width, allocation.height);

  gtk_snapshot_push_clip (snapshot, &bounds, "GdNotification Clip");
  gtk_snapshot_offset (snapshot,
                       0,
                       -allocation.height * (1.0 - priv->animate_y));

  GTK_WIDGET_CLASS (gd_notification_parent_class)->snapshot (widget, snapshot);

  gtk_snapshot_pop (snapshot);
}

static void
gd_notification_class_init (GdNotificationClass *klass)
{
  GObjectClass* object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->finalize = gd_notification_finalize;
  object_class->set_property = gd_notification_set_property;
  object_class->get_property = gd_notification_get_property;

  widget_class->show = gd_notification_show;
  widget_class->hide = gd_notification_hide;
  widget_class->destroy = gd_notification_destroy;
  widget_class->measure = gd_notification_measure;
  widget_class->size_allocate = gd_notification_size_allocate;
  widget_class->snapshot = gd_notification_snapshot;
  widget_class->visibility_notify_event = gd_notification_visibility_notify_event;

  container_class->forall = gd_notification_forall;


  /**
   * GdNotification:timeout:
   *
   * The time it takes to hide the widget, in seconds.
   *
   * Since: 0.1
   */
  g_object_class_install_property (object_class,
                                   PROP_TIMEOUT,
                                   g_param_spec_int("timeout", "timeout",
                                                    "The time it takes to hide the widget, in seconds",
                                                    -1, G_MAXINT, -1,
                                                    GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (object_class,
                                   PROP_SHOW_CLOSE_BUTTON,
                                   g_param_spec_boolean("show-close-button", "show-close-button",
                                                        "Whether to show a stock close button that dismisses the notification",
                                                        TRUE,
                                                        GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  notification_signals[DISMISSED] = g_signal_new ("dismissed",
                                                  G_OBJECT_CLASS_TYPE (klass),
                                                  G_SIGNAL_RUN_LAST,
                                                  G_STRUCT_OFFSET (GdNotificationClass, dismissed),
                                                  NULL,
                                                  NULL,
                                                  g_cclosure_marshal_VOID__VOID,
                                                  G_TYPE_NONE,
                                                  0);

  g_type_class_add_private (object_class, sizeof (GdNotificationPrivate));
}

static void
get_padding_and_border (GdNotification *notification,
                        GtkBorder *border)
{
  GtkStyleContext *context;
  GtkBorder tmp;

  context = gtk_widget_get_style_context (GTK_WIDGET (notification));

  gtk_style_context_get_padding (context, border);

  gtk_style_context_get_border (context, &tmp);
  border->top += tmp.top;
  border->right += tmp.right;
  border->bottom += tmp.bottom;
  border->left += tmp.left;
}

static void
gd_notification_measure (GtkWidget      *widget,
                         GtkOrientation  orientation,
                         gint            for_size,
                         gint           *minimum,
                         gint           *natural,
                         gint           *minimum_baseline,
                         gint           *natural_baseline)
{
  gint minimum_size;
  gint natural_size;
  GdNotification *notification;
  GdNotificationPrivate *priv;
  GtkBorder padding;
  GtkWidget *child;
  gint child_minimum;
  gint child_natural;

  minimum_size = 0;
  natural_size = 0;
  notification = GD_NOTIFICATION (widget);
  priv = notification->priv;

  get_padding_and_border (notification, &padding);

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      child = gtk_bin_get_child (GTK_BIN (widget));
      if (child && gtk_widget_get_visible (child))
        {
          gtk_widget_measure (child, GTK_ORIENTATION_HORIZONTAL, -1,
                              &child_minimum, &child_natural, NULL, NULL);

          minimum_size += child_minimum;
          natural_size += child_natural;
        }

      if (priv->show_close_button)
        {
          gtk_widget_measure (priv->close_button, GTK_ORIENTATION_HORIZONTAL, -1,
                              &child_minimum, &child_natural, NULL, NULL);

          minimum_size += child_minimum;
          natural_size += child_natural;
        }

      minimum_size += padding.left + padding.right + 2 * SHADOW_OFFSET_X;
      natural_size += padding.left + padding.right + 2 * SHADOW_OFFSET_X;
    }
  else
    {
      GtkSizeRequestMode request_mode;

      request_mode = gtk_widget_get_request_mode (widget);
      if (request_mode == GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH)
        {
          gint button_width;
          gint child_width;

          if (priv->show_close_button)
            {
              gtk_widget_measure (priv->close_button, GTK_ORIENTATION_VERTICAL, -1,
                                  &minimum_size, &natural_size, NULL, NULL);
              gtk_widget_measure (priv->close_button, GTK_ORIENTATION_HORIZONTAL, -1,
                                  NULL, &button_width, NULL, NULL);
            }

          child = gtk_bin_get_child (GTK_BIN (widget));
          if (child && gtk_widget_get_visible (child))
            {
              child_width = for_size - button_width -
                2 * SHADOW_OFFSET_X - padding.left - padding.right;

              gtk_widget_measure (child, GTK_ORIENTATION_VERTICAL, child_width,
                                  &child_minimum, &child_natural, NULL, NULL);
              minimum_size = MAX (minimum_size, child_minimum);
              natural_size = MAX (natural_size, child_natural);
            }

          minimum_size += padding.top + padding.bottom + SHADOW_OFFSET_Y;
          natural_size += padding.top + padding.bottom + SHADOW_OFFSET_Y;
        }
      else if (request_mode == GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT)
        {
          gint child_height;

          child_height = for_size - SHADOW_OFFSET_Y - padding.top - padding.bottom;

          child = gtk_bin_get_child (GTK_BIN (widget));
          if (child && gtk_widget_get_visible (child))
            {
              gtk_widget_measure (child, GTK_ORIENTATION_HORIZONTAL, child_height,
                                  &child_minimum, &child_natural, NULL, NULL);
              minimum_size += child_minimum;
              natural_size += child_natural;
            }

          if (priv->show_close_button)
            {
              gtk_widget_measure (priv->close_button, GTK_ORIENTATION_HORIZONTAL, child_height,
                                  &child_minimum, &child_natural, NULL, NULL);
              minimum_size += child_minimum;
              natural_size += child_natural;
            }

          minimum_size += padding.left + padding.right + 2 * SHADOW_OFFSET_X;
          natural_size += padding.left + padding.right + 2 * SHADOW_OFFSET_X;
        }
      else
        {
          GtkWidgetClass *widget_class;
          gint unused;

          widget_class = GTK_WIDGET_CLASS (gd_notification_parent_class);

          widget_class->measure (widget, GTK_ORIENTATION_HORIZONTAL, -1,
                                 &minimum_size, &unused, NULL, NULL);
          widget_class->measure (widget, GTK_ORIENTATION_VERTICAL, minimum_size,
                                 &minimum_size, &natural_size, NULL, NULL);

          minimum_size += padding.top + padding.bottom + SHADOW_OFFSET_Y;
          natural_size += padding.top + padding.bottom + SHADOW_OFFSET_Y;
        }
    }

  if (minimum != NULL)
    *minimum = minimum_size;
  if (natural != NULL)
    *natural = natural_size;
}

static void
gd_notification_size_allocate (GtkWidget           *widget,
                               const GtkAllocation *allocation,
                               gint                 baseline,
                               GtkAllocation       *out_clip)
{
  GdNotification *notification = GD_NOTIFICATION (widget);
  GdNotificationPrivate *priv = notification->priv;
  GtkBin *bin = GTK_BIN (widget);
  GtkAllocation child_allocation;
  GtkBorder padding;
  GtkRequisition button_req;
  GtkWidget *child;

  /* If somehow the notification changes while not hidden
     and we're not animating, immediately follow the resize */
  if (priv->animate_y > 0.0 &&
      !priv->animate_timeout)
    priv->animate_y = 1.0;

  get_padding_and_border (notification, &padding);

  child_allocation.x = SHADOW_OFFSET_X + padding.left;
  child_allocation.y = padding.top;

  if (priv->show_close_button)
    gtk_widget_get_preferred_size (priv->close_button, &button_req, NULL);
  else
    button_req.width = button_req.height = 0;

  child_allocation.height = MAX (1, allocation->height - SHADOW_OFFSET_Y - padding.top - padding.bottom);
  child_allocation.width = MAX (1, (allocation->width - button_req.width -
                                    2 * SHADOW_OFFSET_X - padding.left - padding.right));

  child = gtk_bin_get_child (bin);
  if (child && gtk_widget_get_visible (child))
    gtk_widget_size_allocate (child, &child_allocation, baseline, out_clip);

  if (priv->show_close_button)
    {
      child_allocation.x += child_allocation.width;
      child_allocation.width = button_req.width;
      child_allocation.y += (child_allocation.height - button_req.height) / 2;
      child_allocation.height = button_req.height;

      gtk_widget_size_allocate (priv->close_button, &child_allocation, baseline, out_clip);
    }
}

static gboolean
gd_notification_timeout_cb (gpointer user_data)
{
  GdNotification *notification = GD_NOTIFICATION (user_data);

  gd_notification_dismiss (notification);

  return G_SOURCE_REMOVE;
}

void
gd_notification_set_timeout (GdNotification *notification,
                             gint            timeout_sec)
{
  GdNotificationPrivate *priv = notification->priv;

  priv->timeout = timeout_sec;
  g_object_notify (G_OBJECT (notification), "timeout");
}

void
gd_notification_set_show_close_button (GdNotification *notification,
                                       gboolean show_close_button)
{
  GdNotificationPrivate *priv = notification->priv;

  priv->show_close_button = show_close_button;

  gtk_widget_set_visible (priv->close_button, show_close_button);
  gtk_widget_queue_resize (GTK_WIDGET (notification));
}

void
gd_notification_dismiss (GdNotification *notification)
{
  GdNotificationPrivate *priv = notification->priv;

  unqueue_autohide (notification);

  priv->dismissed = TRUE;
  priv->revealed = FALSE;
  start_animation (notification);
}

static void
gd_notification_close_button_clicked_cb (GtkWidget *widget, gpointer user_data)
{
  GdNotification *notification = GD_NOTIFICATION(user_data);

  gd_notification_dismiss (notification);
}

GtkWidget *
gd_notification_new (void)
{
  return g_object_new (GD_TYPE_NOTIFICATION, NULL);
}
