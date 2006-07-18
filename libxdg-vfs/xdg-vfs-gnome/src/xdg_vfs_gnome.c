/*
#   xdg_vfs_gnome.c
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

/**
 *  READ:
 *  http://cvs.gnome.org/viewcvs/libgnomeui/file-chooser/
 *  http://cvs.gnome.org/viewcvs/libgnomeui/libgnomeui/gnome-icon-lookup.c
 *  http://standards.freedesktop.org/desktop-entry-spec/
 *  http://developer.gimp.org/api/2.0/glib/glib-Key-value-file-parser.html
 *  http://developer.gnome.org/doc/API/2.0/gnome-vfs-2.0/
 * 
 */


#include <string.h>

#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomeui/libgnomeui.h>
#include <time.h>

#include <pwd.h>
#include <grp.h>

#include "xdg_vfs_common.h"
#include "xdg_vfs.h"
#include "xdg_vfs_gnome.h"

#define DESKTOP_GROUP "Desktop Entry"


/*
*   	translate error codes
*/
typedef struct 
{
	GnomeVFSResult result;
	int xdgVfsErr;
} MapGnomeVFSResult;

MapGnomeVFSResult mapGVFSErrArr[] = 
{
	{ GNOME_VFS_OK				, XDGVFS_ERRORCODE_OK } ,
	{ GNOME_VFS_ERROR_NOT_FOUND		, XDGVFS_ERRORCODE_NOT_FOUND } ,
	{ GNOME_VFS_ERROR_GENERIC		, XDGVFS_ERRORCODE_GENERIC } ,
	{ GNOME_VFS_ERROR_INTERNAL		, XDGVFS_ERRORCODE_INTERNAL } ,
	{ GNOME_VFS_ERROR_BAD_PARAMETERS	, XDGVFS_ERRORCODE_BAD_PARAMETER } ,
	{ GNOME_VFS_ERROR_NOT_SUPPORTED         , XDGVFS_ERRORCODE_NOT_SUPPORTED } ,
	{ GNOME_VFS_ERROR_IO                    , XDGVFS_ERRORCODE_IO_ERROR } ,
	{ GNOME_VFS_ERROR_CORRUPTED_DATA	, XDGVFS_ERRORCODE_CORRUPTED_DATA } ,
	{ GNOME_VFS_ERROR_WRONG_FORMAT		, XDGVFS_ERRORCODE_WRONG_FORMAT } ,
	{ GNOME_VFS_ERROR_BAD_FILE		, XDGVFS_ERRORCODE_ERROR_BAD_FILE } ,
	{ GNOME_VFS_ERROR_TOO_BIG		, XDGVFS_ERRORCODE_TOO_BIG } ,
	{ GNOME_VFS_ERROR_NO_SPACE		, XDGVFS_ERRORCODE_NO_SPACE } ,
	{ GNOME_VFS_ERROR_READ_ONLY		, XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_WRITING } ,
	{ GNOME_VFS_ERROR_INVALID_URI		, XDGVFS_ERRORCODE_MALFORMED_URL } ,
	{ GNOME_VFS_ERROR_NOT_OPEN		, XDGVFS_ERRORCODE_NOT_OPEN } ,
	{ GNOME_VFS_ERROR_INVALID_OPEN_MODE	, XDGVFS_ERRORCODE_INVALID_OPEN_MODE } ,
	{ GNOME_VFS_ERROR_ACCESS_DENIED		, XDGVFS_ERRORCODE_ACCESS_DENIED } ,
	{ GNOME_VFS_ERROR_TOO_MANY_OPEN_FILES	, XDGVFS_ERRORCODE_TOO_MANY_OPEN_FILES } ,
	{ GNOME_VFS_ERROR_EOF			, XDGVFS_ERRORCODE_EOF } ,
	{ GNOME_VFS_ERROR_NOT_A_DIRECTORY	, XDGVFS_ERRORCODE_NOT_A_DIRECTORY } ,
	{ GNOME_VFS_ERROR_IN_PROGRESS		, XDGVFS_ERRORCODE_IN_PROGRESS } ,
	{ GNOME_VFS_ERROR_INTERRUPTED		, XDGVFS_ERRORCODE_INTERRUPTED } ,
	{ GNOME_VFS_ERROR_FILE_EXISTS		, XDGVFS_ERRORCODE_ALREADY_EXISTS } , 
	{ GNOME_VFS_ERROR_LOOP			, XDGVFS_ERRORCODE_LOOP } ,
	{ GNOME_VFS_ERROR_NOT_PERMITTED		, XDGVFS_ERRORCODE_NOT_PERMITTED } ,
	{ GNOME_VFS_ERROR_IS_DIRECTORY		, XDGVFS_ERRORCODE_IS_DIRECTORY } ,
	{ GNOME_VFS_ERROR_NO_MEMORY		, XDGVFS_ERRORCODE_OUT_OF_MEMORY } ,
	{ GNOME_VFS_ERROR_HOST_NOT_FOUND	, XDGVFS_ERRORCODE_HOST_NOT_FOUND } ,
	{ GNOME_VFS_ERROR_INVALID_HOST_NAME	, XDGVFS_ERRORCODE_HOST_NOT_FOUND } ,
	{ GNOME_VFS_ERROR_HOST_HAS_NO_ADDRESS   , XDGVFS_ERRORCODE_HOST_HAS_NO_ADDRESS } ,
	{ GNOME_VFS_ERROR_LOGIN_FAILED		, XDGVFS_ERRORCODE_LOGIN_FAILED } ,
	{ GNOME_VFS_ERROR_CANCELLED		, XDGVFS_ERRORCODE_CANCELED } ,
	{ GNOME_VFS_ERROR_DIRECTORY_BUSY	, XDGVFS_ERRORCODE_DIRECTORY_BUSY } ,
	{ GNOME_VFS_ERROR_DIRECTORY_NOT_EMPTY	, XDGVFS_ERRORCODE_DIRECTORY_NOT_EMPTY } ,
	{ GNOME_VFS_ERROR_TOO_MANY_LINKS	, XDGVFS_ERRORCODE_TOO_MANY_LINKS } ,
	{ GNOME_VFS_ERROR_READ_ONLY_FILE_SYSTEM , XDGVFS_ERRORCODE_READ_ONLY_FILE_SYSTEM } ,
	{ GNOME_VFS_ERROR_NOT_SAME_FILE_SYSTEM	, XDGVFS_ERRORCODE_NOT_SAME_FILE_SYSTEM } ,
	{ GNOME_VFS_ERROR_NAME_TOO_LONG		, XDGVFS_ERRORCODE_NAME_TOO_LONG } ,
	{ GNOME_VFS_ERROR_SERVICE_NOT_AVAILABLE	, XDGVFS_ERRORCODE_SERVICE_NOT_AVAILABLE } ,
	{ GNOME_VFS_ERROR_SERVICE_OBSOLETE 	, XDGVFS_ERRORCODE_SERVICE_OBSOLETE } ,
	{ GNOME_VFS_ERROR_PROTOCOL_ERROR	, XDGVFS_ERRORCODE_PROTOCOL_ERROR } ,
	{ GNOME_VFS_ERROR_NO_MASTER_BROWSER	, XDGVFS_ERRORCODE_INTERNAL } ,
	{ GNOME_VFS_ERROR_NO_DEFAULT		, XDGVFS_ERRORCODE_NO_DEFAULT } ,
	{ GNOME_VFS_ERROR_NO_HANDLER		, XDGVFS_ERRORCODE_PROTOCOL_ERROR } ,
	{ GNOME_VFS_ERROR_PARSE			, XDGVFS_ERRORCODE_CORRUPTED_DATA } ,
	{ GNOME_VFS_ERROR_LAUNCH		, XDGVFS_ERRORCODE_INIT_ERROR } ,
	{ GNOME_VFS_ERROR_TIMEOUT		, XDGVFS_ERRORCODE_TIMEOUT } ,
	{ GNOME_VFS_ERROR_NAMESERVER		, XDGVFS_ERRORCODE_NAMESERVER } ,
	{ GNOME_VFS_ERROR_LOCKED		, XDGVFS_ERRORCODE_LOCKED } ,
	{ GNOME_VFS_ERROR_DEPRECATED_FUNCTION	, XDGVFS_ERRORCODE_DEPRECATED_FUNCTION } 
};

