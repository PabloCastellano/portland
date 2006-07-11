/*
#   xdg_vfs_client.h
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


#ifndef XDGVFS_CLIENT_H_
#define XDGVFS_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xdg_vfs_common.h>

#define XDG_VFS_DEBUG_IPC 0

#define XDGVFS_CLIENT_BUFFERSIZE 8192

typedef size_t XdgVfsFileSize;


/* ====== incoming Items ======== */


/**
*  itemtypes
*/
typedef enum {
	XDGVFS_ITEMTYPE_FILEINFO = 1,
	XDGVFS_ITEMTYPE_SETATTRS_HEAD = 2,
	XDGVFS_ITEMTYPE_DIR_HEAD = 3,
	XDGVFS_ITEMTYPE_DIR_ENTRY = 4,
	XDGVFS_ITEMTYPE_PROGRESS = 5,
	XDGVFS_ITEMTYPE_GET_HEAD = 6,
	XDGVFS_ITEMTYPE_PUT_HEAD = 7,
	XDGVFS_ITEMTYPE_COPY_HEAD = 8,
	XDGVFS_ITEMTYPE_MOVE_HEAD = 9,
	XDGVFS_ITEMTYPE_MKDIR_HEAD = 10,
	XDGVFS_ITEMTYPE_RMDIR_HEAD = 11,
	XDGVFS_ITEMTYPE_RM_HEAD = 12,
	XDGVFS_ITEMTYPE_LS_HEAD = 13,
	XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE = 14,
	XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE = 15,
	XDGVFS_ITEMTYPE_DATAIN_DONE = 16,
	XDGVFS_ITEMTYPE_DATAOUT_DONE = 17,
	XDGVFS_DATAIN = 18,
	XDGVFS_DATAOUT = 19,
	XDGVFS_ITEMTYPE_BACKENDINFO = 20,

	XDGVFS_ITEMTYPE_NONE = 0
} XdgVfsItemType;


typedef struct {
	XdgVfsItemType type;
	int refcount;
} XdgVfsItem;

typedef struct {
	XdgVfsItem itm;
	char * backend_id;
	char * system_uri;
	char * file_icon_theme;
} XdgVfsBackendInfo;


typedef struct {
	XdgVfsItem itm;
	XdgVfsFileSize bytecount;
} XdgVfsDataInDoneItem;

typedef struct {
	XdgVfsItem itm;
	XdgVfsFileSize bytecount;
} XdgVfsDataOutDoneItem;

typedef struct {
	XdgVfsItem itm;
	char * uri;
} XdgVfsSimpleHead;

/**
*  fileinfo filetypes
*/
typedef enum {
	XDGVFS_FILE_TYPE_UNKNOWN = 1,
	XDGVFS_FILE_TYPE_REGULAR = 2,
	XDGVFS_FILE_TYPE_DIRECTORY = 3,
	XDGVFS_FILE_TYPE_FIFO = 4,
	XDGVFS_FILE_TYPE_SOCKET = 5,
	XDGVFS_FILE_TYPE_CHARDEV = 6,
	XDGVFS_FILE_TYPE_BLOCKDEV = 7,
	XDGVFS_FILE_TYPE_SYMLINK = 8,
	XDGVFS_FILE_TYPE_VFSMOUNTPOINT = 9,
	XDGVFS_FILE_TYPE_UNREC = 0
} XdgVfsFileType;

/**
*  fileinfo item
*/
typedef struct {
	XdgVfsItem itm;
	char * uri;
	char * basename;
	char * mimetype;
	size_t size;	
	int permissions;
	char * user;
	char * group;
	int atime;
	int ctime;
	int mtime;
	char * linkdest;
	XdgVfsFileType filetype;
	char * iconname;
	char * mountpoint_id;
	int is_mounted;
} XdgVfsFileInfo;

typedef struct {
	XdgVfsItem itm;
	char * selected_localpath;
	char * selected_uri;
} XdgVfsOpenFileDlgResponse;

typedef struct {
	XdgVfsItem itm;
	char * selected_localpath;
	char * selected_uri;
} XdgVfsSaveFileDlgResponse;

typedef struct {
	XdgVfsItem itm;
	char * uri_src;
	char * uri_target;
} XdgVfsCopyHead;

typedef struct {
	XdgVfsItem itm;
	char * uri_src;
	char * uri_target;
} XdgVfsMoveHead;

