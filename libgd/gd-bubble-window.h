/*
 * Copyright © 2013 Carlos Garnacho <carlosg@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GD_BUBBLE_WINDOW_H__
#define __GD_BUBBLE_WINDOW_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GD_TYPE_BUBBLE_WINDOW           (_gd_bubble_window_get_type ())
#define GD_BUBBLE_WINDOW(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), GD_TYPE_BUBBLE_WINDOW, GdBubbleWindow))
#define GD_BUBBLE_WINDOW_CLASS(c)       (G_TYPE_CHECK_CLASS_CAST ((c), GD_TYPE_BUBBLE_WINDOW, GdBubbleWindowClass))
#define GD_IS_BUBBLE_WINDOW(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), GD_TYPE_BUBBLE_WINDOW))
#define GD_IS_BUBBLE_WINDOW_CLASS(o)    (G_TYPE_CHECK_CLASS_TYPE ((o), GD_TYPE_BUBBLE_WINDOW))
#define GD_BUBBLE_WINDOW_GET_CLASS(o)   (G_TYPE_INSTANCE_GET_CLASS ((o), GD_TYPE_BUBBLE_WINDOW, GdBubbleWindowClass))

typedef struct _GdBubbleWindow GdBubbleWindow;
typedef struct _GdBubbleWindowClass GdBubbleWindowClass;

struct _GdBubbleWindow
{
  GtkWindow parent_instance;

  /*< private >*/
  gpointer priv;
};

struct _GdBubbleWindowClass
{
  GtkWindowClass parent_class;
};

GType       _gd_bubble_window_get_type        (void) G_GNUC_CONST;

GtkWidget * _gd_bubble_window_new             (void);

void        _gd_bubble_window_set_relative_to (GdBubbleWindow *window,
                                               GdkWindow       *relative_to);
GdkWindow * _gd_bubble_window_get_relative_to (GdBubbleWindow *window);

void        _gd_bubble_window_set_pointing_to (GdBubbleWindow       *window,
                                               cairo_rectangle_int_t *rect);
gboolean    _gd_bubble_window_get_pointing_to (GdBubbleWindow       *window,
                                               cairo_rectangle_int_t *rect);
void        _gd_bubble_window_set_position    (GdBubbleWindow       *window,
                                               GtkPositionType        position);

GtkPositionType
            _gd_bubble_window_get_position    (GdBubbleWindow       *window);

void        _gd_bubble_window_popup           (GdBubbleWindow       *window,
                                               GdkWindow             *relative_to,
                                               cairo_rectangle_int_t *pointing_to,
                                               GtkPositionType        position);

void        _gd_bubble_window_popdown         (GdBubbleWindow       *window);

gboolean    _gd_bubble_window_grab            (GdBubbleWindow       *window,
                                               GdkDevice             *device,
                                               guint32                activate_time);

void        _gd_bubble_window_ungrab          (GdBubbleWindow       *window);

G_END_DECLS

#endif /* __GD_BUBBLE_WINDOW_H__ */
