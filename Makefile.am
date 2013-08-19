ACLOCAL_AMFLAGS = -I m4
SUBDIRS = yyast

include am/doxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)
EXTRA_DIST = autogen.sh $(DX_CONFIG)

doc: doxygen-run

clean: clean-recursive doxygen-clean

doxygen-clean:
	@rm -f $(DX_CLEANFILES)

