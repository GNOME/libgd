/*
 * gd-bubble-button.h
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


#ifndef __GD_BUBBLE_BUTTON_H__
#define __GD_BUBBLE_BUTTON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GD_TYPE_BUBBLE_BUTTON             (gd_bubble_button_get_type ())
#define GD_BUBBLE_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GD_TYPE_BUBBLE_BUTTON, GdBubbleButton))
#define GD_BUBBLE_BUTTON_CONST(obj)       (G_TYPE_CHECK_INSTANCE_CAST ((obj), GD_TYPE_BUBBLE_BUTTON, GdBubbleButton const))
#define GD_BUBBLE_BUTTON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GD_TYPE_BUBBLE_BUTTON, GdBubbleButtonClass))
#define GD_IS_BUBBLE_BUTTON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GD_TYPE_BUBBLE_BUTTON))
#define GD_IS_BUBBLE_BUTTON_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GD_TYPE_BUBBLE_BUTTON))
#define GD_BUBBLE_BUTTON_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GD_TYPE_BUBBLE_BUTTON, GdBubbleButtonClass))

typedef struct _GdBubbleButton        GdBubbleButton;
typedef struct _GdBubbleButtonClass   GdBubbleButtonClass;
typedef struct _GdBubbleButtonPrivate GdBubbleButtonPrivate;

struct _GdBubbleButton
{
  GtkToggleButton parent;

  GdBubbleButtonPrivate *priv;
};

struct _GdBubbleButtonClass
{
  GtkToggleButtonClass parent_class;
};

GType                 gd_bubble_button_get_type        (void) G_GNUC_CONST;

GtkWidget            *gd_bubble_button_new             (void);

void                  gd_bubble_button_set_direction   (GdBubbleButton *button,
                                                        GtkArrowType    direction);

GtkArrowType          gd_bubble_button_get_direction   (GdBubbleButton *button);

GtkWidget            *gd_bubble_button_get_widget      (GdBubbleButton *button);

void                  gd_bubble_button_set_widget      (GdBubbleButton *button,
                                                        GtkWidget      *widget);

G_END_DECLS

#endif /* __GD_BUBBLE_BUTTON_H__ */

/* ex:set ts=2 et: */
