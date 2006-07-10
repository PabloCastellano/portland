/*
#   xdg_vfs_kde.cpp
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

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <klocale.h>
#include <qmap.h>
#include <kapp.h>
#include <kfiledialog.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/chmodjob.h>

#include <kdirwatch.h>
#include <qsocketnotifier.h>

#include <qvaluelist.h>
#include <kfileitem.h>
#include <iostream> 

#include "xdg_vfs_common.h"
#include "xdg_vfs.h"
#include "xdg_vfs_kde.h"


/*
*   	translate error codes
*/
typedef struct 
{
	KIO::Error kioErr;
	int xdgVfsErr;
} MapKioErr;

MapKioErr mapErrArr[] = 
{
	{ KIO::ERR_CANNOT_OPEN_FOR_READING 	, XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_READING },
	{ KIO::ERR_CANNOT_OPEN_FOR_WRITING 	, XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_WRITING },
	{ KIO::ERR_CANNOT_LAUNCH_PROCESS 	, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_INTERNAL 			, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_MALFORMED_URL 		, XDGVFS_ERRORCODE_MALFORMED_URL },
	{ KIO::ERR_UNSUPPORTED_PROTOCOL 	, XDGVFS_ERRORCODE_PROTOCOL_ERROR },
	{ KIO::ERR_NO_SOURCE_PROTOCOL 		, XDGVFS_ERRORCODE_PROTOCOL_ERROR },
	{ KIO::ERR_UNSUPPORTED_ACTION 		, XDGVFS_ERRORCODE_NOT_SUPPORTED },
	{ KIO::ERR_IS_DIRECTORY 		, XDGVFS_ERRORCODE_IS_DIRECTORY },
	{ KIO::ERR_IS_FILE 			, XDGVFS_ERRORCODE_IS_FILE },
	{ KIO::ERR_DOES_NOT_EXIST 		, XDGVFS_ERRORCODE_NOT_FOUND },
	{ KIO::ERR_FILE_ALREADY_EXIST 		, XDGVFS_ERRORCODE_ALREADY_EXISTS },
	{ KIO::ERR_DIR_ALREADY_EXIST 		, XDGVFS_ERRORCODE_ALREADY_EXISTS },
	{ KIO::ERR_UNKNOWN_HOST 		, XDGVFS_ERRORCODE_HOST_NOT_FOUND},
	{ KIO::ERR_ACCESS_DENIED 		, XDGVFS_ERRORCODE_ACCESS_DENIED },
	{ KIO::ERR_WRITE_ACCESS_DENIED 		, XDGVFS_ERRORCODE_CANNOT_OPEN_FOR_WRITING },
	{ KIO::ERR_CANNOT_ENTER_DIRECTORY 	, XDGVFS_ERRORCODE_CANNOT_ENTER_DIRECTORY },
	{ KIO::ERR_PROTOCOL_IS_NOT_A_FILESYSTEM , XDGVFS_ERRORCODE_PROTOCOL_IS_NOT_A_FILESYSTEM },
	{ KIO::ERR_CYCLIC_LINK 			, XDGVFS_ERRORCODE_LOOP },
	{ KIO::ERR_USER_CANCELED 		, XDGVFS_ERRORCODE_CANCELED },
	{ KIO::ERR_CYCLIC_COPY 			, XDGVFS_ERRORCODE_LOOP },
	{ KIO::ERR_COULD_NOT_CREATE_SOCKET 	, XDGVFS_ERRORCODE_COULD_NOT_CREATE_SOCKET },
	{ KIO::ERR_COULD_NOT_CONNECT 		, XDGVFS_ERRORCODE_COULD_NOT_CONNECT },
	{ KIO::ERR_CONNECTION_BROKEN 		, XDGVFS_ERRORCODE_ERR_CONNECTION_BROKEN },
	{ KIO::ERR_NOT_FILTER_PROTOCOL 		, XDGVFS_ERRORCODE_PROTOCOL_ERROR },
	{ KIO::ERR_COULD_NOT_MOUNT 		, XDGVFS_ERRORCODE_COULD_NOT_MOUNT },
	{ KIO::ERR_COULD_NOT_UNMOUNT 		, XDGVFS_ERRORCODE_COULD_NOT_UNMOUNT },
	{ KIO::ERR_COULD_NOT_READ 		, XDGVFS_ERRORCODE_IO_ERROR },
	{ KIO::ERR_COULD_NOT_WRITE 		, XDGVFS_ERRORCODE_IO_ERROR },
	{ KIO::ERR_COULD_NOT_BIND 		, XDGVFS_ERRORCODE_SOCKET_ERROR },
	{ KIO::ERR_COULD_NOT_LISTEN 		, XDGVFS_ERRORCODE_SOCKET_ERROR },
	{ KIO::ERR_COULD_NOT_ACCEPT 		, XDGVFS_ERRORCODE_SOCKET_ERROR },
	{ KIO::ERR_COULD_NOT_LOGIN 		, XDGVFS_ERRORCODE_LOGIN_FAILED },
	{ KIO::ERR_COULD_NOT_STAT 		, XDGVFS_ERRORCODE_OPERATION_FAILED },
	{ KIO::ERR_COULD_NOT_CLOSEDIR 		, XDGVFS_ERRORCODE_OPERATION_FAILED },
	{ KIO::ERR_COULD_NOT_MKDIR 		, XDGVFS_ERRORCODE_OPERATION_FAILED },
	{ KIO::ERR_COULD_NOT_RMDIR 		, XDGVFS_ERRORCODE_OPERATION_FAILED },
	{ KIO::ERR_CANNOT_RESUME 		, XDGVFS_ERRORCODE_CANNOT_RESUME },
	{ KIO::ERR_CANNOT_RENAME 		, XDGVFS_ERRORCODE_NOT_PERMITTED },
	{ KIO::ERR_CANNOT_CHMOD 		, XDGVFS_ERRORCODE_NOT_PERMITTED },
	{ KIO::ERR_CANNOT_DELETE		, XDGVFS_ERRORCODE_NOT_PERMITTED },
	{ KIO::ERR_SLAVE_DIED 			, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_OUT_OF_MEMORY 		, XDGVFS_ERRORCODE_OUT_OF_MEMORY },
	{ KIO::ERR_UNKNOWN_PROXY_HOST 		, XDGVFS_ERRORCODE_UNKNOWN_PROXY_HOST },
	{ KIO::ERR_COULD_NOT_AUTHENTICATE 	, XDGVFS_ERRORCODE_LOGIN_FAILED },
	{ KIO::ERR_ABORTED 			, XDGVFS_ERRORCODE_ABORTED },
	{ KIO::ERR_INTERNAL_SERVER 		, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_SERVER_TIMEOUT 		, XDGVFS_ERRORCODE_TIMEOUT },
	{ KIO::ERR_SERVICE_NOT_AVAILABLE 	, XDGVFS_ERRORCODE_SERVICE_NOT_AVAILABLE },
	{ KIO::ERR_UNKNOWN 			, XDGVFS_ERRORCODE_GENERIC },
	{ KIO::ERR_UNKNOWN_INTERRUPT 		, XDGVFS_ERRORCODE_GENERIC },
	{ KIO::ERR_CANNOT_DELETE_ORIGINAL 	, XDGVFS_ERRORCODE_CANNOT_DELETE_ORIGINAL },
	{ KIO::ERR_CANNOT_DELETE_PARTIAL 	, XDGVFS_ERRORCODE_CANNOT_DELETE_PARTIAL },
	{ KIO::ERR_CANNOT_RENAME_ORIGINAL 	, XDGVFS_ERRORCODE_CANNOT_RENAME_ORIGINAL },
	{ KIO::ERR_CANNOT_RENAME_PARTIAL 	, XDGVFS_ERRORCODE_CANNOT_RENAME_PARTIAL },
	{ KIO::ERR_NEED_PASSWD 			, XDGVFS_ERRORCODE_NEED_PASSWD },
	{ KIO::ERR_CANNOT_SYMLINK 		, XDGVFS_ERRORCODE_NOT_PERMITTED },
	{ KIO::ERR_NO_CONTENT			, XDGVFS_ERRORCODE_NO_CONTENT },
	{ KIO::ERR_DISK_FULL			, XDGVFS_ERRORCODE_NO_SPACE },
	{ KIO::ERR_IDENTICAL_FILES		, XDGVFS_ERRORCODE_IDENTICAL_FILES },
	{ KIO::ERR_SLAVE_DEFINED		, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_UPGRADE_REQUIRED		, XDGVFS_ERRORCODE_INTERNAL },
	{ KIO::ERR_POST_DENIED			, XDGVFS_ERRORCODE_NOT_PERMITTED }
};

