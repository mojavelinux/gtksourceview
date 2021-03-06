/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 * gtksourcesearchcontext.h
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2013 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * GtkSourceView is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * GtkSourceView is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __GTK_SOURCE_SEARCH_CONTEXT_H__
#define __GTK_SOURCE_SEARCH_CONTEXT_H__

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcetypes.h>

G_BEGIN_DECLS

#define GTK_SOURCE_TYPE_SEARCH_CONTEXT             (gtk_source_search_context_get_type ())
#define GTK_SOURCE_SEARCH_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_TYPE_SEARCH_CONTEXT, GtkSourceSearchContext))
#define GTK_SOURCE_SEARCH_CONTEXT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_TYPE_SEARCH_CONTEXT, GtkSourceSearchContextClass))
#define GTK_SOURCE_IS_SEARCH_CONTEXT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_TYPE_SEARCH_CONTEXT))
#define GTK_SOURCE_IS_SEARCH_CONTEXT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_TYPE_SEARCH_CONTEXT))
#define GTK_SOURCE_SEARCH_CONTEXT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_TYPE_SEARCH_CONTEXT, GtkSourceSearchContextClass))

typedef struct _GtkSourceSearchContextClass    GtkSourceSearchContextClass;
typedef struct _GtkSourceSearchContextPrivate  GtkSourceSearchContextPrivate;

struct _GtkSourceSearchContext
{
	GObject parent;

	GtkSourceSearchContextPrivate *priv;
};

struct _GtkSourceSearchContextClass
{
	GObjectClass parent_class;

	gpointer padding[10];
};

GType			 gtk_source_search_context_get_type			(void) G_GNUC_CONST;

GtkSourceSearchContext	*gtk_source_search_context_new				(GtkSourceBuffer	 *buffer,
										 GtkSourceSearchSettings *settings);

GtkSourceBuffer		*gtk_source_search_context_get_buffer			(GtkSourceSearchContext  *search);

GtkSourceSearchSettings	*gtk_source_search_context_get_settings			(GtkSourceSearchContext	 *search);

void			 gtk_source_search_context_set_settings			(GtkSourceSearchContext  *search,
										 GtkSourceSearchSettings *settings);

gboolean		 gtk_source_search_context_get_highlight		(GtkSourceSearchContext  *search);

void			 gtk_source_search_context_set_highlight		(GtkSourceSearchContext  *search,
										 gboolean                 highlight);

GError			*gtk_source_search_context_get_regex_error		(GtkSourceSearchContext	 *search);

gint			 gtk_source_search_context_get_occurrences_count	(GtkSourceSearchContext	 *search);

gint			 gtk_source_search_context_get_occurrence_position	(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *match_start,
										 const GtkTextIter	 *match_end);

gboolean		 gtk_source_search_context_forward			(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *iter,
										 GtkTextIter		 *match_start,
										 GtkTextIter		 *match_end);

void			 gtk_source_search_context_forward_async		(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *iter,
										 GCancellable		 *cancellable,
										 GAsyncReadyCallback	  callback,
										 gpointer		  user_data);

gboolean		 gtk_source_search_context_forward_finish		(GtkSourceSearchContext	 *search,
										 GAsyncResult		 *result,
										 GtkTextIter		 *match_start,
										 GtkTextIter		 *match_end,
										 GError		        **error);

gboolean		 gtk_source_search_context_backward			(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *iter,
										 GtkTextIter		 *match_start,
										 GtkTextIter		 *match_end);

void			 gtk_source_search_context_backward_async		(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *iter,
										 GCancellable		 *cancellable,
										 GAsyncReadyCallback	  callback,
										 gpointer		  user_data);

gboolean		 gtk_source_search_context_backward_finish		(GtkSourceSearchContext	 *search,
										 GAsyncResult		 *result,
										 GtkTextIter		 *match_start,
										 GtkTextIter		 *match_end,
										 GError		        **error);

gboolean		 gtk_source_search_context_replace			(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *match_start,
										 const GtkTextIter	 *match_end,
										 const gchar		 *replace,
										 gint			  replace_length,
										 GError			**error);

guint			 gtk_source_search_context_replace_all			(GtkSourceSearchContext	 *search,
										 const gchar		 *replace,
										 gint			  replace_length,
										 GError			**error);

G_GNUC_INTERNAL
void			 _gtk_source_search_context_update_highlight		(GtkSourceSearchContext	 *search,
										 const GtkTextIter	 *start,
										 const GtkTextIter	 *end,
										 gboolean		  synchronous);

G_END_DECLS

#endif /* __GTK_SOURCE_SEARCH_CONTEXT_H__ */
