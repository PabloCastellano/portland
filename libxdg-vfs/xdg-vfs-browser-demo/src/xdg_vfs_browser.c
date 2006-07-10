/*
#   browser.c
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

#include <gtk/gtk.h>

#include <xdg_vfs_client.h>

enum
{
	ICON_COLUMN,
	BASENAME_COLUMN,
	SIZE_COLUMN,
	MIMETYPE_COLUMN,
	FILETYPE_COLUMN,
	URI_COLUMN,
	MOUNTPOINT_ID_COLUMN,
	IS_MOUNTED_COLUMN,
	N_COLUMNS
};

GtkWidget *window;
GtkTreeSelection *listselect;
GtkListStore *model;
GtkWidget *url_entry;

guint status_bar_context_id;
GtkWidget* status_bar;

XdgVfsSession * session;
char * system_uri;
char * current_uri=NULL;

GtkIconTheme * iconTheme;

static void initIconTheme(char * name) {

	iconTheme = gtk_icon_theme_new (); // gtk_icon_theme_get_default ();


/*	gtk_icon_theme_prepend_search_path
                                            (iconTheme,
                                             "/usr/share/icons/crystalsvg/scalable"); */

	gtk_icon_theme_set_custom_theme (iconTheme,
                                             name);
/*	gtk_icon_theme_set_custom_theme (iconTheme,
                                             "gnome"); */


/*	
	gchar ** themepath;
	gint n_elements;
	int i;
	gtk_icon_theme_get_search_path  (iconTheme,
                                             &themepath,
                                             &n_elements);

	for (i=0;i<n_elements;i++)
		fprintf(stderr, "themepath=%s\n", themepath[i]);
*/
}



static void errorbox(int code) {
	GtkWidget * dialog = gtk_message_dialog_new (GTK_WINDOW(window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE,
				"xdg-vfs error #%d: %s",
				code, xdg_vfs_resultToString(code));
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (GTK_WIDGET(dialog));

}

static void list_dir(const char * uri)
{
	XdgVfsResult r;
	XdgVfsItemType type;
	XdgVfsItem * item;
	int i;
	GtkTreeIter iter;

	char * message = g_strdup_printf ("listing %s ...", uri);

	gtk_statusbar_push(GTK_STATUSBAR(status_bar),  status_bar_context_id,
                                             message);

	g_free(message);

	while (gtk_events_pending ())
		gtk_main_iteration_do(FALSE);

	gtk_list_store_clear(model); // clear list

	while (gtk_events_pending ())
		gtk_main_iteration_do(FALSE);

	r = xdg_vfs_sess_cmd_listDirectory(session, uri);

	if (r) {
		fprintf(stderr, "test 3 cmd problem=%d\n", r);
		errorbox(r);
		return;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_LS_HEAD:
			{
					XdgVfsSimpleHead * head = (XdgVfsSimpleHead*) item;
					fprintf(stdout, "got ls header uri='%s'\n", head->uri);
					break;
			}
			
			case XDGVFS_ITEMTYPE_FILEINFO:
			{
					XdgVfsFileInfo * info = (XdgVfsFileInfo*) item;
					// fprintf(stdout, "got fileinfo uri='%s'\n", info->uri);	
					GdkPixbuf* pixBuf;
					if (info->iconname) 
					{
						GError *gerror=NULL;
						pixBuf =  gtk_icon_theme_load_icon(
							iconTheme,
							info->iconname,
							24,
							0 /*GTK_ICON_LOOKUP_NO_SVG*/,
							&gerror);
						if (gerror) 
						{
							fprintf (stderr, "Unable to load icon: %s\n", gerror->message);
							g_error_free (gerror);
						}
						else 
						{
							

						}
					}

				gtk_list_store_append (GTK_LIST_STORE (model), &iter);
				gtk_list_store_set (GTK_LIST_STORE (model), 
					&iter,
					ICON_COLUMN, pixBuf,
					BASENAME_COLUMN, info->basename,
					SIZE_COLUMN, info->size, 
					MIMETYPE_COLUMN, info->mimetype,
					FILETYPE_COLUMN, info->filetype,
					URI_COLUMN, info->uri,
					MOUNTPOINT_ID_COLUMN, info->mountpoint_id,
					IS_MOUNTED_COLUMN, info->is_mounted,
				-1);

					break;
			}
			case XDGVFS_ITEMTYPE_NONE:
			{
					break;
			}
			default:
			{
					fprintf(stderr, "unexpected item - type=%d\n", type);
					break;
			}
		}
		xdg_vfs_item_unref(item);
	}
	fprintf(stderr, "list result=%d\n", r);
	if (r) { errorbox(r);}
	else { 
		gtk_entry_set_text(GTK_ENTRY(url_entry), uri); 
			
		if (current_uri)
		{
			r = xdg_vfs_sess_cmd_removeMonitor(session, current_uri);
			fprintf(stderr, "remove monitor result=%d\n", r);
		}
		//if (current_uri) free(current_uri);
		current_uri = strdup(uri); 
		// if (r && r!= XDGVFS_ERRORCODE_NOT_FOUND) { errorbox(r);}
		r = xdg_vfs_sess_cmd_monitorDir(session, uri);
		fprintf(stderr, "monitor dir result=%d\n", r);
		// if (r && r != XDGVFS_ERRORCODE_ALREADY_EXISTS) { errorbox(r);}
	}
	
	
	
	gtk_statusbar_pop (GTK_STATUSBAR(status_bar), status_bar_context_id);
	
/*
    for (i = 0; i < 10; i++) {
        gchar *msg = g_strdup_printf ("Message %s #%d", uri, i);
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), 
	                    &iter,
                            0, msg,
			    1, i, 
	                    -1);
	g_free (msg);
    }
*/
}