int translate_kio_error(int kioErr) 
{
	int i;
	for (i=0;i<sizeof(mapErrArr)/sizeof(MapKioErr);i++) 
	{
		/* printf(" %d %d\n", mapErrArr[i].kioErr, mapErrArr[i].xdgVfsErr); */
		if (mapErrArr[i].kioErr == kioErr) return mapErrArr[i].xdgVfsErr;
	}
	return XDGVFS_ERRORCODE_GENERIC;

}

/* ================ dir watch ================= */

KioWrap dirWatchObj;
KDirWatch * dirWatch;
QString changedlist="";

bool monitor_inited=false;

int _init_monitor()
{
	if (!monitor_inited)
	{
		dirWatch = new KDirWatch();
		QObject::connect(dirWatch,  SIGNAL(dirty(const QString &)),
        	&dirWatchObj, SLOT(dirWatchChanged(const QString &)));
		QObject::connect(dirWatch,  SIGNAL(deleted(const QString &)),
        	&dirWatchObj, SLOT(dirWatchChanged(const QString &)));
		QObject::connect(dirWatch,  SIGNAL(created(const QString &)),
        	&dirWatchObj, SLOT(dirWatchChanged(const QString &)));
        	
		monitor_inited=true;
	}
	dirWatch->addFile("/etc/mtab");
	return XDGVFS_ERRORCODE_OK;
}

