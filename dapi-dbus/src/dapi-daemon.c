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
 * Author: Mikael Hallendal <micke@imendio.com>
 * Author: Martyn Russell <martyn@imendio.com>
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <libgnomevfs/gnome-vfs.h>

#include "dapi-debug.h"
#include "dapi-command.h"
#include "dapi-command-glue.h"
#include "dapi-gnome.h"

#define DEBUG_DOMAIN "Daemon"

static GMainLoop *main_loop;

static void daemon_show_version           (void);
static void daemon_show_debug_information (void);
static void daemon_quit_cb                (int sig);

static gboolean show_version = FALSE;
static gboolean show_debug_information = FALSE;

static const GOptionEntry options[] = {
	{ "version", 'v', 
	  0, G_OPTION_ARG_NONE, &show_version,
	  N_("Show version information"),
	  NULL },
	{ "debug", 'd', 
	  0, G_OPTION_ARG_NONE, &show_debug_information,
	  N_("Show debugging information"),
	  NULL },
	{ NULL }
};

static void
daemon_show_version (void)
{
	g_print ("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
}

static void
daemon_show_debug_information (void)
{
	g_print ("\n"
		 "You can set the \"DAPI_DEBUG\" environment variable to a \n"
		 "colon/comma/space separated list of domains you want \n"
		 "debugging information on, or \"all\" to get all debug output.\n"
		 "\n");
}

static void
daemon_quit_cb (int sig)
{
	dapi_debug (DEBUG_DOMAIN, "Caught SIGINT or SIGQUIT");
	g_main_loop_quit (main_loop);
}

static gboolean
daemon_register_object (DBusGConnection *connection,
			GObject         *obj)
{
	DBusGProxy *proxy;
	GError     *error = NULL;
	guint       result;
	
	dapi_debug (DEBUG_DOMAIN, "Registering DBus object...");

        proxy = dbus_g_proxy_new_for_name (connection,
					   DBUS_SERVICE_DBUS,
					   DBUS_PATH_DBUS,
					   DBUS_INTERFACE_DBUS);

	if (!org_freedesktop_DBus_request_name (proxy,
						DAPI_SERVICE,
						DBUS_NAME_FLAG_DO_NOT_QUEUE,
						&result, &error)) {
		g_warning ("Could not aquire name:'%s', %s",
			   DAPI_SERVICE,
			   error ? error->message : "no error given");
		g_error_free (error);
		g_object_unref (proxy);

 		return FALSE;
	}

        g_object_unref (proxy);

        if (result != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		g_warning ("DBus service name already taken, "
			   "are you already running this daemon?");
		return FALSE;
	}

	dbus_g_object_type_install_info (DAPI_TYPE_COMMAND, &dbus_glib_dapi_command_object_info);
	dbus_g_connection_register_g_object (connection, DAPI_PATH, obj);

	dapi_debug (DEBUG_DOMAIN, "Registered!");
	return TRUE;
}

int
main (int argc, char **argv)
{
	GOptionContext  *context;
	GError          *error = NULL;
	DBusGConnection *bus;
	DapiCommand     *command; 

	g_type_init ();

	context = g_option_context_new ("- DAPI Gnome Daemon");
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	if (show_version) {
		daemon_show_version ();
		return EXIT_SUCCESS;
	}

	if (show_debug_information) {
		daemon_show_debug_information ();
		return EXIT_SUCCESS;
	}

	/* Make sure we quit nicely with Ctrl+C */
	signal (SIGINT, daemon_quit_cb);
	signal (SIGQUIT, daemon_quit_cb);

	dapi_debug (DEBUG_DOMAIN, "Starting up...");

	bus = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (!bus) {
		g_warning ("Could not connect to the session bus, %s",
			   error ? error->message : "no error given.");
 		return EXIT_FAILURE; 
	}

 	dapi_debug (DEBUG_DOMAIN, "Creating new command object"); 
 	command = g_object_new (DAPI_TYPE_COMMAND, NULL); 

	if (!daemon_register_object (bus, G_OBJECT (command))) {
		return EXIT_FAILURE;
	}

	if (!dapi_gnome_startup ()) {
		return EXIT_FAILURE;
	}

	dapi_debug (DEBUG_DOMAIN, "Starting main loop");
	dapi_debug (DEBUG_DOMAIN, "Ready!");

	main_loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (main_loop);

	dapi_debug (DEBUG_DOMAIN, "Shutting down...");

	dapi_gnome_shutdown ();

 	if (command) { 
 		g_object_unref (command); 
 	} 

	g_main_loop_unref (main_loop);

	return EXIT_SUCCESS;
}