int translate_GnomeVFSResult(GnomeVFSResult result) 
{
	int i;
	for (i=0;i<sizeof(mapGVFSErrArr)/sizeof(MapGnomeVFSResult);i++) 
	{
		/* printf(" %d %d\n", mapErrArr[i].kioErr, mapErrArr[i].xdgVfsErr); */
		if (mapGVFSErrArr[i].result == result) return mapGVFSErrArr[i].xdgVfsErr;
	}
	return XDGVFS_ERRORCODE_GENERIC;

}

/**
*   	initialize gnome
*/
int xdg_vfs_init_desklib(int argc, char** argv) 
{
	GnomeProgram *my_app;
	GOptionContext *option_context;
	
	my_app = gnome_program_init(PACKAGE, VERSION, LIBGNOMEUI_MODULE,
	                            argc, argv, GNOME_PARAM_NONE);

	gnome_authentication_manager_init();

	if (!gnome_vfs_init ()) 
	{ 
		fprintf(stderr, "Count not init Gnome-VFS!\n");
		return XDGVFS_ERRORCODE_INIT_ERROR;
	}

	return 0;
}

/**
*   	finish pending GUI-events
*/
int xdg_vfs_iterate_mainloop() 
{
	while (g_main_context_pending (NULL))
		g_main_context_iteration(NULL, FALSE);
}

/**
*       print gnome-vfs error
*/
int print_and_translate_result(GnomeVFSResult result) 
{
	const gchar * s;
	s = gnome_vfs_result_to_string (result);

	fprintf(stderr, "ERROR: GnomeVFSResult=%s\n",s);
	//free(s);
	return translate_GnomeVFSResult(result);
}

