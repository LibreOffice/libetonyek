# Process this file with autoconf to create configure.

AC_PREREQ([2.65])

# ====================
# Version informations
# ====================
m4_define([libetonyek_version_major],[0])
m4_define([libetonyek_version_minor],[1])
m4_define([libetonyek_version_micro],[1])
m4_define([libetonyek_version],[libetonyek_version_major.libetonyek_version_minor.libetonyek_version_micro])

# =============
# Automake init
# =============
AC_INIT([libetonyek],[libetonyek_version])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz dist-bzip2])
AM_SILENT_RULES([yes])
AC_LANG([C++])

# ===========================
# Find required base packages
# ===========================
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT([win32-dll disable-static pic-only])
AC_CANONICAL_HOST

PKG_PROG_PKG_CONFIG([0.20])

AC_PROG_EGREP
AC_PROG_SED

AC_PATH_PROG(GPERF, gperf)
if test -z "$GPERF"; then
    AC_MSG_ERROR([gperf not found but needed. Install it.])
fi

# ===============
# Find librevenge
# ===============
PKG_CHECK_MODULES([REVENGE],[
	librevenge-0.0
])
AC_SUBST([REVENGE_CFLAGS])
AC_SUBST([REVENGE_LIBS])

# ============
# Find libxml2
# ============
PKG_CHECK_MODULES([XML],[
    libxml-2.0
])

# =========
# Find zlib
# =========
PKG_CHECK_MODULES([ZLIB],[zlib],[],[
    AC_CHECK_HEADER(zlib.h, [ZLIB_CFLAGS=],
        [AC_MSG_ERROR(zlib.h not found. install zlib)], [])
    AC_CHECK_LIB(z, inflate, [ ZLIB_LIBS=-lz ],
        [AC_MSG_ERROR(zlib not found or functional)], [])
])
AC_SUBST(ZLIB_CFLAGS)
AC_SUBST(ZLIB_LIBS)

# ==================
# Find boost headers
# ==================
saved_CPPFLAGS="${CPPFLAGS}"
CPPFLAGS="${BOOST_CFLAGS} ${CPPFLAGS}"
AC_CHECK_HEADERS(
        boost/any.hpp \
        boost/bind.hpp \
        boost/function.hpp \
        boost/lexical_cast.hpp \
        boost/logic/tribool.hpp \
        boost/make_shared.hpp \
        boost/numeric/conversion/cast.hpp \
        boost/optional.hpp \
        boost/ref.hpp \
        boost/scoped_ptr.hpp \
        boost/shared_ptr.hpp \
        boost/spirit/include/classic_core.hpp \
        boost/unordered_map.hpp,
    [],
    [AC_MSG_ERROR([Required boost headers not found.])],
    []
)
CPPFLAGS="${saved_CPPFLAGS}"
AC_SUBST([BOOST_CFLAGS])

# =================================
# Libtool/Version Makefile settings
# =================================
AC_SUBST(ETONYEK_MAJOR_VERSION, [libetonyek_version_major])
AC_SUBST(ETONYEK_MINOR_VERSION, [libetonyek_version_minor])
AC_SUBST(ETONYEK_MICRO_VERSION, [libetonyek_version_micro])
AC_SUBST(KN_VERSION, [libetonyek_version])
# AC_SUBST(LT_RELEASE, [libetonyek_version_major.libetonyek_version_minor])
LT_CURRENT=`expr 100 '*' libetonyek_version_major + libetonyek_version_minor`
# For 1.0.0 comment the first line and uncomment the second
LT_AGE=0
# LT_AGE=libetonyek_version_minor
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION, [libetonyek_version_micro])
AC_SUBST(LT_AGE)

