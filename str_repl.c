/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * str_repl.c - String replacer
 *
 * Last Change: 30-Oct-2004.
 * Written by:  Taro Muraoka  <koron@tka.att.ne.jp>
 */
#include "str_repl.h"
#include <stdio.h>

    int
str_replace(char* buf, int len, char* format, replace_data* p_repdat)
{
    char cmd;
    int pos_write = 0;
    char work_area[256];
    replace_data *p;

    if (!buf)
	len = 0;
    for (; *format; ++format)
    {
	/* Normal character (not '$') is only copied. */
	if (!p_repdat || *format != '$')
	{
	    if (pos_write < len)
		buf[pos_write] = *format;
	    ++pos_write;
	    continue;
	}

	/* Replace "$$" with "$" */
	if ((cmd = *(format + 1)) == '$')
	{
	    if (pos_write < len)
		buf[pos_write] = *format;
	    ++pos_write;
	    ++format;
	    continue;
	}

	work_area[0] = '\0';
	p = p_repdat;
	while (p->label && p->mode && p->p_value)
	{
	    char *p_src = &work_area[0];

	    /* Go next label check, when don't match */
	    if (cmd != p->label)
	    {
		++p;
		continue;
	    }
	    ++format;
	    switch (p->mode)
	    {
		case 'c':
		    sprintf(p_src, "%c", *(char*)p->p_value);
		    break;
		case 'd':
		    sprintf(p_src, "%d", *(int*)p->p_value);
		    break;
		case 'f':
		    sprintf(p_src, "%f", *(float*)p->p_value);
		    break;
		case 'p':
		    sprintf(p_src, "%p", p->p_value);
		    break;
		case 's':
		    p_src = (char*)p->p_value;
		    break;
		case 'u':
		    sprintf(p_src, "%u", *(unsigned int*)p->p_value);
		    break;
		case 'x':
		    sprintf(p_src, "%x", *(unsigned int*)p->p_value);
		    break;
	    }

	    /* Write generated string */
	    while (*p_src != '\0')
	    {
		if (pos_write < len)
		    buf[pos_write] = *p_src;
		++p_src;
		++pos_write;
	    }
	    break;
	}
    }
    
    /* Add terminate NULL character. */
    if (pos_write < len)
	buf[pos_write] = '\0';

    return pos_write + 1;
}
