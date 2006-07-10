/*
* 
* Copyright (C) 2005 Norbert Frese
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
 */

#include "xdg_vfs_client.h"
#include "xdg_vfs_forkexec.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int main(int argc, char *argv[])
{
	printf("hello world\n");

	int child_pid;
	int childs_stdin;
	int childs_stdout;
	int r;

	// /home/norbert/work/c/xdg-vfs-gnome/src/
	r = xdg_vfs_forkexec("xdg_vfs_gnome", argv, &child_pid, &childs_stdin, &childs_stdout);
	printf("r=%d\n", r);
	printf("pid=%d\n", child_pid);

	FILE * fout = fdopen (childs_stdin, "w");

	fprintf(fout, "info /\nquit\n");
	fflush(fout);

	puts("=====");

	FILE * fin = fdopen (childs_stdout, "r");
	if (!fin) perror("fopen");
	int c;

	while ((c =fgetc(fin)) != EOF) {
		putchar (':'); putchar(c);
	}

	perror("reading");
	
	int status;
	r= waitpid(child_pid, &status, 0);
	if (r==-1) perror("waitpid:");
	printf("r=%d\n", r);
}
