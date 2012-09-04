/*
 *  Copyright (C) 2002 Jorn Baayen <jorn@nl.linux.org>
 *  Copyright (C) 2003 Colin Walters <walters@gnome.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  The Rhythmbox authors hereby grant permission for non-GPL compatible
 *  GStreamer plugins to be used and distributed together with GStreamer
 *  and Rhythmbox. This permission is above and beyond the permissions granted
 *  by the GPL license by which Rhythmbox is covered. If you modify this code
 *  you may extend this exception to your version of the code, but you are not
 *  obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA.
 *
 */

#ifndef __RB_SOURCE_H
#define __RB_SOURCE_H

#include <gtk/gtk.h>

#include <sources/rb-display-page.h>
#include <sources/rb-source-search.h>
#include <widgets/rb-entry-view.h>
#include <widgets/rb-search-entry.h>
#include <shell/rb-shell-preferences.h>
#include <shell/rb-track-transfer-batch.h>
#include <rhythmdb/rhythmdb-import-job.h>

G_BEGIN_DECLS

typedef enum {
	RB_SOURCE_EOF_ERROR,
	RB_SOURCE_EOF_STOP,
	RB_SOURCE_EOF_RETRY,
	RB_SOURCE_EOF_NEXT,
} RBSourceEOFType;

typedef enum {
	RB_SOURCE_LOAD_STATUS_NOT_LOADED,
	RB_SOURCE_LOAD_STATUS_WAITING,
	RB_SOURCE_LOAD_STATUS_LOADING,
	RB_SOURCE_LOAD_STATUS_LOADED
} RBSourceLoadStatus;

typedef struct _RBSource	RBSource;
typedef struct _RBSourceClass	RBSourceClass;
typedef struct _RBSourcePrivate	RBSourcePrivate;

typedef void (*RBSourceActionCallback) (GtkAction *action, RBSource *source);

GType rb_source_eof_type_get_type (void);
#define RB_TYPE_SOURCE_EOF_TYPE	(rb_source_eof_type_get_type())

GType rb_source_load_status_get_type (void);
#define RB_TYPE_SOURCE_LOAD_STATUS (rb_source_load_status_get_type())

#define RB_TYPE_SOURCE         (rb_source_get_type ())
#define RB_SOURCE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), RB_TYPE_SOURCE, RBSource))
#define RB_SOURCE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), RB_TYPE_SOURCE, RBSourceClass))
#define RB_IS_SOURCE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), RB_TYPE_SOURCE))
#define RB_IS_SOURCE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), RB_TYPE_SOURCE))
#define RB_SOURCE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), RB_TYPE_SOURCE, RBSourceClass))

#define RB_SOURCE_ICON_SIZE	GTK_ICON_SIZE_LARGE_TOOLBAR

typedef gboolean (*RBSourceFeatureFunc) (RBSource *source);
typedef const char * (*RBSourceStringFunc) (RBSource *source);
typedef void (*RBSourceAddCallback) (RBSource *source, const char *uri, gpointer data);

struct _RBSource
{
	RBDisplayPage parent;
	RBSourcePrivate *priv;
};

struct _RBSourceClass
{
	RBDisplayPageClass parent;

	/* signals */
	void (*filter_changed)	(RBSource *source);

	/* methods */

	RBEntryView *	(*impl_get_entry_view)	(RBSource *source);
	GList *		(*impl_get_property_views)	(RBSource *source);

	gboolean	(*impl_can_rename)	(RBSource *source);

	void		(*impl_search)		(RBSource *source, RBSourceSearch *search, const char *cur_text, const char *new_text);
	void		(*impl_reset_filters)	(RBSource *source);

	gboolean	(*impl_can_cut)		(RBSource *source);
	gboolean	(*impl_can_delete)	(RBSource *source);
	gboolean	(*impl_can_move_to_trash) (RBSource *source);
	gboolean	(*impl_can_copy)	(RBSource *source);
	gboolean	(*impl_can_paste)	(RBSource *source);
	gboolean	(*impl_can_add_to_queue)(RBSource *source);