typedef struct {
	XdgVfsItem itm;
	int percent;
	XdgVfsFileSize size;
	XdgVfsFileSize copied;
} XdgVfsProgress;


/* ====== flags and return values ======== */

/*
*  Command flags 
*/
typedef enum {
	XDGVFS_FLAGS_NONE = 0,
	XDGVFS_FLAGS_OVERWRITE = 2,
	XDGVFS_FLAGS_INSTANT_GET = 4,
	XDGVFS_FLAGS_INSTANT_PUT = 8,
	XDGVFS_FLAGS_QRY_FILEINFO = 16
} XdgVfsFlags;

/**
*  xdg-vfs return values: 
*  values less than 0 are client errors.
*  values greater than 0 are XDGVFS_ERRORCODE_* errors!
*/
#define XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND -10
#define XDGVFS_RESULT_CALL_SENDDATA_AGAIN -9
#define XDGVFS_RESULT_GIVE_ME_DATA -8
#define XDGVFS_RESULT_PARAMETER_ERR -7
#define XDGVFS_RESULT_FATAL_ERR -6
#define XDGVFS_RESULT_UNESCAPING_DATA_ERR -5
#define XDGVFS_RESULT_BAD_STATE -4
#define XDGVFS_RESULT_IOERR -3
#define XDGVFS_RESULT_NOT_POSSIBLE -2
#define XDGVFS_RESULT_CONTINUES -1
#define XDGVFS_RESULT_DONE 0
#define XDGVFS_RESULT_OK 0

#define XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND_STR "Client: Cannot launch backend"
#define XDGVFS_RESULT_CALL_SENDDATA_AGAIN_STR "Client: call sendData again"
#define XDGVFS_RESULT_GIVE_ME_DATA_STR "Client: give me data"
#define XDGVFS_RESULT_PARAMETER_ERR_STR "Client: parameter error"
#define XDGVFS_RESULT_FATAL_ERR_STR "Client: fatal error"
#define XDGVFS_RESULT_UNESCAPING_DATA_ERR_STR "Client: unescaping data error"
#define XDGVFS_RESULT_BAD_STATE_STR "Client: bad state"
#define XDGVFS_RESULT_IOERR_STR "Client: IOERR"
#define XDGVFS_RESULT_NOT_POSSIBLE_STR "Client: operation not possible" 
#define XDGVFS_RESULT_CONTINUES_STR "Client: result continues"
#define XDGVFS_RESULT_OK_STR "Client: OK/DONE"

typedef int XdgVfsResult;

/**
*  session state
*/
typedef enum {
	XDGVFS_STATE_ERROR_LEAVING = -1,
	XDGVFS_STATE_READING_DATA = 1,
	XDGVFS_STATE_WRITING_DATA = 2,
	XDGVFS_STATE_EXPECTING_PROMPT = 3,
	XDGVFS_STATE_READY = 4,
	XDGVFS_STATE_READING_TEXT = 5
} XdgVfsState;

typedef struct _XdgVfsSession XdgVfsSession;

typedef void (*XdgVfsMonitorCallback)(XdgVfsSession * sess, char * uri, void * user_data);

/*
*  Session Object 
*  never access the members of this stuct directly (it might change)!
*/
struct _XdgVfsSession {
	/* all members are private! */

	/* pipes to the sub-process */
	int fd_in;
	int fd_out;
	
	int pid;

	/* incoming data buffers */
	char * inBuf;
	int inBuf_used;
	int inBuf_size;
	XdgVfsState stateIn;

	char * rawBufIn;

	/* outgoing data buffers */
	char * outBuf;
	int outBuf_sent;
	int outBuf_used;
	int outBuf_size;
	int outBuf_EOF;
	XdgVfsState stateOut;

	/* incoming items */
	XdgVfsItemType currentItemType;
	XdgVfsItem * currentItem;

	int lastJobErrorCode;
	char * lastJobErrorText;
	
	/* file/dir change monitoring */
	XdgVfsMonitorCallback monitor_cb;
	void * monitor_cb_userdata;
	
	/* recursion protection */
	int recurse_count;

};


/**
*
*  Start Session and setup child process
*  (with a session you can run a 'session of' xdg-vfs commands (one after the other)
*  pass NULL or "this" for the current desktop or "kde"/"gnome" for preferredDesktop.
*/
XdgVfsResult xdg_vfs_sess_start(XdgVfsSession ** sessOut, char * preferredDesktop);

