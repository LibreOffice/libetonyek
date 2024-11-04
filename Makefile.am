ACLOCAL_AMFLAGS = -I m4

pkgconfdir = $(libdir)/pkgconfig

pkgconf_DATA = libetonyek-@ETONYEK_MAJOR_VERSION@.@ETONYEK_MINOR_VERSION@.pc

SUBDIRS = build inc src docs

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	FEATURES.md \
	NEWS \
	README.md \
	autogen.sh \
	libetonyek.pc.in

distclean-local:
	rm -rf *.cache *~ *.pc

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

astyle:
	astyle --options=astyle.options \*.cpp \*.h
