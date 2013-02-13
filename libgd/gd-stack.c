/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Copyright (c) 2013 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Alexander Larsson <alexl@redhat.com>
 *
 */

#include <gtk/gtk.h>
#include "gd-stack.h"
#include <math.h>
#include <string.h>

#define FRAME_TIME_MSEC 17 /* 17 msec => 60 fps */

enum  {
  PROP_0,
  PROP_HOMOGENEOUS,
  PROP_VISIBLE_CHILD,
  PROP_VISIBLE_CHILD_NAME,
  PROP_DURATION
};

enum
{
  CHILD_PROP_0,
  CHILD_PROP_NAME
};

typedef struct _GdStackChildInfo GdStackChildInfo;

struct _GdStackChildInfo {
  GtkWidget *widget;
  char *name;
};

struct _GdStackPrivate {
  GList *children;

  GdStackChildInfo *visible_child;

  gboolean homogeneous;
  gint duration;

  cairo_surface_t *xfade_surface;
  gdouble xfade_pos;

  guint timeout_tag;
  gint64 start_time;
  gint64 end_time;
};

#define GTK_PARAM_READWRITE G_PARAM_READWRITE|G_PARAM_STATIC_NAME|G_PARAM_STATIC_NICK|G_PARAM_STATIC_BLURB
#define GD_STACK_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GD_TYPE_STACK, GdStackPrivate))

static void     gd_stack_add                            (GtkContainer  *widget,
							 GtkWidget     *child);
static void     gd_stack_remove                         (GtkContainer  *widget,
							 GtkWidget     *child);
static void     gd_stack_forall                         (GtkContainer *container,
							 gboolean      include_internals,
							 GtkCallback   callback,
							 gpointer      callback_data);
static void    gd_stack_compute_expand                  (GtkWidget      *widget,
							 gboolean       *hexpand,
							 gboolean       *vexpand);
static void     gd_stack_size_allocate                  (GtkWidget     *widget,
							 GtkAllocation *allocation);
static gboolean gd_stack_draw                           (GtkWidget     *widget,
							 cairo_t       *cr);
static void     gd_stack_get_preferred_height           (GtkWidget     *widget,
							 gint          *minimum_height,
							 gint          *natural_height);
static void     gd_stack_get_preferred_height_for_width (GtkWidget     *widget,
							 gint           width,
							 gint          *minimum_height,
							 gint          *natural_height);
static void     gd_stack_get_preferred_width            (GtkWidget     *widget,
							 gint          *minimum_width,
							 gint          *natural_width);
static void     gd_stack_get_preferred_width_for_height (GtkWidget     *widget,
							 gint           height,
							 gint          *minimum_width,
							 gint          *natural_width);
static void     gd_stack_finalize                       (GObject       *obj);
static void     gd_stack_get_property                   (GObject       *object,
							 guint          property_id,
							 GValue        *value,
							 GParamSpec    *pspec);
static void     gd_stack_set_property                   (GObject       *object,
							 guint          property_id,
							 const GValue  *value,
							 GParamSpec    *pspec);
static void     gd_stack_get_child_property             (GtkContainer *container,
                                                         GtkWidget    *child,
                                                         guint         property_id,
                                                         GValue       *value,
                                                         GParamSpec   *pspec);
static void     gd_stack_set_child_property             (GtkContainer *container,
                                                         GtkWidget    *child,
                                                         guint         property_id,
                                                         const GValue *value,
                                                         GParamSpec   *pspec);

G_DEFINE_TYPE(GdStack, gd_stack, GTK_TYPE_BIN);

static void
gd_stack_init (GdStack *stack)
{
  GdStackPrivate *priv;

  priv = GD_STACK_GET_PRIVATE (stack);
  stack->priv = priv;
  priv->duration = 250;

  gtk_widget_set_has_window ((GtkWidget*) stack, FALSE);
  gtk_widget_set_redraw_on_allocate ((GtkWidget*) stack, TRUE);
}

