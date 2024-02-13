#noinst_LTLIBRARIES = libcppmyth.la
lib_LTLIBRARIES    = libcppmyth.la
# LIB_VERSION   : [$1-$3].[$3].[$2]
# LIB_SOVERSION : [$1-$3]
libcppmyth_la_LDFLAGS = -version-info 19:1:17

AM_CPPFLAGS = -fPIC -Wall -Wextra \
	-I@abs_srcdir@/src

libcppmyth_la_SOURCES = \
	src/private/os/threads/threadpool.cpp \
	src/private/os/threads/latch.cpp \
	src/private/mythdto/mythdto.cpp \
	src/private/builtin.c \
        src/private/compressor.cpp \
        src/private/debug.cpp \
	src/private/jsonparser.cpp \
	src/private/mythjsonbinder.cpp \
	src/private/ringbuffer.cpp \
        src/private/securesocket.cpp \
	src/private/socket.cpp \
	src/private/uriparser.cpp \
	src/private/wscontent.cpp \
	src/private/wsrequest.cpp \
	src/private/wsresponse.cpp \
	src/proto/mythprotobase.cpp \
	src/proto/mythprotoevent.cpp \
	src/proto/mythprotomonitor.cpp \
	src/proto/mythprotoplayback.cpp \
	src/proto/mythprotorecorder.cpp \
	src/proto/mythprototransfer.cpp \
	src/mythcontrol.cpp \
	src/mytheventhandler.cpp \
	src/mythfileplayback.cpp \
	src/mythsharedptr.cpp \
	src/mythlivetvplayback.cpp \
	src/mythlocked.cpp \
	src/mythrecordingplayback.cpp \
	src/mythtypes.cpp \
	src/mythwsapi.cpp \
	src/mythwsstream.cpp

cppmythheadersdir = $(includedir)/cppmyth
cppmythheaders_HEADERS = \
	src/local_config.h \
	src/mythlocked.h \
	src/mythsharedptr.h \
	src/mythtypes.h \
	src/mythcontrol.h \
	src/mytheventhandler.h \
	src/mythfileplayback.h \
	src/mythlivetvplayback.h \
	src/mythrecordingplayback.h \
	src/mythwsapi.h \
	src/mythdebug.h \
	src/mythstream.h \
	src/mythwsstream.h

protoheadersdir = $(includedir)/cppmyth/proto
protoheaders_HEADERS = \
	src/proto/mythprotobase.h \
	src/proto/mythprotomonitor.h \
	src/proto/mythprotorecorder.h \
	src/proto/mythprotoevent.h \
	src/proto/mythprotoplayback.h \
	src/proto/mythprototransfer.h
