
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-long-long -Wno-unused -std=c99

DEFAULT_INCLUDES=-I$(top_builddir)

lib_LTLIBRARIES = libyyast.la

libyyast_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libyyast_la_SOURCES = yyast.c utils.c count.c token.c node.c

library_includedir=$(includedir)/yyast-$(VERSION)/yyast
nobase_library_include_HEADERS = yyast.h types.h utils.h count.h token.h node.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = yyast.pc

