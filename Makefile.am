ACLOCAL_AMFLAGS = -I m4
SUBDIRS = yyast pyext

include am/doxygen.am

EXTRA_DIST = autogen.sh $(DX_CONFIG)

doc: doxygen-run

clean: clean-recursive doxygen-clean

doxygen-clean:
	-rm -fr $(DX_CLEANFILES)

