/*
#   xdg_vfs.h
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

#ifndef XDG_VFS_H
#define XDG_VFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define DATA_BUF_SIZE 4096
#define SHELL_MAXARGS 30

#define XDGVFS_DEFAULT_FILE_PERMISSIONS S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH 
#define XDGVFS_DEFAULT_DIR_PERMISSIONS S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IXOTH




/* cli option flags */

#define XDGVFS_OPT_TAGGED 1
#define XDGVFS_OPT_DATAESCAPED 2
#define XDGVFS_OPT_SLOW_MIME_TYPE 4
#define XDGVFS_OPT_GET_FILEINFO 8
#define XDGVFS_OPT_INSTANT_GET 16
#define XDGVFS_OPT_INSTANT_PUT 32
#define XDGVFS_OPT_OVERWRITE 64
#define XDGVFS_OPT_SHOW_METADATA 128
#define XDGVFS_OPT_INTERACTIVE_MODE 256
#define XDGVFS_OPT_PROGRESS 512
#define XDGVFS_OPT_CANCELABLE 1024
#define XDGVFS_OPT_DIVE_DE_LINKS 2048



#define XDGVFS_TEXT_SAVEFILEDLG_TITLE "Save File"
#define XDGVFS_TEXT_OPENFILEDLG_TITLE "Open File"

typedef struct {
	const char * user;
	const char * group;
	unsigned int perms;
	int setPermsFlag;
} XdgVfsSetFileInfoData;

/* implementation independent functions */

int xdg_vfs_main(int argc, char ** argv);

int xdg_vfs_getDataEscChar();

char * xdg_vfs_getCustomEndSeparator();

void xdg_vfs_ppair_str(FILE * f, const char * key, const char * value, int opts);

int xdg_vfs_chk_canceled(int opts);

/* implementation functions */

int xdg_vfs_init_desklib(int argc, char** argv);

int xdg_vfs_iterate_mainloop();

int xdg_vfs_iterate_mainloop_until_incoming_data(int fd);

/* command implementations */

int xdg_vfs_backend(int opts);

int xdg_vfs_dir(const char * text_uri, int opts);

int xdg_vfs_mount(const char * mountpoint_id, int opts);

int xdg_vfs_getFileInfo(const char * text_uri, int opts);

int xdg_vfs_setFileInfo(const char * text_uri, XdgVfsSetFileInfoData * fiData, int opts);

int xdg_vfs_get(const char * text_uri, int opts);

int xdg_vfs_put(const char * text_uri, int perms, int opts);

int xdg_vfs_cp(const char * text_uri_src, const char * text_uri_target, unsigned int perms, int move_flag, int opts);

int xdg_vfs_mkdir(const char * text_uri, unsigned int perms, int opts);

int xdg_vfs_rmdir(const char * text_uri, int opts);

int xdg_vfs_rm(const char * text_uri, int opts);

int xdg_vfs_openfiledlg(const char * default_dir, const char * default_filename, const char * title, int opts);

int xdg_vfs_savefiledlg(const char * default_dir, const char * default_filename, 
	const char * title, unsigned int perms, int opts);

int xdg_vfs_monitor(char * uri, int is_directory, int opts);

int xdg_vfs_monitor_cancel(char * uri, int opts);

int xdg_vfs_monitor_list_events(int opts);

#ifdef __cplusplus
}
#endif
#endif