char *  _mount_it(char * mountpoint_id)
{
	char * activation_uri=NULL;
	
	XdgVfsResult r;
	
	char * message = g_strdup_printf ("mounting %s", mountpoint_id);

	gtk_statusbar_push(GTK_STATUSBAR(status_bar),  status_bar_context_id,
                                             message);

	g_free(message);

	while (gtk_events_pending ())
		gtk_main_iteration_do(FALSE);	
	
	r = xdg_vfs_sess_cmd_mount(session, mountpoint_id);

	if (r) { errorbox(r); return NULL;}

	XdgVfsItemType type=0;
	XdgVfsItem * item=NULL;
	int n = 5;

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch (type)
		{
			case XDGVFS_ITEMTYPE_NONE:
			{
					break;
			}
			case XDGVFS_ITEMTYPE_FILEINFO:
			{
					/* lists the volumes just mounted */
					XdgVfsFileInfo * info = (XdgVfsFileInfo*) item;
					fprintf(stderr, "got activation uri=%s\n", info->uri);
					if (!activation_uri) activation_uri = strdup(info->uri);
					break;					
			}
			default:
			{
					fprintf(stderr, "unexpected item - type=%d\n", type);
					break;
			}
		}
		xdg_vfs_item_unref(item);
	}
	if (r) { errorbox(r); activation_uri=NULL;}
	fprintf(stderr, "mount result=%d\n", r);
	gtk_statusbar_pop (GTK_STATUSBAR(status_bar), status_bar_context_id);
	return activation_uri;
}


static void
tree_selection_changed_cb (GtkTreeSelection *selection, gpointer data)
{
	GtkTreeIter iter;
	//GtkTreeModel *model;
	gint filetype;
	gchar *uri;
	gchar *mountpoint_id;
	gint is_mounted;
	
	GtkTreeModel * newTreeModel;

	if (gtk_tree_selection_get_selected (selection, &newTreeModel, &iter))
	{
		model = (GtkListStore*)newTreeModel;
		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, FILETYPE_COLUMN, &filetype, -1);
		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, URI_COLUMN, &uri, -1);
		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, MOUNTPOINT_ID_COLUMN, &mountpoint_id, -1);
		gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, IS_MOUNTED_COLUMN, &is_mounted, -1);

		printf ("You selected uri=%s\tfiletype=%d\tmntid=%s\tis_mounted=%d\n", uri, filetype, mountpoint_id, is_mounted);

		if (filetype==XDGVFS_FILE_TYPE_DIRECTORY) {
			
			list_dir(uri);
		}
		else if (filetype==XDGVFS_FILE_TYPE_VFSMOUNTPOINT) {
			if (is_mounted) list_dir(uri);
			else
			{
				char * activation_uri = _mount_it(mountpoint_id);
				if (activation_uri)
				{
					list_dir(activation_uri);
					free(activation_uri);
				}
			}
			
		}
		

        g_free (uri);
	}
}



