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

#ifndef __DAPI_COMMON_H__
#define __DAPI_COMMON_H__

typedef enum {
 	DAPI_CAP_OPENURL                   = 1 << 0,
	DAPI_CAP_EXECUTEURL                = 1 << 1,
 	DAPI_CAP_BUTTONORDER               = 1 << 2,
	DAPI_CAP_RUNASUSER                 = 1 << 3,
  	DAPI_CAP_SUSPENDSCREENSAVING       = 1 << 4,
	DAPI_CAP_MAILTO                    = 1 << 5, 
 	DAPI_CAP_LOCALFILE                 = 1 << 6, 
 	DAPI_CAP_UPLOADFILE                = 1 << 7, 
 	DAPI_CAP_REMOVETEMPORARYLOCALFILE  = 1 << 8,
  	DAPI_CAP_ADDRESSBOOKLIST           = 1 << 9,  
	DAPI_CAP_ADDRESSBOOKGETNAME        = 1 << 10,
 	DAPI_CAP_ADDRESSBOOKGETEMAILS      = 1 << 11, 
 	DAPI_CAP_ADDRESSBOOKFINDBYNAME     = 1 << 12,  
 	DAPI_CAP_ADDRESSBOOKOWNER          = 1 << 13, 
 	DAPI_CAP_ADDRESSBOOKGETVCARD30     = 1 << 14 
} DapiCapability;

#endif /* __DAPI_COMMON_H__ */

