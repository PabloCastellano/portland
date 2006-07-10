/*
#   xdg_vfs_kde.h
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

#ifndef XDG_VFS_KDE_H
#define XDG_VFS_KDE_H

#include <qobject.h>
#include <qeventloop.h>
#include <kio/job.h>
#include <kio/jobclasses.h>

#define XDGKDE_STATE_STARTED 1
#define XDGKDE_STATE_INFO 2
#define XDGKDE_STATE_PROGRESS 3
#define XDGKDE_STATE_DATA 4
#define XDGKDE_STATE_PUTDATA 5
#define XDGKDE_STATE_DATAEND 6
#define XDGKDE_STATE_METADATA 7
#define XDGKDE_STATE_FINISHED 8

class KioWrap : public QObject 
{
    Q_OBJECT
public:
    KioWrap();

	int errorCode;
	int opts;
	int state;
	KURL dir_uri;
	KIO::filesize_t totalsize;

	void termData();

public slots:

	/* listDir */
	
	void entries(KIO::Job *	 job, const KIO::UDSEntryList & list);

	/* getFile */

	void slotData(KIO::Job*, const QByteArray& data);

	void slotDataReq(KIO::Job*, QByteArray& data);

	void slotMimetype(KIO::Job *job,const QString & type);

	void slotResult(KIO::Job *job);

	void totalSize(KIO::Job *job, KIO::filesize_t size);

	void processedSize(KIO::Job *, KIO::filesize_t);  	

	/* getFileInfo */

	void slotStatResult(KIO::Job *job);

	/* directory monitoring */
	
	/*
	void dirWatchDirty(const QString & path);
	
	void dirWatchCreated(const QString & path);
	
	void dirWatchDeleted(const QString & path);
	*/
	
	void dirWatchChanged(const QString & path);
	
	void sockNotActivated ( int socket );


};

#endif