/* Create the list of "messages" */
static GtkWidget *create_list( void )
{

	GtkWidget *scrolled_window;
	GtkWidget *tree_view;
	
	GtkTreeIter iter;
	/*    GtkCellRenderer *cell0;
	GtkCellRenderer *cell1;
	GtkCellRenderer *cell2;
	GtkTreeViewColumn *column0;
	GtkTreeViewColumn *column1;
	GtkTreeViewColumn *column2; */

	GtkCellRenderer *normrenderer;
	GtkTreeViewColumn *normcolumn;

	int i;
	
	/* Create a new scrolled window, with scrollbars only if needed */
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
					GTK_POLICY_AUTOMATIC, 
					GTK_POLICY_AUTOMATIC);
	
	model = gtk_list_store_new (
		N_COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, 
		G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
	tree_view = gtk_tree_view_new ();
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), 
						tree_view);
	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), GTK_TREE_MODEL (model));
	gtk_widget_show (tree_view);
	
	/* Add some messages to the window */
	/*for (i = 0; i < 10; i++) {
		gchar *msg = g_strdup_printf ("Message #%d", i);
		gtk_list_store_append (GTK_LIST_STORE (model), &iter);
		gtk_list_store_set (GTK_LIST_STORE (model), 
				&iter,
				0, msg,
				1, i, 
				-1);
		g_free (msg);
	}*/

	GtkCellRenderer* pixbufrenderer = gtk_cell_renderer_pixbuf_new();
	normcolumn = gtk_tree_view_column_new_with_attributes ("",
							pixbufrenderer,
							"pixbuf", ICON_COLUMN,
							NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));
	
	normrenderer = gtk_cell_renderer_text_new ();
	
	normcolumn = gtk_tree_view_column_new_with_attributes ("Name",
							normrenderer,
							"text", BASENAME_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));
	
	normcolumn = gtk_tree_view_column_new_with_attributes ("Size",
							normrenderer,
							"text", SIZE_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));
	
	normcolumn = gtk_tree_view_column_new_with_attributes ("MimeType",
							normrenderer,
							"text", MIMETYPE_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));
	
	normcolumn = gtk_tree_view_column_new_with_attributes ("Type",
							normrenderer,
							"text", FILETYPE_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));
	
	normcolumn = gtk_tree_view_column_new_with_attributes ("URI",
							normrenderer,
							"text", URI_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));

	normcolumn = gtk_tree_view_column_new_with_attributes ("MntID",
							normrenderer,
							"text", MOUNTPOINT_ID_COLUMN,
							NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));

							
	normcolumn = gtk_tree_view_column_new_with_attributes ("mnted",
							normrenderer,
							"text", IS_MOUNTED_COLUMN,
							NULL);
	
	gtk_tree_view_append_column (GTK_TREE_VIEW (tree_view),
					GTK_TREE_VIEW_COLUMN (normcolumn));



	listselect = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
	gtk_tree_selection_set_mode (listselect, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (listselect), "changed",
		G_CALLBACK (tree_selection_changed_cb),
		NULL);


	return scrolled_window;
}
   
/* Add some text to our text widget - this is a callback that is invoked
when our window is realized. We could also force our window to be
realized with gtk_widget_realize, but it would have to be part of
a hierarchy first */

static void insert_text( GtkTextBuffer *buffer )
{
	GtkTextIter iter;
	
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);
	
	gtk_text_buffer_insert (buffer, &iter,   
	"", -1);
}
   
/* Create a scrolled text area that displays a "message" */
static GtkWidget *create_text( void )
{
	GtkWidget *scrolled_window;
	GtkWidget *view;
	GtkTextBuffer *buffer;
	
	view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_container_add (GTK_CONTAINER (scrolled_window), view);
	insert_text (buffer);
	
	gtk_widget_show_all (scrolled_window);
	
	return scrolled_window;
}


