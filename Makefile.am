NULL =
MAINTAINERCLEANFILES =
EXTRA_DIST =
noinst_DATA =

AM_CPPFLAGS =					\
        -I$(top_srcdir)				\
        -DPREFIX=\"$(prefix)\"			\
        -DLIBDIR=\"$(libdir)\"			\
        -DG_LOG_DOMAIN=\"libgd\"		\
        -DG_DISABLE_DEPRECATED			\
        $(LIBGD_CFLAGS)				\
        $(NULL)

noinst_LTLIBRARIES = libgd.la
libgd_la_LIBADD = $(LIBGD_LIBS) $(LIBM)
libgd_la_LDFLAGS = -static -avoid-version
nodist_libgd_la_SOURCES =

if LIBGD_GTK_HACKS
gtk_hacks_sources =                             \
        libgd/gd-entry-focus-hack.c             \
        libgd/gd-entry-focus-hack.h             \
        $(NULL)

nodist_libgd_la_SOURCES += $(gtk_hacks_sources)
EXTRA_DIST += $(gtk_hacks_sources)
endif

if LIBGD_MAIN_ICON_VIEW
main_icon_view_sources =			\
	libgd/gd-main-icon-view.c		\
	libgd/gd-main-icon-view.h		\
	libgd/gd-main-view-generic.c            \
	libgd/gd-main-view-generic.h            \
	libgd/gd-toggle-pixbuf-renderer.c	\
	libgd/gd-toggle-pixbuf-renderer.h	\
	libgd/gd-two-lines-renderer.c		\
	libgd/gd-two-lines-renderer.h		\
	$(NULL)

nodist_libgd_la_SOURCES += $(main_icon_view_sources)
EXTRA_DIST += $(main_icon_view_sources)
endif

if LIBGD_MAIN_TOOLBAR
main_toolbar_sources =				\
	libgd/gd-main-toolbar.c			\
	libgd/gd-main-toolbar.h			\
	$(NULL)

nodist_libgd_la_SOURCES += $(main_toolbar_sources)
EXTRA_DIST += $(main_toolbar_sources)
endif

if LIBGD_MARGIN_CONTAINER
margin_container_sources =			\
	libgd/gd-margin-container.c		\
	libgd/gd-margin-container.h		\
	$(NULL)

nodist_libgd_la_SOURCES += $(margin_container_sources)
EXTRA_DIST += $(margin_container_sources)
endif

if LIBGD_TAGGED_ENTRY
tagged_entry_sources =				\
	libgd/gd-tagged-entry.c			\
	libgd/gd-tagged-entry.h			\
	$(NULL)

nodist_libgd_la_SOURCES += $(tagged_entry_sources)
EXTRA_DIST += $(tagged_entry_sources)
endif

if LIBGD_GIR
include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Gd.gir

Gd.gir: libgd.la Makefile
Gd_gir_NAMESPACE = Gd
Gd_gir_VERSION = 1.0
Gd_gir_LIBS = libgd.la
Gd_gir_CFLAGS = $(AM_CPPFLAGS)
Gd_gir_SCANNERFLAGS =				\
       --warn-all				\
       --symbol-prefix=gd			\
       --identifier-prefix=Gd			\
        --c-include="libgd/gd.h"	        \
       $(NULL)
Gd_gir_INCLUDES = $(LIBGD_GIR_INCLUDES)
Gd_gir_FILES = $(nodist_libgd_la_SOURCES)

noinst_DATA += $(INTROSPECTION_GIRS)
MAINTAINERCLEANFILES += $(INTROSPECTION_GIRS)
endif

if LIBGD_VAPI
VAPIS = gd.vapi

gd.vapi: Gd.gir
	$(AM_V_GEN)$(VAPIGEN)			\
		--library gd			\
		--pkg gtk+-3.0			\
		$<

noinst_DATA += $(VAPIS)
MAINTAINERCLEANFILES += $(VAPIS)
endif

-include $(srcdir)/git.mk