# ==========================
# Platform check for windows
# ==========================
AC_MSG_CHECKING([for native Win32])
AS_CASE([$host],
	[*-*-mingw*], [
		native_win32=yes
		LIBETONYEK_WIN32_RESOURCE=libetonyek-win32res.lo
		KEY2RAW_WIN32_RESOURCE=key2raw-win32res.lo
		KEY2TEXT_WIN32_RESOURCE=key2text-win32res.lo
		KEY2XHTML_WIN32_RESOURCE=key2xhtml-win32res.lo
		NUMBERS2CSV_WIN32_RESOURCE=numbers2csv-win32res.lo
		NUMBERS2RAW_WIN32_RESOURCE=numbers2raw-win32res.lo
		NUMBERS2TEXT_WIN32_RESOURCE=numbers2text-win32res.lo
		PAGES2HTML_WIN32_RESOURCE=pages2html-win32res.lo
		PAGES2RAW_WIN32_RESOURCE=pages2raw-win32res.lo
		PAGES2TEXT_WIN32_RESOURCE=pages2text-win32res.lo
	], [
		native_win32=no
		LIBETONYEK_WIN32_RESOURCE=
		KEY2RAW_WIN32_RESOURCE=
		KEY2TEXT_WIN32_RESOURCE=
		KEY2XHTML_WIN32_RESOURCE=
		NUMBERS2CSV_WIN32_RESOURCE=
		NUMBERS2RAW_WIN32_RESOURCE=
		NUMBERS2TEXT_WIN32_RESOURCE=
		PAGES2HTML_WIN32_RESOURCE=
		PAGES2RAW_WIN32_RESOURCE=
		PAGES2TEXT_WIN32_RESOURCE=
	]
)
AC_MSG_RESULT([$native_win32])
AM_CONDITIONAL(OS_WIN32, [test "x$native_win32" = "xyes"])
AC_SUBST(LIBETONYEK_WIN32_RESOURCE)
AC_SUBST(KEY2RAW_WIN32_RESOURCE)
AC_SUBST(KEY2TEXT_WIN32_RESOURCE)
AC_SUBST(KEY2XHTML_WIN32_RESOURCE)
AC_SUBST(NUMBERS2CSV_WIN32_RESOURCE)
AC_SUBST(NUMBERS2RAW_WIN32_RESOURCE)
AC_SUBST(NUMBERS2TEXT_WIN32_RESOURCE)
AC_SUBST(PAGES2HTML_WIN32_RESOURCE)
AC_SUBST(PAGES2RAW_WIN32_RESOURCE)
AC_SUBST(PAGES2TEXT_WIN32_RESOURCE)

AC_MSG_CHECKING([for Win32 platform in general])
AS_CASE([$host],
	[*-*-mingw*|*-*-cygwin*], [platform_win32=yes],
	[platform_win32=no]
)
AC_MSG_RESULT([$platform_win32])
AM_CONDITIONAL([PLATFORM_WIN32], [test "x$platform_win32" = "xyes"])

AS_IF([test "x$platform_win32" = "xyes"],
    [],
    [
        AC_MSG_CHECKING([for -fvisibility=hidden compiler flag])
        saved_CXXFLAGS="$CXXFLAGS"
        CXXFLAGS="$CXXFLAGS -fvisibility=hidden"
        AC_TRY_COMPILE([], [],
            [
                AC_MSG_RESULT([yes])
                CXXFLAGS="$CXXFLAGS -DLIBETONYEK_VISIBILITY"
            ],
            [
                AC_MSG_RESULT([no])
                CXXFLAGS="$saved_CXXFLAGS"
            ]
        )
    ]
)