static void button_root_callback( GtkWidget *widget,
                      gpointer   data )
{
	g_print ("%s was pressed\n", (char *) data);
	
	list_dir(system_uri);

}


static void button_go_callback( GtkWidget *widget,
                      gpointer   data )
{
	g_print ("%s was pressed\n", (char *) data);
	
	list_dir(gtk_entry_get_text (GTK_ENTRY(url_entry)));

}


int read_backend_infos() {
	XdgVfsResult r;
	XdgVfsItemType type;
	XdgVfsItem * item;

	r = xdg_vfs_sess_cmd_backendInfo(session);
	
	if (r) {
		fprintf(stderr, "backend-info cmd problem=%d\n", r);
		return 1;
	}

	while ((r=xdg_vfs_sess_readItem(session, &type, 
		&item, NULL, NULL)) == XDGVFS_RESULT_CONTINUES) 
	{
		switch(type) 
		{
			case XDGVFS_ITEMTYPE_BACKENDINFO:
			{
					XdgVfsBackendInfo * backendInfo = (XdgVfsBackendInfo*) item;
					fprintf(stderr, "backend_id='%s' system_uri='%s' file_icon_theme='%s'\n", 
						backendInfo->backend_id,
						backendInfo->system_uri,
						backendInfo->file_icon_theme);
					
					initIconTheme(backendInfo->file_icon_theme);
					system_uri = strdup(backendInfo->system_uri);

			}
			case XDGVFS_ITEMTYPE_NONE:
			{
					break;
			}
			default:
			{
					fprintf(stderr, "unexpected item - type=%d\n", type);
					break;
			}				
		}
		xdg_vfs_item_unref(item);
	}
	if (r) { errorbox(r); exit(1); }
	fprintf(stderr, "backend-info result=%d\n", r);
}

/* avoid recursion when getting dir-monitor events */
gboolean    _list_again(gpointer data)
{
	char * uri = (char*)data;
	printf("dir changed event2: current_uri=%s; uri=%s\n", current_uri, uri);
	if (current_uri && strcmp(current_uri, uri)==0) list_dir(uri);
	g_free(uri);
	return FALSE;
}


void myXdgVfsMonitorCallback(XdgVfsSession * sess, char * uri, void * user_data)
{
	printf("dir changed event: %s\n", uri);
	//g_idle_add(_list_again,strdup(uri));
	g_timeout_add(700,_list_again,strdup(uri));
}


gboolean myGIOFunc(GIOChannel *source, GIOCondition condition,  gpointer data)
{
	XdgVfsResult r;
	if(!xdg_vfs_sess_isBusy(session))
	{
		XdgVfsItemType type=0;
		XdgVfsItem * item=NULL;
		
		fprintf(stderr, "incoming event!\n");
		if ((r=xdg_vfs_sess_readItem(session, &type, 
			&item, NULL, NULL)) == XDGVFS_RESULT_OK) 
		{
			switch (type)
			{
				case XDGVFS_ITEMTYPE_NONE:
				{
					break;
				}
				default:
				{
					fprintf(stderr, "unexpected item - type=%d\n", type);
					break;
				}
			}
		xdg_vfs_item_unref(item);
		}
	}
	return TRUE;
}


