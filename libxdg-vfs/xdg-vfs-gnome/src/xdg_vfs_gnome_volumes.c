/*
#   xdg_vfs_gnome_volumes.c
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

#include <glib.h>
#include <libgnomevfs/gnome-vfs.h>

#include "xdg_vfs_common.h"
#include "xdg_vfs.h"

#define DEBUG_VOLUMES 1

/* =============== print entries =============== */


void _print_filesystem()
{
	fprintf(stdout,
		"basename=Filesystem\n"
		"uri=file:///\n"
		"filetype=DIRECTORY\n"
		"iconname=gnome-dev-harddisk\n"
		);
}

void _print_network()
{
	fprintf(stdout,
		"basename=Network\n"
		"uri=network://\n"
		"filetype=DIRECTORY\n"
		"iconname=gnome-fs-network\n"
		);
}

void _print_home()
{
	char * home_uri = gnome_vfs_get_uri_from_local_path (g_get_home_dir ());
	fprintf(stdout,
		"basename=Home\n"
		"uri=%s\n"
		"filetype=DIRECTORY\n"
		"iconname=gnome-fs-home\n"
		, home_uri
	);
}


int _print_volume(GnomeVFSVolume *volume) 
{
	char* activation_uri = gnome_vfs_volume_get_activation_uri
                                        (volume);
	char* display_name = gnome_vfs_volume_get_display_name
                                        (volume);
	GnomeVFSDeviceType device_type = gnome_vfs_volume_get_device_type
                                        (volume);                                            
                                        
	char* filesystem_type = gnome_vfs_volume_get_filesystem_type
                                        (volume);
	GnomeVFSVolumeType volume_type = gnome_vfs_volume_get_volume_type
                                            (volume);
                              
	char* icon = gnome_vfs_volume_get_icon(volume);                                  
                                        
	char* device_path =  gnome_vfs_volume_get_device_path
                                        (volume);     

    gboolean is_mounted =  gnome_vfs_volume_is_mounted(volume);

	gboolean user_visible = gnome_vfs_volume_is_user_visible
                                        (volume);

	gboolean read_only = gnome_vfs_volume_is_read_only (volume);

	if (DEBUG_VOLUMES)
	{
   		fprintf(stdout, "# (VOLUME)\n");
		fprintf(stdout, "# activation_uri=%s\n", activation_uri);
  		fprintf(stdout, "# display_name=%s\n", display_name);
		fprintf(stdout, "# icon=%s\n", icon);        
		fprintf(stdout, "# volume_type=%d\n", volume_type);    
		fprintf(stdout, "# device_type=%d\n", device_type);
		fprintf(stdout, "# filesystem_type=%s\n", filesystem_type);
		fprintf(stdout, "# device_path=%s\n", device_path);
		fprintf(stdout, "# is_mounted=%d\n", is_mounted);
		fprintf(stdout, "# user_visible=%d\n", user_visible);        
		fprintf(stdout, "# read_only=%d\n", read_only);                
	}
    
	if (is_mounted)
	{
		fprintf(stdout, "uri=%s\n", activation_uri);
		fprintf(stdout, "is_mounted=1\n");
		/* fprintf(stdout, "filetype=DIRECTORY\n"); */
	} 
	else
 		fprintf(stdout, "is_mounted=0\n");
	{
		GnomeVFSDrive * drive = gnome_vfs_volume_get_drive(volume);
		if (drive)
			fprintf(stdout, "mountpoint_id=%d\n", gnome_vfs_drive_get_id (drive));		
		gnome_vfs_drive_unref(drive);
		
		fprintf(stdout, "filetype=VFSMOUNTPOINT\n");
	}
   	fprintf(stdout, "basename=%s\n", display_name);
    fprintf(stdout, "iconname=%s\n", icon);        
    	
	fprintf(stdout, "\n");

	g_free(activation_uri);
	g_free(display_name);
	g_free(filesystem_type);
	g_free(icon);
	g_free(device_path);
	
}