#define READ_ENTIRE_FILE_INITAL_BUFSIZE 100
/**
 * read entire file for desktop files
 */
GnomeVFSResult _gnome_vfs_readEntireFile(const char * text_uri, size_t * size, char ** fileData, int opts) 
{
	char *  buffer = malloc(READ_ENTIRE_FILE_INITAL_BUFSIZE);
	size_t bufSize = READ_ENTIRE_FILE_INITAL_BUFSIZE;
	GnomeVFSFileSize bytes_read;
	int pos=0;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	GnomeVFSHandle *handle;
	GnomeVFSResult r;
	
	*fileData = NULL;
	
	r=gnome_vfs_open(&handle, full_uri, GNOME_VFS_OPEN_READ);
	if (r!=GNOME_VFS_OK) 
	{
		return r;
	}

	while (1) 
	{
		if (bufSize-pos <= 1) {
			bufSize += READ_ENTIRE_FILE_INITAL_BUFSIZE;
			buffer = realloc(buffer, bufSize);	
		}
		
		r=gnome_vfs_read(handle, buffer+pos, bufSize - pos, &bytes_read);
		//fprintf(stderr, "gnome-vfs-read fname=%s, pos=%d bufSize=%d bytes_read=%d\n", full_uri, pos, bufSize, bytes_read);		
		
		if (r!=GNOME_VFS_OK) 
		{
			if (r==GNOME_VFS_ERROR_EOF) {
				break;
			}
			free(buffer);
			gnome_vfs_close (handle);
			return r;			
		}
		pos += bytes_read;

	}
/*	if (pos != size) { 
		free(buffer);
		fprintf(stderr, "Reading entire file problem pos=%d != size=%d\n", pos, size);
		gnome_vfs_close (handle);
		return GNOME_VFS_ERROR_IO;
	} */
	*fileData = buffer;
	*size = pos;
	return GNOME_VFS_OK;
}


GnomeVFSResult _follow_desktop_link(const char * filename, int opts) {
	GnomeVFSResult r;
	GKeyFile *desktop_file;
	char * desktop_file_data;
	size_t desktop_file_data_size;
	GError * gerror = NULL;
	
	desktop_file = g_key_file_new ();
	
	r = _gnome_vfs_readEntireFile(filename, &desktop_file_data_size, &desktop_file_data, opts);
	if (r != GNOME_VFS_OK) return r;
	
	int parseR = g_key_file_load_from_data(desktop_file,
                                             desktop_file_data,
                                             desktop_file_data_size,
                                             G_KEY_FILE_NONE,
                                             &gerror);
    g_free(desktop_file_data);
	if (!parseR) {
		if (gerror != NULL)
		{
			fprintf (stderr, "Unable to parse desktop file: %s\n", gerror->message);
			g_error_free (gerror);
			g_key_file_free (desktop_file);
		} 
	}
	
	gchar *name;
	gchar *type;
	gchar *URL;
	gchar *icon;

	type = g_key_file_get_value (desktop_file, DESKTOP_GROUP, "Type", NULL);

	if (!strncmp (type, "Link", 4) && !strncmp (type, "FSDevice", 8)) 
	{
		g_free(type);
		g_key_file_free (desktop_file);
		return ! GNOME_VFS_OK;
	}

	name = g_key_file_get_value (desktop_file, DESKTOP_GROUP, "Name", NULL);

	

	URL = g_key_file_get_value (desktop_file, DESKTOP_GROUP, "URL", NULL);
	
	icon = g_key_file_get_value (desktop_file, DESKTOP_GROUP, "Icon", NULL);	

	if (name) fprintf (stdout, "basename=%s\n", name);
	if (type) fprintf (stdout, "# de type=%s\n", type);
	if (URL) fprintf (stdout, "uri=%s\n", URL);
	if (icon) fprintf (stdout, "iconname=%s\n", icon);

	fprintf(stdout, "filetype=DIRECTORY\n");

	/*
  	ret = 
  	g_free  (type);	
	*/
	
	g_free(name);
	g_free(type);	
	g_free(URL);	
	g_free(icon);

	g_key_file_free (desktop_file);
	
	return GNOME_VFS_OK;

}

