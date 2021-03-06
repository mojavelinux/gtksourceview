/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 * test-widget.c
 * This file is part of GtkSourceView
 *
 * Copyright (C) 2001 - Mikael Hermansson <tyan@linux.se>
 * Copyright (C) 2003 - Gustavo Giráldez <gustavo.giraldez@gmx.net>
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

#include <string.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gtksourceview/gtksource.h>

/* Global list of open windows */
static GList *windows = NULL;

static GtkSourceStyleScheme *style_scheme = NULL;

#define MARK_TYPE_1      "one"
#define MARK_TYPE_2      "two"


/* Private prototypes -------------------------------------------------------- */

static void       open_file_cb                   (GtkAction       *action,
						  gpointer         user_data);
static void       print_file_cb                  (GtkAction       *action,
						  gpointer         user_data);

static void       new_view_cb                    (GtkAction       *action,
						  gpointer         user_data);
static void       numbers_toggled_cb             (GtkAction       *action,
						  gpointer         user_data);
static void       marks_toggled_cb               (GtkAction       *action,
						  gpointer         user_data);
static void       margin_toggled_cb              (GtkAction       *action,
						  gpointer         user_data);
static void       hl_syntax_toggled_cb           (GtkAction       *action,
						  gpointer         user_data);
static void       hl_bracket_toggled_cb          (GtkAction       *action,
						  gpointer         user_data);
static void       hl_line_toggled_cb             (GtkAction       *action,
						  gpointer         user_data);
static void       draw_spaces_toggled_cb	 (GtkAction       *action,
						  gpointer         user_data);
static void       wrap_lines_toggled_cb          (GtkAction       *action,
						  gpointer         user_data);
static void       auto_indent_toggled_cb         (GtkAction       *action,
						  gpointer         user_data);
static void       insert_spaces_toggled_cb       (GtkAction       *action,
						  gpointer         user_data);
static void       tabs_toggled_cb                (GtkAction       *action,
						  GtkAction       *current,
						  gpointer         user_data);
static void       indent_toggled_cb              (GtkAction       *action,
						  GtkAction       *current,
						  gpointer         user_data);

static void       forward_string_cb              (GtkAction       *action,
						  gpointer         user_data);

static void       backward_string_cb             (GtkAction       *action,
						  gpointer         user_data);


static GtkWidget *create_view_window             (GtkSourceBuffer *buffer,
						  GtkSourceView   *from);


/* Actions & UI definition ---------------------------------------------------- */

static GtkActionEntry buffer_action_entries[] = {
	{ "Open", GTK_STOCK_OPEN, "_Open", "<control>O",
	  "Open a file", G_CALLBACK (open_file_cb) },
	{ "Quit", GTK_STOCK_QUIT, "_Quit", "<control>Q",
	  "Exit the application", G_CALLBACK (gtk_main_quit) }
};

static GtkActionEntry view_action_entries[] = {
	{ "FileMenu", NULL, "_File", NULL, NULL, NULL },
	{ "Print", GTK_STOCK_PRINT, "_Print", "<control>P",
	  "Print the current file", G_CALLBACK (print_file_cb) },
	{ "ViewMenu", NULL, "_View", NULL, NULL, NULL },
	{ "NewView", GTK_STOCK_NEW, "_New View", NULL,
	  "Create a new view of the file", G_CALLBACK (new_view_cb) },
	{ "TabWidth", NULL, "_Tab Width", NULL, NULL, NULL },
	{ "IndentWidth", NULL, "I_ndent Width", NULL, NULL, NULL },
	{ "SmartHomeEnd", NULL, "_Smart Home/End", NULL, NULL, NULL },
	{ "ForwardString", NULL, "_Forward to string toggle", "<control>S",
	  "Forward to the start or end of the next string", G_CALLBACK (forward_string_cb) },
	{ "BackwardString", NULL, "_Backward to string toggle", "<control><shift>S",
	  "Backward to the start or end of the next string", G_CALLBACK (backward_string_cb) }
};

static GtkToggleActionEntry toggle_entries[] = {
	{ "HlSyntax", NULL, "Highlight _Syntax", NULL,
	  "Toggle syntax highlighting",
	  G_CALLBACK (hl_syntax_toggled_cb), FALSE },
	{ "HlBracket", NULL, "Highlight Matching _Bracket", NULL,
	  "Toggle highlighting of matching bracket",
	  G_CALLBACK (hl_bracket_toggled_cb), FALSE },
	{ "ShowNumbers", NULL, "Show _Line Numbers", NULL,
	  "Toggle visibility of line numbers in the left margin",
	  G_CALLBACK (numbers_toggled_cb), FALSE },
	{ "ShowMarks", NULL, "Show Line _Marks", NULL,
	  "Toggle visibility of marks in the left margin",
	  G_CALLBACK (marks_toggled_cb), FALSE },
	{ "ShowMargin", NULL, "Show Right M_argin", NULL,
	  "Toggle visibility of right margin indicator",
	  G_CALLBACK (margin_toggled_cb), FALSE },
	{ "HlLine", NULL, "_Highlight Current Line", NULL,
	  "Toggle highlighting of current line",
	  G_CALLBACK (hl_line_toggled_cb), FALSE },
	{ "DrawSpaces", NULL, "_Draw Spaces", NULL,
	  "Draw Spaces",
	  G_CALLBACK (draw_spaces_toggled_cb), FALSE },
	{ "WrapLines", NULL, "_Wrap Lines", NULL,
	  "Toggle line wrapping",
	  G_CALLBACK (wrap_lines_toggled_cb), FALSE },
	{ "AutoIndent", NULL, "Enable _Auto Indent", NULL,
	  "Toggle automatic auto indentation of text",
	  G_CALLBACK (auto_indent_toggled_cb), FALSE },
	{ "InsertSpaces", NULL, "Insert _Spaces Instead of Tabs", NULL,
	  "Whether to insert space characters when inserting tabulations",
	  G_CALLBACK (insert_spaces_toggled_cb), FALSE }
};

