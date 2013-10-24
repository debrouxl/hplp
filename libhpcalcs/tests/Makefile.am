## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src
#	@HPCABLES_CFLAGS@ @HPFILES_CFLAGS@

EXTRA_DIST =

noinst_PROGRAMS = test_hpcalcs torture_hpcalcs

test_hpcalcs_LDADD = $(top_builddir)/src/libhpcalcs.la
#	@HPCABLES_LIBS@ @HPFILES_LIBS@

torture_hpcalcs_LDADD = $(top_builddir)/src/libhpcalcs.la
#	@HPCABLES_LIBS@ @HPFILES_LIBS@

TESTS = torture_hpcalcs