	GList *		(*impl_cut)		(RBSource *source);
	GList *		(*impl_copy)		(RBSource *source);
	RBTrackTransferBatch *(*impl_paste)	(RBSource *source, GList *entries);
	void		(*impl_delete)		(RBSource *source);
	void		(*impl_add_to_queue)	(RBSource *source, RBSource *queue);
	void		(*impl_move_to_trash)	(RBSource *source);

	void		(*impl_song_properties)	(RBSource *source);

	gboolean	(*impl_try_playlist)	(RBSource *source);
	guint		(*impl_want_uri)	(RBSource *source, const char *uri);
	void		(*impl_add_uri)		(RBSource *source,
						 const char *uri,
						 const char *title,
						 const char *genre,
						 RBSourceAddCallback callback,
						 gpointer data,
						 GDestroyNotify destroy_data);
	gboolean	(*impl_uri_is_source)	(RBSource *source, const char *uri);

	gboolean	(*impl_can_pause)	(RBSource *source);
	RBSourceEOFType	(*impl_handle_eos)	(RBSource *source);

	char *		(*impl_get_delete_action) (RBSource *source);
};

GType		rb_source_get_type		(void);

void		rb_source_notify_filter_changed	(RBSource *source);

void		rb_source_update_play_statistics(RBSource *source, RhythmDB *db,
						 RhythmDBEntry *entry);

/* general interface */

RBEntryView *	rb_source_get_entry_view	(RBSource *source);

GList *		rb_source_get_property_views	(RBSource *source);

gboolean	rb_source_can_rename		(RBSource *source);

void		rb_source_search		(RBSource *source,
						 RBSourceSearch *search,
						 const char *cur_text,
						 const char *new_text);

void		rb_source_reset_filters		(RBSource *source);

gboolean	rb_source_can_cut		(RBSource *source);
gboolean	rb_source_can_delete		(RBSource *source);
gboolean	rb_source_can_move_to_trash	(RBSource *source);
gboolean	rb_source_can_copy		(RBSource *source);
gboolean	rb_source_can_paste		(RBSource *source);
gboolean	rb_source_can_add_to_queue	(RBSource *source);
gboolean	rb_source_can_show_properties	(RBSource *source);

GList *		rb_source_cut			(RBSource *source);
GList *		rb_source_copy			(RBSource *source);
RBTrackTransferBatch *rb_source_paste			(RBSource *source, GList *entries);
void		rb_source_delete		(RBSource *source);
void		rb_source_add_to_queue		(RBSource *source, RBSource *queue);
void		rb_source_move_to_trash		(RBSource *source);

void		rb_source_song_properties	(RBSource *source);

gboolean	rb_source_try_playlist		(RBSource *source);
guint		rb_source_want_uri		(RBSource *source, const char *uri);
gboolean	rb_source_uri_is_source		(RBSource *source, const char *uri);
void		rb_source_add_uri		(RBSource *source,
						 const char *uri,
						 const char *title,
						 const char *genre,
						 RBSourceAddCallback callback,
						 gpointer data,
						 GDestroyNotify destroy_data);

gboolean	rb_source_can_pause		(RBSource *source);
RBSourceEOFType	rb_source_handle_eos		(RBSource *source);

char *		rb_source_get_delete_action	(RBSource *source);

GList *		rb_source_gather_selected_properties (RBSource *source, RhythmDBPropType prop);

void            rb_source_set_hidden_when_empty (RBSource *source,
                                                 gboolean  hidden);

/* Protected methods, should only be used by objects inheriting from RBSource */

gboolean	_rb_source_check_entry_type	(RBSource *source,
						 RhythmDBEntry *entry);

void		_rb_source_set_import_status	(RBSource *source,
						 RhythmDBImportJob *job,
						 char **progress_text,
						 float *progress);
void		rb_source_bind_settings		(RBSource *source,
						 GtkWidget *entry_view,
						 GtkWidget *paned,
						 GtkWidget *browser);

G_END_DECLS

#endif /* __RB_SOURCE_H */
