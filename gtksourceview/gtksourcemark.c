/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcebuffer.c
 *
 *  Copyright (C) 2007 by:
 *		Johannes Schmid <jhs@gnome.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "gtksourcemark.h"
#include "gtksourcebuffer.h"

#include <glib/gi18n.h>

enum
{
	PROP_0,
	PROP_CATEGORY
};

struct _GtkSourceMarkPrivate
{
	gchar *category;
};

G_DEFINE_TYPE (GtkSourceMark, gtk_source_mark, GTK_TYPE_TEXT_MARK);

static void
gtk_source_mark_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
	GtkSourceMarkPrivate *priv;

	g_return_if_fail (GTK_IS_SOURCE_MARK (object));

	priv = GTK_SOURCE_MARK (object)->priv;

	switch (prop_id)
	{
		case PROP_CATEGORY:
			g_return_if_fail (g_value_get_string (value) != NULL);
			g_free (priv->category);
			priv->category = g_value_dup_string (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
							   prop_id,
							   pspec);
	}
}

static void
gtk_source_mark_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
	GtkSourceMark *mark;

	g_return_if_fail (GTK_IS_SOURCE_MARK (object));

	mark = GTK_SOURCE_MARK (object);

	switch (prop_id)
	{
		case PROP_CATEGORY:
			g_value_set_string (value,
					    gtk_source_mark_get_category (mark));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object,
							   prop_id,
							   pspec);
	}
}

static void
gtk_source_mark_class_init (GtkSourceMarkClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = gtk_source_mark_set_property;
	object_class->get_property = gtk_source_mark_get_property;

	/**
	 * GtkSourceMark:category:
	 *
	 * The category of the #GtkSourceMark, classified the mark and control
	 * what pixbuf is used and with which priority it is drawn.
	 */
	g_object_class_install_property (object_class,
					 PROP_CATEGORY,
					 g_param_spec_string ("category",
							      _("category"),
							      _("The mark category"),
							      NULL,
							      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	
	g_type_class_add_private (object_class, sizeof (GtkSourceMarkPrivate));
}

static void
gtk_source_mark_init (GtkSourceMark *mark)
{
	mark->priv = G_TYPE_INSTANCE_GET_PRIVATE (mark, GTK_TYPE_SOURCE_MARK,
						  GtkSourceMarkPrivate);
}

/**
 * gtk_source_mark_new:
 * @name: Name of the #GtkSourceMark, can be NULL when not using a name
 * @category is used to classify marks according to common characteristics
 * (e.g. all the marks representing a bookmark could belong to the "bookmark" 
 * category, or all the marks representing a compilation error could belong to 
 * "error" category).
 *
 * Creates a text mark. Add it to a buffer using gtk_text_buffer_add_mark(). 
 * If name is NULL, the mark is anonymous; otherwise, the mark can be retrieved
 * by name using gtk_text_buffer_get_mark().
 * Normally marks are created using the utility function 
 * gtk_source_buffer_create_mark().
 *
 * Returns: a new #GtkSourceMark that can be added using gtk_text_buffer_add_mark()
 *
 * Since: 2.2
 */
GtkSourceMark *
gtk_source_mark_new (const gchar *name,
		     const gchar *category)
{
	g_return_val_if_fail (category != NULL, NULL);
	
	return GTK_SOURCE_MARK (g_object_new (GTK_TYPE_SOURCE_MARK, 
					      "category", category, 
					      "name", name,
					      "left-gravity", TRUE,
					       NULL));
}

/**
 * gtk_source_mark_get_category:
 * @mark: a #GtkSourceMark
 *
 * Returns the mark category
 *
 * Returns: the category of the #GtkSourceMark
 *
 * Since: 2.2
 */
const gchar *
gtk_source_mark_get_category (GtkSourceMark *mark)
{
	g_return_val_if_fail (GTK_IS_SOURCE_MARK (mark), NULL);

	return mark->priv->category;
}

/**
 * gtk_source_mark_next:
 * @mark: a #GtkSourceMark
 * @category: a string specifying the mark category or %NULL
 *
 * Returns the next #GtkSourceMark in the buffer or %NULL if the mark
 * was not added to a buffer. If there is no next mark, %NULL will be returned.
 *
 * If @category is %NULL, looks for marks of any category
 *
 * Returns: the next #GtkSourceMark or %NULL
 *
 * Since: 2.2
 */
GtkSourceMark *
gtk_source_mark_next (GtkSourceMark *mark,
		      const gchar   *category)
{
	GtkTextBuffer *buffer;

	g_return_val_if_fail (GTK_IS_SOURCE_MARK (mark), NULL);

	buffer = gtk_text_mark_get_buffer (GTK_TEXT_MARK (mark));
	if (buffer != NULL)
		return _gtk_source_buffer_source_mark_next (GTK_SOURCE_BUFFER (buffer),
							    mark, category);
	else
		return NULL;
}

/**
 * gtk_source_mark_prev:
 * @mark: a #GtkSourceMark
 * @category: a string specifying the mark category or %NULL
 *
 * Returns the previous #GtkSourceMark in the buffer or %NULL if the mark
 * was not added to a buffer. If there is no previous mark, %NULL is returned.
 * 
 * If @category is %NULL, looks for marks of any category
 *
 * Returns: the previous #GtkSourceMark or %NULL
 *
 * Since: 2.2
 */
GtkSourceMark *
gtk_source_mark_prev (GtkSourceMark *mark,
		      const gchar   *category)
{
	GtkTextBuffer *buffer;

	g_return_val_if_fail (GTK_IS_SOURCE_MARK (mark), NULL);

	buffer = gtk_text_mark_get_buffer (GTK_TEXT_MARK (mark));
	if (buffer != NULL)
		return _gtk_source_buffer_source_mark_prev (GTK_SOURCE_BUFFER (buffer),
							    mark, category);
	else
		return NULL;
}