/**
*  Close session and kill child process
*
*/
XdgVfsResult xdg_vfs_sess_close(XdgVfsSession * sess);

/**
*  Cancel the current Command 
*
*/
XdgVfsResult xdg_vfs_sess_cancelCommand(XdgVfsSession * sess);

/**
*  Watch this fd to avoid blocking in xdgvfs_sess_readItem()
*
*/
int xdg_vfs_sess_getIncomingFiledescriptor(XdgVfsSession * sess);

/**
*  Watch this fd to avoid blocking in xdg_vfs_sess_sendData();
*
*/
int xdg_vfs_sess_getOutgoingFiledescriptor(XdgVfsSession * sess);

/**
*  Check if the session is ready for commands
*
*/
int xdg_vfs_sess_isBusy(XdgVfsSession * sess);

/* ============== file/dirwatch monitor ===================== */

void xdg_vfs_sess_set_monitor_callback(
		XdgVfsSession * sess,
		XdgVfsMonitorCallback cb,
		void * userdata
		);

/* ====================== VFS Commands =======================*/

/*
*  After running a command the session will leave the READY state and 
*  you have to read items by calling xdg_vfs_sess_readItem() (except for the 'simple' commands)
*
*  See the samples in the 'tests' directory for a tutorial.
*/

/**
*  Command: get Backend Info
*
*  returns items:
*     - XDGVFS_ITEMTYPE_BACKENDINFO
*/
XdgVfsResult xdg_vfs_sess_cmd_backendInfo(XdgVfsSession * sess);

/**
*  Command: get File
*
*  returns items:
*     - XDGVFS_ITEMTYPE_GET_HEAD
*     - XDGVFS_DATAIN (multi)
*     - XDGVFS_ITEMTYPE_DATAIN_DONE
*/
XdgVfsResult xdg_vfs_sess_cmd_getFile(XdgVfsSession * sess, const char * uri);

/**
*  Command: put File
*
*  returns items:
*     - XDGVFS_ITEMTYPE_PUT_HEAD
*     - XDGVFS_DATAOUT (multi -> you have to put data into the outgoing buffer and send it)
*     - XDGVFS_ITEMTYPE_DATAOUT_DONE
*/
XdgVfsResult xdg_vfs_sess_cmd_putFile(XdgVfsSession * sess, const char * uri, 
	XdgVfsFlags flags);

/**
*  Command: getFileInfo
*
*  returns items:
*     - XDGVFS_ITEMTYPE_FILEINFO
*/
XdgVfsResult xdg_vfs_sess_cmd_getFileInfo(XdgVfsSession * sess, const char * uri);

/**
*  Command: SetAttrs
*  
*  leave user or group NULL when you don't want to change it.
*  set setPermFlag=0 if you don't want to change the permissions.
*
*  returns items:
*     - XDGVFS_ITEMTYPE_SETATTRS_HEAD
*
*/
XdgVfsResult xdg_vfs_sess_cmd_setAttrs(XdgVfsSession * sess, 
	const char * uri, int permissions, int setPermFlag, char * user, char * group);

/**
*  Command: CopyFile
*
*  returns items:
*     - XDGVFS_ITEMTYPE_COPY_HEAD
*     - XDGVFS_ITEMTYPE_PROGRESS (multi)
*/
XdgVfsResult xdg_vfs_sess_cmd_copyFile(XdgVfsSession * sess, const char * uri_src, const char * uri_target);

/**
*  Command: Move File
*
*  returns items:
*     - XDGVFS_ITEMTYPE_MOVE_HEAD
*     - XDGVFS_ITEMTYPE_PROGRESS (multi)
*/
XdgVfsResult xdg_vfs_sess_cmd_moveFile(XdgVfsSession * sess, const char * uri_src, const char * uri_target);

/**
*  Command: Make Directory
*
*  returns items:
*     - XDGVFS_ITEMTYPE_MKDIR_HEAD
*
*/
XdgVfsResult xdg_vfs_sess_cmd_makeDirectory(XdgVfsSession * sess, const char * uri);

/**
*  Command: Remove Directory
*
*  returns items:
*     - XDGVFS_ITEMTYPE_RMDIR_HEAD
*/
XdgVfsResult xdg_vfs_sess_cmd_removeDirectory(XdgVfsSession * sess, const char * uri);

