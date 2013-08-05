ACLOCAL_AMFLAGS = -I m4

pkgconfdir = $(libdir)/pkgconfig

pkgconf_DATA = libkeynote-@KN_MAJOR_VERSION@.@KN_MINOR_VERSION@.pc

SUBDIRS = build inc src docs

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	NEWS \
	README \
	autogen.sh \
	libkeynote.pc.in

distclean-local:
	rm -rf *.cache *~ *.pc

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

