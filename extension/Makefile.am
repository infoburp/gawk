#
# extension/Makefile.am --- automake input file for gawk
#
# Copyright (C) 1995-2006, 2012 the Free Software Foundation, Inc.
#
# This file is part of GAWK, the GNU implementation of the
# AWK Programming Language.
#
# GAWK is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# GAWK is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
#

## Process this file with automake to produce Makefile.in.

AM_CPPFLAGS = -I$(srcdir)/..

# This variable insures that aclocal runs
# correctly after changing configure.ac
ACLOCAL_AMFLAGS = -I m4

# Note: rwarray does not currently compile.

pkgextension_LTLIBRARIES =	\
	filefuncs.la	\
	fnmatch.la	\
	fork.la		\
	inplace.la	\
	ordchr.la	\
	readdir.la	\
	readfile.la	\
	revoutput.la	\
	revtwoway.la	\
	rwarray.la	\
	testext.la	\
	time.la

MY_MODULE_FLAGS = -module -avoid-version -no-undefined
# on Cygwin, gettext requires that we link with -lintl 
MY_LIBS = $(LTLIBINTL)

filefuncs_la_SOURCES  = filefuncs.c stack.h stack.c gawkfts.h \
			gawkfts.c gawkdirfd.h
filefuncs_la_LDFLAGS  = $(MY_MODULE_FLAGS)
filefuncs_la_LIBADD   = $(MY_LIBS)

fnmatch_la_SOURCES    = fnmatch.c
fnmatch_la_LDFLAGS    = $(MY_MODULE_FLAGS)
fnmatch_la_LIBADD     = $(MY_LIBS)

fork_la_SOURCES       = fork.c
fork_la_LDFLAGS       = $(MY_MODULE_FLAGS)
fork_la_LIBADD        = $(MY_LIBS)

inplace_la_SOURCES    = inplace.c
inplace_la_LDFLAGS    = $(MY_MODULE_FLAGS)
inplace_la_LIBADD     = $(MY_LIBS)

ordchr_la_SOURCES     = ordchr.c
ordchr_la_LDFLAGS     = $(MY_MODULE_FLAGS)
ordchr_la_LIBADD      = $(MY_LIBS)

readdir_la_SOURCES    = readdir.c gawkdirfd.h
readdir_la_LDFLAGS    = $(MY_MODULE_FLAGS)
readdir_la_LIBADD     = $(MY_LIBS)

readfile_la_SOURCES   = readfile.c
readfile_la_LDFLAGS   = $(MY_MODULE_FLAGS)
readfile_la_LIBADD    = $(MY_LIBS)

revoutput_la_SOURCES  = revoutput.c
revoutput_la_LDFLAGS  = $(MY_MODULE_FLAGS)
revoutput_la_LIBADD   = $(MY_LIBS)

revtwoway_la_SOURCES  = revtwoway.c
revtwoway_la_LDFLAGS  = $(MY_MODULE_FLAGS)
revtwoway_la_LIBADD   = $(MY_LIBS)

rwarray_la_SOURCES    = rwarray.c
rwarray_la_LDFLAGS    = $(MY_MODULE_FLAGS)
rwarray_la_LIBADD     = $(MY_LIBS)

time_la_SOURCES       = time.c
time_la_LDFLAGS       = $(MY_MODULE_FLAGS)
time_la_LIBADD        = $(MY_LIBS)

testext_la_SOURCES    = testext.c
testext_la_LDFLAGS    = $(MY_MODULE_FLAGS)
testext_la_LIBADD     = $(MY_LIBS)

install-data-hook:
	for i in $(pkgextension_LTLIBRARIES) ; do \
		$(RM) $(DESTDIR)$(pkgextensiondir)/$$i ; \
	done

# Keep the uninstall check working:
uninstall-so:
	$(RM) $(pkgextensiondir)/*.so

uninstall-recursive: uninstall-so

EXTRA_DIST = build-aux/config.rpath  \
	ChangeLog \
	ChangeLog.0 \
	fts.3 \
	README.fts

dist_man_MANS = \
	filefuncs.3am fnmatch.3am fork.3am inplace.3am \
	ordchr.3am readdir.3am readfile.3am revoutput.3am \
	revtwoway.3am rwarray.3am time.3am

# gettext requires this
SUBDIRS =
