#
# Makefile -
#
# Last Change: 18:25:27 28-Sep-2000.
# Written by:  Taro Muraoka  <koron@tka.att.ne.jp>
#
# Copyright (C) 2000 Taro Muraoka =KoRoN=

SUBDIR =

RM = rm -f

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
	$(RM) -r Debug Release *.opt *.ncb
	$(RM) *.opt *.ncb *.aps *.plg

# vi:ts=8 sts=8 sw=8 tw=0
