/*
#   sample2_getfile.c
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

int main(int argc, char *argv[])
{
	XdgVfsResult r;
	XdgVfsSession * session;

	if (argc!=3) {
		fprintf(stderr,"usage: %s {gnome|kde|this} FILENAME\n", argv[0]);
		exit(1);
	}

	r = xdg_vfs_sess_start(&session, argv[1]);

	if (r) {
		fprintf(stderr, "start session error=%d\n", r);
		return 1;
	}

	fprintf(stderr, "======= test : getFile -> stdout =======\n", r);

	r = xdg_vfs_sess_cmd_getFile(session, argv[2]);

	if (r) {
		fprintf(stderr, "test cmd problem=%d\n", r);
		return 1;
	}

	char * buf=NULL;
	int len=0;
	XdgVfsItemType type;
	XdgVfsItem * item;

	while ((r = xdg_vfs_sess_readItem(session, &type, &item, &buf, &len)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch (type)
		{
			case XDGVFS_ITEMTYPE_GET_HEAD:
			{
					XdgVfsSimpleHead * head = (XdgVfsSimpleHead*) item;
					fprintf(stderr, "reading file uri='%s'\n", head->uri);
					break;		
			}
			case XDGVFS_DATAIN:
			{ 
					fprintf(stderr, "file-data chunklen=%d\n", len);
					fwrite (buf, 1, len, stdout); /* your file data */
					break;
			}
			case XDGVFS_ITEMTYPE_DATAIN_DONE:
			{
					XdgVfsDataInDoneItem * dii = (XdgVfsDataInDoneItem *) item;
					fprintf(stderr, "read done - bytecount='%d'\n", dii->bytecount);
					break;		
			}			
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
	if (r) {
		fprintf(stderr, "test 4 result=%d\n", r);
		return 1;
	}

	xdg_vfs_sess_close(session);
	sleep (1);
}
