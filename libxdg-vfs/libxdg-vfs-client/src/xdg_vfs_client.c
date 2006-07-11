/*
#   xdg_vfs_client.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "xdg_vfs_client.h"
#include "xdg_vfs_common.h"

/* ==== utils ==== */

static XdgVfsFileType _translate_filetype(char * filetype) {
	if (strcmp (filetype, "UNKNOWN")==0)
		return XDGVFS_FILE_TYPE_UNKNOWN;
	if (strcmp (filetype, "REGULAR")==0)
		return XDGVFS_FILE_TYPE_REGULAR;
	if (strcmp (filetype, "DIRECTORY")==0)
		return XDGVFS_FILE_TYPE_DIRECTORY;
	if (strcmp (filetype, "FIFO")==0)
		return XDGVFS_FILE_TYPE_FIFO;
	if (strcmp (filetype, "SOCKET")==0)
		return XDGVFS_FILE_TYPE_SOCKET;
	if (strcmp (filetype, "CHARDEV")==0)
		return XDGVFS_FILE_TYPE_CHARDEV;
	if (strcmp (filetype, "BLOCKDEV")==0)
		return XDGVFS_FILE_TYPE_BLOCKDEV;
	if (strcmp (filetype, "SYMLINK")==0) 
		return XDGVFS_FILE_TYPE_SYMLINK;
	if (strcmp (filetype, "VFSMOUNTPOINT")==0) 
		return XDGVFS_FILE_TYPE_VFSMOUNTPOINT;		
	return XDGVFS_FILE_TYPE_UNREC;
};


static int parse_pair(char * theLine, char ** keyName, char ** valueStr, long long int * valueLLInt) {
	char * pValue = NULL;
	char * tailptr;


	*keyName = theLine;
	*valueStr = NULL;
	*valueLLInt = -1;
	if (!theLine) return -1; //error
	
	if (*theLine) {
		int c=0;
		while (theLine[c] != '\0') {
			if (theLine[c] == '=') {
				theLine[c] = '\0';
				pValue = theLine +c +1;
			}
				
			c++;
		}
	}
	if (!pValue) return -2;
	*valueStr = pValue;
	errno = 0;
	*valueLLInt = strtoll (pValue, &tailptr, 10);
	if (errno) {
		*valueLLInt =0;	
		return 2;
	}
	return 1;
}

static int _parse_failure(char * failureStr, char ** errstr) {
	char * pValue = failureStr + strlen(XDGVFS_TAG_FAILED);
	char * tailptr;
	errno = 0;
	int errcode = strtol (pValue, &tailptr, 10);
	
	if (errno) {
		*errstr = NULL;
		return -1;
	}
	// fprintf(stderr, "errorcode = %d\n", errcode);
	*errstr=tailptr;
	return errcode;
}

static void _init_job(XdgVfsSession * sess) {

	sess->currentItemType = 0;
	sess->currentItem = NULL;
	sess->lastJobErrorCode = 0;
	sess->lastJobErrorText = NULL;
	
	sess->outBuf_used = 0;
	
}