static GtkRadioActionEntry tabs_radio_entries[] = {
	{ "TabWidth4", NULL, "4", NULL, "Set tabulation width to 4 spaces", 4 },
	{ "TabWidth6", NULL, "6", NULL, "Set tabulation width to 6 spaces", 6 },
	{ "TabWidth8", NULL, "8", NULL, "Set tabulation width to 8 spaces", 8 },
	{ "TabWidth10", NULL, "10", NULL, "Set tabulation width to 10 spaces", 10 },
	{ "TabWidth12", NULL, "12", NULL, "Set tabulation width to 12 spaces", 12 }
};

static GtkRadioActionEntry indent_radio_entries[] = {
	{ "IndentWidthUnset", NULL, "Use Tab Width", NULL, "Set indent width same as tab width", -1 },
	{ "IndentWidth4", NULL, "4", NULL, "Set indent width to 4 spaces", 4 },
	{ "IndentWidth6", NULL, "6", NULL, "Set indent width to 6 spaces", 6 },
	{ "IndentWidth8", NULL, "8", NULL, "Set indent width to 8 spaces", 8 },
	{ "IndentWidth10", NULL, "10", NULL, "Set indent width to 10 spaces", 10 },
	{ "IndentWidth12", NULL, "12", NULL, "Set indent width to 12 spaces", 12 }
};

static GtkRadioActionEntry smart_home_end_entries[] = {
	{ "SmartHomeEndDisabled", NULL, "Disabled", NULL,
	  "Smart Home/End disabled", GTK_SOURCE_SMART_HOME_END_DISABLED },
	{ "SmartHomeEndBefore", NULL, "Before", NULL,
	  "Smart Home/End before", GTK_SOURCE_SMART_HOME_END_BEFORE },
	{ "SmartHomeEndAfter", NULL, "After", NULL,
	  "Smart Home/End after", GTK_SOURCE_SMART_HOME_END_AFTER },
	{ "SmartHomeEndAlways", NULL, "Always", NULL,
	  "Smart Home/End always", GTK_SOURCE_SMART_HOME_END_ALWAYS }
};

static const gchar *view_ui_description =
"<ui>"
"  <menubar name=\"MainMenu\">"
"    <menu action=\"FileMenu\">"
"      <!--"
"      <menuitem action=\"PrintPreview\"/>"
"      -->"
"    </menu>"
"    <menu action=\"ViewMenu\">"
"      <menuitem action=\"NewView\"/>"
"      <separator/>"
"      <menuitem action=\"HlSyntax\"/>"
"      <menuitem action=\"HlBracket\"/>"
"      <menuitem action=\"ShowNumbers\"/>"
"      <menuitem action=\"ShowMarks\"/>"
"      <menuitem action=\"ShowMargin\"/>"
"      <menuitem action=\"HlLine\"/>"
"      <menuitem action=\"DrawSpaces\"/>"
"      <menuitem action=\"WrapLines\"/>"
"      <separator/>"
"      <menuitem action=\"AutoIndent\"/>"
"      <menuitem action=\"InsertSpaces\"/>"
"      <separator/>"
"      <menu action=\"TabWidth\">"
"        <menuitem action=\"TabWidth4\"/>"
"        <menuitem action=\"TabWidth6\"/>"
"        <menuitem action=\"TabWidth8\"/>"
"        <menuitem action=\"TabWidth10\"/>"
"        <menuitem action=\"TabWidth12\"/>"
"      </menu>"
"      <menu action=\"IndentWidth\">"
"        <menuitem action=\"IndentWidthUnset\"/>"
"        <menuitem action=\"IndentWidth4\"/>"
"        <menuitem action=\"IndentWidth6\"/>"
"        <menuitem action=\"IndentWidth8\"/>"
"        <menuitem action=\"IndentWidth10\"/>"
"        <menuitem action=\"IndentWidth12\"/>"
"      </menu>"
"      <separator/>"
"      <menu action=\"SmartHomeEnd\">"
"        <menuitem action=\"SmartHomeEndDisabled\"/>"
"        <menuitem action=\"SmartHomeEndBefore\"/>"
"        <menuitem action=\"SmartHomeEndAfter\"/>"
"        <menuitem action=\"SmartHomeEndAlways\"/>"
"      </menu>"
"      <separator/>"
"      <menuitem action=\"ForwardString\"/>"
"      <menuitem action=\"BackwardString\"/>"
"    </menu>"
"  </menubar>"
"</ui>";

static const gchar *buffer_ui_description =
"<ui>"
"  <menubar name=\"MainMenu\">"
"    <menu action=\"FileMenu\">"
"      <menuitem action=\"Open\"/>"
"      <menuitem action=\"Print\"/>"
"      <separator/>"
"      <menuitem action=\"Quit\"/>"
"    </menu>"
"    <menu action=\"ViewMenu\">"
"    </menu>"
"  </menubar>"
"</ui>";


/* File loading code ----------------------------------------------------------------- */

static gboolean
gtk_source_buffer_load_file (GtkSourceBuffer *buffer,
			     const gchar     *filename)
{
	GtkTextIter iter;
	gchar *contents;
	GError *error = NULL;

	g_return_val_if_fail (GTK_SOURCE_IS_BUFFER (buffer), FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);

	if (!g_file_get_contents (filename, &contents, NULL, &error))
	{
		GtkWidget *dialog = gtk_message_dialog_new (NULL,
							    0,
							    GTK_MESSAGE_ERROR,
							    GTK_BUTTONS_OK,
							    "%s\nFile %s",
							    error->message,
							    filename);

		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		g_error_free (error);
		return FALSE;
	}

	gtk_source_buffer_begin_not_undoable_action (buffer);
	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (buffer), contents, -1);
	gtk_source_buffer_end_not_undoable_action (buffer);
	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (buffer), FALSE);

	/* move cursor to the beginning */
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (buffer), &iter);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (buffer), &iter);

	g_free (contents);
	return TRUE;
}