int _print_drive(GnomeVFSDrive *drive) 
{

	char * icon= gnome_vfs_drive_get_icon(drive);
	int drive_id=gnome_vfs_drive_get_id (drive);
	gboolean is_mounted= gnome_vfs_drive_is_mounted(drive);
	gboolean is_user_visible=gnome_vfs_drive_is_user_visible (drive);
	char * device_path=gnome_vfs_drive_get_device_path (drive);
	char * activation_uri=gnome_vfs_drive_get_activation_uri(drive);
	char * display_name=gnome_vfs_drive_get_display_name(drive);

	if (DEBUG_VOLUMES)
	{
		fprintf(stdout, "# (DRIVE)\n");
		fprintf(stdout, "# drive_icon=%s\n", icon);
		fprintf(stdout, "# drive_id=%d\n", drive_id);
		fprintf(stdout, "# is_mounted=%d\n", is_mounted);
		fprintf(stdout, "# is_user_visible=%d\n", is_user_visible);
		fprintf(stdout, "# device_path=%s\n", device_path);
		fprintf(stdout, "# activation_uri=%s\n", activation_uri);
		fprintf(stdout, "# display_name=%s\n", display_name);
	}
	fprintf(stdout, "iconname=%s\n", icon); 
	fprintf(stdout, "basename=%s\n", display_name);
	if (is_mounted)
		fprintf(stdout, "uri=%s\n", activation_uri);
	fprintf(stdout, "mountpoint_id=%d\n", drive_id);
	fprintf(stdout, "is_mounted=%d\n", is_mounted);
	
	fprintf(stdout, "filetype=VFSMOUNTPOINT\n");

	fprintf(stdout, "\n");
	
	g_free(icon);
	g_free(device_path);
	g_free(activation_uri);
	g_free(display_name);
	
}


/* =================== implementation ================ */


GList * _xdg_vfs_gnome_vfsroot_list() {

	GList* entries=NULL;

	GnomeVFSVolumeMonitor * volume_monitor = gnome_vfs_get_volume_monitor();
	
	GList* drive_list = gnome_vfs_volume_monitor_get_connected_drives
                                            (volume_monitor);	

	

	GList *current_drive;
                                            
    for (current_drive = drive_list; current_drive != NULL; current_drive = current_drive->next) 
    {  
    	
		GnomeVFSDrive *drive = GNOME_VFS_DRIVE(current_drive->data);

		//_print_drive(drive);
		//printf("drive\n");

		char* drive_activation_uri = gnome_vfs_drive_get_activation_uri(drive);

		/* don't list root filesystem, because it's included by hand */
		if (drive_activation_uri && strcmp(drive_activation_uri, "file:///")==0) 
		{
			g_free(drive_activation_uri);
			gnome_vfs_drive_unref(drive);
			continue; 
		}



    	GList* vol_list = gnome_vfs_drive_get_mounted_volumes(drive);
    
		GList *current_vol;
		int visible_volumes_count = 0; // count the visible volumes on this drive
		
		for (current_vol = vol_list; current_vol != NULL; current_vol = current_vol->next) 
		{  
			GnomeVFSVolume *volume;
			volume = GNOME_VFS_VOLUME (current_vol->data);
			
			char * device_path = gnome_vfs_volume_get_device_path(volume);
			char* activation_uri = gnome_vfs_volume_get_activation_uri(volume);

			/*
			if (device_path && 
				(strcmp(device_path, "/dev/root")==0
				|| strcmp(device_path, "rootfs")==0
			))*/
			/* don't list root filesystem, because it's included by hand */
			if (activation_uri && strcmp(activation_uri, "file:///")==0) 
			{
				g_free(device_path);
				g_free(activation_uri);
				gnome_vfs_volume_unref(volume);
				continue; 
			}
			g_free(device_path);
			g_free(activation_uri);
			
			if (TRUE || gnome_vfs_volume_is_user_visible(volume))
			{
				// printf("volume\n");
	    		entries = g_list_append(entries, volume);
	    		visible_volumes_count++;/*		fprintf(stdout, "\n"); */
			}
	    	else 
	    	{
	    		gnome_vfs_volume_unref (volume);
	    	}
		
		}
		
		/* if there is no volumes, append the drive object */
		if (visible_volumes_count<1 /* && gnome_vfs_drive_is_user_visible (drive)*/) 
		{
			entries = g_list_append(entries, drive);
		}
		else
		{
			gnome_vfs_drive_unref(drive);
			
		}
		g_list_free(vol_list);
    
    }     
    
	/* read the remote shares */

	//fprintf(stderr, "\nand now the network shares #####\n");

	{	
		GList *vol_list;
		GList *current_vol;
	
		GnomeVFSVolumeMonitor * volume_monitor = gnome_vfs_get_volume_monitor();                                          
	
		vol_list = gnome_vfs_volume_monitor_get_mounted_volumes(volume_monitor);
	
		for (current_vol = vol_list; current_vol != NULL; current_vol = current_vol->next) 
		{  
			GnomeVFSVolume *volume = GNOME_VFS_VOLUME (current_vol->data);
			
			if (gnome_vfs_volume_get_volume_type(volume) == GNOME_VFS_VOLUME_TYPE_CONNECTED_SERVER)
			{
	    		entries = g_list_append(entries, volume);
			}
	    	else 
	    	{
	    		gnome_vfs_volume_unref (volume);
	    	}
		}
		g_list_free(vol_list);
	}
       
    return entries;
    
}