int xdg_vfs_monitor(char * uri, int is_directory, int opts)
{
	_init_monitor();
	const KURL full_uri = KURL::fromPathOrURL(uri);
		
	if (is_directory) dirWatch->addDir(full_uri.pathOrURL());  	
	else dirWatch->addFile(QString(full_uri.pathOrURL()));
	return XDGVFS_ERRORCODE_OK;
}

int xdg_vfs_monitor_cancel(char * uri, int opts)
{
	_init_monitor();
	dirWatch->removeDir(QString(uri));
	dirWatch->removeFile(QString(uri));
	return XDGVFS_ERRORCODE_OK;	
}

int xdg_vfs_monitor_list_events(int opts)
{
	_init_monitor();
	fprintf(stdout, "%s", changedlist.ascii());
	changedlist = "";
	fflush(stdout);
	return XDGVFS_ERRORCODE_OK;
}

void KioWrap::dirWatchChanged(const QString & path)
{
	const KURL full_uri(path);
	fprintf(stderr, "file monitor changed event uri=%s\n", path.ascii());
	changedlist.append("* ");
	if (path.compare("/etc/mtab")==0)
	{
		changedlist.append("system:/media");
	}
	else
	{
		changedlist.append(full_uri.url().ascii());
	}
	changedlist.append("\n");	
	QApplication::eventLoop()->exitLoop();	
}
	

int incoming_flag=0;

void KioWrap::sockNotActivated( int socket )
{
	/* fprintf(stderr, "activated\n"); */
	incoming_flag=1;
	QApplication::eventLoop()->exitLoop();	
}

int xdg_vfs_iterate_mainloop_until_incoming_data(int fd)
{
	_init_monitor();
	incoming_flag=0;
	QSocketNotifier sn( fd, QSocketNotifier::Read);
	QObject::connect(&sn,  SIGNAL(activated( int)),
        	&dirWatchObj, SLOT(sockNotActivated( int )));
	
	/* fprintf(stderr, "enter ml fd=%d\n", fd); */
	QApplication::eventLoop()->exec();
	return incoming_flag;	
}

/* ================ file info ================ */

/* for desktop file KDesktopFile could be used... */

void print_mimeType(KURL & full_uri, mode_t mode, int opts, bool printIconName) 
{
	/* found in http://developer.kde.org/documentation/library/kdeqt/kde3arch/mime.html */

	KMimeType::Ptr type = KMimeType::findByURL(full_uri, mode, full_uri.isLocalFile(), false);
	/* if (type->name() == KMimeType::defaultMimeType())
		puts("mimetype="); */
	xdg_vfs_ppair_str(stdout, "mimetype", type->name().ascii(), opts);
	
	const char * iconname = type->icon(QString(),false).ascii();
	if (!iconname) iconname="unknown";
	xdg_vfs_ppair_str(stdout, "iconname", iconname, opts);
}