static void
remove_all_marks (GtkSourceBuffer *buffer)
{
	GtkTextIter start;
	GtkTextIter end;

	gtk_text_buffer_get_bounds (GTK_TEXT_BUFFER (buffer), &start, &end);

	gtk_source_buffer_remove_source_marks (buffer, &start, &end, NULL);
}

static GtkSourceLanguage *
get_language_for_file (GtkTextBuffer *buffer,
		       const gchar   *filename)
{
	GtkSourceLanguageManager *manager;
	GtkSourceLanguage *language;
	GtkTextIter start;
	GtkTextIter end;
	gchar *text;
	gchar *content_type;
	gboolean result_uncertain;

	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_iter_at_offset (buffer, &end, 1024);
	text = gtk_text_buffer_get_slice (buffer, &start, &end, TRUE);

	content_type = g_content_type_guess (filename,
					     (guchar*) text,
					     strlen (text),
					     &result_uncertain);

	if (result_uncertain)
	{
		g_free (content_type);
		content_type = NULL;
	}

	manager = gtk_source_language_manager_get_default ();
	language = gtk_source_language_manager_guess_language (manager,
							       filename,
							       content_type);

	g_message ("Detected '%s' mime type for file %s, chose language %s",
		   content_type != NULL ? content_type : "(null)",
		   filename,
		   language != NULL ? gtk_source_language_get_id (language) : "(none)");

	g_free (content_type);
	g_free (text);
	return language;
}

static GtkSourceLanguage *
get_language_by_id (const gchar *id)
{
	GtkSourceLanguageManager *manager;
	manager = gtk_source_language_manager_get_default ();
	return gtk_source_language_manager_get_language (manager, id);
}

static GtkSourceLanguage *
get_language (GtkTextBuffer *buffer,
	      const gchar   *filename)
{
	GtkSourceLanguage *language = NULL;
	GtkTextIter start;
	GtkTextIter end;
	gchar *text;
	gchar *lang_string;

	gtk_text_buffer_get_start_iter (buffer, &start);
	end = start;
	gtk_text_iter_forward_line (&end);

#define LANG_STRING "gtk-source-lang:"

	text = gtk_text_iter_get_slice (&start, &end);
	lang_string = strstr (text, LANG_STRING);

	if (lang_string != NULL)
	{
		gchar **tokens;

		lang_string += strlen (LANG_STRING);
		g_strchug (lang_string);

		tokens = g_strsplit_set (lang_string, " \t\n", 2);

		if (tokens != NULL && tokens[0] != NULL)
		{
			language = get_language_by_id (tokens[0]);
		}

		g_strfreev (tokens);
	}

	if (language == NULL)
	{
		language = get_language_for_file (buffer, filename);
	}

	g_free (text);
	return language;
}

static void
print_language_style_ids (GtkSourceLanguage *language)
{
	gchar **styles;

	g_assert (language != NULL);

	styles = gtk_source_language_get_style_ids (language);

	if (styles == NULL)
	{
		g_print ("No styles in language '%s'\n",
			 gtk_source_language_get_name (language));
	}
	else
	{
		gchar **ids;
		g_print ("Styles in language '%s':\n",
			 gtk_source_language_get_name (language));

		for (ids = styles; *ids != NULL; ids++)
		{
			const gchar *name = gtk_source_language_get_style_name (language, *ids);

			g_print ("- %s (name: '%s')\n", *ids, name);
		}

		g_strfreev (styles);
	}

	g_print ("\n");
}

static void
open_file (GtkSourceBuffer *buffer,
	   const gchar     *filename)
{
	GtkSourceLanguage *language = NULL;
	gchar *absolute_filename;

	if (g_path_is_absolute (filename))
	{
		absolute_filename = g_strdup (filename);
	}
	else
	{
		gchar *curdir = g_get_current_dir ();
		absolute_filename = g_build_filename (curdir, filename, NULL);
		g_free (curdir);
	}

	remove_all_marks (buffer);

	if (!gtk_source_buffer_load_file (buffer, absolute_filename))
	{
		g_free (absolute_filename);
		return;
	}

	g_object_set_data_full (G_OBJECT (buffer),
				"filename", absolute_filename,
				(GDestroyNotify) g_free);

	language = get_language (GTK_TEXT_BUFFER (buffer), absolute_filename);
	gtk_source_buffer_set_language (buffer, language);

	if (language != NULL)
	{
		print_language_style_ids (language);
	}
	else
	{
		g_print ("No language found for file '%s'\n", absolute_filename);
	}
}


/* View action callbacks -------------------------------------------------------- */