# ================
# Check for cflags
# ================
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--enable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=no]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CFLAGS="$CFLAGS -Werror"
	CXXFLAGS="$CXXFLAGS -Werror"
])
AC_ARG_ENABLE([weffc],
	[AS_HELP_STRING([--disable-weffc], [ Disable -Weffc++ warnings, usefull when using an old version of gcc or of boost])],
	[enable_weffc="$enableval"],
	[enable_weffc=yes]
)
AC_ARG_ENABLE([wparanoic],
	[AS_HELP_STRING([--enable-wparanoic], [Enable a lot of warnings...])],
	[enable_wparanoic="$enableval"],
	[enable_wparanoic=no]
)
# Courtesy of Glib: Ensure MSVC-compatible struct packing convention
# is used when compiling for Win32 with gcc.
AS_IF([test "x$native_win32" = "xyes"], [
	AC_CHECK_TOOL(WINDRES, windres)
	AS_IF([test x"$GCC" = xyes], [
		AC_MSG_CHECKING([how to get MSVC-compatible struct packing])
		AS_IF([test -z "$ac_cv_prog_CC"], [
			our_gcc="$CC"
		], [
			our_gcc="$ac_cv_prog_CC"
		])
		AS_IF([$our_gcc -v --help 2>/dev/null | grep ms-bitfields >/dev/null], [
			msnative_struct="-mms-bitfields"
			CFLAGS="$CFLAGS $msnative_struct"
			CXXFLAGS="$CXXFLAGS $msnative_struct"
			AC_MSG_RESULT([${msnative_struct}])
		], [
			AC_MSG_RESULT([no way])
			AC_MSG_WARN([produced libraries might be incompatible with MSVC-compiled code])
		])
	])
	CFLAGS="$CFLAGS -Wall -Wextra -pedantic"
		CXXFLAGS="$CXXFLAGS -Wall -Wextra -pedantic -Wshadow -Wunused-variable"
		AS_IF([test x"$enable_weffc" != "xno"], [
			CXXFLAGS="$CXXFLAGS -Weffc++"
		])
		AS_IF([test x"$enable_wparanoic" != "xno"], [
			CXXFLAGS="$CXXFLAGS -Wcast-align -Wcast-qual -Wchar-subscripts"
			CXXFLAGS="$CXXFLAGS -Wcomment -Wconversion -Wdisabled-optimization"
			CXXFLAGS="$CXXFLAGS -Wfloat-equal -Wformat -Wformat=2"
			CXXFLAGS="$CXXFLAGS -Wformat-nonliteral -Wformat-security"
			CXXFLAGS="$CXXFLAGS -Wformat-y2k -Wimport -Winit-self -Winvalid-pch"
			CXXFLAGS="$CXXFLAGS -Wmissing-braces -Wmissing-field-initializers"
			CXXFLAGS="$CXXFLAGS -Wmissing-format-attribute -Wmissing-include-dirs"
			CXXFLAGS="$CXXFLAGS -Wmissing-noreturn -Wpacked -Wparentheses"
			CXXFLAGS="$CXXFLAGS -Wpointer-arith -Wredundant-decls -Wreturn-type"
			CXXFLAGS="$CXXFLAGS -Wsequence-point -Wsign-compare -Wstrict-aliasing"
			CXXFLAGS="$CXXFLAGS -Wstrict-aliasing=2 -Wswitch -Wswitch-default"
			CXXFLAGS="$CXXFLAGS -Wswitch-enum -Wtrigraphs -Wunknown-pragmas -Wunused"
			CXXFLAGS="$CXXFLAGS -Wunused-function -Wunused-label -Wunused-parameter"
			CXXFLAGS="$CXXFLAGS -Wunused-value -Wvariadic-macros"
			CXXFLAGS="$CXXFLAGS -Wvolatile-register-var -Wwrite-strings"
		])
], [
	AS_IF([test x"$GCC" = xyes], [
		# Be tough with warnings and produce less careless code
		CFLAGS="$CFLAGS -Wall -Wextra -pedantic"
		CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wshadow -pedantic"
	])
])