/**
*       print fileinfo in key-value pair style
*/
void xdg_gnome_vfs_printFileInfo(FILE * f, const char * full_uri, GnomeVFSFileInfo * info, int opts) 
{

	GnomeVFSFileInfoFields valid = info->valid_fields;
	int did_de_dive = 0;
	
	
	if (opts & XDGVFS_OPT_DIVE_DE_LINKS) 
	{
		
		if ((info->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_MIME_TYPE) != 0
	  && strcmp (gnome_vfs_file_info_get_mime_type (info), "application/x-desktop") == 0) 
		{
			
			if (_follow_desktop_link(full_uri, opts) == GNOME_VFS_OK)
			{
				// fprintf(stderr, "dive!\n");
				return;	
			}
	
		}
	}

	fprintf(f, "basename=%s\n", info->name);

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_SIZE) 
	{
		fprintf(f, "size=%u\n", info->size);
	}
	if (1) 
	{   
		struct passwd * passwdEntry;
		struct group * groupEntry;

		passwdEntry = getpwuid ((uid_t)info->uid);
		if (passwdEntry) 
		{
			xdg_vfs_ppair_str(f, "user", passwdEntry->pw_name, opts);
		}
		
		groupEntry = getgrgid ((gid_t)info->gid);
		if (groupEntry) 
		{
			xdg_vfs_ppair_str(f, "group", groupEntry->gr_name, opts);
		}

		/* fprintf(f, "userid=%d\n", info->uid);
                fprintf(f, "groupid=%d\n", info->gid); */
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_PERMISSIONS) 
	{
		fprintf(f, "permissions=%d\n", info->permissions);
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_MTIME) 
	{
		fprintf(f, "mtime=%d\n", info->mtime);
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_ATIME) 
	{
		fprintf(f, "atime=%d\n", info->atime);
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_CTIME) 
	{
		fprintf(f, "ctime=%d\n", info->ctime);
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_TYPE) 
	{
		char * tstr="UNKNOWN";

		switch(info->type) 
		{
        	case(GNOME_VFS_FILE_TYPE_UNKNOWN): tstr = "UNKNOWN"; break;
			case(GNOME_VFS_FILE_TYPE_REGULAR): tstr = "REGULAR"; break;
			case(GNOME_VFS_FILE_TYPE_DIRECTORY): tstr = "DIRECTORY"; break;
			case(GNOME_VFS_FILE_TYPE_FIFO): tstr = "FIFO"; break;
			case(GNOME_VFS_FILE_TYPE_SOCKET): tstr = "SOCKET"; break;
			case(GNOME_VFS_FILE_TYPE_CHARACTER_DEVICE): tstr = "CHARDEV"; break;
			case(GNOME_VFS_FILE_TYPE_BLOCK_DEVICE): tstr = "BLOCKDEV"; break;
			case(GNOME_VFS_FILE_TYPE_SYMBOLIC_LINK): tstr = "SYMLINK"; break;
			/* default: tstr = "UNKNOWN"; break; */
		}
		fprintf(f, "filetype=%s\n", tstr);

	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_SYMLINK_NAME) 
	{
		fprintf(f, "linkdest=%s\n", info->symlink_name);
	}

	if (valid & GNOME_VFS_FILE_INFO_FIELDS_SIZE) 
	{
		fprintf(f, "mimetype=%s\n", info->mime_type);
	}

	GnomeIconLookupResultFlags iconLookupResultFlags;
	char * iconname = gnome_icon_lookup (gtk_icon_theme_get_default (),
                                             NULL,
                                             NULL /* const char *file_uri */,
                                             NULL,
                                             info,
                                             info->mime_type,
                                             GNOME_ICON_LOOKUP_FLAGS_NONE,
                                             &iconLookupResultFlags);
	if (iconname) fprintf(f, "iconname=%s\n", iconname);
	g_free(iconname);
	
	xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	
	if (xdg_vfs_getCustomEndSeparator()) 
		fprintf(stdout, "\n%s\n", xdg_vfs_getCustomEndSeparator());
}


/**
*   	Command: fileinfo
*/
int xdg_vfs_getFileInfo(const char * text_uri, int opts) 
{
	GnomeVFSResult r;
	GnomeVFSFileInfo * info = gnome_vfs_file_info_new ();
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);

	if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_FILEINFO);

	GnomeVFSFileInfoOptions infoOpts = GNOME_VFS_FILE_INFO_FOLLOW_LINKS | GNOME_VFS_FILE_INFO_GET_MIME_TYPE;

	if (opts & XDGVFS_OPT_SLOW_MIME_TYPE) infoOpts |= GNOME_VFS_FILE_INFO_FORCE_SLOW_MIME_TYPE;

	r=gnome_vfs_get_file_info(full_uri,
                                             info,
                                             infoOpts);
	xdg_vfs_iterate_mainloop();

	if (r!=GNOME_VFS_OK) 
	{
		g_free(full_uri);
		gnome_vfs_file_info_unref (info);
		return print_and_translate_result(r);
	}

	xdg_gnome_vfs_printFileInfo(stdout, full_uri, info, opts);
		

	gnome_vfs_file_info_unref (info);
	g_free(full_uri);
	return 0;
}