static void
numbers_toggled_cb (GtkAction *action,
		    gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_show_line_numbers (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
marks_toggled_cb (GtkAction *action,
		  gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_show_line_marks (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
margin_toggled_cb (GtkAction *action,
		   gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_show_right_margin (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
hl_syntax_toggled_cb (GtkAction *action,
		      gpointer   user_data)
{
	GtkTextBuffer *buffer;
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	buffer = gtk_text_view_get_buffer (user_data);
	gtk_source_buffer_set_highlight_syntax (
		GTK_SOURCE_BUFFER (buffer),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
hl_bracket_toggled_cb (GtkAction *action,
		       gpointer   user_data)
{
	GtkTextBuffer *buffer;
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	buffer = gtk_text_view_get_buffer (user_data);
	gtk_source_buffer_set_highlight_matching_brackets (
		GTK_SOURCE_BUFFER (buffer),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
hl_line_toggled_cb (GtkAction *action,
		    gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_highlight_current_line (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
draw_spaces_toggled_cb (GtkAction *action,
		       	gpointer   user_data)
{
	gboolean draw_spaces;

	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	draw_spaces = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

	if (draw_spaces)
	{
		gtk_source_view_set_draw_spaces (GTK_SOURCE_VIEW (user_data),
						 GTK_SOURCE_DRAW_SPACES_ALL);
	}
	else
	{
		gtk_source_view_set_draw_spaces (GTK_SOURCE_VIEW (user_data),
						 0);
	}
}

static void
wrap_lines_toggled_cb (GtkAction *action,
		       gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_text_view_set_wrap_mode (user_data,
				     gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)) ?
					GTK_WRAP_WORD : GTK_WRAP_NONE);
}

static void
auto_indent_toggled_cb (GtkAction *action,
			gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_auto_indent (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
insert_spaces_toggled_cb (GtkAction *action,
			  gpointer   user_data)
{
	g_return_if_fail (GTK_IS_TOGGLE_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_insert_spaces_instead_of_tabs (
		GTK_SOURCE_VIEW (user_data),
		gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)));
}

static void
tabs_toggled_cb (GtkAction *action,
		 GtkAction *current,
		 gpointer   user_data)
{
	g_return_if_fail (GTK_IS_RADIO_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_tab_width (
		GTK_SOURCE_VIEW (user_data),
		gtk_radio_action_get_current_value (GTK_RADIO_ACTION (action)));
}

static void
indent_toggled_cb (GtkAction *action,
		   GtkAction *current,
		   gpointer   user_data)
{
	g_return_if_fail (GTK_IS_RADIO_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_indent_width (
		GTK_SOURCE_VIEW (user_data),
		gtk_radio_action_get_current_value (GTK_RADIO_ACTION (action)));
}

static void
smart_home_end_toggled_cb (GtkAction *action,
			   GtkAction *current,
			   gpointer   user_data)
{
	g_return_if_fail (GTK_IS_RADIO_ACTION (action) && GTK_SOURCE_IS_VIEW (user_data));
	gtk_source_view_set_smart_home_end (
		GTK_SOURCE_VIEW (user_data),
		gtk_radio_action_get_current_value (GTK_RADIO_ACTION (action)));
}

static void
new_view_cb (GtkAction *action,
	     gpointer   user_data)
{
	GtkSourceBuffer *buffer;
	GtkSourceView *view;
	GtkWidget *window;

	g_return_if_fail (GTK_SOURCE_IS_VIEW (user_data));

	view = GTK_SOURCE_VIEW (user_data);
	buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

	window = create_view_window (buffer, view);
	gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
	gtk_widget_show (window);
}

static void
forward_string_cb (GtkAction *action,
		   gpointer   user_data)
{
	GtkSourceBuffer *buffer;
	GtkSourceView *view;
	GtkTextIter iter;
	GtkTextMark *insert;

	g_return_if_fail (GTK_SOURCE_IS_VIEW (user_data));

	view = GTK_SOURCE_VIEW (user_data);
	buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
	insert = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (buffer));

	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (buffer),
	                                  &iter,
	                                  insert);

	if (gtk_source_buffer_iter_forward_to_context_class_toggle (buffer,
	                                                            &iter,
	                                                            "string"))
	{
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (buffer), &iter);
		gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view), insert);
	}
}

static void
backward_string_cb (GtkAction *action,
		    gpointer   user_data)
{
	GtkSourceBuffer *buffer;
	GtkSourceView *view;
	GtkTextIter iter;
	GtkTextMark *insert;

	g_return_if_fail (GTK_SOURCE_IS_VIEW (user_data));

	view = GTK_SOURCE_VIEW (user_data);
	buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));
	insert = gtk_text_buffer_get_insert (GTK_TEXT_BUFFER (buffer));

	gtk_text_buffer_get_iter_at_mark (GTK_TEXT_BUFFER (buffer),
	                                  &iter,
	                                  insert);

	if (gtk_source_buffer_iter_backward_to_context_class_toggle (buffer,
	                                                             &iter,
	                                                             "string"))
	{
		gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (buffer), &iter);
		gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view), insert);
	}
}

/* Buffer action callbacks ------------------------------------------------------------ */

static void
open_file_cb (GtkAction *action,
	      gpointer   user_data)
{
	GtkWidget *chooser;
	gint response;
	static gchar *last_dir;

	g_return_if_fail (GTK_SOURCE_IS_BUFFER (user_data));

	chooser = gtk_file_chooser_dialog_new ("Open file...",
					       NULL,
					       GTK_FILE_CHOOSER_ACTION_OPEN,
					       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					       GTK_STOCK_OPEN, GTK_RESPONSE_OK,
					       NULL);

	if (last_dir == NULL)
	{
		last_dir = g_strdup (TOP_SRCDIR "/gtksourceview");
	}

	if (last_dir != NULL && g_path_is_absolute (last_dir))
	{
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser),
						     last_dir);
	}

	response = gtk_dialog_run (GTK_DIALOG (chooser));

	if (response == GTK_RESPONSE_OK)
	{
		gchar *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));

		if (filename != NULL)
		{
			g_free (last_dir);
			last_dir = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (chooser));
			open_file (GTK_SOURCE_BUFFER (user_data), filename);
			g_free (filename);
		}
	}

	gtk_widget_destroy (chooser);
}

#define NON_BLOCKING_PAGINATION

#ifndef NON_BLOCKING_PAGINATION

static void
begin_print (GtkPrintOperation        *operation,
	     GtkPrintContext          *context,
	     GtkSourcePrintCompositor *compositor)
{
	gint n_pages;

	while (!gtk_source_print_compositor_paginate (compositor, context))
		;

	n_pages = gtk_source_print_compositor_get_n_pages (compositor);
	gtk_print_operation_set_n_pages (operation, n_pages);
}

#else

static gboolean
paginate (GtkPrintOperation        *operation,
	  GtkPrintContext          *context,
	  GtkSourcePrintCompositor *compositor)
{
	g_print ("Pagination progress: %.2f %%\n", gtk_source_print_compositor_get_pagination_progress (compositor) * 100.0);

	if (gtk_source_print_compositor_paginate (compositor, context))
	{
		gint n_pages;

		g_assert (gtk_source_print_compositor_get_pagination_progress (compositor) == 1.0);
		g_print ("Pagination progress: %.2f %%\n", gtk_source_print_compositor_get_pagination_progress (compositor) * 100.0);

		n_pages = gtk_source_print_compositor_get_n_pages (compositor);
		gtk_print_operation_set_n_pages (operation, n_pages);

		return TRUE;
	}

	return FALSE;
}

