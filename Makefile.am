SUBDIRS = src po

EXTRA_DIST =  \
	IDEAS \
	QUOTES \
	LICENSE.DEJAVU \
	DEPENDENCIES \
	EXTERNAL

ACLOCAL_AMFLAGS = -I m4

dist-hook: ChangeLog.hg

ChangeLog.hg:
	hg log -R $(srcdir) -r reverse\(..tip\) --style changelog > $(distdir)/ChangeLog.hg