/**
*  Command: Delete File
*
*  returns items:
*     - XDGVFS_ITEMTYPE_RM_HEAD
*/
XdgVfsResult xdg_vfs_sess_cmd_removeFile(XdgVfsSession * sess, const char * uri);

/**
*  Command: List Directory
*
*  returns items:
*     - XDGVFS_ITEMTYPE_LS_HEAD
*     - XDGVFS_ITEMTYPE_FILEINFO (multi)
*/
XdgVfsResult xdg_vfs_sess_cmd_listDirectory(XdgVfsSession * sess, const char * uri);

/**
*  Command: Mount
*
*  returns items:
*     (none, but you have to loop xdg_vfs_sess_readItem() to wait for the result)
*/
XdgVfsResult xdg_vfs_sess_cmd_mount(XdgVfsSession * sess, const char * mountpoint_id);

/**
*  Command: monitor directory
*  This is a simple command, you don't have to call xdg_vfs_sess_readItem().
*/
XdgVfsResult xdg_vfs_sess_cmd_monitorDir(XdgVfsSession * sess, const char * uri);

/**
*  Command: monitor file
*  This is a simple command, you don't have to call xdg_vfs_sess_readItem().
*/
XdgVfsResult xdg_vfs_sess_cmd_monitorFile(XdgVfsSession * sess, const char * uri);

/**
*  Command: remove monitor
*  This is a simple command, you don't have to call xdg_vfs_sess_readItem().
*/
XdgVfsResult xdg_vfs_sess_cmd_removeMonitor(XdgVfsSession * sess, const char * uri);

/**
*  Command: Show Open-File-Dialog
*
*  returns items:
*     - XDGVFS_ITEMTYPE_OPENFILEDLG_RESPONSE
*/
XdgVfsResult xdg_vfs_sess_cmd_openFileDialog(XdgVfsSession * sess, const char * default_uri, XdgVfsFlags flags);

/**
*  Command: Show Save-File-Dialog
*
*  returns itmes:
*     - XDGVFS_ITEMTYPE_SAVEFILEDLG_RESPONSE
*/
XdgVfsResult xdg_vfs_sess_cmd_saveFileDialog(XdgVfsSession * sess, 
	const char * default_folder_uri, 
	const char * default_filename, 
	XdgVfsFlags flags);

/* ============= READ / WRITE FUNCTIONS ============= */

/**
*  Read an Item or a Chunk of File-Data. Continue reading items until
*  the return value is != XDGVFS_RESULT_CONTINUES.
*
*  This function has to be looped after executing a xdg-vfs command (but not for the 'simple' commands).
*  
*  If you want to do this non-blocking check the incoming fd in your event-loop.
*/
XdgVfsResult xdg_vfs_sess_readItem(XdgVfsSession * sess, XdgVfsItemType * typeOut, 
		XdgVfsItem ** itemOut, char ** buffer, int * len);


/* ------ used when writing a file with xdg_vfs_sess_cmd_putFile() -------- */

/**
*  Put a Chunk of File-Data into the outgoing buffer.
*  set len = 0 on EOF. 
*  If there is no error this function will usually return XDGVFS_RESULT_CALL_SENDDATA_AGAIN 
*  -> You have to call the function xdg_vfs_sess_sendData() until the outgoing 
*  buffer is emptied.
*/
XdgVfsResult xdg_vfs_sess_putDataChunk(XdgVfsSession * sess, char * data, int len);

/**
*  Send data from the outgoing buffer - repeat this on 
*  XDGVFS_RESULT_CALL_SENDDATA_AGAIN until you get 
*  XDGVFS_RESULT_GIVE_ME_DATA or XDGVFS_RESULT_OK (finished);
* 
*  if you want to do that nonblocking check the outgoing fd first
*/
XdgVfsResult xdg_vfs_sess_sendData(XdgVfsSession * sess);



/* ================= XdgVfsItem  ======================*/

/**
*  increase reference Count
*
*/
XdgVfsItem * xdg_vfs_item_ref(XdgVfsItem * item);

/**
*  decrease reference Count
*
*/
void xdg_vfs_item_unref(XdgVfsItem * item);

/* =================== tools ========================= */

/*
*  get string message for XdgVfsResult. The string is statically allocated, you don't have to free() it.
*/
char * xdg_vfs_resultToString(XdgVfsResult code);


#ifdef __cplusplus
}
#endif
#endif 