#endif

#define ENABLE_CUSTOM_OVERLAY

static void
draw_page (GtkPrintOperation        *operation,
	   GtkPrintContext          *context,
	   gint                      page_nr,
	   GtkSourcePrintCompositor *compositor)
{
#ifdef ENABLE_CUSTOM_OVERLAY

	/* This part of the code shows how to add a custom overlay to the
	   printed text generated by GtkSourcePrintCompositor */

	cairo_t *cr;
	PangoLayout *layout;
	PangoFontDescription *desc;
	PangoRectangle rect;


	cr = gtk_print_context_get_cairo_context (context);

	cairo_save (cr);

	layout = gtk_print_context_create_pango_layout (context);

	pango_layout_set_text (layout, "Draft", -1);

	desc = pango_font_description_from_string ("Sans Bold 120");
	pango_layout_set_font_description (layout, desc);
	pango_font_description_free (desc);


	pango_layout_get_extents (layout, NULL, &rect);

  	cairo_move_to (cr,
  		       (gtk_print_context_get_width (context) - ((double) rect.width / (double) PANGO_SCALE)) / 2,
  		       (gtk_print_context_get_height (context) - ((double) rect.height / (double) PANGO_SCALE)) / 2);

	pango_cairo_layout_path (cr, layout);

  	/* Font Outline */
	cairo_set_source_rgba (cr, 0.85, 0.85, 0.85, 0.80);
	cairo_set_line_width (cr, 0.5);
	cairo_stroke_preserve (cr);

	/* Font Fill */
	cairo_set_source_rgba (cr, 0.8, 0.8, 0.8, 0.60);
	cairo_fill (cr);

	g_object_unref (layout);
	cairo_restore (cr);
#endif

	/* To print page_nr you only need to call the following function */
	gtk_source_print_compositor_draw_page (compositor, context, page_nr);
}

static void
end_print (GtkPrintOperation        *operation,
	   GtkPrintContext          *context,
	   GtkSourcePrintCompositor *compositor)
{
	g_object_unref (compositor);
}

#define LINE_NUMBERS_FONT_NAME	"Sans 8"
#define HEADER_FONT_NAME	"Sans 11"
#define FOOTER_FONT_NAME	"Sans 11"
#define BODY_FONT_NAME		"Monospace 9"

/*
#define SETUP_FROM_VIEW
*/

#undef SETUP_FROM_VIEW


static void
print_file_cb (GtkAction *action,
	       gpointer   user_data)
{
	GtkSourceView *view;
	GtkSourceBuffer *buffer;
	GtkSourcePrintCompositor *compositor;
	GtkPrintOperation *operation;
	const gchar *filename;
	gchar *basename;

	g_return_if_fail (GTK_SOURCE_IS_VIEW (user_data));

	view = GTK_SOURCE_VIEW (user_data);

	buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

	filename = g_object_get_data (G_OBJECT (buffer), "filename");
	basename = g_filename_display_basename (filename);

#ifdef SETUP_FROM_VIEW
	compositor = gtk_source_print_compositor_new_from_view (view);
#else

	compositor = gtk_source_print_compositor_new (buffer);

	gtk_source_print_compositor_set_tab_width (compositor,
						   gtk_source_view_get_tab_width (view));

	gtk_source_print_compositor_set_wrap_mode (compositor,
						   gtk_text_view_get_wrap_mode (GTK_TEXT_VIEW (view)));

	gtk_source_print_compositor_set_print_line_numbers (compositor, 1);

	gtk_source_print_compositor_set_body_font_name (compositor,
							BODY_FONT_NAME);

	/* To test line numbers font != text font */
	gtk_source_print_compositor_set_line_numbers_font_name (compositor,
								LINE_NUMBERS_FONT_NAME);

	gtk_source_print_compositor_set_header_format (compositor,
						       TRUE,
						       "Printed on %A",
						       "test-widget",
						       "%F");

	gtk_source_print_compositor_set_footer_format (compositor,
						       TRUE,
						       "%T",
						       basename,
						       "Page %N/%Q");

	gtk_source_print_compositor_set_print_header (compositor, TRUE);
	gtk_source_print_compositor_set_print_footer (compositor, TRUE);

	gtk_source_print_compositor_set_header_font_name (compositor,
							  HEADER_FONT_NAME);

	gtk_source_print_compositor_set_footer_font_name (compositor,
							  FOOTER_FONT_NAME);
#endif
	operation = gtk_print_operation_new ();

	gtk_print_operation_set_job_name (operation, basename);

	gtk_print_operation_set_show_progress (operation, TRUE);

#ifndef NON_BLOCKING_PAGINATION
  	g_signal_connect (G_OBJECT (operation), "begin-print",
			  G_CALLBACK (begin_print), compositor);
#else
  	g_signal_connect (G_OBJECT (operation), "paginate",
			  G_CALLBACK (paginate), compositor);
#endif
	g_signal_connect (G_OBJECT (operation), "draw-page",
			  G_CALLBACK (draw_page), compositor);
	g_signal_connect (G_OBJECT (operation), "end-print",
			  G_CALLBACK (end_print), compositor);

	gtk_print_operation_run (operation,
				 GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
				 NULL, NULL);

	g_object_unref (operation);
	g_free (basename);
}

/* View UI callbacks ------------------------------------------------------------------ */

