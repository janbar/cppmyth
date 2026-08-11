noinst_PROGRAMS = cppmythtest wgettest

AM_CPPFLAGS = -I@abs_top_srcdir@/cppmyth/src

cppmythtest_SOURCES = cppmythtest.cpp

cppmythtest_LDFLAGS = -static

cppmythtest_LDADD   = \
	@abs_top_builddir@/cppmyth/libcppmyth.la

wgettest_SOURCES = wgettest.cpp

wgettest_LDFLAGS = -static

wgettest_LDADD   = \
	@abs_top_builddir@/cppmyth/libcppmyth.la
