AC_INIT([amcompareaddr], [1.0], [alex.yegorov@gmail.com])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC

# Link against libconfig
LIBS="$LIBS -lconfig"
CPPFLAGS="$CPPFLAGS -I/usr/include"
AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_CHECK_LIB(config)
AC_OUTPUT
