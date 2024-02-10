noinst_PROGRAMS = cppmythtest testintrinsic testwget

AM_CPPFLAGS = -I@abs_top_srcdir@/cppmyth/src

cppmythtest_SOURCES = src/test.cpp

cppmythtest_LDFLAGS = -static

cppmythtest_LDADD   = \
	@abs_top_builddir@/cppmyth/libcppmyth.la

testintrinsic_SOURCES = src/testintrinsic.cpp

testintrinsic_LDFLAGS = -static

testintrinsic_LDADD   = \
        @abs_top_builddir@/cppmyth/libcppmyth.la

testwget_SOURCES = src/testwget.cpp

testwget_LDFLAGS = -static

testwget_LDADD   = \
        @abs_top_builddir@/cppmyth/libcppmyth.la
