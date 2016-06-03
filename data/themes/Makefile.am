AUTOMAKE_OPTIONS     = 1.4 foreign
MAINTAINERCLEANFILES = Makefile.in

EDJE_CC = @edje_cc@
EDJE_FLAGS_VERBOSE_ = 
EDJE_FLAGS_VERBOSE_0 = 
EDJE_FLAGS_VERBOSE_1 = -v
EDJE_FLAGS = $(EDJE_FLAGS_$(V)) \
-id $(top_srcdir)/data/themes/img \
-id $(top_srcdir)/data/themes/default \
-fd $(top_srcdir)/data/themes/default \
@EDJE_DEF@

filesdir = $(datadir)/enform/data/themes
files_DATA = default.edj \
dark.edj \
detourious.edj

EXTRA_DIST = 

default:
	echo "Don't build by default (RAM)"
clean-local:
	echo "Don't clean (RAM)"