void print_fileInfo(FILE * f, KURL & full_uri, const KIO::UDSEntry& udsEntry, int opts) 
{
	QString filename_s = NULL;

	bool islink = false;
	bool hasUri = false;
	bool hasIcon = false;
	int mode = -1;    

	QString uds_uri;
	QString uds_linkdest;

	KIO::UDSEntry::ConstIterator it;
	for ( it = udsEntry.begin(); it != udsEntry.end(); ++it ) 
	{
		switch ((*it).m_uds) 
		{
            		case(KIO::UDS_SIZE | KIO::UDS_SIZE) :
                		fprintf(f, "size=%lli\n", (*it).m_long);
                		break;
            		case(KIO::UDS_USER | KIO::UDS_STRING) :
                		xdg_vfs_ppair_str(f, "user", (*it).m_str.ascii (), opts);
                		break;

            		case(KIO::UDS_ICON_NAME | KIO::UDS_STRING) :
                		xdg_vfs_ppair_str(f, "iconname", (*it).m_str.ascii (), opts);
                		hasIcon = true;
                		break;

            		case(KIO::UDS_GROUP | KIO::UDS_STRING) :
                		xdg_vfs_ppair_str(f, "group", (*it).m_str.ascii (), opts);
                		break;
            		case(KIO::UDS_EXTRA) :
                		break;
            		case(KIO::UDS_NAME | KIO::UDS_STRING) :
						filename_s = (*it).m_str;
                		xdg_vfs_ppair_str(f, "basename", (*it).m_str.ascii (), opts);
                		break;
					case(KIO::UDS_ACCESS | KIO::UDS_LONG) :
                		fprintf(f, "permissions=%lli\n", (*it).m_long);
                		break;
            		case(KIO::UDS_MODIFICATION_TIME | KIO::UDS_TIME) :
                		fprintf(f, "mtime=%lli\n", (*it).m_long);
                		break;
            		case(KIO::UDS_ACCESS_TIME | KIO::UDS_TIME) :
                		fprintf(f, "atime=%lli\n", (*it).m_long);
                		break;
            		case(KIO::UDS_CREATION_TIME | KIO::UDS_TIME) :
                		fprintf(f, "ctime=%lli\n", (*it).m_long);
                		break;
            		case(KIO::UDS_FILE_TYPE | KIO::UDS_LONG) :
                		mode = (*it).m_long;
                		break;
            		case(KIO::UDS_LINK_DEST| KIO::UDS_STRING) :
                		/* xdg_vfs_ppair_str(f, "linkdest", (*it).m_str.ascii (), opts); */
                		uds_linkdest = (*it).m_str;
                		islink=true;
                		break;
            		case(KIO::UDS_URL | KIO::UDS_STRING) :
                		/* xdg_vfs_ppair_str(f, "uri", (*it).m_str.ascii (), opts); */
                		uds_uri = (*it).m_str;
						hasUri=true;
                		break;
            		case(KIO::UDS_MIME_TYPE | KIO::UDS_STRING) :
                		xdg_vfs_ppair_str(f, "mimetype", (*it).m_str.ascii (), opts);
                		break;
            		case(KIO::UDS_GUESSED_MIME_TYPE | KIO::UDS_STRING) :
                		xdg_vfs_ppair_str(f, "# guessed_mimetype", (*it).m_str.ascii (), opts);
                		break;
            		case(KIO::UDS_XML_PROPERTIES) :
                		break;
        	}
    	}
    	
	if (hasUri && islink) 
		xdg_vfs_ppair_str(f, "uri", uds_linkdest.ascii (), opts);
	else if (hasUri)
		xdg_vfs_ppair_str(f, "uri", uds_uri.ascii (), opts);
	else if (islink)
		xdg_vfs_ppair_str(f, "linkdest", uds_linkdest.ascii (), opts);
    
    
	if (mode != -1) 
	{  // we always report the filetype of the target of a link)
        	char * tstr;
        	/*
	        if (islink) 
	            filetype = DA_VFS_FILE_TYPE_SYMBOLIC_LINK;
	        else {
        	*/

		if(S_ISREG(mode)) tstr = "REGULAR";
		else if(S_ISDIR(mode)) tstr ="DIRECTORY";
		else if(S_ISFIFO(mode)) tstr="FIFO";
		else if(S_ISSOCK(mode)) tstr="SOCKET";
		else if(S_ISCHR(mode)) tstr="CHARDEV";
		else if(S_ISBLK(mode)) tstr="BLOCKDEV";
		/* if(S_ISLNK(mode)) return DA_VFS_FILE_TYPE_SYMBOLIC_LINK; */
        	else tstr="UNKNOWN";

	        fprintf(f, "filetype=%s\n", tstr);

    }
	if (!hasUri) 
	{
		KURL fileUrl = KURL(full_uri);
		fileUrl.setFileName ( filename_s );

		xdg_vfs_ppair_str(f, "uri", fileUrl.url().ascii(), opts);
		print_mimeType(fileUrl, mode, opts, !hasIcon);
	}
}


