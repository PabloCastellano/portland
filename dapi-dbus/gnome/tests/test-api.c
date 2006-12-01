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

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include "tests.h"

#define DAPI_DBUS_SERVICE   "org.freedesktop.dapi"
#define DAPI_DBUS_PATH      "/org/freedesktop/dapi"
#define DAPI_DBUS_INTERFACE "org.freedesktop.dapi"

DBusGProxy *
tests_get_dbus_proxy (void)
{
	DBusGConnection   *connection;
	static DBusGProxy *proxy = NULL;
	GError            *error = NULL;
  
	if (proxy) {
		return proxy;
	}

	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	if (connection == NULL) {
		g_warning ("Failed to open connection to session bus: %s\n",
			   error->message);
		g_clear_error (&error);
		return NULL;
	}

	proxy = dbus_g_proxy_new_for_name (connection,
					   DAPI_DBUS_SERVICE,
					   DAPI_DBUS_PATH,
					   DAPI_DBUS_INTERFACE);

	return proxy;
}

static GValue *
tests_get_windowinfo (gboolean clear)
{
	static GValue windowinfo = {0, };

	if (clear && G_IS_VALUE (&windowinfo)) {
		g_value_unset (&windowinfo);
		return NULL;
	}

	if (G_IS_VALUE (&windowinfo)) {
		return &windowinfo;
	}

	g_value_init (&windowinfo, G_TYPE_INT);

	return &windowinfo;
}

/*
 * Actual tests 
 */

START_TEST (test_get_capabilities)
{
	DBusGProxy *proxy;
	gboolean    result;
	gint        capabilities;
	
	proxy = tests_get_dbus_proxy ();
	
	result = org_freedesktop_dapi_capabilities (proxy, &capabilities, NULL);
	fail_if (result == FALSE);
	fail_if (capabilities < 1);
}
END_TEST

START_TEST (test_get_button_order)
{
	DBusGProxy *proxy;
	gboolean    result;
	gint        order;
	
	proxy = tests_get_dbus_proxy ();
	
	result = org_freedesktop_dapi_button_order (proxy, &order, NULL);
	fail_if (result == FALSE);
	fail_if (order < 1);
	fail_if (order > 2);
}
END_TEST

START_TEST (test_run_as_user)
{
	DBusGProxy *proxy;
	gboolean    result;
	
	proxy = tests_get_dbus_proxy ();

	/* Expected successes */
	result = org_freedesktop_dapi_run_as_user (proxy, "root", "/usr/bin/test", 
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == FALSE);

	/* Expected failures */

	/* gksu doesn't return FALSE on failure, so we always get
	 * TRUE, this test can't be concluded until we can trust the
	 * return value.
	 *
	 * result = org_freedesktop_dapi_run_as_user (proxy, "invalidusername", "/test", NULL);
	 * fail_if (result == TRUE); 
	 */

	result = org_freedesktop_dapi_run_as_user (proxy, NULL, NULL,
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == TRUE);
}
END_TEST

START_TEST (test_screen_saving_suspend)
{
	DBusGProxy *proxy;
	gboolean    result;
	
	proxy = tests_get_dbus_proxy ();

	result = org_freedesktop_dapi_suspend_screen_saving (proxy, 1, TRUE, NULL);
	fail_if (result == FALSE);

	/* Should fail due to the client id already setting suspend on */
	result = org_freedesktop_dapi_suspend_screen_saving (proxy, 1, TRUE, NULL);
	fail_if (result == TRUE);

	result = org_freedesktop_dapi_suspend_screen_saving (proxy, 1, FALSE, NULL);
	fail_if (result == FALSE);

	/* Should fail due to the client id already setting resume */
	result = org_freedesktop_dapi_suspend_screen_saving (proxy, 1, FALSE, NULL);
	fail_if (result == TRUE);
}
END_TEST

START_TEST (test_open_url)
{
	DBusGProxy *proxy;
	gboolean    result;
	
	proxy = tests_get_dbus_proxy ();
	
	/* Expected successes */
	result = org_freedesktop_dapi_open_url (proxy, "http://www.google.com",
						tests_get_windowinfo (FALSE), NULL);
	fail_if (result == FALSE);

	/* Expected failures */
	result = org_freedesktop_dapi_open_url (proxy, "www.google.com", 
						tests_get_windowinfo (FALSE), NULL);
	fail_if (result == TRUE);
}
END_TEST

START_TEST (test_execute_url)
{
	DBusGProxy *proxy;
	gboolean    result;
	
	proxy = tests_get_dbus_proxy ();
	
	/* Expected successes */
	result = org_freedesktop_dapi_execute_url (proxy, "http://www.imendio.com", 
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == FALSE);

	result = org_freedesktop_dapi_execute_url (proxy, "file:///etc/fstab", 
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == FALSE);

	/* Expected failures */
	result = org_freedesktop_dapi_execute_url (proxy, "etc/fstab", 
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == TRUE);

	result = org_freedesktop_dapi_execute_url (proxy, "aaa:///etc/fstab", 
						   tests_get_windowinfo (FALSE), NULL);
	fail_if (result == TRUE);
}
END_TEST