static void
update_cursor_position_info (GtkTextBuffer *buffer,
			     GtkSourceView *view)
{
	gchar *msg;
	gint row;
	gint chars;
	guint col;
	GtkTextIter iter;
	GtkLabel *pos_label;
	gchar **classes;
	gchar **classes_ptr;
	GString *classes_str;

	gtk_text_buffer_get_iter_at_mark (buffer,
					  &iter,
					  gtk_text_buffer_get_insert (buffer));

	chars = gtk_text_iter_get_offset (&iter);
	row = gtk_text_iter_get_line (&iter) + 1;
	col = gtk_source_view_get_visual_column (view, &iter) + 1;

	classes = gtk_source_buffer_get_context_classes_at_iter (GTK_SOURCE_BUFFER (buffer),
	                                                         &iter);

	classes_str = g_string_new ("");

	for (classes_ptr = classes; classes_ptr != NULL && *classes_ptr != NULL; classes_ptr++)
	{
		if (classes_ptr != classes)
		{
			g_string_append (classes_str, ", ");
		}

		g_string_append_printf (classes_str, "%s", *classes_ptr);
	}

	g_strfreev (classes);

	msg = g_strdup_printf ("char: %d, line: %d, column: %u, classes: %s",
			       chars,
			       row,
			       col,
			       classes_str->str);

	pos_label = GTK_LABEL (g_object_get_data (G_OBJECT (view), "pos_label"));
	gtk_label_set_text (pos_label, msg);

	g_free (msg);
	g_string_free (classes_str, TRUE);
}

static void
mark_set_cb (GtkTextBuffer *buffer,
	     GtkTextIter   *cursoriter,
	     GtkTextMark   *mark,
	     GtkSourceView *view)
{
	if (mark == gtk_text_buffer_get_insert (buffer))
	{
		update_cursor_position_info (buffer, view);
	}
}

