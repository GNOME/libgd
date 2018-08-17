/*
 * Copyright (c) 2011 Red Hat, Inc.
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
 * Author: Cosimo Cecchi <cosimoc@redhat.com>
 *
 */

#ifndef _GD_STYLED_TEXT_RENDERER_H
#define _GD_STYLED_TEXT_RENDERER_H

#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GD_TYPE_STYLED_TEXT_RENDERER gd_styled_text_renderer_get_type()
G_DECLARE_DERIVABLE_TYPE (GdStyledTextRenderer,
                          gd_styled_text_renderer,
                          GD,
                          STYLED_TEXT_RENDERER,
                          GtkCellRendererText)

struct _GdStyledTextRendererClass
{
  GtkCellRendererTextClass parent_class;
};

GtkCellRenderer *gd_styled_text_renderer_new (void);
void gd_styled_text_renderer_add_class (GdStyledTextRenderer *self,
                                        const gchar *class);
void gd_styled_text_renderer_remove_class (GdStyledTextRenderer *self,
                                           const gchar *class);

G_END_DECLS

#endif /* _GD_STYLED_TEXT_RENDERER_H */
