## Process this file with automake to produce Makefile.in

SUBDIRS = 

# libraries and files to compile and install
lib_LTLIBRARIES = libhpcalcs.la
libhpcalcs_includedir = $(includedir)/hplp
libhpcalcs_include_HEADERS = \
	hplibs.h export.h hpfiles.h hpcables.h hpcalcs.h hpopers.h \
	filetypes.h \
	prime_cmd.h typesprime.h

# build instructions
libhpcalcs_la_CPPFLAGS = -I$(top_srcdir)/intl \
	-DLOCALEDIR=\"$(datadir)/locale\" \
	@HIDAPI_CFLAGS@ \
	-DHPCALCS_EXPORTS
#	@HPCABLES_CFLAGS@ @HPFILES_CFLAGS@

libhpcalcs_la_LDFLAGS = -no-undefined -version-info @LT_LIBVERSION@
libhpcalcs_la_LIBADD = @LTLIBINTL@ \
	@HIDAPI_LIBS@
#	@HPCABLES_LIBS@ @HPFILES_LIBS@

if OS_WIN32
	libhpcalcs_la_DEPENDENCIES += ../build/mingw/hpcalcs-rc.o
	libhpcalcs_la_LDFLAGS += -Wl,../build/mingw/hpcalcs-rc.o
endif

libhpcalcs_la_SOURCES = \
	hplibs.h export.h hpfiles.h hpcables.h hpcalcs.h hpopers.h \
	error.h gettext.h internal.h logging.h utils.h \
	filetypes.h \
	prime_cmd.h typesprime.h \
	hpfiles.c hpcables.c hpcalcs.c hpopers.c \
	error.c logging.c utils.c type2str.c \
	filetypes.c typesprime.c \
	link_prime_hid.c link_nul.c \
	prime_rpkt.c prime_vpkt.c prime_cmd.c calc_prime.c \
	calc_none.c