/**
*   	Command: setattrs
*/
int xdg_vfs_setFileInfo(const char * text_uri, XdgVfsSetFileInfoData * fiData, int opts) 
{
	GnomeVFSResult r;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	struct passwd * passwdEntry;
	struct group * groupEntry;
	GnomeVFSFileInfo * info = gnome_vfs_file_info_new ();
	info->valid_fields = GNOME_VFS_FILE_INFO_FIELDS_NONE;
	GnomeVFSSetFileInfoMask setFileInfoMask = GNOME_VFS_SET_FILE_INFO_NONE;

	if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_SETATTRS);

	if (fiData->user) 
	{
		passwdEntry = getpwnam(fiData->user);
		if (passwdEntry) 
		{
			setFileInfoMask |= GNOME_VFS_SET_FILE_INFO_OWNER;
			 
			info->uid = passwdEntry->pw_uid;
		} 
		else 
		{
			fprintf(stderr, "ERROR: User '%s' not found in passwd\n", fiData->user);
			gnome_vfs_file_info_unref (info); g_free(full_uri);
			return XDGVFS_ERRORCODE_USR_OR_GRP_NOT_FOUND;
		}
	}

	if (fiData->group) 
	{
		groupEntry = getgrnam(fiData->group);
		if (groupEntry) 
		{
			setFileInfoMask |= GNOME_VFS_SET_FILE_INFO_OWNER;
			info->gid = groupEntry->gr_gid;
		} 
		else 
		{
			fprintf(stderr, "ERROR: Group '%s' not found in group database\n", fiData->group);
			gnome_vfs_file_info_unref (info); g_free(full_uri);
			return XDGVFS_ERRORCODE_USR_OR_GRP_NOT_FOUND;
		}
	}

	if (fiData->setPermsFlag) 
	{
		setFileInfoMask |= GNOME_VFS_SET_FILE_INFO_PERMISSIONS;
		info->valid_fields |= GNOME_VFS_FILE_INFO_FIELDS_PERMISSIONS;
		info->permissions = fiData->perms;
	}

	r = gnome_vfs_set_file_info(full_uri, info, setFileInfoMask);

	xdg_vfs_iterate_mainloop();

	if (r!=GNOME_VFS_OK) 
	{
		g_free(full_uri);
		gnome_vfs_file_info_unref (info);
		return print_and_translate_result(r);
	}

	xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);

	gnome_vfs_file_info_unref (info);
	g_free(full_uri);
	return XDGVFS_ERRORCODE_OK;
}

/**
*    Command: list directory
*/
int xdg_vfs_dir(const char * text_uri, int opts) 
{
	GnomeVFSResult r;
	GnomeVFSDirectoryHandle *handle;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	GnomeVFSFileInfo * info = gnome_vfs_file_info_new ();

	if (opts & XDGVFS_OPT_TAGGED) { 
		puts(XDGVFS_TAG_LS);		
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	}

	if (strcmp(full_uri, "tvolumes:")==0)
	{
		test_volume_list();
		return XDGVFS_ERRORCODE_OK;
	}

	if (strcmp(full_uri, "tdrives:")==0)
	{
		test_drive_list();
		return XDGVFS_ERRORCODE_OK;
	}
	
	if (strcmp(full_uri, "vfsroot://")==0)
	{
		_xdg_vfs_gnome_ls_vfsroot(opts & XDGVFS_OPT_TAGGED);
		return XDGVFS_ERRORCODE_OK;
	}	
	

	GnomeVFSFileInfoOptions infoOpts = GNOME_VFS_FILE_INFO_FOLLOW_LINKS | GNOME_VFS_FILE_INFO_GET_MIME_TYPE;

	if (opts & XDGVFS_OPT_SLOW_MIME_TYPE) infoOpts |= GNOME_VFS_FILE_INFO_FORCE_SLOW_MIME_TYPE;

	r=gnome_vfs_directory_open     (&handle,
                                             full_uri,
                                             infoOpts);

	xdg_vfs_iterate_mainloop();

	if (r!=GNOME_VFS_OK) 
	{
		if (r==GNOME_VFS_ERROR_CANCELLED) 
		{
			fprintf(stderr, "ERROR: Operation canceled\n");
			return XDGVFS_ERRORCODE_CANCELED;
		}
		return print_and_translate_result(r);
	}

	while(1) 
	{
		// char *entryUri;
		r=gnome_vfs_directory_read_next(handle, info);
		if (r!=GNOME_VFS_OK) 
		{
			if (r==GNOME_VFS_ERROR_EOF) break;   /* ok EOF */
			return print_and_translate_result(r);
		}
		if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_DIRENTRY);		
		
		
		GnomeVFSURI* dirUri = gnome_vfs_uri_new(full_uri);		
		GnomeVFSURI* fileUri = gnome_vfs_uri_append_path(dirUri,
                                             info->name);
		
		char * nuri = gnome_vfs_uri_to_string(fileUri,
                                             GNOME_VFS_URI_HIDE_NONE);
/*		
		if (!g_str_has_suffix (full_uri, "/"))
			entryUri = g_strconcat(full_uri, "/", info->name, NULL);
		else 
			entryUri = g_strconcat(full_uri, info->name, NULL);

		char * entryUriEscaped = gnome_vfs_make_uri_from_input(entryUri);
*/
		
		xdg_gnome_vfs_printFileInfo(stdout, nuri, info, opts);
		
		
		// xdg_vfs_ppair_str(stdout, "uri", entryUriEscaped, opts);
		//g_free(entryUriEscaped);
		//g_free(entryUri);
		gnome_vfs_uri_unref (dirUri);
		gnome_vfs_uri_unref (fileUri);
		g_free(nuri);
		
		gnome_vfs_file_info_clear (info);
		if (!(opts & XDGVFS_OPT_TAGGED)) putchar(10);
	}	
	
	gnome_vfs_directory_close(handle);
	gnome_vfs_file_info_unref (info);
	return XDGVFS_ERRORCODE_OK;

}

