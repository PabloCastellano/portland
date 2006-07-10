/*
#   xdg_vfs_errstrs.c
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
#include "xdg_vfs_common.h"

typedef struct 
{
	int code;
	char * str;
} XdgVfsErrorToStringMapping;

XdgVfsErrorToStringMapping _xdg_vfs_error2string_mapping_arr[] = 
{

	{ XDGVFS_ERRORCODE_OK 							, "OK" },
	{ XDGVFS_ERRORCODE_CANCELED 					, "Canceled" },
	{ XDGVFS_ERRORCODE_BAD_PARAMETER 				, "Bad Parameter" },
	{ XDGVFS_ERRORCODE_NOT_SUPPORTED 				, "Not supported" },
	{ XDGVFS_ERRORCODE_GENERIC 						, "Generic error" },
	{ XDGVFS_ERRORCODE_INIT_ERROR 					, "init error" },
	{ XDGVFS_ERRORCODE_DATA_ERROR 					, "Data error" },
	{ XDGVFS_ERRORCODE_IO_ERROR 					, "IO error" },
	{ XDGVFS_ERRORCODE_NOT_PERMITTED 				, "Not permitted" },
	{ XDGVFS_ERRORCODE_MALFORMED_URL 				, "Malformed URL" },
	{ XDGVFS_ERRORCODE_PROTOCOL_ERROR 				, "Protocol error" },
	{ XDGVFS_ERRORCODE_USR_OR_GRP_NOT_FOUND 		, "User or Group not found" },
	{ XDGVFS_ERRORCODE_INTERNAL 					, "Internal error" },
	{ XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_WRITING 		, "Cannot open for writing"},
	{ XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_READING 		, "Cannot open for reading"},
	{ XDGVFS_ERRORCODE_IS_DIRECTORY 				, "is directory"},
	{ XDGVFS_ERRORCODE_IS_FILE 						, "is file"},
	{ XDGVFS_ERRORCODE_NOT_FOUND 					, "not found"},
	{ XDGVFS_ERRORCODE_ALREADY_EXISTS 				, "already exists"},
	{ XDGVFS_ERRORCODE_HOST_NOT_FOUND 				, "host not found"},
	{ XDGVFS_ERRORCODE_ACCESS_DENIED 				, "access denied"},
	{ XDGVFS_ERRORCODE_CANNOT_ENTER_DIRECTORY 		, "cannot enter directory"},
	{ XDGVFS_ERRORCODE_PROTOCOL_IS_NOT_A_FILESYSTEM , "protocol is not a filesystem"},
	{ XDGVFS_ERRORCODE_LOOP 						, "loop"},
	{ XDGVFS_ERRORCODE_COULD_NOT_CREATE_SOCKET 		, "cannot create socket"},
	{ XDGVFS_ERRORCODE_COULD_NOT_CONNECT 			, "could not connect"},
	{ XDGVFS_ERRORCODE_ERR_CONNECTION_BROKEN 		, "conection broken"},
	{ XDGVFS_ERRORCODE_COULD_NOT_MOUNT 				, "could not mount"},
	{ XDGVFS_ERRORCODE_COULD_NOT_UNMOUNT 			, "could not unmount"},
	{ XDGVFS_ERRORCODE_SOCKET_ERROR 				, "socket error"},
	{ XDGVFS_ERRORCODE_LOGIN_FAILED 				, "login failed"},
	{ XDGVFS_ERRORCODE_OPERATION_FAILED 			, "operation failed"},
	{ XDGVFS_ERRORCODE_CANNOT_RESUME 				, "cannot resume"},
	{ XDGVFS_ERRORCODE_OUT_OF_MEMORY 				, "out of memory"},
	{ XDGVFS_ERRORCODE_UNKNOWN_PROXY_HOST 			, "unknown proxy host"},
	{ XDGVFS_ERRORCODE_ABORTED 						, "aborted"},
	{ XDGVFS_ERRORCODE_TIMEOUT 						, "timeout"},
	{ XDGVFS_ERRORCODE_SERVICE_NOT_AVAILABLE 		, "service not available"},
	{ XDGVFS_ERRORCODE_CANNOT_DELETE_ORIGINAL 		, "cannot delete original"},
	{ XDGVFS_ERRORCODE_CANNOT_DELETE_PARTIAL 		, "cannot delete partial"},
	{ XDGVFS_ERRORCODE_CANNOT_RENAME_ORIGINAL 		, "cannot rename original"},
	{ XDGVFS_ERRORCODE_CANNOT_RENAME_PARTIAL 		, "cannot rename partial"},
	{ XDGVFS_ERRORCODE_NEED_PASSWD 					, "need passwd"},
	{ XDGVFS_ERRORCODE_NO_CONTENT 					, "no content"},
	{ XDGVFS_ERRORCODE_NO_SPACE 					, "no space"},
	{ XDGVFS_ERRORCODE_IDENTICAL_FILES 				, "identical files"},
	{ XDGVFS_ERRORCODE_CORRUPTED_DATA 				, "corrupted data"},
	{ XDGVFS_ERRORCODE_WRONG_FORMAT 				, "wrong format"},
	{ XDGVFS_ERRORCODE_ERROR_BAD_FILE 				, "bad file"},
	{ XDGVFS_ERRORCODE_TOO_BIG 						, "too big"},
	{ XDGVFS_ERRORCODE_NOT_OPEN 					, "not open"},
	{ XDGVFS_ERRORCODE_INVALID_OPEN_MODE 			, "invalid open mode"},
	{ XDGVFS_ERRORCODE_TOO_MANY_OPEN_FILES 			, "too many open files"},
	{ XDGVFS_ERRORCODE_EOF 							, "End of file (EOF)"},
	{ XDGVFS_ERRORCODE_IN_PROGRESS 					, "In Progress" },
	{ XDGVFS_ERRORCODE_INTERRUPTED 					, "interrupted" },
	{ XDGVFS_ERRORCODE_HOST_HAS_NO_ADDRESS 			, "Host has no address" },
	{ XDGVFS_ERRORCODE_DIRECTORY_NOT_EMPTY 			, "Directory not empty" },
	{ XDGVFS_ERRORCODE_TOO_MANY_LINKS 				, "Too many links" },
	{ XDGVFS_ERRORCODE_READ_ONLY_FILE_SYSTEM 		, "Read only filesystem" },
	{ XDGVFS_ERRORCODE_NOT_SAME_FILE_SYSTEM 		, "not the same filesystem" },
	{ XDGVFS_ERRORCODE_NAME_TOO_LONG 				, "name too long" },
	{ XDGVFS_ERRORCODE_SERVICE_OBSOLETE 			, "service obsolete" },
	{ XDGVFS_ERRORCODE_NO_DEFAULT 					, "no default" },
	{ XDGVFS_ERRORCODE_NAMESERVER 					, "nameserver error" },
	{ XDGVFS_ERRORCODE_LOCKED 						, "locked" },
	{ XDGVFS_ERRORCODE_DEPRECATED_FUNCTION 			, "deprecated function" },
	{ XDGVFS_ERRORCODE_DIRECTORY_BUSY 				, "directory busy" },
	{ XDGVFS_ERRORCODE_NOT_A_DIRECTORY 				, "not a directory" }
};

char * xdg_vfs_resultToString(XdgVfsResult code) 
{
	int i;
	for (i=0;i<sizeof(_xdg_vfs_error2string_mapping_arr)/sizeof(XdgVfsErrorToStringMapping);i++) 
	{
		if (_xdg_vfs_error2string_mapping_arr[i].code == code) 
			return _xdg_vfs_error2string_mapping_arr[i].str;
	}

	switch(code)
	{
		case(XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND): return XDGVFS_RESULT_CANNOT_LAUNCH_BACKEND_STR;
		case(XDGVFS_RESULT_CALL_SENDDATA_AGAIN): return XDGVFS_RESULT_CALL_SENDDATA_AGAIN_STR;
		case(XDGVFS_RESULT_GIVE_ME_DATA): return XDGVFS_RESULT_GIVE_ME_DATA_STR;
		case(XDGVFS_RESULT_PARAMETER_ERR): return XDGVFS_RESULT_PARAMETER_ERR_STR;
		case(XDGVFS_RESULT_FATAL_ERR): return XDGVFS_RESULT_FATAL_ERR_STR;
		case(XDGVFS_RESULT_UNESCAPING_DATA_ERR): return XDGVFS_RESULT_UNESCAPING_DATA_ERR_STR;
		case(XDGVFS_RESULT_BAD_STATE): return XDGVFS_RESULT_BAD_STATE_STR;
		case(XDGVFS_RESULT_IOERR): return XDGVFS_RESULT_IOERR_STR;
		case(XDGVFS_RESULT_NOT_POSSIBLE): return XDGVFS_RESULT_NOT_POSSIBLE_STR;
		case(XDGVFS_RESULT_CONTINUES): return XDGVFS_RESULT_CONTINUES_STR;
		case(XDGVFS_RESULT_OK): return XDGVFS_RESULT_OK_STR;
	}
	return "Client: cannot translate error code to string!";
}


