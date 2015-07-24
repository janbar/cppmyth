noinst_PROGRAMS = livetvdemo

livetvdemo_SOURCES = src/demo.cpp

AM_CPPFLAGS = -I@abs_top_srcdir@/cppmyth/src

livetvdemo_LDFLAGS = -static

livetvdemo_LDADD   = \
	@abs_top_builddir@/cppmyth/libcppmyth.la