static void
gd_stack_finalize (GObject* obj)
{
  GdStack *stack = GD_STACK (obj);
  GdStackPrivate *priv = stack->priv;

  if (priv->timeout_tag != 0)
    g_source_remove (priv->timeout_tag);
  priv->timeout_tag = 0;

  if (priv->xfade_surface != NULL)
    cairo_surface_destroy (priv->xfade_surface);

  G_OBJECT_CLASS (gd_stack_parent_class)->finalize (obj);
}

static void
gd_stack_get_property (GObject *object,
		       guint property_id,
		       GValue *value,
		       GParamSpec *pspec)
{
  GdStack *stack = GD_STACK (object);
  GdStackPrivate *priv = stack->priv;

  switch (property_id)
    {
    case PROP_HOMOGENEOUS:
      g_value_set_boolean (value, priv->homogeneous);
      break;
    case PROP_VISIBLE_CHILD:
      g_value_set_object (value, priv->visible_child);
      break;
    case PROP_VISIBLE_CHILD_NAME:
      g_value_set_string (value, gd_stack_get_visible_child_name (stack));
      break;
    case PROP_DURATION:
      g_value_set_int (value, gd_stack_get_duration (stack));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gd_stack_set_property (GObject *object,
		       guint property_id,
		       const GValue *value,
		       GParamSpec *pspec)
{
  GdStack *stack = GD_STACK (object);

  switch (property_id)
    {
    case PROP_HOMOGENEOUS:
      gd_stack_set_homogeneous (stack, g_value_get_boolean (value));
      break;
    case PROP_VISIBLE_CHILD:
      gd_stack_set_visible_child (stack, g_value_get_object (value));
      break;
    case PROP_VISIBLE_CHILD_NAME:
      gd_stack_set_visible_child_name (stack, g_value_get_string (value));
      break;
    case PROP_DURATION:
      gd_stack_set_duration (stack, g_value_get_int (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gd_stack_class_init (GdStackClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->get_property = gd_stack_get_property;
  object_class->set_property = gd_stack_set_property;
  object_class->finalize = gd_stack_finalize;

  widget_class->size_allocate = gd_stack_size_allocate;
  widget_class->draw = gd_stack_draw;
  widget_class->get_preferred_height = gd_stack_get_preferred_height;
  widget_class->get_preferred_height_for_width = gd_stack_get_preferred_height_for_width;
  widget_class->get_preferred_width = gd_stack_get_preferred_width;
  widget_class->get_preferred_width_for_height = gd_stack_get_preferred_width_for_height;
  widget_class->compute_expand = gd_stack_compute_expand;

  container_class->add = gd_stack_add;
  container_class->remove = gd_stack_remove;
  container_class->forall = gd_stack_forall;
  container_class->set_child_property = gd_stack_set_child_property;
  container_class->get_child_property = gd_stack_get_child_property;
  /*container_class->get_path_for_child = gd_stack_get_path_for_child; */
  gtk_container_class_handle_border_width (container_class);

  g_object_class_install_property (object_class,
				   PROP_HOMOGENEOUS,
				   g_param_spec_boolean ("homogeneous",
							 "Homogeneous",
							 "Homogeneous sizing",
							 FALSE,
							 GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (object_class,
				   PROP_VISIBLE_CHILD,
				   g_param_spec_object ("visible-child",
							"Visible child",
							"The widget currently visible in the stack",
							GTK_TYPE_WIDGET,
							GTK_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_VISIBLE_CHILD_NAME,
				   g_param_spec_string ("visible-child-name",
							"Name of visible child",
							"The name of the widget currently visible in the stack",
							NULL,
							GTK_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_DURATION,
                                   g_param_spec_int ("duration", "duration",
                                                     "The animation duration, in milliseconds",
                                                     G_MININT, G_MAXINT,
                                                     250,
                                                     GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  gtk_container_class_install_child_property (container_class, CHILD_PROP_NAME,
    g_param_spec_string ("name",
                         "Name",
                         "The name of the child page",
                         NULL,
                         GTK_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (GdStackPrivate));
}


GtkWidget *
gd_stack_new (void)
{
  return g_object_new (GD_TYPE_STACK, NULL);
}

static GdStackChildInfo *
find_child_info_for_widget (GdStack *stack,
                            GtkWidget *child)
{
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *info;
  GList *l;

  for (l = priv->children; l != NULL; l = l->next)
    {
      info = l->data;
      if (info->widget == child)
        return info;
    }

  return NULL;
}

static void
gd_stack_get_child_property (GtkContainer *container,
                             GtkWidget    *child,
                             guint         property_id,
                             GValue       *value,
                             GParamSpec   *pspec)
{
  GdStack *stack = GD_STACK (container);
  GdStackChildInfo *info;

  info = find_child_info_for_widget (stack, child);
  if (info == NULL)
    {
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      return;
    }

  switch (property_id)
    {
    case CHILD_PROP_NAME:
      g_value_set_string (value, info->name);
      break;

    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}

static void
gd_stack_set_child_property (GtkContainer *container,
                             GtkWidget    *child,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GdStack *stack = GD_STACK (container);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *info;

  info = find_child_info_for_widget (stack, child);
  if (info == NULL)
    {
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      return;
    }

  switch (property_id)
    {
    case CHILD_PROP_NAME:
      info->name = g_value_dup_string (value);

      if (priv->visible_child == info)
        g_object_notify (G_OBJECT (stack), "visible-child-name");

      break;

    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container, property_id, pspec);
      break;
    }
}

static void
gd_stack_set_xfade_position (GdStack *stack,
                             gdouble pos)
{
  GdStackPrivate *priv = stack->priv;

  priv->xfade_pos = pos;

  if (priv->visible_child)
    gtk_widget_queue_draw (GTK_WIDGET (stack));

  if (pos >= 1.0 && priv->xfade_surface != NULL)
    {
      cairo_surface_destroy (priv->xfade_surface);
      priv->xfade_surface = NULL;
    }
}

static gboolean
gd_stack_xfade_cb (GdStack *stack)
{
  GdStackPrivate *priv = stack->priv;
  gint64 now;
  gdouble t;

  now = g_get_monotonic_time ();

  t = 1.0;
  if (now < priv->end_time)
    t = (now - priv->start_time) / (double) (priv->end_time - priv->start_time);

  /* Finish animation early if not mapped anymore */
  if (!gtk_widget_get_mapped (GTK_WIDGET (stack)))
    t = 1.0;

  gd_stack_set_xfade_position (stack, t);

  if (t >= 1.0)
    {
      gtk_widget_set_opacity (GTK_WIDGET (stack), 1.0);
      priv->timeout_tag = 0;

      return FALSE;
    }

  return TRUE;
}

static void
gd_stack_start_xfade (GdStack *stack)
{
  GdStackPrivate *priv = stack->priv;

  if (gtk_widget_get_mapped (GTK_WIDGET (stack)) &&
      priv->xfade_surface != NULL)
    {
      gtk_widget_set_opacity (GTK_WIDGET (stack), 0.999);

      priv->xfade_pos = 0.0;
      priv->start_time = g_get_monotonic_time ();
      priv->end_time = priv->start_time + (priv->duration * 1000);
      if (priv->timeout_tag == 0)
        priv->timeout_tag =
          gdk_threads_add_timeout ((guint) FRAME_TIME_MSEC,
                                   (GSourceFunc)gd_stack_xfade_cb, stack);
    }
  else
    gd_stack_set_xfade_position (stack, 1.0);
}

static void
set_visible_child (GdStack *stack,
		   GdStackChildInfo *child_info)
{
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *info;
  GtkWidget *widget = GTK_WIDGET (stack);
  GList *l;
  cairo_surface_t *surface;
  int surface_w, surface_h;
  cairo_t *cr;

  /* If none, pick first visible */
  if (child_info == NULL)
    {
      for (l = priv->children; l != NULL; l = l->next)
	{
          info = l->data;
	  if (gtk_widget_get_visible (info->widget))
	    {
	      child_info = info;
	      break;
	    }
	}
    }

  surface = NULL;
  if (priv->visible_child)
    {
      if (gtk_widget_is_visible (widget) &&
          /* Only crossfade in homogeneous mode */
          priv->homogeneous)
        {
          surface_w = gtk_widget_get_allocated_width (widget);
          surface_h = gtk_widget_get_allocated_height (widget);
          surface =
            gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR_ALPHA,
                                               surface_w, surface_h);
          cr = cairo_create (surface);
          gtk_widget_draw (priv->visible_child->widget, cr);
          cairo_destroy (cr);
        }

      gtk_widget_set_child_visible (priv->visible_child->widget, FALSE);
    }

  priv->visible_child = child_info;

  if (child_info)
    gtk_widget_set_child_visible (child_info->widget, TRUE);

  if (priv->xfade_surface)
    cairo_surface_destroy (priv->xfade_surface);

  priv->xfade_surface = surface;

  gtk_widget_queue_resize (GTK_WIDGET (stack));

  g_object_notify (G_OBJECT (stack), "visible-child");
  g_object_notify (G_OBJECT (stack), "visible-child-name");

  gd_stack_start_xfade (stack);

}

static void
stack_child_visibility_notify_cb (GObject *obj,
				  GParamSpec *pspec,
				  gpointer user_data)
{
  GdStack *stack = GD_STACK (user_data);
  GdStackPrivate *priv = stack->priv;
  GtkWidget *child = GTK_WIDGET (obj);
  GdStackChildInfo *child_info;

  child_info = find_child_info_for_widget (stack, child);

  if (priv->visible_child == NULL &&
      gtk_widget_get_visible (child))
    set_visible_child (stack, child_info);
  else if (priv->visible_child == child_info &&
	   !gtk_widget_get_visible (child))
    set_visible_child (stack, NULL);
}

void
gd_stack_add_named (GdStack    *stack,
                    GtkWidget  *child,
                    const char *name)
{
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;

  g_return_if_fail (child != NULL);

  child_info = g_slice_new (GdStackChildInfo);
  child_info->widget = child;
  child_info->name = g_strdup (name);

  priv->children = g_list_append (priv->children, child_info);

  gtk_widget_set_parent (child, GTK_WIDGET (stack));

  g_signal_connect (child, "notify::visible",
                    G_CALLBACK (stack_child_visibility_notify_cb), stack);

  if (priv->visible_child == NULL &&
      gtk_widget_get_visible (child))
    set_visible_child (stack, child_info);
  else
    gtk_widget_set_child_visible (child, FALSE);

  if (priv->homogeneous)
    gtk_widget_queue_resize (GTK_WIDGET (stack));
}

static void
gd_stack_add (GtkContainer *container,
	      GtkWidget *child)
{
  gd_stack_add_named (GD_STACK (container), child, NULL);
}
static void
gd_stack_remove (GtkContainer *container,
		 GtkWidget    *child)
{
  GdStack *stack = GD_STACK (container);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  gboolean was_visible;

  child_info = find_child_info_for_widget (stack, child);
  if (child_info == NULL)
    return;

  priv->children = g_list_remove (priv->children, child_info);

  g_signal_handlers_disconnect_by_func (child,
					stack_child_visibility_notify_cb,
					stack);

  was_visible = gtk_widget_get_visible (child);

  if (priv->visible_child == child_info)
    set_visible_child (stack, NULL);

  gtk_widget_unparent (child);

  g_slice_free (GdStackChildInfo, child_info);

  if (priv->homogeneous && was_visible)
    gtk_widget_queue_resize (GTK_WIDGET (stack));
}

void
gd_stack_set_homogeneous (GdStack *stack,
			 gboolean homogeneous)
{
  GdStackPrivate *priv;

  g_return_if_fail (stack != NULL);

  priv = stack->priv;

  homogeneous = !!homogeneous;

  if (priv->homogeneous == homogeneous)
    return;

  priv->homogeneous = homogeneous;

  gtk_widget_queue_resize (GTK_WIDGET (stack));

  g_object_notify (G_OBJECT (stack), "homogeneous");
}

gboolean
gd_stack_get_homogeneous (GdStack *stack)
{
  g_return_val_if_fail (stack != NULL, FALSE);

  return stack->priv->homogeneous;
}

gint
gd_stack_get_duration (GdStack *stack)
{
  g_return_val_if_fail (stack != NULL, 0);

  return stack->priv->duration;
}

void
gd_stack_set_duration (GdStack *stack,
                       gint value)
{
  g_return_if_fail (stack != NULL);

  stack->priv->duration = value;
  g_object_notify (G_OBJECT (stack), "duration");
}

/**
 * gd_stack_get_visible_child:
 * @stack: a #GdStack
 *
 * Gets the currently visible child of the #GdStack, or %NULL if the
 * there are no visible children. The returned widget does not have a reference
 * added, so you do not need to unref it.
 *
 * Return value: (transfer none): pointer to child of the #GdStack
 **/
GtkWidget *
gd_stack_get_visible_child (GdStack *stack)
{
  g_return_val_if_fail (stack != NULL, NULL);

  return stack->priv->visible_child->widget;
}

const char *
gd_stack_get_visible_child_name (GdStack *stack)
{
  g_return_val_if_fail (stack != NULL, NULL);

  if (stack->priv->visible_child)
    return stack->priv->visible_child->name;

  return NULL;
}

void
gd_stack_set_visible_child (GdStack    *stack,
			    GtkWidget  *child)
{
  GdStackChildInfo *child_info;

  g_return_if_fail (stack != NULL);
  g_return_if_fail (child != NULL);

  child_info = find_child_info_for_widget (stack, child);
  if (child_info == NULL)
    return;

  if (gtk_widget_get_visible (child_info->widget))
    set_visible_child (stack, child_info);
}

void
gd_stack_set_visible_child_name (GdStack    *stack,
				 const char *name)
{
  GdStackPrivate *priv;
  GdStackChildInfo *child_info, *info;
  GList *l;

  g_return_if_fail (stack != NULL);
  g_return_if_fail (name != NULL);

  priv = stack->priv;

  child_info = NULL;
  for (l = priv->children; l != NULL; l = l->next)
    {
      info = l->data;
      if (info->name != NULL &&
	  strcmp (info->name, name) == 0)
	{
	  child_info = info;
	  break;
	}
    }

  if (child_info != NULL && gtk_widget_get_visible (child_info->widget))
    set_visible_child (stack, child_info);
}

static void
gd_stack_forall (GtkContainer *container,
		 gboolean      include_internals,
		 GtkCallback   callback,
		 gpointer      callback_data)
{
  GdStack *stack = GD_STACK (container);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  GList *l;

  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      (* callback) (child_info->widget, callback_data);
    }
}

static void
gd_stack_compute_expand (GtkWidget      *widget,
			 gboolean       *hexpand_p,
			 gboolean       *vexpand_p)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;
  gboolean hexpand, vexpand;
  GdStackChildInfo *child_info;
  GtkWidget *child;
  GList *l;

  hexpand = FALSE;
  vexpand = FALSE;
  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      child = child_info->widget;

      if (!hexpand &&
	  gtk_widget_compute_expand (child, GTK_ORIENTATION_HORIZONTAL))
	hexpand = TRUE;

      if (!vexpand &&
	  gtk_widget_compute_expand (child, GTK_ORIENTATION_VERTICAL))
	vexpand = TRUE;

      if (hexpand && vexpand)
	break;
    }

  *hexpand_p = hexpand;
  *vexpand_p = vexpand;
}

static gboolean
gd_stack_draw (GtkWidget *widget,
	       cairo_t *cr)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;

  if (priv->visible_child)
    {
      if (priv->xfade_pos < 1.0)
        {
          if (priv->xfade_surface)
            {
              cairo_set_source_surface (cr, priv->xfade_surface, 0, 0);
              cairo_set_operator (cr, CAIRO_OPERATOR_ADD);
              cairo_paint_with_alpha (cr, MAX (1.0 - priv->xfade_pos, 0));
            }

          cairo_push_group (cr);
          cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
          gtk_container_propagate_draw (GTK_CONTAINER (stack),
                                        priv->visible_child->widget,
                                        cr);
          cairo_pop_group_to_source (cr);
          cairo_set_operator (cr, CAIRO_OPERATOR_ADD);
          cairo_paint_with_alpha (cr, priv->xfade_pos);
        }
      else
        gtk_container_propagate_draw (GTK_CONTAINER (stack),
                                      priv->visible_child->widget,
                                      cr);
    }

  return TRUE;
}

static void
gd_stack_size_allocate (GtkWidget *widget,
			GtkAllocation *allocation)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;

  g_return_if_fail (allocation != NULL);

  gtk_widget_set_allocation (widget, allocation);

  if (priv->visible_child)
    {
      GtkAllocation child_allocation = *allocation;
      gtk_widget_size_allocate (priv->visible_child->widget, &child_allocation);
    }
}

static void
gd_stack_get_preferred_height (GtkWidget *widget,
			       gint *minimum_height,
			       gint *natural_height)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  GtkWidget *child;
  gint child_min, child_nat;
  GList *l;

  *minimum_height = 0;
  *natural_height = 0;

  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      child = child_info->widget;

      if (!priv->homogeneous &&
	  priv->visible_child != child_info)
	continue;
      if (gtk_widget_get_visible (child))
	{
	  gtk_widget_get_preferred_height (child, &child_min, &child_nat);

	  *minimum_height = MAX (*minimum_height, child_min);
	  *natural_height = MAX (*natural_height, child_nat);
	}
    }
}

static void
gd_stack_get_preferred_height_for_width (GtkWidget* widget,
					 gint width,
					 gint *minimum_height,
					 gint *natural_height)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  GtkWidget *child;
  gint child_min, child_nat;
  GList *l;

  *minimum_height = 0;
  *natural_height = 0;

  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      child = child_info->widget;

      if (!priv->homogeneous &&
	  priv->visible_child != child_info)
	continue;
      if (gtk_widget_get_visible (child))
	{
	  gtk_widget_get_preferred_height_for_width (child, width, &child_min, &child_nat);

	  *minimum_height = MAX (*minimum_height, child_min);
	  *natural_height = MAX (*natural_height, child_nat);
	}
    }
}

static void
gd_stack_get_preferred_width (GtkWidget *widget,
			       gint *minimum_width,
			       gint *natural_width)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  GtkWidget *child;
  gint child_min, child_nat;
  GList *l;

  *minimum_width = 0;
  *natural_width = 0;

  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      child = child_info->widget;

      if (!priv->homogeneous &&
	  priv->visible_child != child_info)
	continue;
      if (gtk_widget_get_visible (child))
	{
	  gtk_widget_get_preferred_width (child, &child_min, &child_nat);

	  *minimum_width = MAX (*minimum_width, child_min);
	  *natural_width = MAX (*natural_width, child_nat);
	}
    }
}

static void
gd_stack_get_preferred_width_for_height (GtkWidget* widget,
					 gint height,
					 gint *minimum_width,
					 gint *natural_width)
{
  GdStack *stack = GD_STACK (widget);
  GdStackPrivate *priv = stack->priv;
  GdStackChildInfo *child_info;
  GtkWidget *child;
  gint child_min, child_nat;
  GList *l;

  *minimum_width = 0;
  *natural_width = 0;

  for (l = priv->children; l != NULL; l = l->next)
    {
      child_info = l->data;
      child = child_info->widget;

      if (!priv->homogeneous &&
	  priv->visible_child != child_info)
	continue;
      if (gtk_widget_get_visible (child))
	{
	  gtk_widget_get_preferred_width_for_height (child, height, &child_min, &child_nat);

	  *minimum_width = MAX (*minimum_width, child_min);
	  *natural_width = MAX (*natural_width, child_nat);
	}
    }
}