/* =================== KioWrap ==================== */

KioWrap::KioWrap()
{
}

void KioWrap::termData() 
{
	if (state == XDGKDE_STATE_DATA) 
	{
		if (opts & XDGVFS_OPT_DATAESCAPED)
			putchar(xdg_vfs_getDataEscChar()); // Escape char
		if (opts & XDGVFS_OPT_TAGGED)
			putchar(10);
	} 
}

void KioWrap::entries(KIO::Job * job, const KIO::UDSEntryList & list) 
{
	KIO::UDSEntryList::ConstIterator it;
    	for ( it = list.begin(); it != list.end(); ++it ) 
	{
		if (opts & XDGVFS_OPT_TAGGED) 
			puts(XDGVFS_TAG_DIRENTRY);
		print_fileInfo(stdout, dir_uri, (*it), opts);
		if (!(opts & XDGVFS_OPT_TAGGED)) 
			putchar(10);
	}
}

void KioWrap::slotData(KIO::Job* job, const QByteArray& data)
{
	int escChar = xdg_vfs_getDataEscChar();

    	int i;
	int bytes_read;
	char * buffer;

	bytes_read = data.size();
    	buffer = (char *)data.data();

	if (bytes_read == 0) return;  // EOF!

	if (opts & XDGVFS_OPT_TAGGED && !(state == XDGKDE_STATE_DATA)) 
		{ puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_OUT : XDGVFS_TAG_DATA_OUT);}
	
	state = XDGKDE_STATE_DATA;

	for(i=0;i<bytes_read;i++) 
	{
		int c = (int)buffer[i];
		/*  escape char */
		if (c == escChar && opts & XDGVFS_OPT_DATAESCAPED) putchar(escChar); 
		putchar (c);
	}
}


void KioWrap::slotDataReq(KIO::Job* job, QByteArray& data)
{
	char buffer[DATA_BUF_SIZE];
	int i, c;
	int eofflag=0;
	int len = 0;
	int escChar = xdg_vfs_getDataEscChar();

	if (state == XDGKDE_STATE_DATAEND || state == XDGKDE_STATE_FINISHED) 
	{
		state = XDGKDE_STATE_FINISHED;
		return;
	}

	if (state != XDGKDE_STATE_PUTDATA) {
		puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_IN : XDGVFS_TAG_DATA_IN);
		fflush(stdout);
	}

	state = XDGKDE_STATE_PUTDATA;


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
					fprintf(stderr, "ERROR: Data escaping error\n");
					errorCode =  XDGVFS_ERRORCODE_DATA_ERROR;
					return;
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
		perror("ERROR: Stdin error\n");
		errorCode =  XDGVFS_ERRORCODE_DATA_ERROR;
		return;
	}

	if (len==0) state = XDGKDE_STATE_FINISHED;

	data.duplicate (buffer,len);
	/* cout << "That is it:" <<len << ": " << data; */
	if(eofflag) state = XDGKDE_STATE_DATAEND;
}

void KioWrap::slotMimetype(KIO::Job *job,const QString & type) 
{

	if (opts & XDGVFS_OPT_TAGGED && opts & XDGVFS_OPT_PROGRESS) 
	{
		termData();
		if (state != XDGKDE_STATE_PROGRESS) 
		{
			printf(XDGVFS_TAG_PROGRESS);
			putchar(10);
		}
    		printf("mimetype=%s\n", type.ascii());
		type.ascii();
		state = XDGKDE_STATE_PROGRESS;
	}
}

void KioWrap::totalSize(KIO::Job *job, KIO::filesize_t size) 
{
	totalsize = size;
/*
	if (opts & XDGVFS_OPT_TAGGED && opts & XDGVFS_OPT_PROGRESS) {
		termData();
		if (state != XDGKDE_STATE_PROGRESS) {
			printf(XDGVFS_TAG_PROGRESS);
			putchar(10);
		}
		printf("totalsize=%lli\n", size);
		state = XDGKDE_STATE_PROGRESS;
	}
*/	
        /* job->kill(true); */
}