static XdgVfsResult _finish_simple_job(XdgVfsSession * sess)
{
	XdgVfsResult r;
	XdgVfsItemType type=0;
	XdgVfsItem * item=NULL;

	while ((r=xdg_vfs_sess_readItem(sess, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		// if (XDG_VFS_DEBUG_IPC) fprintf(stderr, "_finish simple job\n");
		switch (type)
		{
			case XDGVFS_ITEMTYPE_NONE:
			{
					break;
			}
			default:
			{
					fprintf(stderr, "simple job - unexpected item - type=%d\n", type);
					break;
			}
		}
		xdg_vfs_item_unref(item);
	}
	return r;
}

/* =============== sessions =============== */

static char * _detect_desktop() {
	char * e;
	e = getenv ("GNOME_DESKTOP_SESSION_ID");
	if (e && strcmp(e, "") != 0) return "gnome";
	e = getenv ("KDE_FULL_SESSION");
	if (e && strcmp(e, "true") == 0) return "kde";
	return NULL;
}


XdgVfsResult xdg_vfs_sess_start(XdgVfsSession ** sessOut, char * preferredDesktop) {
	int r;
	char * desktop;
	char * exePrefix = "xdg_vfs_";
	char * childArgv[] = { "xdg_vfs_client", "shell", NULL };

	if (preferredDesktop && strcmp(preferredDesktop, "this") != 0)
		desktop = preferredDesktop;
	else 
		desktop = _detect_desktop();

	if (!desktop)
		return XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND;

	XdgVfsSession * newSess = malloc(sizeof(XdgVfsSession));

	char executableName[strlen(exePrefix) + strlen(preferredDesktop) + 1];
	executableName[0] = '\0';

	strcat(executableName, exePrefix);
	strcat(executableName, desktop);

	//puts(executableName);

	r = xdg_vfs_forkexec(executableName, childArgv, &newSess->pid, &newSess->fd_out, &newSess->fd_in);
	
	if (r) {
		free(newSess); 
		*sessOut = NULL;
		return XDGVFS_RESULT_IOERR;
	} 
	newSess->inBuf = malloc(XDGVFS_CLIENT_BUFFERSIZE+1);
	newSess->rawBufIn = malloc(XDGVFS_CLIENT_BUFFERSIZE+1);
	newSess->inBuf_size = XDGVFS_CLIENT_BUFFERSIZE;
	newSess->inBuf_used = 0;
	_init_job(newSess);
	newSess->stateIn = XDGVFS_STATE_EXPECTING_PROMPT;
	newSess->recurse_count = 0;

	while (1) {
		r = xdg_vfs_sess_readItem(newSess, NULL, 
			NULL, NULL, NULL);
		/* fprintf(stderr, "readItem r=%d state=%d\n", r, newSess->stateIn); */
		
		if (newSess->stateIn == XDGVFS_STATE_READY) break;
		if (r!=XDGVFS_RESULT_CONTINUES) {
			xdg_vfs_sess_close(newSess);
			return XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND;
		}
	}

	*sessOut = newSess;

	return XDGVFS_RESULT_OK;
}

int xdg_vfs_sess_getIncomingFiledescriptor(XdgVfsSession * sess) 
{
	return sess->fd_in;	
}

int xdg_vfs_sess_getOutgoingFiledescriptor(XdgVfsSession * sess) 
{
	return sess->fd_out;
}

int xdg_vfs_sess_isBusy(XdgVfsSession * sess)
{
	return sess->stateIn != XDGVFS_STATE_READY;
}


XdgVfsResult xdg_vfs_sess_close(XdgVfsSession * sess) {
	if (!sess) { 
		fprintf(stderr, "NULL pointer passed to xdg_vfs_sess_close!\n");
		return XDGVFS_ERRORCODE_GENERIC;
	}
	close(sess->fd_out);
	close(sess->fd_in);
	free(sess->inBuf);
	free(sess);
	return XDGVFS_RESULT_OK;
}

/* ================ file/dir change monitoring ============ */

void xdg_vfs_sess_set_monitor_callback(
		XdgVfsSession * sess,
		XdgVfsMonitorCallback cb,
		void * userdata
		)
{
	sess->monitor_cb = cb;
	sess->monitor_cb_userdata=userdata;
}

/* ================ items ================= */

XdgVfsItem * xdg_vfs_item_ref(XdgVfsItem * item) {
	item->refcount++;
	return item;	
}

void xdg_vfs_item_unref(XdgVfsItem * item) {
	if (item && --item->refcount <= 0) {
		switch (item->type) 
			{
			case XDGVFS_ITEMTYPE_FILEINFO: 
				{
					XdgVfsFileInfo * _item = (XdgVfsFileInfo *) item;	
					if (_item->uri) free(_item->uri);
					if (_item->basename) free(_item->basename);
					if (_item->mimetype) free(_item->mimetype);
					if (_item->user) free(_item->user);
					if (_item->group) free(_item->group);
					if (_item->mountpoint_id) free(_item->mountpoint_id);
					break;
				}
			case XDGVFS_ITEMTYPE_GET_HEAD:
			case XDGVFS_ITEMTYPE_PUT_HEAD:
			case XDGVFS_ITEMTYPE_SETATTRS_HEAD:
			case XDGVFS_ITEMTYPE_MKDIR_HEAD:
			case XDGVFS_ITEMTYPE_RMDIR_HEAD:				
			case XDGVFS_ITEMTYPE_RM_HEAD:
			case XDGVFS_ITEMTYPE_LS_HEAD:
				{
					// fprintf(stderr,"why1\n");
					XdgVfsSimpleHead * _item = (XdgVfsSimpleHead*) item;
					if (_item->uri) free (_item->uri);
					break;
				}
			case XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE:
				{
					XdgVfsOpenFileDlgResponse * _item = (XdgVfsOpenFileDlgResponse*) item;
					if (_item->selected_localpath) free(_item->selected_localpath);
					if (_item->selected_uri) free(_item->selected_uri);
					break;
				}
			case XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE:
				{
					// fprintf(stderr,"why\n");
					XdgVfsSaveFileDlgResponse * _item = (XdgVfsSaveFileDlgResponse*) item;
					if (_item->selected_localpath) free(_item->selected_localpath);
					if (_item->selected_uri) free(_item->selected_uri);
					break;
				}
			case XDGVFS_ITEMTYPE_COPY_HEAD:
				{
					XdgVfsCopyHead * _item = (XdgVfsCopyHead*) item;
					if (_item->uri_src) free(_item->uri_src);
					if (_item->uri_target) free(_item->uri_target);
					break;
				}
			case XDGVFS_ITEMTYPE_MOVE_HEAD:
				{
					XdgVfsMoveHead * _item = (XdgVfsMoveHead*) item;
					if (_item->uri_src) free(_item->uri_src);
					if (_item->uri_target) free(_item->uri_target);
					break;
				}
			case XDGVFS_ITEMTYPE_BACKENDINFO:
				{
					XdgVfsBackendInfo * _item = (XdgVfsBackendInfo*) item;
					if (_item->backend_id) free(_item->backend_id);
					if (_item->system_uri) free(_item->system_uri);
					if (_item->file_icon_theme) free(_item->file_icon_theme);
					break;
				}			

		}
		free(item);	
	
	}	
	
}


/* ================ command utils =============== */

static XdgVfsResult _write_command(XdgVfsSession * sess, char * command, const char * argv[]) {
	if (!sess) 
	{
		fprintf(stderr, "NULL pointer passed to _write_command!\n");
		return XDGVFS_ERRORCODE_GENERIC;
	}
	if (sess->stateIn != XDGVFS_STATE_READY) return XDGVFS_RESULT_BAD_STATE;
	FILE* out = fdopen(dup(sess->fd_out), "w");
	fputs(command, out);
	int c=0;
	while(argv[c] != NULL) {
		const char * arg = argv[c];
		fputc((int)' ', out);
		fputc((int)'"', out);
		fputs(arg, out);  /* todo: escape double quotes! */
		fputc((int)'"', out);
		c++;
	}
	fputc(10, out);
	fflush(out);
	fclose(out);
	//printf("written command!\n");
	sess->stateIn = XDGVFS_STATE_READING_TEXT;
	_init_job(sess);
	return XDGVFS_RESULT_OK;
}

XdgVfsResult xdg_vfs_sess_cancelCommand(XdgVfsSession * sess)
{
	/* not implemented yet */
	fprintf(stderr, "xdg_vfs_sess_cmd_cancel() not implemented yet\n");
	return XDGVFS_RESULT_OK;
}

/* ========================= Commands ========================== */

XdgVfsResult xdg_vfs_sess_cmd_backendInfo(XdgVfsSession * sess) 
{
	const char * argv[] = { NULL };
	return _write_command(sess, "backend", argv);
}


XdgVfsResult xdg_vfs_sess_cmd_getFile(XdgVfsSession * sess, const char * filename) 
{
	const char * argv[] = { filename, "-e", NULL };
	return _write_command(sess, "get", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_putFile(XdgVfsSession * sess, const char * filename, 
	XdgVfsFlags flags) 
{
	const char * argv[] = { filename, "-e", NULL, NULL };
	if (flags & XDGVFS_FLAGS_OVERWRITE) 
	{
		argv[2] = "--overwrite";			
	}
	return _write_command(sess, "put", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_getFileInfo(XdgVfsSession * sess, const char * filename) 
{
	const char * argv[] = { filename, NULL };
	return _write_command(sess, "info", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_setAttrs(XdgVfsSession * sess, 
	const char * filename, int permissions, int setPermFlag, char * user, char * group) 
{
	XdgVfsResult r;
	int an = 1;
	const char * argv[] = { filename, NULL, NULL, NULL, NULL };
	
	if ((!user) && (!group) && (!setPermFlag)) return XDGVFS_RESULT_PARAMETER_ERR;
	
	if (user) 
	{ 
		char * str = calloc(256,1);
		strncat(str, "--user=", 255);
		strncat(str, user, 255);
		argv[an++] = str;
	}
	if (group) 
	{ 
		char * str = calloc(256,1);
		strncat(str, "--group=", 255);
		strncat(str, user, 255);
		argv[an++] = str;
	}
	if (setPermFlag)
	{ 
		char * str = calloc(256,1);
		snprintf (str, 255, "--permissions=%d\n", permissions);
		argv[an++] = str;
	}
	r = _write_command(sess, "info", argv);
	
	if (argv[1]) free((void*)argv[1]);
	if (argv[2]) free((void*)argv[2]);	
	if (argv[3]) free((void*)argv[3]);	
	return r;
}

XdgVfsResult xdg_vfs_sess_cmd_copyFile(XdgVfsSession * sess, const char * filename_src, const char * filename_target)
{
	const char * argv[] = { filename_src, filename_target, "--progress", NULL };
	return _write_command(sess, "cp", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_moveFile(XdgVfsSession * sess, const char * filename_src, const char * filename_target)
{
	const char * argv[] = { filename_src, filename_target, "--progress", NULL };
	return _write_command(sess, "mv", argv);
}


XdgVfsResult xdg_vfs_sess_cmd_listDirectory(XdgVfsSession * sess, const char * filename) 
{
	const char * argv[] = { filename, "--dive-de-links", NULL };
	return _write_command(sess, "ls", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_makeDirectory(XdgVfsSession * sess, const char * filename) 
{
	const char * argv[] = { filename, NULL };
	return _write_command(sess, "mkdir", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_removeDirectory(XdgVfsSession * sess, const char * filename) 
{
	const char * argv[] = { filename, NULL };
	return _write_command(sess, "rmdir", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_removeFile(XdgVfsSession * sess, const char * filename)
{
	const char * argv[] = { filename, NULL };
	return _write_command(sess, "rm", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_mount(XdgVfsSession * sess, const char * mountpoint_id)
{
	const char * argv[] = { mountpoint_id, NULL };
	return _write_command(sess, "mount", argv);
}

XdgVfsResult xdg_vfs_sess_cmd_monitorDir(XdgVfsSession * sess, const char * uri)
{
	const char * argv[] = { uri, NULL };
	XdgVfsResult r = _write_command(sess, "mondir", argv);
	if (r) return r;
	return _finish_simple_job(sess);
}

XdgVfsResult xdg_vfs_sess_cmd_monitorFile(XdgVfsSession * sess, const char * uri)
{
	const char * argv[] = { uri, NULL };
	XdgVfsResult r = _write_command(sess, "monfile", argv);
	if (r) return r;
	return _finish_simple_job(sess);
	
}

XdgVfsResult xdg_vfs_sess_cmd_removeMonitor(XdgVfsSession * sess, const char * uri)
{
	const char * argv[] = { uri, NULL };
	XdgVfsResult r = _write_command(sess, "rmmon", argv);
	if (r) return r;
	return _finish_simple_job(sess);
}


XdgVfsResult xdg_vfs_sess_cmd_openFileDialog(XdgVfsSession * sess, const char * default_uri, XdgVfsFlags flags) 
{
	const char * argv[] = { default_uri, NULL, NULL, NULL };

	if (flags & XDGVFS_FLAGS_INSTANT_GET) {
		argv[1] = "--instant-get";
	} 
	if (flags & XDGVFS_FLAGS_QRY_FILEINFO) {
		argv[2] = "-i";
	}
	return _write_command(sess, "openfiledlg", argv);

}

XdgVfsResult xdg_vfs_sess_cmd_saveFileDialog(XdgVfsSession * sess, 
	const char * default_folder_uri, 
	const char * default_filename, 
	XdgVfsFlags flags)
{
	const char * argv[] = { default_folder_uri, default_filename, NULL, NULL };
	

	if (flags & XDGVFS_FLAGS_INSTANT_PUT) {
		if (!default_filename)
			argv[1] = "--instant-put";
		else
			argv[2] = "--instant-put";
	} 
	return _write_command(sess, "savefiledlg", argv);
}

/* ========================= Reading and writing ========================== */

static XdgVfsResult _read_chunk(XdgVfsSession * sess)
{
	int i, n;
	char * buf = sess->inBuf;
	int used = sess->inBuf_used;
	int size = sess->inBuf_size;

	n = read (sess->fd_in, buf+used, size-used,0);
	if (n==0) 
	{ 
		return XDGVFS_RESULT_IOERR;  // EOF -> child process has died
	}
	if (n<0) 
	{
		printf ("reading head returns: %d\n", n); 
		perror ("error reading");
		return XDGVFS_RESULT_IOERR;  // EOF -> child process has died
	}
	if (XDG_VFS_DEBUG_IPC)
	{
		fwrite (buf+used, 1, n, stderr);
		fflush(stderr);
	}
	
	sess->inBuf_used = used+n;
	return XDGVFS_RESULT_OK;
}

static char * _get_str_blocking(XdgVfsSession * sess, int size) {
	char * buf = sess->inBuf;	
	int used = sess->inBuf_used;
	char * str;
	while (sess->inBuf_used < size)
	{
		XdgVfsResult r = _read_chunk(sess);
		if (r) 
		{
			fprintf(stdout, "_get_str_blocking problem: %d\n", r);
			return NULL;
		}
	}
	str = malloc(size+1);
	memcpy(str, buf, size);
	str[size]='\0';
	memcpy(buf, buf+size, used-size);
	sess->inBuf_used = used-size;
	return str;
}

static char * _get_next_line(XdgVfsSession * sess) {
	char * theLine = NULL;
	char * buf = sess->inBuf;
	int i;
	int used = sess->inBuf_used;
	//buf[used+1] = '\0';
	//fprintf(stderr, "_get_next_line used=%d buf=%s\n", used, buf);
	for (i = 0; i < used; i++) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
			theLine=strdup(buf);
			memcpy(buf, buf+i+1, used-i); /* shift data left */
			sess->inBuf_used = used-i-1;
			break;
		}
	}
	return theLine;
}



static XdgVfsResult _xdg_vfs_sess_parseLine(XdgVfsSession * sess, XdgVfsItemType * type, XdgVfsItem ** item) 
{
	char * theLine;

	*type = XDGVFS_ITEMTYPE_NONE;
	*item = NULL;


	theLine = _get_next_line(sess);	
	if (!theLine) 
	{ 
		XdgVfsResult r = _read_chunk(sess);
		if (r) return r;
		theLine = _get_next_line(sess);		
	}
	

	if (XDG_VFS_DEBUG_IPC) fprintf(stdout, "t=%d line: >>%s<<\n", (sess->currentItemType), theLine);

	if (theLine && theLine[0] != '\0') 
	{
		if (theLine[0] == '[' || theLine[0] == '(' || theLine[0] == ':') 
		{
			/* We got a new Object - thus we have to return the old one */			
			XdgVfsItem * newItem = NULL;		

			if (strcmp(theLine, XDGVFS_TAG_BACKEND)==0) 
			{
				XdgVfsBackendInfo * _newItem = calloc(1, sizeof(XdgVfsBackendInfo));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_BACKENDINFO;
			} 
			else if (strcmp(theLine, XDGVFS_TAG_GET)==0) 
			{
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_GET_HEAD;
			} 
			else if (strcmp(theLine, XDGVFS_TAG_PUT)==0) 
			{
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_PUT_HEAD;
			}			
			else if (strcmp(theLine, XDGVFS_TAG_ESCAPEDDATA_OUT)==0) 
			{
				XdgVfsDataInDoneItem * _newItem = calloc(1, sizeof(XdgVfsDataInDoneItem));
				_newItem->bytecount = 0;
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_DATAIN_DONE;
				sess->stateIn = XDGVFS_STATE_READING_DATA;
			} 
			else if (strcmp(theLine,XDGVFS_TAG_PUT)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_GET_HEAD;
			}
			else if (strcmp(theLine, XDGVFS_TAG_ESCAPEDDATA_IN)==0) 
			{
				XdgVfsDataOutDoneItem * _newItem = calloc(1, sizeof(XdgVfsDataOutDoneItem));
				newItem = (XdgVfsItem*) _newItem;
				_newItem->bytecount = 0;
				newItem->type = XDGVFS_ITEMTYPE_DATAOUT_DONE;
				sess->stateIn = XDGVFS_STATE_WRITING_DATA;
			} 			
			else if (strcmp(theLine,XDGVFS_TAG_CP)==0) 
			{ 
				XdgVfsCopyHead * _newItem = calloc(1, sizeof(XdgVfsCopyHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_COPY_HEAD;
			}			
			else if (strcmp(theLine,XDGVFS_TAG_MV)==0) 
			{ 
				XdgVfsMoveHead * _newItem = calloc(1, sizeof(XdgVfsMoveHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_MOVE_HEAD;
			}			
			else if (strcmp(theLine,XDGVFS_TAG_PROGRESS)==0) 
			{ 
				XdgVfsProgress * _newItem = calloc(1, sizeof(XdgVfsProgress));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_PROGRESS;
			}			
			else if (strcmp(theLine,XDGVFS_TAG_FILEINFO)==0 
				|| strcmp(theLine,XDGVFS_TAG_DIRENTRY)==0) 
			{ 
				XdgVfsFileInfo * _newItem = calloc(1, sizeof(XdgVfsFileInfo));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_FILEINFO;
			}
			else if (strcmp(theLine,XDGVFS_TAG_SETATTRS)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_SETATTRS_HEAD;
			}			
			else if (strcmp(theLine,XDGVFS_TAG_LS)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_LS_HEAD;
			}
			else if (strcmp(theLine,XDGVFS_TAG_MKDIR)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_MKDIR_HEAD;
			}
			else if (strcmp(theLine,XDGVFS_TAG_RMDIR)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_RMDIR_HEAD;
			}
			else if (strcmp(theLine,XDGVFS_TAG_RM)==0) 
			{ 
				XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_RMDIR_HEAD;
			}
			else if (strcmp(theLine,XDGVFS_TAG_MOUNT)==0) 
			{ 
				//XdgVfsSimpleHead * _newItem = calloc(1, sizeof(XdgVfsSimpleHead));
				newItem = NULL;
				//newItem->type = XDGVFS_ITEMTYPE_NONE;
			}
			else if (strcmp(theLine,XDGVFS_TAG_OPENFILEDLG)==0) 
			{ 
				XdgVfsOpenFileDlgResponse * _newItem = calloc(1, sizeof(XdgVfsOpenFileDlgResponse));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE;
			}
			else if (strcmp(theLine,XDGVFS_TAG_SAVEFILEDLG)==0) 
			{ 
				XdgVfsSaveFileDlgResponse * _newItem = calloc(1, sizeof(XdgVfsSaveFileDlgResponse));
				newItem = (XdgVfsItem*) _newItem;
				newItem->type = XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE;
			}
			/* finished? */
			else if (strcmp(theLine,XDGVFS_TAG_DONE)==0) 
			{ 
				//fprintf(stderr,"done\n");
				sess->stateIn = XDGVFS_STATE_EXPECTING_PROMPT;
				sess->lastJobErrorCode = XDGVFS_ERRORCODE_OK;
			}
			else if (strncmp(theLine,XDGVFS_TAG_FAILED, strlen(XDGVFS_TAG_FAILED)) ==0) 
			{
				char * errstr;
				sess->lastJobErrorCode = _parse_failure(theLine, &errstr);
				sess->stateIn = XDGVFS_STATE_EXPECTING_PROMPT;
				//fprintf(stderr,"failed code=%d\n", sess->lastJobErrorCode);
				sess->currentItemType = XDGVFS_ITEMTYPE_NONE;
				sess->currentItem = NULL;  // fixme unref!
			} else {
				fprintf(stderr,"unexpected tag: %s\n", theLine);
				sess->stateIn = XDGVFS_STATE_ERROR_LEAVING;
				return XDGVFS_RESULT_IOERR;
			}
			 
			*type = sess->currentItemType;
			*item = sess->currentItem;
			if (newItem) {
				newItem->refcount = 1;
				sess->currentItemType = newItem->type;
				sess->currentItem = newItem;
			} else {
				sess->currentItemType = XDGVFS_ITEMTYPE_NONE;
				sess->currentItem = NULL;
			}

		} 
		else if(theLine[0] == XDGVFS_TAG_MONITOREVENT [0])
		{
			int l = strlen(theLine);
			if (l>2 && sess->monitor_cb)
			{ 
				char * changed_uri = strdup(theLine+2);
				sess->monitor_cb(sess, changed_uri, sess->monitor_cb_userdata);
				free(changed_uri);
			}
			else
				fprintf(stderr, "monitorevent: line too short\n");
		}
		else 
		{
			XdgVfsItem * rItem = sess->currentItem;
			char * keyname;
			char * valueStr;
			long long int valueLLInt;
			parse_pair(theLine, &keyname, &valueStr, &valueLLInt);
			switch(sess->currentItemType) 
			{
				case(XDGVFS_ITEMTYPE_GET_HEAD):
				case(XDGVFS_ITEMTYPE_PUT_HEAD):
				case(XDGVFS_ITEMTYPE_MKDIR_HEAD):
				case(XDGVFS_ITEMTYPE_RMDIR_HEAD):				
				case(XDGVFS_ITEMTYPE_RM_HEAD):
				case(XDGVFS_ITEMTYPE_LS_HEAD):
				case(XDGVFS_ITEMTYPE_SETATTRS_HEAD):
				{
					XdgVfsSimpleHead * _rItem = (XdgVfsSimpleHead*)rItem;
					if (strcmp(keyname, "uri")==0) 
						_rItem->uri = strdup(valueStr);
					break;
				}
				case(XDGVFS_ITEMTYPE_BACKENDINFO):
				{
		
					XdgVfsBackendInfo * _rItem = (XdgVfsBackendInfo*)rItem;
					if (strcmp(keyname, "backend_id")==0) 
						_rItem->backend_id = strdup(valueStr);
					if (strcmp(keyname, "system_uri")==0) 
						_rItem->system_uri = strdup(valueStr);
					if (strcmp(keyname, "file_icon_theme")==0) 
						_rItem->file_icon_theme = strdup(valueStr);
					break;
				}
				case(XDGVFS_ITEMTYPE_FILEINFO):
				{
		
					XdgVfsFileInfo * _rItem = (XdgVfsFileInfo*)rItem;
					if (strcmp(keyname, "basename")==0) 
						_rItem->basename = strdup(valueStr);
					if (strcmp(keyname, "mimetype")==0) 
						_rItem->mimetype = strdup(valueStr);
					if (strcmp(keyname, "iconname")==0) 
						_rItem->iconname = strdup(valueStr);
					if (strcmp(keyname, "uri")==0)
						_rItem->uri = strdup(valueStr);
					if (strcmp(keyname, "user")==0)
						_rItem->user = strdup(valueStr);
					if (strcmp(keyname, "group")==0)
						_rItem->group = strdup(valueStr);
					if (strcmp(keyname, "linkdest")==0)
						_rItem->linkdest = strdup(valueStr);
					if (strcmp(keyname, "filetype")==0)
						_rItem->filetype = _translate_filetype(valueStr);
					if (strcmp(keyname, "mountpoint_id")==0)
						_rItem->mountpoint_id = strdup(valueStr);

					if (strcmp(keyname, "permissions")==0)
						_rItem->permissions = valueLLInt;
					if (strcmp(keyname, "size")==0)
						_rItem->size = valueLLInt;
					if (strcmp(keyname, "atime")==0)
						_rItem->atime = valueLLInt;
					if (strcmp(keyname, "mtime")==0)
						_rItem->mtime = valueLLInt;
					if (strcmp(keyname, "ctime")==0)
						_rItem->ctime = valueLLInt;
					if (strcmp(keyname, "is_mounted")==0)
						_rItem->is_mounted = valueLLInt;
					break;
				}
				case(XDGVFS_ITEMTYPE_PROGRESS):
				{
					XdgVfsProgress * _rItem = (XdgVfsProgress*)rItem;
					if (strcmp(keyname, "total_bytes")==0)
						_rItem->size = valueLLInt;
					if (strcmp(keyname, "bytes_copied")==0)
						_rItem->copied = valueLLInt;
					break;
				}
				case(XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE):
				{
					XdgVfsOpenFileDlgResponse * _rItem = (XdgVfsOpenFileDlgResponse*) rItem;
					if (strcmp(keyname, "selected_localpath")==0)
						_rItem->selected_localpath = strdup(valueStr);;
					if (strcmp(keyname, "selected_uri")==0)
						_rItem->selected_uri = strdup(valueStr);;
					break;
				}
				case(XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE):
				{
					XdgVfsSaveFileDlgResponse * _rItem = (XdgVfsSaveFileDlgResponse*) rItem;
					if (strcmp(keyname, "selected_localpath")==0)
						_rItem->selected_localpath = strdup(valueStr);;
					if (strcmp(keyname, "selected_uri")==0)
						_rItem->selected_uri = strdup(valueStr);;
					break;
				}
				case(XDGVFS_ITEMTYPE_COPY_HEAD):
				{
					XdgVfsCopyHead * _rItem = (XdgVfsCopyHead*) rItem;
					if (strcmp(keyname, "uri_src")==0)
						_rItem->uri_src = strdup(valueStr);;
					if (strcmp(keyname, "uri_target")==0)
						_rItem->uri_target = strdup(valueStr);;
					break;
				}				
				case(XDGVFS_ITEMTYPE_MOVE_HEAD):
				{
					XdgVfsMoveHead * _rItem = (XdgVfsMoveHead*) rItem;
					if (strcmp(keyname, "uri_src")==0)
						_rItem->uri_src = strdup(valueStr);;
					if (strcmp(keyname, "uri_target")==0)
						_rItem->uri_target = strdup(valueStr);;
					break;
				}
				case(XDGVFS_ITEMTYPE_DATAIN_DONE):
					break;
			}
			//if (valueStr) free(valueStr);
		}
	}
	return XDGVFS_RESULT_CONTINUES;
}

static void _debug_print_out_buffer(XdgVfsSession * sess) {
	int i;
	//sess->inBuf[sess->inBuf_used+1] = '\0';
	//
	fprintf(stderr, "buf: used=%d '", sess->inBuf_used);
	for (i=0;i<sess->inBuf_used;i++)
	{
		int c = sess->inBuf[i];
		if (c < 32 || c > 126)
			fputc('.', stderr);
		else 
			fputc(c, stderr);
	}
	fprintf(stderr, "'\n");
}

static XdgVfsResult _report_size(XdgVfsSession * sess, int len) 
{ 
	if (sess->currentItemType == XDGVFS_ITEMTYPE_DATAIN_DONE) 
	{
		XdgVfsDataInDoneItem * item = (XdgVfsDataInDoneItem *) sess->currentItem;
		item->bytecount+=len;
		return XDGVFS_RESULT_OK;
	
	} 
	else 
	{
		fprintf(stderr,"fatal: data-in-item not found!!!\n");
		sess->stateIn = XDGVFS_STATE_ERROR_LEAVING;
		return XDGVFS_RESULT_FATAL_ERR;
	}
}

static XdgVfsResult _read_data(XdgVfsSession * sess, XdgVfsItemType * typeOut, char ** buffer, int * len) 
{
	*buffer = NULL;
	*len = 0;
	if (sess->inBuf_used<2) 
	{ 
		//fprintf(stderr, "readchunk\n");
		XdgVfsResult r = _read_chunk(sess);
		if (r) return r;
	}

	char * buf = sess->inBuf;
	int used = sess->inBuf_used;
	//fprintf(stderr, "\nlen = %d\n", used);
	int i=0;
	char * outptr = sess->rawBufIn;
	while (i<used) 
	{
		char c = buf[i];
		if (c == '~') 
		{
			i++;
			if (i<used) 
			{
				if (buf[i] == '~') 
				{
					i++;
					*outptr++ = '~';
					continue;
				}
				else 
				if (buf[i] == '\n')
				{
					memcpy(buf, buf+i+1, used-i);
					*buffer = sess->rawBufIn;
					*len = outptr - sess->rawBufIn;
					sess->inBuf_used = used-i-1;

					_report_size(sess, *len);
					/* fprintf(stderr, "read all data - end found len=%d\n", *len); */
					/* _debug_print_out_buffer(sess); */

					sess->stateIn = XDGVFS_STATE_READING_TEXT;  
					/* finished - switch back to text-mode */
					*typeOut = XDGVFS_DATAIN;
					return XDGVFS_RESULT_CONTINUES;
				} 
				else 
				{
					*buffer = NULL;
					*len = 0;
					fprintf(stderr, "unescaping error at pos=%d buffer len=%d:\n", i, used);
					_debug_print_out_buffer(sess);
					fprintf(stderr, "unescaped buffer:\n");
					fwrite (sess->rawBufIn, 1, outptr - sess->rawBufIn, stdout);
					sess->inBuf_used = 0;
					sess->stateIn = XDGVFS_STATE_ERROR_LEAVING;
					
					return XDGVFS_RESULT_UNESCAPING_DATA_ERR;
				}
			}
			else
			{
				buf[0]='~';	/* cannot process this - leave escape char in buffer */
				sess->inBuf_used = 1;
				*buffer = sess->rawBufIn;
				*len = outptr - sess->rawBufIn;
				_report_size(sess, *len);
				*typeOut = XDGVFS_DATAIN;
				return XDGVFS_RESULT_CONTINUES;
			}
		}
		else
		{
			i++;
			*outptr++ = c;
		}
	}

	*buffer = sess->rawBufIn;
	*len = outptr - sess->rawBufIn;
	_report_size(sess, *len);
	sess->inBuf_used = 0;
	*typeOut = XDGVFS_DATAIN;
	return XDGVFS_RESULT_CONTINUES;
}


static XdgVfsResult _xdg_vfs_sess_readItem(XdgVfsSession * sess, XdgVfsItemType * typeOut, 
		XdgVfsItem ** itemOut, char ** buffer, int * len) 
{

	if (typeOut) *typeOut = 0;
	if (itemOut) *itemOut = NULL;
	if (buffer) *buffer = NULL;
	if (len) * len=0;

	if (XDG_VFS_DEBUG_IPC) fprintf(stderr, "state=%d\n", sess->stateIn);
	// _debug_print_out_buffer(sess);


	switch(sess->stateIn) 
	{
		case(XDGVFS_STATE_EXPECTING_PROMPT):

			if (sess->inBuf_used >= 2) 
			{
				char * prompt = _get_str_blocking(sess, 2);
				if (prompt && strcmp(prompt, "> ")==0) 
				{
					sess->stateIn = XDGVFS_STATE_READY;
					free(prompt);
					return sess->lastJobErrorCode;
				}
				else 
				{
					if(prompt) free (prompt);
					return XDGVFS_ERRORCODE_CORRUPTED_DATA;
				}
			} 
/*			else if (sess->inBuf_used > 2) 
			{
				return XDGVFS_ERRORCODE_CORRUPTED_DATA;
			} */
			else 
			{
				XdgVfsResult r = _read_chunk(sess);
				if (r) return r;
				return XDGVFS_RESULT_CONTINUES;
			}
			break;	
		case(XDGVFS_STATE_READY):
		{
			XdgVfsResult r = _xdg_vfs_sess_parseLine(sess, typeOut, itemOut);
			if (typeOut && *typeOut != XDGVFS_ITEMTYPE_NONE)
			{
				fprintf(stderr, 
					"WARNING: calling this function in 'ready-state' shouldn't "
					"return anything except triggering file monitoring events!"
					" itemType=%d\n", typeOut);
				
				if (typeOut) *typeOut=XDGVFS_ITEMTYPE_NONE;
				if (itemOut)
				{
					xdg_vfs_item_unref(*itemOut);	
					*itemOut=NULL;
				}
				return XDGVFS_RESULT_BAD_STATE;
			} 
			if (r!=XDGVFS_RESULT_CONTINUES)
			{
				fprintf(stderr, 
					"WARNING: calling this function in 'ready-state' shouldn't"
					" return errors!\n");
				return r;
			}
			return XDGVFS_RESULT_OK;
		}
		case(XDGVFS_STATE_READING_TEXT):
			return _xdg_vfs_sess_parseLine(sess, typeOut, itemOut);
		case(XDGVFS_STATE_READING_DATA):
			return _read_data(sess, typeOut, buffer, len);
		case(XDGVFS_STATE_WRITING_DATA):
			*typeOut = XDGVFS_DATAOUT;
			return XDGVFS_RESULT_CONTINUES;			
		case(XDGVFS_STATE_ERROR_LEAVING):
			return XDGVFS_RESULT_IOERR;

		default:
			return XDGVFS_RESULT_BAD_STATE;
	}
	return XDGVFS_ERRORCODE_OK;
}

XdgVfsResult xdg_vfs_sess_readItem(XdgVfsSession * sess, XdgVfsItemType * typeOut, 
		XdgVfsItem ** itemOut, char ** buffer, int * len) 
{
		XdgVfsResult r;
		if (sess->recurse_count > 0) {
			fprintf(stderr, "Warning: cannot recurse readItem function!\n");			
			return XDGVFS_RESULT_BAD_STATE;
		}
		sess->recurse_count = sess->recurse_count+1;
		r= _xdg_vfs_sess_readItem(sess, typeOut, itemOut, buffer, len);
		sess->recurse_count = sess->recurse_count-1;
		return r;
}

static int _chk_incoming(XdgVfsSession * sess) 
{
       	/* check for canceled */
       	int filedes = sess->fd_in;
       	int r;
       	fd_set set;
       	struct timeval timeout;

       	/* Initialize the file descriptor set. */
       	FD_ZERO (&set);
       	FD_SET (filedes, &set);
     
       	/* Initialize the timeout data structure. */
       	timeout.tv_sec = 0;
       	timeout.tv_usec = 0;
     
       	/* select returns 0 if timeout, 1 if input available, -1 if error. */
       	r= select (FD_SETSIZE, &set, NULL, NULL, &timeout);
       	if (r==0) return 0;
       	if (r==-1) 
		{
			fprintf(stderr, "WARNING: Error when checking stdin for incoming data\n");
       		return 0;
		}
       	else 
		{
			return 1;
       	}
}


XdgVfsResult xdg_vfs_sess_sendData(XdgVfsSession * sess) {
	
	if (_chk_incoming(sess))
	{
			sess->stateIn = XDGVFS_STATE_READING_TEXT;	
			return XDGVFS_RESULT_IOERR;
	}
	
	
	if (sess->outBuf_used - sess->outBuf_sent == 0) return XDGVFS_RESULT_GIVE_ME_DATA;
	
	ssize_t len = write (sess->fd_out, 
			sess->outBuf + sess->outBuf_sent, 
			sess->outBuf_used - sess->outBuf_sent);
	// fprintf(stderr, "sent %d bytes\n", len);
	//sleep(1);
	if (len == -1) {
		if (errno == EAGAIN) return XDGVFS_RESULT_CALL_SENDDATA_AGAIN;
		perror ("sending data to xdg-vfs process - IOERR");
		return XDGVFS_RESULT_IOERR;
	}
	sess->outBuf_sent += len;


	if (sess->outBuf_used - sess->outBuf_sent == 0) {
		if (sess->outBuf_EOF) { 
			sess->stateIn = XDGVFS_STATE_READING_TEXT;	
			return XDGVFS_RESULT_OK;
		}
		return XDGVFS_RESULT_GIVE_ME_DATA;
	}
	
	return XDGVFS_RESULT_CALL_SENDDATA_AGAIN;
		
}

XdgVfsResult xdg_vfs_sess_putDataChunk(XdgVfsSession * sess, char * data, int len) {

	sess->outBuf_sent = 0;
	if (len == 0 ) {  /* EOF */
		if (data) {
			fprintf(stderr, "WARNING: set data to NULL on EOF!!\n");
		}

		sess->outBuf[0] = '~';
		sess->outBuf[1] = '\n';
		sess->outBuf_used = 2;
		sess->outBuf_EOF = 1;
		return XDGVFS_RESULT_CALL_SENDDATA_AGAIN;
	}

	if (sess->currentItemType == XDGVFS_ITEMTYPE_DATAOUT_DONE) {
		XdgVfsDataOutDoneItem * dii = (XdgVfsDataOutDoneItem *) sess->currentItem;
		dii->bytecount += len;
	} 
	else 
	{
		fprintf(stderr, "Where is my object?\n");
		return XDGVFS_RESULT_BAD_STATE;
	}

	if (!sess->outBuf) sess->outBuf = malloc(len*2); /* can be max twice the data size */
	if (sess->outBuf_size < len * 2) 
	{
		sess->outBuf = realloc (sess->outBuf, len * 2);
		sess->outBuf_size = len * 2;
	}
	int i = 0;
	char * wptr = sess->outBuf;
	for (i = 0; i<len; i++) 
	{
		char c = data[i];
		if (c == '~') /* escape it */
		{
			*wptr++ = '~';
			*wptr++ = '~';
		} 
		else
			*wptr++ = c;
	}
	sess->outBuf_used = wptr - sess->outBuf;

	return XDGVFS_RESULT_CALL_SENDDATA_AGAIN;
	
}




