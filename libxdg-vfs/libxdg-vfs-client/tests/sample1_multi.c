/*
#   sample1_multi.c
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

/*
   demonstrates how to use the commands:
     xdg_vfs_sess_cmd_backendInfo()
     xdg_vfs_sess_cmd_getFileInfo()
     xdg_vfs_sess_cmd_listDirectory()
     xdg_vfs_sess_cmd_openFileDialog()
     xdg_vfs_sess_cmd_saveFileDialog()
     xdg_vfs_sess_cmd_getFile()
*/


#include "xdg_vfs_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int main(int argc, char *argv[])
{
	XdgVfsResult r;
	XdgVfsSession * session;

	if (argc!=2) {
		fprintf(stderr,"usage: %s {gnome|kde|this}\n", argv[0]);
		exit(1);
	}

	r = xdg_vfs_sess_start(&session, argv[1]);

	if (r) {
		fprintf(stderr, " session start problem=%d\n", r);
		return 1;
	}

	fprintf(stderr, "======= test 0 : backend info =======\n", r);

	r = xdg_vfs_sess_cmd_backendInfo(session);
	
	if (r) {
		fprintf(stderr, "test 0 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	XdgVfsItemType type;
	XdgVfsItem * item;

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_BACKENDINFO:
			{
					XdgVfsBackendInfo * backendInfo = (XdgVfsBackendInfo*) item;
					fprintf(stderr, "backend_id='%s' system_uri='%s' file_icon_theme='%s'\n", 
						backendInfo->backend_id,
						backendInfo->system_uri,
						backendInfo->file_icon_theme);
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
	fprintf(stderr, "test 0 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 1 : info / =======\n", r);

	r = xdg_vfs_sess_cmd_getFileInfo(session, "/");
	
	if (r) {
		fprintf(stderr, "test 1 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		//fprintf(stderr, "got item type %d\n", typeOut);
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_FILEINFO:
			{
					XdgVfsFileInfo * info = (XdgVfsFileInfo*) item;
					fprintf(stderr, "got fileinfo uri='%s'\n", info->uri);
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
	fprintf(stderr, "test 1 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 2 : info /notexist =======\n", r);

	r = xdg_vfs_sess_cmd_getFileInfo(session, "/notexist");
	if (r) {
		fprintf(stderr, "test 2 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_FILEINFO:
			{
					XdgVfsFileInfo * info = (XdgVfsFileInfo*) item;
					fprintf(stderr, "got fileinfo uri='%s'\n", info->uri);
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
	fprintf(stderr, "test 2 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 3 : ls /usr/local/bin =======\n", r);

	r = xdg_vfs_sess_cmd_listDirectory(session, "/usr/local/bin");

	if (r) {
		fprintf(stderr, "test 3 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_LS_HEAD:
			{
					XdgVfsSimpleHead * head = (XdgVfsSimpleHead*) item;
					fprintf(stdout, "got ls header uri='%s'\n", head->uri);
					break;
			}
			
			case XDGVFS_ITEMTYPE_FILEINFO:
			{
					XdgVfsFileInfo * info = (XdgVfsFileInfo*) item;
					fprintf(stdout, "got fileinfo uri='%s'\n", info->uri);	
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
	fprintf(stderr, "test 3 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 4 : openfiledlg =======\n", r);

	r = xdg_vfs_sess_cmd_openFileDialog(session, "/tmp", XDGVFS_FLAGS_NONE);
	if (r) {
		fprintf(stderr, "test 4 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE:
			{
					XdgVfsOpenFileDlgResponse * dlgResp = (XdgVfsOpenFileDlgResponse*) item;
					fprintf(stderr, "selected_uri='%s'\n", dlgResp->selected_uri);
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

	fprintf(stderr, "test 4 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 5 : savefiledlg =======\n", r);

	r = xdg_vfs_sess_cmd_saveFileDialog(session, "/tmp", "test.txt", XDGVFS_FLAGS_NONE);
	if (r) {
		fprintf(stderr, "test 5 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE:
			{
					XdgVfsSaveFileDlgResponse * dlgResp = (XdgVfsSaveFileDlgResponse*) item;
					fprintf(stderr, "selected_uri='%s'\n", dlgResp->selected_uri);
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
	fprintf(stderr, "test 5 result=%d %s\n", r, xdg_vfs_resultToString(r));

	fprintf(stderr, "======= test 6 : get /etc/passwd =======\n", r);

	r = xdg_vfs_sess_cmd_getFile(session, "/etc/passwd");
	if (r) {
		fprintf(stderr, "test 6 cmd problem #%d %s\n", r, xdg_vfs_resultToString(r));
		return 1;
	}

	char * buf;
	int len;

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, &buf, &len)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch (type)
		{
			case XDGVFS_ITEMTYPE_GET_HEAD:
			{
					XdgVfsSimpleHead * sh = (XdgVfsSimpleHead*) item;
					fprintf(stderr, "'get' header uri='%s'\n", sh->uri);
					break;		
			}
			case XDGVFS_DATAIN:
			{ 
					fprintf(stderr, "'data' chunklen=%d\n", len);
					fwrite (buf, 1, len, stderr);
					break;
			}
			case XDGVFS_ITEMTYPE_DATAIN_DONE:
			{
					XdgVfsDataInDoneItem * dii = (XdgVfsDataInDoneItem *) item;
					fprintf(stderr, "data done bytecount='%d'\n", dii->bytecount);
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
	fprintf(stderr, "test 6 result=%d %s\n", r, xdg_vfs_resultToString(r));
	return 1;

	xdg_vfs_sess_close(session);
	sleep (1);
}