void KioWrap::processedSize(KIO::Job * job, KIO::filesize_t  size) 
{
	if (opts & XDGVFS_OPT_TAGGED && opts & XDGVFS_OPT_PROGRESS) 
	{
		termData();
		puts(XDGVFS_TAG_PROGRESS);
		state = XDGKDE_STATE_PROGRESS;
		printf("total_bytes=%lli\n", totalsize);
		printf("bytes_copied=%lli\n", size);
		
	}
	if (xdg_vfs_chk_canceled(opts)) job->kill(false);

}

void KioWrap::slotResult(KIO::Job *job)
{
	termData();
    	if (job->error()) 
	{
		fprintf(stderr, "ERROR: KIO Job Failure #%d: %s \n---\n", job->error(), job->errorString().ascii());

		errorCode = translate_kio_error(job->error());

        	/* job->showErrorDialog(); */

    	} 
	/* else if (opts & XDGVFS_OPT_TAGGED && opts & XDGVFS_OPT_SHOW_METADATA) {  // show Metadata
		printf(XDGVFS_TAG_METADATA); printf("\n");
        	KIO::MetaData metaData = job->metaData();
        	if (!metaData.isEmpty()) { 
            		KIO::MetaData::Iterator it;
         		for ( it = metaData.begin(); it != metaData.end(); ++it ) {
                		printf("%s=%s\n", it.key().latin1(), it.data().latin1());
	                	char * value = NULL;
        	        	if (it.data().latin1())
                	  		value = strdup(it.data().latin1());
                		
            		}
            
        	}
		printf("\n");
    	}*/
	state = XDGKDE_STATE_FINISHED;
    	QApplication::eventLoop()->exitLoop();
}

void KioWrap::slotStatResult(KIO::Job* job)
{
	if (job->error()) 
	{
        	/* job->showErrorDialog(); */
        	fprintf(stderr, "ERROR: KIO StatJob Failure #%d: %s\n", job->error(), job->errorString().ascii() );
		errorCode = translate_kio_error(job->error());;

    	} 
	else 
	{ 
        	KIO::StatJob * statJob = (KIO::StatJob*) job;
		
        	const KIO::UDSEntry& udsEntry = statJob->statResult();
        	print_fileInfo(stdout, dir_uri, udsEntry, opts);
 
    	}
    	QApplication::eventLoop()->exitLoop();
}

/* ======================================================== */

/* Use uri.url().local8Bit().data() for output? */


int xdg_vfs_init_desklib(int argc, char** argv) 
{
	return 0;
}

int xdg_vfs_iterate_mainloop() 
{
	return 0;
}

/**
*	Command: ls
*/
int xdg_vfs_dir(const char * text_uri, int opts) 
{
	KioWrap w; 

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	w.dir_uri = KURL::fromPathOrURL(text_uri);
	w.dir_uri.addPath ("xxxxdummyxxxx");
	

	if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_LS);

	KIO::ListJob *job = KIO::listDir( KURL(text_uri),true,true);  	
	QObject::connect(job,  SIGNAL(entries (KIO::Job *, const KIO::UDSEntryList &)),
		&w, SLOT(entries (KIO::Job *, const KIO::UDSEntryList &)));
	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));

	QApplication::eventLoop()->exec();

	return w.errorCode;
}

/**
*	Command: info
*/
int xdg_vfs_getFileInfo(const char * text_uri, int opts) 
{
	KioWrap w; 
	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;
	w.dir_uri = KURL(text_uri);

	if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_FILEINFO);

    	KIO::StatJob *job = KIO::stat(full_uri, true);
    	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
          	&w, SLOT(slotStatResult(KIO::Job*)));

    	QApplication::eventLoop()->exec();

	//print_mimeType(text_uri, opts);

	return w.errorCode;
}

/**
*	Command: setattrs
*/
int xdg_vfs_setFileInfo(const char * text_uri, XdgVfsSetFileInfoData * fiData, int opts) 
{
	KioWrap w; 
	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_SETATTRS);
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
	}

	KFileItemList  lstItems;
	lstItems.setAutoDelete( TRUE );
	const KFileItem * item = new KFileItem (KURL(full_uri), QString(NULL), (mode_t)0);


	lstItems.append (item);

	KIO::Job * job = KIO::chmod(lstItems,
		fiData->perms,
		fiData->setPermsFlag ? 7777 : 0,
		QString(fiData->user),
		QString(fiData->group),
		false,
		true
	);
/*
 	SimpleJob * job = KIO::chmod(   	const KURL &   	 url,
		int  	permissions
	)  	
*/
    	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
          	&w, SLOT(slotResult(KIO::Job*)));

    	QApplication::eventLoop()->exec();	

	return w.errorCode;
}