int _xdg_vfs_gnome_ls_vfsroot(int tagged) 
{
	
	GList* entries=_xdg_vfs_gnome_vfsroot_list();
	
	GList* current_entry;

	if (tagged) puts(XDGVFS_TAG_DIRENTRY);
	_print_home();
	puts("");
	if (tagged) puts(XDGVFS_TAG_DIRENTRY);
	_print_filesystem();
	puts("");		
	if (tagged) puts(XDGVFS_TAG_DIRENTRY);
	_print_network();
	puts("");	
	
	for (current_entry = entries; current_entry!=NULL; current_entry = current_entry->next)
	{
		
		if (tagged) puts(XDGVFS_TAG_DIRENTRY);
		if (GNOME_IS_VFS_VOLUME(current_entry->data))
		{
			GnomeVFSVolume *volume = GNOME_VFS_VOLUME (current_entry->data);
			_print_volume(volume);
			gnome_vfs_volume_unref (volume);
		}
		else if (GNOME_IS_VFS_DRIVE(current_entry->data))
		{
			GnomeVFSDrive *drive = GNOME_VFS_DRIVE(current_entry->data);
			_print_drive(drive);
			gnome_vfs_drive_unref(drive);
		}	
		else 
			fprintf(stdout, "# unexpected list item!\n");
	}
	g_list_free(entries);
	return 1;
}


/* ====================  mounting =================== */

int mount_stoploop;
int mount_tagged;

void volume_monitor_mount_handler(GnomeVFSVolumeMonitor *vfsvolumemonitor,
		GnomeVFSVolume *arg1,  gpointer user_data)
{

	int drive_id = *((int*)user_data);

	GnomeVFSDrive * drive = gnome_vfs_volume_get_drive(arg1);
	if (gnome_vfs_drive_get_id(drive) == drive_id)
	{
		/* ha! we got the right drive */
		mount_stoploop|=2;
		
		GList* vol_list = gnome_vfs_drive_get_mounted_volumes(drive);
    
		GList *current_vol;
		int visible_volumes_count = 0; // count the visible volumes on this drive

		if (mount_tagged) puts(XDGVFS_TAG_MOUNT);		
		for (current_vol = vol_list; current_vol != NULL; current_vol = current_vol->next) 
		{  
			GnomeVFSVolume * volume = GNOME_VFS_VOLUME (current_vol->data);
			if (mount_tagged) puts(XDGVFS_TAG_DIRENTRY);
			_print_volume(volume);
		}

		
		
	}
}

