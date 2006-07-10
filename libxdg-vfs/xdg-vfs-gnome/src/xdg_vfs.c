/*
#   xdg_vfs.c
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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>

#include "xdg_vfs_common.h"
#include "xdg_vfs.h"

char * end_seperator;
int data_esc_char= DEFAULT_DATA_ESC_CHAR;
int xdgvfs_file_permissions = XDGVFS_DEFAULT_FILE_PERMISSIONS;
int xdgvfs_dir_permissions = XDGVFS_DEFAULT_DIR_PERMISSIONS;
int debug_mode = 1;

int xdg_vfs_getDataEscChar() 
{
	return data_esc_char;
}

char * xdg_vfs_getCustomEndSeparator() 
{
	return end_seperator;
}

/**
*	print a key/value pair 
*	(value is a string)
*/
void xdg_vfs_ppair_str(FILE * f, const char * key, const char * value, int opts) 
{
	fprintf(f,key);
	fprintf(f,"=");
	if (value) fprintf(f,"%s\n", value);
	else fprintf(f, "\n");
}

/**
*	look if there is a cancel character on stdin
*/
int xdg_vfs_chk_stdin(int wait_secs) 
{
       	/* check for canceled */
       	int filedes = fileno (stdin);
       	int r;
       	fd_set set;
       	struct timeval timeout;
     

       	/* Initialize the file descriptor set. */
       	FD_ZERO (&set);
       	FD_SET (filedes, &set);
     
       	/* Initialize the timeout data structure. */
       	timeout.tv_sec = wait_secs;
       	timeout.tv_usec = 0;
     
       	/* select returns 0 if timeout, 1 if input available, -1 if error. */
       	r= select (FD_SETSIZE, &set, NULL, NULL, &timeout);
       	if (r==0) return 0;
       	if (r==-1) 
	{
		fprintf(stderr, "WARNING: Error when checking stdin for cancel character\n");
       		return -1; /* error! */
	}
	return 1;
}

int xdg_vfs_chk_canceled(int opts) {

	if (!(opts & XDGVFS_OPT_CANCELABLE)) return 0;

	int r = xdg_vfs_chk_stdin(0)==1;

	if (r==1)
	{
		int c = getc(stdin);
		if (c==DEFAULT_CANCEL_CHAR) return 1;
		fprintf(stderr, "WARNING: Bad cancel character!\n");
		return 1;
       	}
	return r;
}

int empty_stdin() { /* not used */

	/* fclean(stdin); */
	while (xdg_vfs_chk_stdin(0)==1) {
		char buf[10];
		read(stdin, buf, 10);
	}
}


/**
*   	print usage
*/
int pusage(int opts) 
{
	if (!(opts & XDGVFS_OPT_INTERACTIVE_MODE))
	fprintf(stderr, "Usage: xdg_vfs [command] [options] [filenames]\n");
	else 
	fprintf(stderr, "Usage: [command] [options] [filename]\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Commands: \n");

	fprintf(stderr, "      backend                    : print backend infos\n");
	fprintf(stderr, "      get  [uri]                 : remote file->stdout\n");
	fprintf(stderr, "      put  [uri]                 : stdin->create remote file\n");
	fprintf(stderr, "      info [uri]                 : stat a remote file\n");
	fprintf(stderr, "      setattrs [uri]             : set file attributes\n");
	fprintf(stderr, "      cp  [uri] [uri]            : copy file\n");
	fprintf(stderr, "      mv  [uri] [uri]            : move/rename file\n");
	fprintf(stderr, "      rm  [uri]                  : remove file\n");	
	fprintf(stderr, "      mkdir  [uri]               : make directory\n");
	fprintf(stderr, "      rmdir  [uri]               : remove directory\n");
	fprintf(stderr, "      ls   [uri]                 : list directory\n");
	fprintf(stderr, "      mount [id]                 : mount\n");
	fprintf(stderr, "      openfiledlg [uri]          : show file-save dialog\n");
	fprintf(stderr, "      savefiledlg [uri] [fname]  : show file-open dialog\n");
	if (!(opts & XDGVFS_OPT_INTERACTIVE_MODE))
	fprintf(stderr, "      shell                      : interactive mode\n");
	else 
	fprintf(stderr, "      quit                       : exit interactive mode\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  Options: \n");
	fprintf(stderr, "      -t, --tagged               : tagged mode (file-data should be escaped)\n");
	fprintf(stderr, "      -i, --qry-fileinfo         : qry file-info when retrieving a file\n");
	/* fprintf(stderr, "      --show-metadata            : show metadata when retrieving a file\n"); */
	fprintf(stderr, "                                   requires tagged mode!\n");
	fprintf(stderr, "      --dive-de-links            : dive into desktop-entry links (if folders)\n");
	fprintf(stderr, "      -m, --slow-mime            : more accurate mime-type detection (slow)\n");
	fprintf(stderr, "      -e, --escape-data          : escape file data (for end of data detection)\n");
	fprintf(stderr, "      -eXXX, --escape-data=XXX   : set and use a user defined escape character\n");
	fprintf(stderr, "                                   XXX = ascii code!\n");
	fprintf(stderr, "      --cancelable               : allow sending cancel character to stdin\n");
	fprintf(stderr, "      --user=                    : setattrs: set user\n");
	fprintf(stderr, "      --group=                   : setattrs: set group\n");
	fprintf(stderr, "      --permissions=             : set permissions when creating a file or dir\n");
	fprintf(stderr, "                                 : or in setattrs\n");
	/* fprintf(stderr, "      --end-seperator=           : add a custom seperator inbetween blocks\n"); */
	fprintf(stderr, "      --overwrite                : allow overwriting files (put)\n");
	fprintf(stderr, "      --instant-get              : openfiledilog instant get file->stdout\n");
	fprintf(stderr, "      --instant-put              : savefiledilog instant put file from stdin\n");
	fprintf(stderr, "      --title=                   : set dialog title\n");
	fprintf(stderr, "      --progress                 : show progress when copying or moving a file\n");
	fprintf(stderr, "\n");
	return XDGVFS_ERRORCODE_BAD_PARAMETER ;
}