START_TEST (test_mail_to)
{
	DBusGProxy   *proxy;
	const gchar  *nullp[] = { 0, };
	gboolean      result;
	
	proxy = tests_get_dbus_proxy ();

	/* Note: DBus sucks because it doesn't NULL check the
	 * G_TYPE_STRV attachments so we have to pass our nullp
	 * variable in otherwise it will SEGV, --MJR
	 */

	/* Expected successes */
	result = org_freedesktop_dapi_mail_to (proxy,
					       "test", "foo",
					       "martyn@imendio.com",
					       "micke@imendio.com",
					       "waldo.bastian@intel.com",
					       nullp, 
					       tests_get_windowinfo (FALSE),
					       NULL);
	fail_if (result == FALSE);
}
END_TEST

START_TEST (test_files)
{
	DBusGProxy   *proxy;
	gchar        *filename;
	gchar        *p = NULL;
	gboolean      result;
	
	proxy = tests_get_dbus_proxy ();

	/* Expected failures */
	result = org_freedesktop_dapi_local_file (proxy,
						  "http://www.google.com/index.html", 
						  "file:///tmp/index.html",
						  FALSE,
						  tests_get_windowinfo (FALSE),
						  &filename,
						  NULL);

	
	fail_if (result == TRUE);

	/* FIXME: This next line WILL fail, because DBus sucks and
	 * doesn't set data that we are receiving to NULL when there
	 * is an error.
	 *	 
	 * fail_if (filename != NULL || strlen (filename) > 0); 
	 */

	result = org_freedesktop_dapi_remove_temporary_local_file (proxy, "file:///tmp/somefile.txt",  NULL);
	fail_if (result == TRUE);

	/* Expected successes (we do this after because we use the other APIs to clean up */
	result = org_freedesktop_dapi_local_file (proxy,
						  "http://www.google.com/index.html", 
						  "file:///tmp/index.html",
						  TRUE,
						  tests_get_windowinfo (FALSE),
						  &filename,
						  NULL);
					
	/* FIXME: We can't use a filename in the format of
	 * 'file:///tmp/foo' here, we need to remove the 'file://' part 
	 */
	if (filename && strstr (filename, "file://")) {
		p = filename + 7;
	}

	fail_if (result == FALSE);
	fail_if (filename == NULL || strlen (filename) < 1);
	fail_if (p && g_file_test (p, G_FILE_TEST_EXISTS) == FALSE);

	result = org_freedesktop_dapi_remove_temporary_local_file (proxy, filename,  NULL);
	fail_if (result == FALSE);
	fail_if (p && g_file_test (p, G_FILE_TEST_EXISTS) == TRUE);

	/* Final clean up */
	g_free (filename);
}
END_TEST

static Suite *
tests_create_synchronous_test_suite (void)
{
	Suite *suite;
	TCase *t_case;

	suite = suite_create ("Synchronous");

	t_case = tcase_create ("Capabilities");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_get_capabilities); 
	t_case = tcase_create ("Button Order");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_get_button_order); 
	t_case = tcase_create ("Run As User");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_run_as_user); 
	t_case = tcase_create ("Suspend Screen Saving");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_screen_saving_suspend); 
	t_case = tcase_create ("Opening URL");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_open_url); 
	t_case = tcase_create ("Executing URL");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_execute_url); 
	t_case = tcase_create ("Creating Mail");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_mail_to); 

	return suite;
}

static Suite *
tests_create_asynchronous_test_suite (void)
{
	Suite *suite;
	TCase *t_case;

	suite = suite_create ("Asynchronous");

	t_case = tcase_create ("Local/Upload/Remove");
	suite_add_tcase (suite, t_case);
	tcase_add_test (t_case, test_files); 

	return suite;
}

int
main (int argc, char **argv)
{
	DBusGProxy *proxy;
	Suite      *suite;
	SRunner    *runner;
	int         failures, successes;

	g_type_init ();

	proxy = tests_get_dbus_proxy ();
	if (!proxy) {
		g_warning ("Could not connect to DBus service");
		return EXIT_FAILURE;
	}

	suite = suite_create ("-");
	runner = srunner_create (suite);
 	srunner_add_suite (runner, tests_create_synchronous_test_suite ()); 
 	srunner_add_suite (runner, tests_create_asynchronous_test_suite ()); 
	srunner_add_suite (runner, tests_create_addressbook_test_suite ());

	srunner_run_all (runner, CK_NORMAL);
	failures = srunner_ntests_failed (runner);
	successes = srunner_ntests_run (runner) - failures;
	srunner_free (runner);

	/* Clean up */
	tests_get_windowinfo (TRUE);

	if (successes < 1) {
		g_print ("\nHave you checked that the daemon is running?\n\n");
	}

	return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