/**
*       Command: read file
*/
int xdg_vfs_get(const char * text_uri, int opts) 
{
	char buffer[DATA_BUF_SIZE];
	GnomeVFSFileSize bytes_read;
	int i, rr;
	int escChar = xdg_vfs_getDataEscChar();
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	GnomeVFSHandle *handle;
	GnomeVFSResult r;
	
	if (opts & XDGVFS_OPT_GET_FILEINFO) 
	{
		if (!(opts & XDGVFS_OPT_TAGGED)) 
		{
				fprintf(stderr, "ERROR: qry fileinfo in 'get' command requires tagged mode!\n");
				return XDGVFS_ERRORCODE_BAD_PARAMETER;
		}
		rr=xdg_vfs_getFileInfo(text_uri, opts);
		if (rr != 0) return rr;
	}

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_GET);
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	}
	r=gnome_vfs_open(&handle, full_uri, GNOME_VFS_OPEN_READ);
	if (r!=GNOME_VFS_OK) 
	{
		if (r==GNOME_VFS_ERROR_CANCELLED) 
		{
			fprintf(stderr, "ERROR: Operation canceled\n");
			return XDGVFS_ERRORCODE_CANCELED;
		}
		return print_and_translate_result(r);
	}

	xdg_vfs_iterate_mainloop();

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_OUT : XDGVFS_TAG_DATA_OUT);
	}

	while (1) 
	{
		r=gnome_vfs_read(handle, buffer, DATA_BUF_SIZE, &bytes_read);
		if (r!=GNOME_VFS_OK) 
		{
			if (r==GNOME_VFS_ERROR_EOF) break;   /* ok EOF */
			return print_and_translate_result(r);
		}
		char outBuf[bytes_read*2];
		int oc=0;
		for(i=0;i<bytes_read;i++) 
		{
			int c = (int)buffer[i];
			/*  escape char */
			/*
			if (c == escChar && opts & XDGVFS_OPT_DATAESCAPED) putchar(escChar); 
			putchar (c);
			*/
			if (c == escChar && opts & XDGVFS_OPT_DATAESCAPED) 
				outBuf[oc++] = escChar; 
			outBuf[oc++] = (char)c;
		}
		fwrite (outBuf, 1, oc, stdout);
	}
	if (opts & XDGVFS_OPT_DATAESCAPED) 
	{ 
		putchar(escChar); // Escape char
		putchar(10);
		//fprintf(stdout, "\n[/Data]\n");
	} else if (opts & XDGVFS_OPT_TAGGED) putchar(10);

	if (xdg_vfs_getCustomEndSeparator()) 
		fprintf(stdout, "%s\n", xdg_vfs_getCustomEndSeparator());
	gnome_vfs_close(handle);
	return XDGVFS_ERRORCODE_OK;
}

/**
*       Command: mkdir
*/ 
int xdg_vfs_mkdir(const char * text_uri, unsigned int perms, int opts) 
{
	GnomeVFSResult r;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_MKDIR);
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	}	
	r = gnome_vfs_make_directory(full_uri, perms);
	if (r!=GNOME_VFS_OK) 
	{
		return print_and_translate_result(r);
	}
	return XDGVFS_ERRORCODE_OK;
}

/**
*       Command: rmdir
*/
int xdg_vfs_rmdir(const char * text_uri, int opts) 
{
	GnomeVFSResult r;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_RMDIR);
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	}	
	r= gnome_vfs_remove_directory(full_uri);
	if (r!=GNOME_VFS_OK) 
	{
		return print_and_translate_result(r);
	}
	return XDGVFS_ERRORCODE_OK;
}

/**
*       Command: rm
*/
int xdg_vfs_rm(const char * text_uri, int opts) 
{
	GnomeVFSResult r;
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_RM);
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
	}	
	r= gnome_vfs_unlink (full_uri);
	if (r!=GNOME_VFS_OK) 
	{
		return print_and_translate_result(r);
	}
	return XDGVFS_ERRORCODE_OK;
}

/**
*       Copy/Move progress callback
*/
time_t progress_lasttime=0; 