/**
*	Command: cp/mv
*/
int xdg_vfs_cp(const char * text_uri_src, const char * text_uri_target, unsigned int perms, int move_flag, int opts) 
{
	KioWrap w; 
	const KURL full_uri_src = KURL::fromPathOrURL(text_uri_src);
	const KURL full_uri_target = KURL::fromPathOrURL(text_uri_target);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;
	
	if (opts & XDGVFS_OPT_TAGGED) 
	{
		puts(move_flag ? XDGVFS_TAG_MV : XDGVFS_TAG_CP);
		xdg_vfs_ppair_str(stdout, "uri_src", full_uri_src.url().ascii(), opts);
		xdg_vfs_ppair_str(stdout, "uri_target", full_uri_target.url().ascii(), opts);
	}

	KIO::CopyJob * job;

	if (opts & XDGVFS_OPT_OVERWRITE) 
	{
		if (move_flag)
			job =  KIO::move(full_uri_src,
				full_uri_target,
				true
			);
		else 
			job =  KIO::copy(full_uri_src,
				full_uri_target,
				true
			);
	} else 
	{

		if (move_flag)
			job =  KIO::moveAs(full_uri_src,
				full_uri_target,
				true
			);
		else 
			job =  KIO::copyAs(full_uri_src,
				full_uri_target,
				true
			);
	}

	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));
	QObject::connect(job,  SIGNAL(processedSize(KIO::Job *, KIO::filesize_t)),
		&w, SLOT(processedSize(KIO::Job *, KIO::filesize_t)));
	QObject::connect(job, SIGNAL(totalSize(KIO::Job *, KIO::filesize_t )),
          	&w, SLOT(totalSize(KIO::Job *, KIO::filesize_t)));

	QApplication::eventLoop()->exec();
	
	return w.errorCode;
}

/**
*	Command: mkdir
*/
int xdg_vfs_mkdir(const char * text_uri, unsigned int perms, int opts) 
{
	KioWrap w; 
	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_MKDIR);
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
	}

	KIO::SimpleJob * job = KIO::mkdir(full_uri, perms);
	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));

	QApplication::eventLoop()->exec();  	
	
	return w.errorCode;
}

/**
*	Command: rmdir
*/
int xdg_vfs_rmdir(const char * text_uri, int opts) 
{
	KioWrap w; 
	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_RMDIR);
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
	}

	KIO::SimpleJob * job = KIO::rmdir(full_uri);
	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));

	QApplication::eventLoop()->exec();  	
	
	return w.errorCode;
}

/**
*	Command: rm
*/
int xdg_vfs_rm(const char * text_uri, int opts) 
{
	KioWrap w; 

	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_RM);
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
	}

	KIO::DeleteJob * job = KIO::del(full_uri,
		false,
		true
	);
	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));

	QApplication::eventLoop()->exec();  	
	
	return w.errorCode;
}

/**
*	Command: read file
*/
int xdg_vfs_get(const char * text_uri, int opts) 
{
	const KURL full_uri = KURL::fromPathOrURL(text_uri);

	if (opts & XDGVFS_OPT_GET_FILEINFO) 
	{
		int rr;
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
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
	}

	KioWrap w; 

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	KIO::TransferJob *job = KIO::get(full_uri);
    	QObject::connect(job,  SIGNAL(data(KIO::Job*, const QByteArray&)),
          	&w, SLOT(slotData(KIO::Job*, const QByteArray&)));
    	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));
    	QObject::connect(job,  SIGNAL(mimetype(KIO::Job *,const QString &)),
          	&w, SLOT(slotMimetype(KIO::Job *,const QString &)));
    	

    	QApplication::eventLoop()->exec();
	return w.errorCode;
}

/**
*	Command: write file
*/
int xdg_vfs_put(const char * text_uri, int perms, int opts) 
{
	const KURL full_uri = KURL::fromPathOrURL(text_uri);
	KioWrap w; 

	w.errorCode = XDGVFS_ERRORCODE_OK;
	w.opts = opts;
	w.state = XDGKDE_STATE_STARTED;

	if (opts & XDGVFS_OPT_TAGGED) 
	{
		puts(XDGVFS_TAG_PUT); 
		xdg_vfs_ppair_str(stdout, "uri", full_uri.url().ascii(), opts);
		// puts(opts & XDGVFS_OPT_DATAESCAPED ? XDGVFS_TAG_ESCAPEDDATA_IN : XDGVFS_TAG_DATA_IN);
	}

	KIO::TransferJob * job = KIO::put(
		full_uri,
		XDGVFS_DEFAULT_FILE_PERMISSIONS,
		(opts & XDGVFS_OPT_OVERWRITE),
		false,
		false
	);
    	QObject::connect(job,  SIGNAL(result(KIO::Job*)),
        	&w, SLOT(slotResult(KIO::Job*)));
	QObject::connect(job, SIGNAL(dataReq(KIO::Job *,QByteArray &)),
		&w, SLOT(slotDataReq(KIO::Job *,QByteArray &)));
    	QApplication::eventLoop()->exec();
	return w.errorCode;
}

