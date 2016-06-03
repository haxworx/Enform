MAINTAINERCLEANFILES = Makefile.in

AM_CFLAGS = \
-I$(top_builddir) \
-I$(top_builddir)/src/bin \
-I$(top_srcdir) \
-I$(top_srcdir)/src/bin \
@e_cflags@ \
@cf_cflags@ \
@VALGRIND_CFLAGS@ \
@EDJE_DEF@ \
-DPACKAGE_BIN_DIR=\"@PACKAGE_BIN_DIR@\" \
-DPACKAGE_LIB_DIR=\"@PACKAGE_LIB_DIR@\" \
-DPACKAGE_DATA_DIR=\"@PACKAGE_DATA_DIR@\" \
-DLOCALE_DIR=\"@LOCALE_DIR@\" \
-DPACKAGE_SYSCONF_DIR=\"@PACKAGE_SYSCONF_DIR@\"

internal_bindir = $(libdir)/enform/utils
internal_bin_PROGRAMS = \
enform_fm 

enform_fm_SOURCES = \
e_fm_main.h \
e_fm_ipc.h \
e_fm_ipc.c \
e_fm_main.c \
e_fm_shared_codec.c \
e_fm_shared_device.c \
e_user.c \
e_sha1.c

enform_fm_LDADD = @e_libs@ -lm