gint    gnomeVFSXferProgressCallback (GnomeVFSXferProgressInfo *info,
                                       gpointer data) 
{
	int opts = 0;

	if (data) opts = *((int *)data);
	if (opts & XDGVFS_OPT_PROGRESS && info) 
	{
		time_t now = time(NULL);
		if ((progress_lasttime < now)) 
		{
			progress_lasttime=now;
			puts(XDGVFS_TAG_PROGRESS);
			printf("total_bytes=%d\n", info->file_size);
			printf("bytes_copied=%d\n", info->bytes_copied);
		}
	}
	info->vfs_status = GNOME_VFS_OK;
	if (xdg_vfs_chk_canceled(opts)) 
	{
		return FALSE;
	}
	return TRUE;
}

/**
*       Command: cp/mv
*/
int xdg_vfs_cp(const char * text_uri_src, const char * text_uri_target, unsigned int perms, int move_flag, int opts) 
{
	GnomeVFSResult r;
	GnomeVFSXferOptions xfer_opts = (move_flag ? GNOME_VFS_XFER_REMOVESOURCE : GNOME_VFS_XFER_DEFAULT);
	GnomeVFSXferErrorMode error_mode = GNOME_VFS_XFER_ERROR_MODE_ABORT;
	GnomeVFSXferOverwriteMode overwrite_mode = 
		(opts & XDGVFS_OPT_OVERWRITE) 
			? GNOME_VFS_XFER_OVERWRITE_MODE_REPLACE 
			: GNOME_VFS_XFER_OVERWRITE_MODE_ABORT;
	const char * full_uri_src = gnome_vfs_make_uri_from_input(text_uri_src);
	const char * full_uri_target = gnome_vfs_make_uri_from_input(text_uri_target);
	GnomeVFSURI* gfull_uri_src = gnome_vfs_uri_new(full_uri_src);
	GnomeVFSURI* gfull_uri_target = gnome_vfs_uri_new(full_uri_target);

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(move_flag ? XDGVFS_TAG_MV : XDGVFS_TAG_CP);
		xdg_vfs_ppair_str(stdout, "uri_src", full_uri_src, opts);
		xdg_vfs_ppair_str(stdout, "uri_target", full_uri_target, opts);
	}	

	r = gnome_vfs_xfer_uri           (gfull_uri_src,
                                             gfull_uri_target,
                                             xfer_opts,
                                             error_mode,
                                             overwrite_mode,
                                             gnomeVFSXferProgressCallback,
                                             &opts);
	gnome_vfs_uri_unref (gfull_uri_src);
	gnome_vfs_uri_unref (gfull_uri_target);

	if (r!=GNOME_VFS_OK) 
	{
		return print_and_translate_result(r);
	}
	return XDGVFS_ERRORCODE_OK;
}

/**
*       Command: write file
*/
int xdg_vfs_put(const char * text_uri, int perms, int opts) 
{
	char buffer[DATA_BUF_SIZE];
	GnomeVFSFileSize bytes_written;
	int i, rr, c;
	int escChar = xdg_vfs_getDataEscChar();
	char * full_uri = gnome_vfs_make_uri_from_input(text_uri);
	GnomeVFSHandle *handle;
	GnomeVFSResult r;

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_PUT);
		xdg_vfs_ppair_str(stdout, "uri", full_uri, opts);
		// puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_IN : XDGVFS_TAG_DATA_IN);
	}	

 	r=gnome_vfs_create(&handle, full_uri,
                                             GNOME_VFS_OPEN_WRITE,
                                             !(opts & XDGVFS_OPT_OVERWRITE),
                                             perms);

	xdg_vfs_iterate_mainloop();

	//r=gnome_vfs_open(&handle, text_uri, GNOME_VFS_OPEN_WRITE);
	if (r!=GNOME_VFS_OK) 
	{
		g_free(full_uri);
		if (r==GNOME_VFS_ERROR_CANCELLED) 
		{
			fprintf(stderr, "ERROR: Operation canceled\n");
			return XDGVFS_ERRORCODE_CANCELED;
		}
		return print_and_translate_result(r);
	}

	if (opts & XDGVFS_OPT_TAGGED) // wait for data!
	{ 
		puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_IN : XDGVFS_TAG_DATA_IN);
		fflush(stdout);
	}	


	while (1) 
	{
		int eofflag=0;
		int len = 0;
		for (i=0;i<DATA_BUF_SIZE;i++) 
		{  /* read chunk */
			c = getc(stdin);
			if (c==EOF) 
			{
				eofflag =1;
				break;
			}
			if (c == escChar && opts & XDGVFS_OPT_DATAESCAPED) 
			{
				int c2 = getc(stdin);
				if (c2==EOF) 
				{
					eofflag =1;
					break;
				}

				if (c2!=escChar) 
				{
					if (c2!='\n') 
					{
						g_free(full_uri);
						fprintf(stderr, "ERROR: Data escaping error\n");
						return XDGVFS_ERRORCODE_DATA_ERROR;
					}
					eofflag = 1; 
					break;
				}
			}
			
			
			buffer[i] = (unsigned char)c;
			len++;
		}
		
		/* check for errors */
		if (ferror (stdin)) 
		{
			//fprintf(stderr, "ERROR: Stdin error\n");
			g_free(full_uri);
			perror("ERROR: Stdin error\n");
			return XDGVFS_ERRORCODE_DATA_ERROR;
		}

		char * p = buffer;
		while (len>0) 
		{  /* write chunk */
			r=gnome_vfs_write(handle, p, len, &bytes_written);
			if (r!=GNOME_VFS_OK) 
			{
				// error delete file
				g_free(full_uri);
				return print_and_translate_result(r);
			}
			p += bytes_written;
			len -= bytes_written;
		}
		if(eofflag) break;
	}
	gnome_vfs_close(handle);
	g_free(full_uri);
	return XDGVFS_ERRORCODE_OK;
}