/**
*	Command: open file dialog
*/
int xdg_vfs_openfiledlg(const char * default_dir, const char * default_filename, const char * title, int opts) 
{
        KURL filename=KFileDialog::getOpenURL(QString (default_dir), "*", NULL );
        if (filename.url()!=NULL) 
	{
		if (!(opts & XDGVFS_OPT_INSTANT_GET) || opts & XDGVFS_OPT_TAGGED) 
		{
			if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_OPENFILEDLG);
        		if (filename.isLocalFile ()) 
				fprintf(stdout, "selected_localpath=%s/%s\n", 
					filename.directory().ascii(), filename.fileName().ascii());
			fprintf(stdout, "selected_uri=%s\n", filename.url().ascii());
			//fprintf(stdout, "\n");
			if (xdg_vfs_getCustomEndSeparator()) 
				fprintf(stdout, "\n%s\n", xdg_vfs_getCustomEndSeparator());
		}
		
		if (opts & XDGVFS_OPT_INSTANT_GET) return xdg_vfs_get(filename.url().ascii(), opts);
		return 0;
    	} else 
	{
		fprintf(stderr, "ERROR: Dialog canceled\n");
		return XDGVFS_ERRORCODE_CANCELED;
    	}
}

/**
*	Command: save file dialog
*/
int xdg_vfs_savefiledlg(const char * default_dir, const char * default_filename, 
	const char * title, unsigned int perms, int opts) 
{
	QString daf = QString (default_dir) + QString (default_filename);

    	KURL filename=KFileDialog::getSaveURL(daf, "*", NULL );
        if (filename.url() != NULL) 
	{
		if (!(opts & XDGVFS_OPT_INSTANT_GET) || opts & XDGVFS_OPT_TAGGED) 
		{
			if (opts & XDGVFS_OPT_TAGGED) puts(XDGVFS_TAG_SAVEFILEDLG);
			if (filename.isLocalFile ()) 
				fprintf(stdout, "selected_localpath=%s/%s\n", 
					filename.directory().ascii(), filename.fileName().ascii());
			fprintf(stdout, "selected_uri=%s\n", filename.url().ascii());
			//fprintf(stdout, "\n");
			if (xdg_vfs_getCustomEndSeparator()) 
			fprintf(stdout, "\n%s\n", xdg_vfs_getCustomEndSeparator());
		}

		if (opts & XDGVFS_OPT_INSTANT_PUT) 
			return xdg_vfs_put(filename.url().ascii(), perms, opts | XDGVFS_OPT_OVERWRITE);
		return 0;

    	} 
	else 
	{
		fprintf(stderr, "ERROR: Dialog canceled\n");
		return XDGVFS_ERRORCODE_CANCELED;
    	}
}

int xdg_vfs_mount(const char * mountpoint_id, int opts)
{
	return XDGVFS_ERRORCODE_NOT_SUPPORTED;
}
int xdg_vfs_backend(int opts) 
{
	if (opts & XDGVFS_OPT_TAGGED) 
	{ 
		puts(XDGVFS_TAG_BACKEND);
	}
	xdg_vfs_ppair_str(stdout, "backend_id", "kde", opts);
	xdg_vfs_ppair_str(stdout, "system_uri", "system:/", opts);
	xdg_vfs_ppair_str(stdout, "file_icon_theme", "crystalsvg", opts);
	return XDGVFS_ERRORCODE_OK;		
}
/**
*   main() 
*/
int main( int argc, char* argv[] )
{
    	KCmdLineArgs::init( 1, argv, "xdg_vfs_kde", I18N_NOOP( "xdg_vfs_kde" ), I18N_NOOP( "xdg_vfs_kde" ), "0.1" ); 

    	KApplication app;

	return xdg_vfs_main(argc, argv);
}

