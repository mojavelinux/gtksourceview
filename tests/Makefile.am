@GNOME_CODE_COVERAGE_RULES@

AM_CPPFLAGS = 				\
	-DDATADIR=\""$(datadir)"\" 	\
	-DG_LOG_DOMAIN=\"GtkSourceView\"\
	-I$(top_srcdir) -I$(srcdir) 	\
	-DTOP_SRCDIR=\""$(ABS_TOP_SRCDIR)"\"\
	$(DISABLE_DEPRECATED_CFLAGS)	\
	$(WARN_CFLAGS) 			\
	$(DEP_CFLAGS)			\
	$(TESTS_CFLAGS)

noinst_PROGRAMS = $(TEST_PROGS) $(UNIT_TEST_PROGS)
TESTS = $(UNIT_TEST_PROGS)

BUILT_SOURCES =				\
	test-completion-resources.c	\
	test-search-resources.c

test-completion-resources.c: test-completion.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies $(srcdir)/test-completion.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $(srcdir)/test-completion.gresource.xml

test-search-resources.c: test-search.gresource.xml $(shell $(GLIB_COMPILE_RESOURCES) --generate-dependencies $(srcdir)/test-search.gresource.xml)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir) --generate-source $(srcdir)/test-search.gresource.xml

TEST_PROGS = test-completion
test_completion_SOURCES =		\
	test-completion.c		\
	test-completion-resources.c
test_completion_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

TEST_PROGS += test-search
test_search_SOURCES =		\
	test-search.c		\
	test-search-resources.c
test_search_LDADD =						\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

TEST_PROGS += test-search-performances
test_search_performances_SOURCES = \
	test-search-performances.c
test_search_performances_LDADD =				\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

TEST_PROGS += test-widget
test_widget_SOURCES = test-widget.c
test_widget_LDADD = 			\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS = test-buffer
test_buffer_SOURCES = test-buffer.c
test_buffer_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-completion-model
test_completion_model_SOURCES =	test-completion-model.c
test_completion_model_LDADD =					\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(top_builddir)/gtksourceview/libgtksourceview-private.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-completion-words
test_completion_words_SOURCES = test-completion-words.c
test_completion_words_LDADD =					\
	$(top_builddir)/gtksourceview/completion-providers/words/libgtksourcecompletionwords.la	\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-language
test_language_SOURCES =		\
	test-language.c
test_language_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-languagemanager
test_languagemanager_SOURCES =		\
	test-languagemanager.c
test_languagemanager_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-mark
test_mark_SOURCES = test-mark.c
test_mark_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-printcompositor
test_printcompositor_SOURCES =		\
	test-printcompositor.c
test_printcompositor_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-regex
test_regex_SOURCES = test-regex.c
test_regex_LDADD = 						\
	$(top_builddir)/gtksourceview/libgtksourceview-private.la	\
	$(DEP_LIBS)			\
	$(GTK_MAC_LIBS)		\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-region
test_region_SOURCES = test-region.c
test_region_LDADD = 						\
	$(top_builddir)/gtksourceview/libgtksourceview-private.la	\
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-search-context
test_search_context_SOURCES = test-search-context.c
test_search_context_LDADD =						\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la		\
	$(top_builddir)/gtksourceview/libgtksourceview-private.la	\
	$(DEP_LIBS)							\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-styleschememanager
test_styleschememanager_SOURCES =		\
	test-styleschememanager.c
test_styleschememanager_LDADD = 		\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la \
	$(DEP_LIBS)			\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-undo-manager
test_undo_manager_SOURCES = test-undo-manager.c
test_undo_manager_LDADD =					\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

UNIT_TEST_PROGS += test-utils
test_utils_SOURCES = test-utils.c
test_utils_LDADD =						\
	$(top_builddir)/gtksourceview/libgtksourceview-3.0.la	\
	$(DEP_LIBS)						\
	$(TESTS_LIBS)

python_tests =			\
	test-completion.py	\
	test-widget.py

EXTRA_DIST =				\
	language-specs/test-empty.lang	\
	language-specs/test-full.lang	\
	styles/classic.xml		\
	test-completion.gresource.xml	\
	test-completion.ui		\
	test-search.gresource.xml	\
	test-search.ui			\
	$(python_tests)

-include $(top_srcdir)/git.mk
