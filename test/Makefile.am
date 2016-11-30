noinst_PROGRAMS = cppmythtest intrinsictest

AM_CPPFLAGS = -I@abs_top_srcdir@/cppmyth/src

cppmythtest_SOURCES = src/test.cpp

cppmythtest_LDFLAGS = -static

cppmythtest_LDADD   = \
	@abs_top_builddir@/cppmyth/libcppmyth.la

intrinsictest_SOURCES = src/intrinsictest.cpp

intrinsictest_LDFLAGS = -static

intrinsictest_LDADD   = \
        @abs_top_builddir@/cppmyth/libcppmyth.la