/**
*	Command: open file dialog
*/
int xdg_vfs_openfiledlg(const char * default_dir, const char * default_filename, const char * title, int opts) 
{
	GtkWidget *dialog;

	gtk_init (0, NULL);


	dialog = gtk_file_chooser_dialog_new (title ? title : XDGVFS_TEXT_OPENFILEDLG_TITLE,
    	   			      NULL,
    				      GTK_FILE_CHOOSER_ACTION_OPEN,
    				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    				      NULL);

    	/*

    	dialog = gtk_file_chooser_dialog_new ("Save File",
    	   			      NULL,
    				      GTK_FILE_CHOOSER_ACTION_SAVE,
    				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    				      NULL);

    	*/

	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER (dialog), FALSE);

	if (default_dir) gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_dir);
	//if (default_filename) gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), default_filename);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    	{
        	char *filename, *uri;

        	filename =  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

        	uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog));

		if (!(opts & XDGVFS_OPT_INSTANT_GET) || opts & XDGVFS_OPT_TAGGED) 
		{
			if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_OPENFILEDLG);
        		if (filename) fprintf(stdout, "selected_localpath=%s\n", filename);
			if (uri) fprintf(stdout, "selected_uri=%s\n", uri);
		
			if (xdg_vfs_getCustomEndSeparator()) 
				fprintf(stdout, "\n%s\n", xdg_vfs_getCustomEndSeparator());
		}

		gtk_widget_destroy (dialog);
		if (opts & XDGVFS_OPT_INSTANT_GET) return xdg_vfs_get(uri, opts);
		return XDGVFS_ERRORCODE_OK;

    	} 
	else 
	{
		gtk_widget_destroy (dialog);
		fprintf(stderr, "ERROR: Dialog canceled\n");
		return XDGVFS_ERRORCODE_CANCELED;
    	}
}

/**
*  	Command: save file dlg
*/
int xdg_vfs_savefiledlg(const char * default_dir, const char * default_filename, 
	const char * title, unsigned int perms, int opts) 
{
	GtkWidget *dialog;

	gtk_init (0, NULL);

	dialog = gtk_file_chooser_dialog_new (title ? title : XDGVFS_TEXT_SAVEFILEDLG_TITLE,
    	   			      NULL,
    				      GTK_FILE_CHOOSER_ACTION_SAVE,
    				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    				      NULL);

	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER (dialog), FALSE);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	if (default_dir) gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_dir);
	if (default_filename) gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), default_filename);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
        	char *filename, *uri;

		filename =  gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

		uri = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (dialog));

		if (!(opts & XDGVFS_OPT_INSTANT_PUT) || opts & XDGVFS_OPT_TAGGED) 
		{
			if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_SAVEFILEDLG);
        		if (filename) fprintf(stdout, "selected_localpath=%s\n", filename);
			if (uri) fprintf(stdout, "selected_uri=%s\n", uri);
		
			if (xdg_vfs_getCustomEndSeparator()) 
				fprintf(stdout, "\n%s\n", xdg_vfs_getCustomEndSeparator());
		}

		gtk_widget_destroy (dialog);
		if (opts & XDGVFS_OPT_INSTANT_PUT) return xdg_vfs_put(uri, perms, opts | XDGVFS_OPT_OVERWRITE);
		return XDGVFS_ERRORCODE_OK;
	} 
	else 
	{
		gtk_widget_destroy (dialog);
		fprintf(stderr, "ERROR: Dialog canceled\n");
		return XDGVFS_ERRORCODE_CANCELED;
	}
}

int xdg_vfs_backend(int opts) 
{
	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_BACKEND);
	}
	xdg_vfs_ppair_str(stdout, "backend_id", "gnome", opts);
	xdg_vfs_ppair_str(stdout, "system_uri", "vfsroot://", opts);
	xdg_vfs_ppair_str(stdout, "file_icon_theme", "gnome", opts);
	return XDGVFS_ERRORCODE_OK;		
}


int xdg_vfs_mount(const char * mountpoint_id, int opts) 
{
	return _xdg_vfs_gnome_mount(mountpoint_id, opts & XDGVFS_OPT_TAGGED);


}

/**
*   main()
*/
int main(int argc, char ** argv) 
{
	return xdg_vfs_main(argc, argv);
}
