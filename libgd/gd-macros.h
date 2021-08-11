/*
 * Copyright (c) 2012 Red Hat, Inc.
 * Copyright (c) 2021 Chun-wei Fan
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
 */

#ifndef __GD_MACROS_H__
#define __GD_MACROS_H__

#ifndef GD_API
# ifdef MSVC_EXPORT_DLL
#  define GD_API __declspec (dllexport)
# else
#  define GD_API
# endif
#endif

#endif /* __GD_MACROS_H__ */
