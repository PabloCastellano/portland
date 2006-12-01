/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2006 Imendio AB
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Martyn Russell <martyn@imendio.com>
 */

#ifndef __DAPI_DEBUG_H__
#define __DAPI_DEBUG_H__

#include <glib.h>

#ifdef G_HAVE_ISO_VARARGS
#  ifdef DAPI_DISABLE_DEBUG
#    define dapi_debug(...)
#  else
#    define dapi_debug(...) dapi_debug_impl (__VA_ARGS__)
#  endif
#elif defined(G_HAVE_GNUC_VARARGS)
#  if DAPI_DISABLE_DEBUG
#    define dapi_debug(fmt...)
#  else
#    define dapi_debug(fmt...) dapi_debug_impl(fmt)
#  endif
#else
#  if DAPI_DISABLE_DEBUG
#    define dapi_debug(x)
#  else
#    define dapi_debug dapi_debug_impl
#  endif
#endif

void dapi_debug_impl (const gchar *domain, const gchar *msg, ...);

#endif /* __DAPI_DEBUG_H__ */

