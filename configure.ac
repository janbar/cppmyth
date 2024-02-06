AC_PREREQ([2.60])
AC_INIT([cppmyth], [2.17.0], [jlbarriere68@gmail.com])

AM_INIT_AUTOMAKE([1.10 foreign subdir-objects])

AC_CONFIG_SRCDIR([cppmyth/src/mythtypes.cpp])

# Configure external packages.

# Checks for programs.
AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LIBTOOL
AM_CONDITIONAL([GCC], [test x$GCC = xyes])

# Checks for pthread.
AC_SEARCH_LIBS([pthread_create],[pthread],, AC_MSG_ERROR([required library 'pthread' is missing]))


# Checks for rt.
AC_SEARCH_LIBS(clock_gettime, [rt])

no=0
AC_CHECK_HEADERS(zlib.h, [], [no=1])
AC_SEARCH_LIBS(deflate, z, [], [no=1])
if test $no == 0
then
    HAVE_ZLIB=1
else
    HAVE_ZLIB=0
    AC_MSG_ERROR([required library 'zlib' is missing])
fi
AC_SUBST([HAVE_ZLIB])

HAVE_OPENSSL=0
AC_SUBST([HAVE_OPENSSL])

# Check for inttypes.
AC_CHECK_HEADERS([inttypes.h],, AC_MSG_ERROR([required header 'inttypes.h' is missing]))

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
case $ac_cv_c_inline in
    yes) CC_INLINE=inline;;
    no) CC_INLINE=;;
    *) CC_INLINE=$ac_cv_c_inline;;
esac
AC_SUBST([CC_INLINE])

# Checks for library functions.
AC_CHECK_FUNCS([timegm localtime_r gmtime_r])

case "$ac_cv_func_timegm" in
     yes) HAVE_TIMEGM=1;;
     *) HAVE_TIMEGM=0;;
esac
AC_SUBST([HAVE_TIMEGM])

case "$ac_cv_func_localtime_r" in
     yes) HAVE_LOCALTIME_R=1;;
     *) HAVE_LOCATIME_R=0;;
esac
AC_SUBST([HAVE_LOCALTIME_R])

case "$ac_cv_func_gmtime_r" in
     yes) HAVE_GMTIME_R=1;;
     *) HAVE_GMTIME_R=0;;
esac
AC_SUBST([HAVE_GMTIME_R])

# Check for intrinsics
AC_MSG_CHECKING([for __sync_add_and_fetch(temp, 1)])
AC_TRY_COMPILE([],[long* temp=0; __sync_add_and_fetch(temp, 1);],
    [have_builtin_sync_add_and_fetch=yes],
    [have_builtin_sync_add_and_fetch=no])
AC_MSG_RESULT($have_builtin_sync_add_and_fetch)
if test "x$have_builtin_sync_add_and_fetch" = "xyes"; then
    AC_DEFINE(HAS_BUILTIN_SYNC_ADD_AND_FETCH, 1,
        [Define to 1 if your compiler supports the __sync_add_and_fetch() intrinsic.])
fi

AC_MSG_CHECKING([for __sync_sub_and_fetch(temp, 1)])
AC_TRY_COMPILE([],[long* temp=0; __sync_sub_and_fetch(temp, 1);],
    [have_builtin_sync_sub_and_fetch=yes],
    [have_builtin_sync_sub_and_fetch=no])
AC_MSG_RESULT($have_builtin_sync_sub_and_fetch)
if test "x$have_builtin_sync_sub_and_fetch" = "xyes"; then
    AC_DEFINE(HAS_BUILTIN_SYNC_SUB_AND_FETCH, 1,
        [Define to 1 if your compiler supports the __sync_sub_and_fetch() intrinsic.])
fi

### Require c++ standard
m4_include([m4/ax_check_compile_flag.m4])
AX_CHECK_COMPILE_FLAG([-std=c++11], [compile_flag_cxx11=yes])
if test "x${compile_flag_cxx11}" = "xyes"; then
  CXXFLAGS="-std=c++11 $CXXFLAGS"
fi

PACKAGE_VERSION_MAJOR=$(echo $PACKAGE_VERSION | awk -F. '{print $1}')
PACKAGE_VERSION_MINOR=$(echo $PACKAGE_VERSION | awk -F. '{
  sub("-.*","",$2); print $2}')
PACKAGE_VERSION_PATCH=$(echo $PACKAGE_VERSION | awk -F. '{
  sub("-.*","",$3); print $3 ~ /^@<:@0-9@:>@+$/ ? $3 : 0}')
AC_SUBST([PACKAGE_VERSION_MAJOR])
AC_SUBST([PACKAGE_VERSION_MINOR])
AC_SUBST([PACKAGE_VERSION_PATCH])

AC_CONFIG_FILES([
        Makefile
        cppmyth/cppmyth.pc
        cppmyth/Makefile
        cppmyth/src/local_config.h
        test/Makefile
        demo/Makefile
])

AC_OUTPUT
