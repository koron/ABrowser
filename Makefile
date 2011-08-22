# vi:set ts=8 sts=8 sw=8 tw=0:
#
# Makefile - ABrowser tags・パッケージ作成用
#
# Last Change: 17-Oct-2003.
# Written by:  Taro Muraoka  <koron@tka.att.ne.jp>
#
# Copyright (C) 2000 Taro Muraoka =KoRoN=

PACKAGE = ABrowser
VERSION = 1.3.5
SUBDIR = DelRegAB
TAGSRCS = *.c *.cpp *.h

CP = cp -a
MD = mkdir -p
RM = rm -f
TAR = tar
CTAGS = ctags

project :
tags : $(TAGSRCS)
	$(RM) tags
	$(CTAGS) $(TAGSRCS)
clean :
	@for dir in $(SUBDIR) ; do \
		if [ -d $$dir ] ; then \
			cd $$dir && $(MAKE) clean ; \
		fi ;\
	done
	$(RM) -r Debug Release
	$(RM) *.opt *.ncb *.aps *.plg

PACKAGE_DIR = $(PACKAGE)-$(VERSION)
ARCHIVE = $(PACKAGE_DIR).tar.bz2
$(ARCHIVE):
	$(MD) $(PACKAGE_DIR)
package: $(ARCHIVE)

# vi:ts=8 sts=8 sw=8 tw=0