void myGnomeVFSVolumeOpCallback(gboolean succeeded,
                                             char *error,
                                             char *detailed_error,
                                             gpointer data)
{
	int * r = (int*)data;
	
	if (succeeded) 
	{ 
		fprintf(stderr, "VolumeOpCallback: mount succeeded\n");
		mount_stoploop|=1;	/* wait for volume monitor callback before stopping loop */
		*r = XDGVFS_ERRORCODE_OK;
	}
	else 
	{
		fprintf(stderr, "VolumeOpCallback: mount failed: %s %s\n", error, detailed_error);
		*r = XDGVFS_ERRORCODE_COULD_NOT_MOUNT;
		mount_stoploop=3;	/* stop loop instantly*/
	}
	
}


int _xdg_vfs_gnome_mount(char * mountpoint_id, int tagged)
{
	int mount_drive_id = atoi(mountpoint_id);
	mount_tagged = tagged;
	
	GnomeVFSVolumeMonitor * volume_monitor = gnome_vfs_get_volume_monitor();
	
	GList* drive_list = gnome_vfs_volume_monitor_get_connected_drives
                                            (volume_monitor);	

	GList *current_drive;
                                            
    for (current_drive = drive_list; current_drive != NULL; current_drive = current_drive->next) 
    {  
    	
		GnomeVFSDrive *drive = GNOME_VFS_DRIVE(current_drive->data);

		

		if (gnome_vfs_drive_get_id (drive) == mount_drive_id) 
		{
			int r;
			fprintf(stderr, "mounting drive with drive_id=%d!\n", mount_drive_id);

			gulong handler_id = g_signal_connect(volume_monitor, "volume-mounted", 
					G_CALLBACK(volume_monitor_mount_handler), &mount_drive_id);

			gnome_vfs_drive_mount (drive, myGnomeVFSVolumeOpCallback, &r);
	
			/* wait for both callbacks to occur */
			mount_stoploop=0;		
			while (!(mount_stoploop==3))
			{
				// fprintf(stderr, "looping\n");
				g_main_context_iteration(NULL, TRUE);
			}
			
			g_signal_handler_disconnect (volume_monitor, handler_id);			
			g_list_free(drive_list);
			return r;
		}

    }
	g_list_free(drive_list);    
	fprintf(stderr, "mount-problem drive with drive_id=%d not found!\n", mount_drive_id); 
	return XDGVFS_ERRORCODE_GENERIC;

}

/* =============== tests =============== */

int test_drive_list() {
	
	GnomeVFSVolumeMonitor * volume_monitor = gnome_vfs_get_volume_monitor();
	
	GList* drive_list = gnome_vfs_volume_monitor_get_connected_drives
                                            (volume_monitor);	

	printf("%p\n", drive_list);

	GList *current_drive;
                                            
    for (current_drive = drive_list; current_drive != NULL; current_drive = current_drive->next) 
    {  
    	
		GnomeVFSDrive *drive = GNOME_VFS_DRIVE(current_drive->data);

		_print_drive(drive);

    	GList* vol_list = gnome_vfs_drive_get_mounted_volumes(drive);
    
		GList *current_vol;
		
		for (current_vol = vol_list; current_vol != NULL; current_vol = current_vol->next) 
		{  
			GnomeVFSVolume *volume;
			volume = GNOME_VFS_VOLUME (current_vol->data);
		
    		_print_volume(volume);
		
		}
    
    }                                        
	
	
}

int test_volume_list() {

	GList *vol_list;
	GList *current_vol;

	GnomeVFSVolumeMonitor * volume_monitor = gnome_vfs_get_volume_monitor();                                          

	vol_list = gnome_vfs_volume_monitor_get_mounted_volumes(volume_monitor);

	for (current_vol = vol_list; current_vol != NULL; current_vol = current_vol->next) 
	{  
		GnomeVFSVolume *volume;
		volume = GNOME_VFS_VOLUME (current_vol->data);
		
    	_print_volume(volume);
        /* g_free(activation_uri);
        g_free(display_name);
        g_free(filesystem_type);
        g_free(device_path);
        g_free(is_mounted); */
	}
	
}






