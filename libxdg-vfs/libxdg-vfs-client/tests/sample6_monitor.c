/*
#   sample6_monitor.c
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

#include "xdg_vfs_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

void myXdgVfsMonitorCallback(XdgVfsSession * sess, char * uri, void * user_data)
{
	printf("changed: %s\n", uri);
}

int main(int argc, char *argv[])
{
	XdgVfsResult r;
	XdgVfsSession * session;

	if (argc!=3) {
		fprintf(stderr,"usage: %s {gnome|kde|this} MONITOR_DIR_URI\n", argv[0]);
		exit(1);
	}

	r = xdg_vfs_sess_start(&session, argv[1]);

	if (r) {
		fprintf(stderr, "sess start error=%d\n", r);
		return 1;
	}

	xdg_vfs_sess_set_monitor_callback(session, myXdgVfsMonitorCallback, NULL);

	fprintf(stderr, "======= test : monitor  =======\n", r);



	r = xdg_vfs_sess_cmd_monitorDir(session, argv[2]);

	if (r) {
		fprintf(stderr, "monitor cmd problem=%d\n", r);
		return 1;
	}

	fprintf(stderr, "Now waiting for monitor events!\n");

	if(xdg_vfs_sess_isBusy(session))
	{
		fprintf(stderr, "We can't wait for monitor events when the session is busy!\n");
		return 1;
	}

	/* because the session is in READY state, we can not loop with r=XDGVFS_RESULT_CONTINUES 
	   as with xdg-vfs commands */

	XdgVfsItemType type=0;
	XdgVfsItem * item=NULL;

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_OK) 
	{
		switch (type)
		{
			case XDGVFS_ITEMTYPE_NONE:
			{
					break;
			}
			default:
			{
					fprintf(stderr, "unexpected item - type=%d\n", type);
					break;
			}
		}
		xdg_vfs_item_unref(item);
	}

	fprintf(stderr, "cmd monitor result=%d\n", r);


	xdg_vfs_sess_close(session);
	
}