/**
*	count ordinary args
*/
int countargs(int argc, char ** argv) 
{
	int i;
	int ctr=0;
	for (i=0;i<argc;i++) 
	{
		if (argv[i][0] != '-') 
		{
			ctr++;
		}
	}
	return ctr;
}


/**
*  	get n'th arg ignoring options
*/
char * getarg(int argc, char ** argv, int nr) 
{
	int i;
	int ctr=0;
	for (i=0;i<argc;i++) 
	{
		if (argv[i][0] != '-') 
		{
			if (ctr++==nr) return argv[i];
		}
	}
	return NULL;
}

/**
*  	get certain option value
*/
char * getoptarg(int argc, char ** argv, const char * optname) 
{
	int i;
	int ctr=0;
	for (i=0;i<argc;i++) 
	{
		if (strncmp (argv[i], optname, strlen(optname))==0) return argv[i]+strlen(optname);
	}
	return NULL;
}

/**
*  	execute commands
*/
int docommand(int argc, char ** argv, int globopts) 
{
	char * command; char * s; char * title;
	int cargs = countargs(argc, argv);
	int opts = globopts;
	int r;

	command = getarg(argc,argv, 1);

	if (!command) return pusage(opts);
	

	if (getoptarg(argc,argv, "-t") || getoptarg(argc,argv, "--tagged"))
		opts |= XDGVFS_OPT_TAGGED;
	if (getoptarg(argc,argv, "-e") || getoptarg(argc,argv, "--escape-data")) 
	{
		opts |= XDGVFS_OPT_DATAESCAPED;
		s = getoptarg(argc,argv, "-e");
		if (s && strlen(s)>0) data_esc_char = atoi(s);
		s = getoptarg(argc,argv, "--escape-data=");
		if (s && strlen(s)>0) data_esc_char = atoi(s);
	}

	if (getoptarg(argc,argv, "--permissions=")) 
	{
		s = getoptarg(argc,argv, "--permissions=");
		if (s && strlen(s)>0) 
		{ 
			int perms= atoi(s); 
			xdgvfs_file_permissions = perms;
			xdgvfs_dir_permissions = perms;
		}
	}

	if (getoptarg(argc,argv, "-m") || getoptarg(argc,argv, "--slow-mime"))
		opts |= XDGVFS_OPT_SLOW_MIME_TYPE;
	if (getoptarg(argc,argv, "--overwrite"))
		opts |= XDGVFS_OPT_OVERWRITE;

	if (getoptarg(argc,argv, "--cancelable"))
		opts |= XDGVFS_OPT_CANCELABLE;

	if (getoptarg(argc,argv, "--instant-get"))
		opts |= XDGVFS_OPT_INSTANT_GET;

	if (getoptarg(argc,argv, "--instant-put"))
		opts |= XDGVFS_OPT_INSTANT_PUT;
		
	if (getoptarg(argc,argv, "--dive-de-links"))
		opts |= XDGVFS_OPT_DIVE_DE_LINKS;		


	if (getoptarg(argc,argv, "-i") || getoptarg(argc,argv, "--qry-fileinfo")) 
		opts |= XDGVFS_OPT_GET_FILEINFO;

	if (getoptarg(argc,argv, "--show-metadata"))
		opts |= XDGVFS_OPT_SHOW_METADATA;
	if (getoptarg(argc,argv, "-p") || getoptarg(argc,argv, "--progress"))
		opts |= XDGVFS_OPT_PROGRESS;

	title=getoptarg(argc,argv, "--title=");

		
	/* printf("debu=%d %d\n", opts, countargs(argc, argv)); */

	/* get command? */
	if (cargs == 2 && strcmp(command, "backend")==0) 
	{
		return xdg_vfs_backend(opts);
	}	
	else if (cargs == 3 && strcmp(command, "get")==0) 
	{
		return xdg_vfs_get(getarg(argc,argv, 2), opts);
	/* info command? */
	} 
	else if (cargs == 3 && strcmp(command, "info")==0) 
	{
		return xdg_vfs_getFileInfo(getarg(argc,argv, 2), opts);
	} 
	else if (cargs == 3 && strcmp(command, "setattrs")==0) 
	{
		XdgVfsSetFileInfoData fiData = { 0,0,0,0 };
		fiData.user = getoptarg(argc,argv, "--user=");
		fiData.group = getoptarg(argc,argv, "--group=");
		if (getoptarg(argc,argv, "--permissions=")) 
		{
			s = getoptarg(argc,argv, "--permissions=");
			if (s && strlen(s)>0) 
			{ 
				fiData.perms=atoi(s); 
				fiData.setPermsFlag = 1;
			}
		}
		return xdg_vfs_setFileInfo(getarg(argc,argv, 2), &fiData, opts);
	/* ls caommand? */
	} 
	else if (cargs == 3 && strcmp(command, "ls")==0) 
	{
		return xdg_vfs_dir(getarg(argc,argv, 2), opts);
	/* mount command? */
	} 
	else if (cargs == 3 && strcmp(command, "mount")==0) 
	{
		return xdg_vfs_mount(getarg(argc,argv, 2), opts);
	/* mkdir command? */
	}
	else if (cargs == 3 && strcmp(command, "mkdir")==0) 
	{
		return xdg_vfs_mkdir(getarg(argc,argv, 2), xdgvfs_dir_permissions, opts);
	} 
	else if (cargs == 3 && strcmp(command, "rmdir")==0) 
	{
		return xdg_vfs_rmdir(getarg(argc,argv, 2), opts);
	} 
	else if (cargs == 3 && strcmp(command, "put")==0) 
	{
		return xdg_vfs_put(getarg(argc,argv, 2), xdgvfs_file_permissions, opts);
	} 
	else if (cargs == 3 && strcmp(command, "rm")==0) 
	{
		return xdg_vfs_rm(getarg(argc,argv, 2), opts);
	} 
	else if (cargs == 3 && strcmp(command, "monfile")==0) 
	{
		return xdg_vfs_monitor(getarg(argc,argv, 2), 0, opts);
	}
	else if (cargs == 3 && strcmp(command, "mondir")==0) 
	{
		return xdg_vfs_monitor(getarg(argc,argv, 2), 1, opts);
	}
	else if (cargs == 3 && strcmp(command, "rmmon")==0) 
	{
		return xdg_vfs_monitor_cancel(getarg(argc,argv, 2), opts);
	}
	else if (cargs >= 2  && strcmp(command, "openfiledlg")==0) 
	{
		return xdg_vfs_openfiledlg(getarg(argc,argv, 2), NULL, title, opts);
	} 
	else if (cargs >= 2  && strcmp(command, "savefiledlg")==0) 
	{
		return xdg_vfs_savefiledlg(getarg(argc,argv, 2),getarg(argc,argv, 3), 
			title, xdgvfs_file_permissions, opts);
	} 
	else if (cargs == 4  && strcmp(command, "cp")==0) 
	{
		return xdg_vfs_cp(getarg(argc,argv, 2),getarg(argc,argv, 3), xdgvfs_file_permissions, 0, opts);
	} 
	else if (cargs == 4  && strcmp(command, "mv")==0) 
	{
		return xdg_vfs_cp(getarg(argc,argv, 2),getarg(argc,argv, 3), xdgvfs_file_permissions, 1, opts);
	} 
	else 
	{
		return pusage(opts);
	}
	return 1;
}

