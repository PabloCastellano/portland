/*
#   xdg_vfs_forkexec.c
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

#include <unistd.h>
#include <stdio.h>
#include "xdg_vfs_forkexec.h"


int xdg_vfs_forkexec(char * filename, char ** argv, pid_t * child_pid, int * childs_stdin, int * childs_stdout) 
{

	pid_t pid;
	int child_stdin_pipe[2];
	int child_stdout_pipe[2];     


	/* create pipes */
	if (pipe (child_stdin_pipe))
	{
		fprintf (stderr, "Pipe child_stdin_pipe failed.\n");
		return 1;
	}

	if (pipe (child_stdout_pipe))
	{
		fprintf (stderr, "Pipe child_stdout_pipe failed.\n");
		return 1;
	}


	/* create the child process */
	pid = fork ();
	if (pid == (pid_t) 0)
	{
		int r;
		/* this is the child process. close other ends of the pipes. */
		close (child_stdin_pipe[1]);
		close (child_stdout_pipe[0]); 
	
		dup2 (child_stdin_pipe[0], STDIN_FILENO);
		dup2 (child_stdout_pipe[1], STDOUT_FILENO);
	
		r = execvp(filename, argv);
		if (r) {
			close (child_stdin_pipe[0]);
			close (child_stdout_pipe[1]);
			fprintf(stderr, "after fork execvp(%s, argv) ", filename);
			perror("failed");
			exit(1);
		}
		exit(0);
	}
	else if (pid < (pid_t) 0)
	{
		/* the fork failed */
		fprintf (stderr, "Fork failed.\n");
		return 1;
	}
	else
	{
		/* this is the parent process. close other ends first. */
		close (child_stdin_pipe[0]);
		close (child_stdout_pipe[1]);
		//write_to_pipe (mypipe[1]);
	
		*childs_stdin = child_stdin_pipe[1];
		*childs_stdout = child_stdout_pipe[0];
	
		*child_pid = pid;
	
		return 0;
	}
}