# ============
# Debug switch
# ============
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [Turn on debugging])],
	[enable_debug="$enableval"],
	[enable_debug=no]
)
AS_IF([test "x$enable_debug" = "xyes"], [
	DEBUG_CXXFLAGS="-DDEBUG -g"
	CXXFLAGS="$CXXFLAGS -O0"
	CFLAGS="$CFLAGS -O0"
], [
	DEBUG_CXXFLAGS="-DNDEBUG"
])
AC_SUBST(DEBUG_CXXFLAGS)

# ==========
# Unit tests
# ==========
AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--enable-tests], [Build and run unit tests])],
    [enable_tests="$enableval"],
    [enable_tests=yes]
)
AS_IF([test "x$enable_tests" = "xyes"], [
    PKG_CHECK_MODULES([CPPUNIT], [cppunit])
], [])
AC_SUBST([CPPUNIT_CFLAGS])
AC_SUBST([CPPUNIT_LIBS])
AM_CONDITIONAL(TESTS, [test "x$enable_tests" = "xyes"])

# =============
# Documentation
# =============
AC_ARG_WITH(docs,
	[AS_HELP_STRING([--without-docs], [Do not build documentation])],
	[with_docs="$withval"],
	[AS_IF([test "x$native_win32" = "xyes"], [with_docs=no], [with_docs=yes])]
)
AS_IF([test "x$with_docs" != "xno"], [
	AC_PATH_PROG(DOXYGEN, [doxygen])
	AS_IF([test -z "$DOXYGEN"], [
		AC_MSG_WARN([*** Could not find doxygen in your PATH.])
		AC_MSG_WARN([*** The documentation will not be built.])
		build_docs=no
	], [build_docs=yes])
], [build_docs=no])
AM_CONDITIONAL([WITH_LIBETONYEK_DOCS], [test "x$build_docs" != "xno"])

# ================
# Conversion tools
# ================
AC_ARG_WITH(tools,
	[AS_HELP_STRING([--without-tools], [Do not build command line conversion tools])],
	[with_tools="$withval"],
	[]
)
AS_IF([test "x$with_tools" != "xno"], [build_tools=yes], [build_tools=no])
AM_CONDITIONAL([WITH_LIBETONYEK_TOOLS], [test "x$build_tools" != "xno"])

AS_IF([test "x$build_tools" != "xno"], [
    PKG_CHECK_MODULES([REVENGE_GENERATORS],[
        librevenge-generators-0.0
    ])
    PKG_CHECK_MODULES([REVENGE_STREAM],[
        librevenge-stream-0.0
    ])
])
AC_SUBST([REVENGE_GENERATORS_CFLAGS])
AC_SUBST([REVENGE_GENERATORS_LIBS])
AC_SUBST([REVENGE_STREAM_CFLAGS])
AC_SUBST([REVENGE_STREAM_LIBS])

# =====================
# Prepare all .in files
# =====================
AC_CONFIG_FILES([
Makefile
src/Makefile
src/conv/Makefile
src/conv/csv/numbers2csv.rc
src/conv/csv/Makefile
src/conv/html/Makefile
src/conv/html/pages2html.rc
src/conv/raw/Makefile
src/conv/raw/key2raw.rc
src/conv/raw/numbers2raw.rc
src/conv/raw/pages2raw.rc
src/conv/svg/Makefile
src/conv/svg/key2xhtml.rc
src/conv/text/Makefile
src/conv/text/key2text.rc
src/conv/text/numbers2text.rc
src/conv/text/pages2text.rc
src/lib/Makefile
src/lib/libetonyek.rc
src/test/Makefile
inc/Makefile
inc/libetonyek/Makefile
build/Makefile
build/win32/Makefile
docs/Makefile
docs/doxygen/Makefile
libetonyek-$ETONYEK_MAJOR_VERSION.$ETONYEK_MINOR_VERSION.pc:libetonyek.pc.in
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
	debug:           ${enable_debug}
	docs:            ${build_docs}
	tests:           ${enable_tests}
	tools:           ${build_tools}
	werror:          ${enable_werror}
==============================================================================
])
