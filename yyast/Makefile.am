
DEFAULT_INCLUDES=-I$(top_srcdir)

AM_CFLAGS = -Wall -W -pedantic -Wno-sign-compare -Wno-long-long -Wno-unused -std=c99 $(DEFAULT_INCLUDES)

lib_LTLIBRARIES = libyyast.la
bin_PROGRAMS = yadump

libyyast_la_LDFLAGS = -version-info $(SHARED_VERSION_INFO)
libyyast_la_SOURCES = yyast.c utils.c error.c count.c leaf.c node.c header.c main.c

# Do not link against yyast, as yyast was designed to be only linked against a lex & yacc program.
yadump_SOURCES = yadump.c

library_includedir=$(includedir)/yyast-$(VERSION)/yyast
library_include_HEADERS = yyast.h types.h error.h utils.h count.h leaf.h node.h header.h main.h config.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = yyast.pc