static gboolean
window_deleted_cb (GtkWidget     *widget,
		   GdkEvent      *event,
		   GtkSourceView *view)
{
	if (g_list_nth_data (windows, 0) == widget)
	{
		/* Main (first in the list) window was closed, so exit
		 * the application */
		gtk_main_quit ();
	}
	else
	{
		GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER (
			gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

		windows = g_list_remove (windows, widget);

		/* deinstall buffer motion signal handlers */
		g_signal_handlers_disconnect_matched (buffer,
						      G_SIGNAL_MATCH_DATA,
						      0, /* signal_id */
						      0, /* detail */
						      NULL, /* closure */
						      NULL, /* func */
						      view);

		/* we return FALSE since we want the window destroyed */
		return FALSE;
	}

	return TRUE;
}

static void
line_mark_activated (GtkSourceGutter *gutter,
                     GtkTextIter     *iter,
                     GdkEventButton  *event,
                     GtkSourceView   *view)
{
	GtkSourceBuffer *buffer;
	GSList *mark_list;
	const gchar *mark_type;

	mark_type = event->button == 1 ? MARK_TYPE_1 : MARK_TYPE_2;

	buffer = GTK_SOURCE_BUFFER (gtk_text_view_get_buffer (GTK_TEXT_VIEW (view)));

	/* get the marks already in the line */
	mark_list = gtk_source_buffer_get_source_marks_at_line (buffer,
								gtk_text_iter_get_line (iter),
								mark_type);

	if (mark_list != NULL)
	{
		/* just take the first and delete it */
		gtk_text_buffer_delete_mark (GTK_TEXT_BUFFER (buffer),
					     GTK_TEXT_MARK (mark_list->data));
	}
	else
	{
		/* no mark found: create one */
		gtk_source_buffer_create_source_mark (buffer,
						      NULL,
						      mark_type,
						      iter);
	}

	g_slist_free (mark_list);
}

static void
bracket_matched (GtkSourceBuffer           *buffer,
                 GtkTextIter               *iter,
                 GtkSourceBracketMatchType  state)
{
	GEnumClass *eclass;
	GEnumValue *evalue;

	g_return_if_fail (iter != NULL);

	eclass = G_ENUM_CLASS (g_type_class_ref (GTK_SOURCE_TYPE_BRACKET_MATCH_TYPE));
	evalue = g_enum_get_value (eclass, state);

	g_print ("Bracket match state: '%s'\n", evalue->value_nick);

	if (state == GTK_SOURCE_BRACKET_MATCH_FOUND)
	{
		g_print ("Matched bracket: '%c' at row: %"G_GINT32_FORMAT", col: %"G_GINT32_FORMAT"\n",
		         gtk_text_iter_get_char (iter),
		         gtk_text_iter_get_line (iter) + 1,
		         gtk_text_iter_get_line_offset (iter) + 1);
	}

	g_type_class_unref (eclass);
}

/* Window creation functions -------------------------------------------------------- */

static gchar *
mark_tooltip_func (GtkSourceMarkAttributes *attrs,
                   GtkSourceMark           *mark,
                   GtkSourceView           *view)
{
	GtkTextBuffer *buffer;
	GtkTextIter iter;
	gint line;
	gint column;

	buffer = gtk_text_mark_get_buffer (GTK_TEXT_MARK (mark));

	gtk_text_buffer_get_iter_at_mark (buffer, &iter, GTK_TEXT_MARK (mark));
	line = gtk_text_iter_get_line (&iter) + 1;
	column = gtk_text_iter_get_line_offset (&iter);

	if (g_strcmp0 (gtk_source_mark_get_category (mark), MARK_TYPE_1) == 0)
	{
		return g_strdup_printf ("Line: %d, Column: %d", line, column);
	}
	else
	{
		return g_strdup_printf ("<b>Line</b>: %d\n<i>Column</i>: %d", line, column);
	}
}

static void
add_source_mark_attributes (GtkSourceView *view)
{
	GdkRGBA color;
	GtkSourceMarkAttributes *attrs;

	gdk_rgba_parse (&color, "lightgreen");

	attrs = gtk_source_mark_attributes_new ();
	gtk_source_mark_attributes_set_background (attrs, &color);

	/* FIXME: GTK_STOCK_YES is deprecated, but doesn't have a replacement as
	 * a named icon.
	 */
	gtk_source_mark_attributes_set_stock_id (attrs, GTK_STOCK_YES);

	g_signal_connect (attrs,
	                  "query-tooltip-markup",
	                  G_CALLBACK (mark_tooltip_func),
	                  view);

	gtk_source_view_set_mark_attributes (view, MARK_TYPE_1, attrs, 1);

	gdk_rgba_parse (&color, "pink");

	attrs = gtk_source_mark_attributes_new ();

	gtk_source_mark_attributes_set_background (attrs, &color);

	/* FIXME: same for GTK_STOCK_NO */
	gtk_source_mark_attributes_set_stock_id (attrs, GTK_STOCK_NO);

	g_signal_connect (attrs,
	                  "query-tooltip-markup",
	                  G_CALLBACK (mark_tooltip_func),
	                  view);

	gtk_source_view_set_mark_attributes (view, MARK_TYPE_2, attrs, 2);
}

static GtkWidget *
create_view_window (GtkSourceBuffer *buffer,
		    GtkSourceView   *from)
{
	GtkWidget *window;
	GtkWidget *sw;
	GtkWidget *view;
	GtkWidget *vbox;
	GtkWidget *pos_label;
	GtkWidget *menu;
	PangoFontDescription *font_desc = NULL;
	GtkAccelGroup *accel_group;
	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;
	GError *error = NULL;

	g_return_val_if_fail (GTK_SOURCE_IS_BUFFER (buffer), NULL);
	g_return_val_if_fail (from == NULL || GTK_SOURCE_IS_VIEW (from), NULL);

	/* window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (window), 0);
	gtk_window_set_title (GTK_WINDOW (window), "GtkSourceView Demo");
	windows = g_list_append (windows, window);

	/* view */
	view = gtk_source_view_new_with_buffer (buffer);

	if (style_scheme != NULL)
	{
		gtk_source_buffer_set_style_scheme (buffer, style_scheme);
	}

	g_signal_connect (buffer,
			  "mark-set",
			  G_CALLBACK (mark_set_cb),
			  view);

	g_signal_connect (buffer,
			  "changed",
			  G_CALLBACK (update_cursor_position_info),
			  view);

	g_signal_connect (view,
			  "line-mark-activated",
			  G_CALLBACK (line_mark_activated),
			  view);

	g_signal_connect (window,
			  "delete-event",
			  G_CALLBACK (window_deleted_cb),
			  view);

	g_signal_connect (buffer,
			  "bracket-matched",
			  G_CALLBACK (bracket_matched),
			  NULL);

	/* action group and UI manager */
	action_group = gtk_action_group_new ("ViewActions");
	gtk_action_group_add_actions (action_group, view_action_entries,
				      G_N_ELEMENTS (view_action_entries), view);
	gtk_action_group_add_toggle_actions (action_group, toggle_entries,
					     G_N_ELEMENTS (toggle_entries), view);
	gtk_action_group_add_radio_actions (action_group, tabs_radio_entries,
					    G_N_ELEMENTS (tabs_radio_entries),
					    -1, G_CALLBACK (tabs_toggled_cb), view);
	gtk_action_group_add_radio_actions (action_group, indent_radio_entries,
					    G_N_ELEMENTS (indent_radio_entries),
					    -1, G_CALLBACK (indent_toggled_cb), view);
	gtk_action_group_add_radio_actions (action_group, smart_home_end_entries,
					    G_N_ELEMENTS (smart_home_end_entries),
					    -1, G_CALLBACK (smart_home_end_toggled_cb), view);

	ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 0);
	g_object_unref (action_group);

	/* save a reference to the ui manager in the window for later use */
	g_object_set_data_full (G_OBJECT (window), "ui_manager",
				ui_manager, (GDestroyNotify) g_object_unref);

	accel_group = gtk_ui_manager_get_accel_group (ui_manager);
	gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

	if (!gtk_ui_manager_add_ui_from_string (ui_manager, view_ui_description, -1, &error))
	{
		g_error ("building view ui failed: %s", error->message);
	}

	/* misc widgets */
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                             GTK_SHADOW_IN);
	pos_label = gtk_label_new ("Position");
	g_object_set_data (G_OBJECT (view), "pos_label", pos_label);
	menu = gtk_ui_manager_get_widget (ui_manager, "/MainMenu");

	/* layout widgets */
	gtk_container_add (GTK_CONTAINER (window), vbox);
	gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);
	gtk_container_add (GTK_CONTAINER (sw), view);
	gtk_box_pack_start (GTK_BOX (vbox), pos_label, FALSE, FALSE, 0);

	/* setup view */
	font_desc = pango_font_description_from_string ("monospace");
	if (font_desc != NULL)
	{
		gtk_widget_override_font (view, font_desc);
		pango_font_description_free (font_desc);
	}

	/* change view attributes to match those of from */
	if (from != NULL)
	{
		gchar *tmp;
		gint i;
		GtkAction *action;

		action = gtk_action_group_get_action (action_group, "ShowNumbers");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_source_view_get_show_line_numbers (from));

		action = gtk_action_group_get_action (action_group, "ShowMarks");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_source_view_get_show_line_marks (from));

		action = gtk_action_group_get_action (action_group, "ShowMargin");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_source_view_get_show_right_margin (from));

		action = gtk_action_group_get_action (action_group, "HlLine");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_source_view_get_highlight_current_line (from));

		action = gtk_action_group_get_action (action_group, "WrapLines");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_text_view_get_wrap_mode (GTK_TEXT_VIEW (from)) != GTK_WRAP_NONE);

		action = gtk_action_group_get_action (action_group, "AutoIndent");
		gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
					      gtk_source_view_get_auto_indent (from));

		action = gtk_action_group_get_action (action_group, "InsertSpaces");
		gtk_toggle_action_set_active (
			GTK_TOGGLE_ACTION (action),
			gtk_source_view_get_insert_spaces_instead_of_tabs (from));

		tmp = g_strdup_printf ("TabWidth%d", gtk_source_view_get_tab_width (from));
		action = gtk_action_group_get_action (action_group, tmp);
		if (action)
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
		g_free (tmp);

		i = gtk_source_view_get_indent_width (from);
		tmp = i < 0 ? g_strdup ("IndentWidthUnset") : g_strdup_printf ("IndentWidth%d", i);
		action = gtk_action_group_get_action (action_group, tmp);
		if (action)
			gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
		g_free (tmp);
	}

	add_source_mark_attributes (GTK_SOURCE_VIEW (view));

	gtk_widget_show_all (vbox);

	return window;
}

