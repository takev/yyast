
AM_CFLAGS = -Wall -W -Werror -pedantic -Wno-long-long -Wno-unused -Wno-multichar -std=c99

DEFAULT_INCLUDES=-I$(top_builddir)

lib_LTLIBRARIES = libyyast.la

libyyast_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libyyast_la_SOURCES = yyast.c utils.c error.c count.c literal.c node.c main.c

library_includedir=$(includedir)/yyast-$(VERSION)/yyast
nobase_library_include_HEADERS = yyast.h types.h error.h utils.h count.h literal.h node.h main.h config.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = yyast.pc

