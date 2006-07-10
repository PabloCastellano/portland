/*
#   xdg_vfs_gnome_monitor.c
#
#   Copyright 2006, Norbert Frese
#
#   LICENSE:
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
#   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.
#
#---------------------------------------------*/

#include <string.h>

#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomeui/libgnomeui.h>
#include <time.h>

#include "xdg_vfs_common.h"
#include "xdg_vfs.h"
#include "xdg_vfs_gnome.h"

typedef struct 
{
	char * monitor_uri;
	GnomeVFSMonitorHandle * handle;
	int has_events;
} XdgVfsGnomeMonitor;

int global_event_count=0;

GList * monitor_list = NULL;

void myGnomeVFSMonitorCallback(GnomeVFSMonitorHandle *handle,
		const gchar *monitor_uri,
		const gchar *info_uri,
		GnomeVFSMonitorEventType event_type,
		gpointer user_data)
{
	GList *current_monitor;	
	int found = 0;
	/*fprintf(stderr, "monitor event monitor_uri=%s info_uri=%s event_type=%d\n",
		monitor_uri,
		info_uri,
		event_type);*/

    for (current_monitor = monitor_list; current_monitor != NULL; current_monitor = current_monitor->next) 
    {  
  	
		XdgVfsGnomeMonitor *monitor = (XdgVfsGnomeMonitor*)current_monitor->data;
		if (monitor->handle == handle) 
		{
			monitor->has_events++;
			global_event_count++;
			found = 1;
		}
    }			
    if (found==0) fprintf(stderr, 
    	"Warning: got event for unregistered handle monitor_uri=%s\n", monitor_uri); 
}

XdgVfsGnomeMonitor * new_xdg_vfs_gnome_monitor()
{
	XdgVfsGnomeMonitor * monitor = calloc(1,sizeof(XdgVfsGnomeMonitor));
	return monitor;
}


int xdg_vfs_monitor(char * uri, int is_directory, int opts)
{
	GnomeVFSResult r;
	GnomeVFSMonitorHandle *handle;
	char * full_uri = gnome_vfs_make_uri_from_input(uri);
		
	GList *current_monitor;
    for (current_monitor = monitor_list; current_monitor != NULL; current_monitor = current_monitor->next) 
    {  
		XdgVfsGnomeMonitor *monitor = (XdgVfsGnomeMonitor*)current_monitor->data;
		if( strcmp(uri, monitor->monitor_uri) ==0)
			return XDGVFS_ERRORCODE_ALREADY_EXISTS;  // monitor exists
    }

	/* dirty hack: watch computer:// for drive changes */
	char * watch_uri = strcmp(uri, "vfsroot://")==0 ? "computer://" : full_uri;

	r = gnome_vfs_monitor_add(&handle,
			watch_uri,
			is_directory ? GNOME_VFS_MONITOR_DIRECTORY : GNOME_VFS_MONITOR_FILE,
			myGnomeVFSMonitorCallback,
			NULL /* user_data */);
	
	if (r==GNOME_VFS_OK)
	{
		XdgVfsGnomeMonitor * monitor = new_xdg_vfs_gnome_monitor();
		monitor->handle = handle;
		monitor->monitor_uri = strdup(full_uri);
		monitor_list = g_list_append(monitor_list, monitor);
	}
	
	g_free(full_uri);
	if (r!=GNOME_VFS_OK) return print_and_translate_result(r);
	return XDGVFS_ERRORCODE_OK;
}


int xdg_vfs_monitor_cancel(char * uri, int opts)
{
	char * full_uri = gnome_vfs_make_uri_from_input(uri);
	GList *current_monitor;	
	int found = 0;
    for (current_monitor = monitor_list; current_monitor != NULL; current_monitor = current_monitor->next) 
    {  
		XdgVfsGnomeMonitor *monitor = (XdgVfsGnomeMonitor*)current_monitor->data;
		if( strcmp(full_uri, monitor->monitor_uri) == 0)
		{
			GnomeVFSResult r = gnome_vfs_monitor_cancel (monitor->handle);
			if (r!=GNOME_VFS_OK)
				print_and_translate_result(r);
			g_free(monitor->monitor_uri);
			monitor_list = g_list_remove(monitor_list, monitor);
			found = 1;
			break;
		}	
    }

    if (!found) 
    {
    	fprintf(stderr, "monitor with uri=%s not found!\n", full_uri);
   	    g_free(full_uri);
    	return XDGVFS_ERRORCODE_NOT_FOUND;
    }
    g_free(full_uri);
    return XDGVFS_ERRORCODE_OK;
}

int xdg_vfs_monitor_list_events(int opts)
{
	GList *current_monitor;	
	int save_count = global_event_count;
	int found = 0;
    for (current_monitor = monitor_list; current_monitor != NULL; current_monitor = current_monitor->next) 
    {  
		XdgVfsGnomeMonitor *monitor = (XdgVfsGnomeMonitor*)current_monitor->data;
		if (monitor->has_events)
		{
			fprintf(stdout, "%s %s\n", XDGVFS_TAG_MONITOREVENT, monitor->monitor_uri);
			monitor->has_events = 0;
		}
    }	
    fflush(stdout);
	global_event_count = 0;
	return save_count;
}

/* ======================== loop and wait for fd =============== */

typedef struct {
	GSource gs;
	GPollFD * gpoll;
	gboolean done;
} WaitGSource;


gboolean wait_gs_prepare(GSource    *source, gint *timeout_)
{
	WaitGSource * wsource = (WaitGSource*)source;
	//fprintf(stderr, "hi1\n");
	wsource->gpoll->events = G_IO_IN | G_IO_HUP | G_IO_ERR;
	*timeout_ = -1;
	return FALSE;
}

gboolean wait_gs_check(GSource    *source)
{
	WaitGSource * wsource = (WaitGSource*)source;
	//fprintf(stderr, "hi2 %d\n", wsource->gpoll->fd);
	if (wsource->gpoll->revents & (G_IO_IN | G_IO_HUP | G_IO_ERR))
	{
	   	//fprintf(stderr, "hi3\n");
		return TRUE;
	}
	return FALSE;	
}

gboolean wait_gs_dispatch(GSource    *source,
			GSourceFunc callback,
			gpointer    user_data)
{
	WaitGSource * wsource = (WaitGSource*)source;
	wsource->done = TRUE;
	return TRUE;
}

GSourceFuncs wait_gs_funcs = {
	.prepare = wait_gs_prepare,
	.check = wait_gs_check,
	.dispatch = wait_gs_dispatch
};

int xdg_vfs_iterate_mainloop_until_incoming_data(int fd)
{
	GPollFD gpoll={0};
	gpoll.fd = fd;
	
	GSource*  source =  g_source_new(&wait_gs_funcs, sizeof(WaitGSource));
	WaitGSource * wsource = (WaitGSource*)source;
	wsource->gpoll = &gpoll;
	wsource->done = FALSE;
	g_source_add_poll(source, &gpoll);
	g_source_set_can_recurse(source,TRUE);
	
	g_source_attach(source,NULL);	
                               
    //while (!(wsource->done)){
    	//fprintf(stderr, "loop\n");
    g_main_context_iteration(NULL,TRUE);
    //}
    int done = wsource->done;
    g_source_destroy (source);
    return done;               
}

