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