static GtkWidget *
create_main_window (GtkSourceBuffer *buffer)
{
	GtkWidget *window;
	GtkAction *action;
	GtkUIManager *ui_manager;
	GtkActionGroup *action_group;
	GList *groups;
	GError *error = NULL;

	window = create_view_window (buffer, NULL);
	ui_manager = g_object_get_data (G_OBJECT (window), "ui_manager");

	/* buffer action group */
	action_group = gtk_action_group_new ("BufferActions");
	gtk_action_group_add_actions (action_group, buffer_action_entries,
				      G_N_ELEMENTS (buffer_action_entries), buffer);
	gtk_ui_manager_insert_action_group (ui_manager, action_group, 1);
	g_object_unref (action_group);

	/* merge buffer ui */
	if (!gtk_ui_manager_add_ui_from_string (ui_manager, buffer_ui_description, -1, &error))
	{
		g_error ("building buffer ui failed: %s", error->message);
	}

	/* preselect menu checkitems */
	groups = gtk_ui_manager_get_action_groups (ui_manager);
	/* retrieve the view action group at position 0 in the list */
	action_group = g_list_nth_data (groups, 0);

	action = gtk_action_group_get_action (action_group, "HlSyntax");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action),
				      gtk_source_buffer_get_highlight_syntax (buffer));

	action = gtk_action_group_get_action (action_group, "HlBracket");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	action = gtk_action_group_get_action (action_group, "ShowNumbers");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	action = gtk_action_group_get_action (action_group, "ShowMarks");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	action = gtk_action_group_get_action (action_group, "ShowMargin");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);

	action = gtk_action_group_get_action (action_group, "AutoIndent");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	action = gtk_action_group_get_action (action_group, "InsertSpaces");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);

	action = gtk_action_group_get_action (action_group, "TabWidth8");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	action = gtk_action_group_get_action (action_group, "IndentWidthUnset");
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);

	return window;
}

/* Program entry point ------------------------------------------------------------ */

int
main (int argc, char *argv[])
{
	GtkWidget *window;
	GtkSourceLanguageManager *lm;
	GtkSourceStyleSchemeManager *sm;
	GtkSourceBuffer *buffer;

	gboolean no_syntax = FALSE;
	gchar *builtin_lang_dirs[] = {TOP_SRCDIR "/data/language-specs", NULL};
	gchar *builtin_sm_dirs[] = {TOP_SRCDIR "/data/styles", NULL};
	gchar **dirs;
	const gchar * const * schemes;
	gboolean use_default_paths = FALSE;

	gchar *style_scheme_id = NULL;
	GOptionContext *context;

	GOptionEntry entries[] = {
	  { "no-syntax", 'n', 0, G_OPTION_ARG_NONE, &no_syntax, "Disable syntax highlighting", NULL},
	  { "style-scheme", 's', 0, G_OPTION_ARG_STRING, &style_scheme_id, "Style scheme name to use", "SCHEME"},
	  { "default-paths", 'd', 0, G_OPTION_ARG_NONE, &use_default_paths, "Use default search paths", NULL},
	  { NULL }
	};

	context = g_option_context_new ("- test GtkSourceView widget");
	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_add_group (context, gtk_get_option_group (TRUE));
	g_option_context_parse (context, &argc, &argv, NULL);

	/* we do not use defaults so we don't need to install the library */
	dirs = use_default_paths ? NULL : builtin_lang_dirs;
	lm = gtk_source_language_manager_get_default ();
	gtk_source_language_manager_set_search_path (lm, dirs);

	dirs = use_default_paths ? NULL : builtin_sm_dirs;

	sm = gtk_source_style_scheme_manager_get_default ();
	gtk_source_style_scheme_manager_set_search_path (sm, dirs);

	if (!use_default_paths)
	{
		gtk_source_style_scheme_manager_append_search_path (sm, TOP_SRCDIR "/tests/test-scheme.xml");
	}

	schemes = gtk_source_style_scheme_manager_get_scheme_ids (sm);
	g_print ("Available style schemes:\n");
	while (*schemes != NULL)
	{
		const gchar* const *authors;
		gchar *authors_str = NULL;

		style_scheme = gtk_source_style_scheme_manager_get_scheme (sm, *schemes);

		authors = gtk_source_style_scheme_get_authors (style_scheme);
		if (authors != NULL)
		{
			authors_str = g_strjoinv (", ", (gchar **)authors);
		}

		g_print (" - [%s] %s: %s\n",
			 gtk_source_style_scheme_get_id (style_scheme),
			 gtk_source_style_scheme_get_name (style_scheme),
			 gtk_source_style_scheme_get_description (style_scheme) ?
				gtk_source_style_scheme_get_description (style_scheme) : "");

		if (authors_str != NULL)
		{
			g_print ("   by %s\n",  authors_str);
			g_free (authors_str);
		}

		++schemes;
	}
	g_print("\n");

	if (style_scheme_id != NULL)
	{
		style_scheme = gtk_source_style_scheme_manager_get_scheme (sm, style_scheme_id);
	}
	else
	{
		style_scheme = NULL;
	}

	/* create buffer */
	buffer = gtk_source_buffer_new (NULL);

	gtk_source_buffer_set_highlight_syntax (buffer, !no_syntax);

	if (argc > 1)
	{
		open_file (buffer, argv [1]);
	}
	else
	{
		open_file (buffer, TOP_SRCDIR "/gtksourceview/gtksourcebuffer.c");
	}

	/* create first window */
	window = create_main_window (buffer);
	gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);
	gtk_widget_show (window);

	/* ... and action! */
	gtk_main ();

	/* cleanup */
	g_list_foreach (windows, (GFunc) gtk_widget_destroy, NULL);
	g_list_free (windows);
	g_object_unref (buffer);

	g_free (style_scheme_id);

	return 0;
}