/**
*  main() function
*/
int xdg_vfs_main(int argc, char ** argv) 
{
	if (argc < 2) 
	{
		return pusage(0);
	}

	xdg_vfs_init_desklib(argc, argv);

	/* SHELL mode */
	if (countargs(argc,argv)==2 && strcmp(getarg(argc,argv, 1), "shell")==0) 
	{
		while(1) 
		{
			char * lineptr = NULL;
			ssize_t len;
			size_t n = 0;

			char ** av = calloc(SHELL_MAXARGS, sizeof(char*));
			av[0] = "shell";
			int ac = 1;
			
			int rcode;

			fprintf(stdout, "> ");
			fflush(stdout);
			xdg_vfs_monitor_list_events(0);
			while (!xdg_vfs_iterate_mainloop_until_incoming_data(fileno(stdin)))
			{
				xdg_vfs_monitor_list_events(0);
			}
			len=getline (&lineptr, &n, stdin);

			if (len<0) {
				if (len=EOF) {
					fprintf(stderr, "xdg-vfs shell mode: stdin closed -> exit\n");
					exit(XDGVFS_ERRORCODE_IO_ERROR);
				}
				perror("xdg-vfs shell mode: stdin problem -> exit");
				exit(XDGVFS_ERRORCODE_IO_ERROR);
			}

			/* printf("lineptr=%s len=%d\n", lineptr, len); */
			/* while(ac < SHELL_MAXARGS && (av[ac]=strtok(ac==1 ? lineptr : NULL, " \n")) ) 
				ac++; */

			{
				char * lout = calloc(strlen(lineptr)+1,1);
				int c=0;
				int co=0;
				av[ac++] = lout;  /* first arg */
				int parsing_error=0;
				while(lineptr[c]!='\0') /* read shell line into arg array */
				{
					if (lineptr[c] == '"') /* quoted strings */
					{
						c++;
						while(lineptr[c]!='\0') 
						{
							if (lineptr[c] == '\\')
							{
								
								c++;
								if (lineptr[c]=='\\' || lineptr[c]=='"') 
								{
									lout[co++] = lineptr[c++];
									continue;
								} else 
								{
									fprintf(stderr, "ERROR: Only '\\' or '\"' are allowed to be escaped  with '\\' inside double quotes in shell mode!!!\n");
									parsing_error=1;
								}
								
							} else 
							{
								if (lineptr[c] == '"') 
								{
									c++;
									break;
								}
								lout[co++] = lineptr[c++];
							}
						}

					}
					
					if (lineptr[c] == ' ') 
					{
						lout[co] = '\0';
						av[ac++] = lout+co+1;
						co++;
						if (ac >= SHELL_MAXARGS) return XDGVFS_ERRORCODE_BAD_PARAMETER;
					} else if (lineptr[c] == '\n') 
					{
						/* skip newline */
					}
					else 
					{
						lout[co++] = lineptr[c];
					}
					c++;
				}

				/* if (debug_mode) fprintf(stderr,"av= %s|%s|%s|%s|%s|%s n=%d\n", av[0], av[1], av[2], av[3], av[4],av[5],ac); */

				if (av[1] && strcmp(av[1], "quit")==0) 
				{
					fprintf(stderr, "BYE!\n");
					return 0;
				}

				if (!parsing_error) 
				{
					rcode = docommand(ac,av, XDGVFS_OPT_INTERACTIVE_MODE | XDGVFS_OPT_TAGGED);
					if (rcode==0) fprintf(stdout, "%s\n", XDGVFS_TAG_DONE);
					else fprintf(stdout, "%s %d\n", XDGVFS_TAG_FAILED, rcode);
				} 
				else 
				{
					fprintf(stdout, "%s %d\n", XDGVFS_TAG_FAILED, XDGVFS_ERRORCODE_BAD_PARAMETER);
				}

				free(lineptr);
				free(av);
				free(lout);
				xdg_vfs_iterate_mainloop();
			}
		}
	/* single command mode */	
	} 
	else 
	{
		return docommand(argc,argv, 0);
	}

}