int main( int   argc,
          char *argv[] )
{
	XdgVfsResult r;
	
	GtkWidget *main_vbox;
	GtkWidget *url_hbox;
	
	GtkWidget *root_button;
	GtkWidget *go_button;
	
	GtkWidget *vpaned;
	GtkWidget *list;
	GtkWidget *text;
	
	gtk_init (&argc, &argv);
	
	if (argc!=2) {
		fprintf(stderr,"usage: %s {gnome|kde|this}\n", argv[0]);
		exit(1);
	}

	r = xdg_vfs_sess_start(&session, argv[1]);

	if (r) {
		fprintf(stderr, " session start problem=%d\n", r);
		return 1;
	}

	xdg_vfs_sess_set_monitor_callback(session, myXdgVfsMonitorCallback, NULL);

	read_backend_infos();
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "libxdg-vfs browser demo");
	g_signal_connect (G_OBJECT (window), "destroy",
			G_CALLBACK (gtk_main_quit), NULL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	gtk_widget_set_size_request (GTK_WIDGET (window), 450, 400);
	
	/* vbox */
	
	main_vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_set_homogeneous(GTK_BOX(main_vbox),FALSE);
	
	gtk_container_add (GTK_CONTAINER (window), main_vbox);
	gtk_widget_show (main_vbox);
	
	
	/* create url box */
	
	url_hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_set_homogeneous(GTK_BOX(url_hbox),FALSE);
	
	//    gtk_container_add (GTK_CONTAINER (main_vbox), url_hbox);
	
	gtk_box_pack_start              (GTK_BOX(main_vbox),
						url_hbox,
						FALSE,
						FALSE,
						5);
	gtk_widget_show (url_hbox);

	/* create root URI button */

	root_button = gtk_button_new_with_label ("VFS Root");
	
	/* Connect the "clicked" signal of the button to our callback */
	g_signal_connect (G_OBJECT (root_button), "clicked",
			G_CALLBACK (button_root_callback), (gpointer) "root button");
	
	gtk_widget_show (root_button);
	
	
	gtk_box_pack_start              (GTK_BOX(url_hbox),
						root_button,
						FALSE,
						FALSE,
						5);
	
	
	/* url entry */
	
	url_entry = gtk_entry_new ();
	gtk_entry_set_activates_default (GTK_ENTRY (url_entry), TRUE);
	gtk_entry_set_max_length (GTK_ENTRY (url_entry), 100);
	//    gtk_container_add (GTK_CONTAINER (main_vbox), url_entry);
	
	gtk_box_pack_start              (GTK_BOX(url_hbox),
						url_entry,
						TRUE,
						TRUE,
						5);
	
	gtk_widget_show (url_entry);
	
	/* go button */
	
	go_button = gtk_button_new_with_label ("Go");
	GTK_WIDGET_SET_FLAGS (go_button, GTK_CAN_DEFAULT);
	
	
	/* Connect the "clicked" signal of the button to our callback */
	g_signal_connect (G_OBJECT (go_button), "clicked",
			G_CALLBACK (button_go_callback), (gpointer) "go button");
	
	gtk_widget_show (go_button);
	
	gtk_box_pack_start(GTK_BOX (url_hbox),
						go_button,
						FALSE,
						FALSE,
						5);
	gtk_widget_grab_default (go_button);
	
	
	/* create a vpaned widget and add it to our toplevel window */
	
	/*   
	vpaned = gtk_vpaned_new ();
	gtk_container_add (GTK_CONTAINER (main_vbox), vpaned);
	gtk_widget_show (vpaned);
	
	
	list = create_list ();
	gtk_paned_add1 (GTK_PANED (vpaned), list);
	gtk_widget_show (list);
	
	text = create_text ();
	gtk_paned_add2 (GTK_PANED (vpaned), text);
	gtk_widget_show (text);
	gtk_widget_show (window);
	
	
	*/
	list = create_list ();
	gtk_container_add (GTK_CONTAINER (main_vbox), list);
	gtk_widget_show (list);

	status_bar = gtk_statusbar_new();
	gtk_widget_show (status_bar);
	//gtk_container_add (GTK_CONTAINER (main_vbox), status_bar);

	status_bar_context_id=gtk_statusbar_get_context_id    (GTK_STATUSBAR(status_bar),
                                             "xdg_vfs_browser");

	gtk_box_pack_start(GTK_BOX  (main_vbox),
						status_bar,
						FALSE,
						FALSE,
						0);

	gtk_widget_show (window);
	gtk_list_store_clear            (model);

	current_uri = system_uri;
	list_dir(system_uri);

	/* watch the incoming pipe (for change notifications) */
	
	GIOChannel* iochannel = g_io_channel_unix_new(xdg_vfs_sess_getIncomingFiledescriptor(session));

	g_io_add_watch(iochannel,
                G_IO_IN,
                myGIOFunc,
				NULL);

	gtk_main ();

	xdg_vfs_sess_close(session);
	
	return 0;
}
