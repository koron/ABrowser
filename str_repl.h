/*
 * str_repl.h - String replacer
 *
 * Last Change: 16:40:05 28-Sep-2000.
 * Written by:  Taro Muraoka  <koron@tka.att.ne.jp>
 */
#ifndef _INC_STR_REPL
#define _INC_STR_REPL

/*
 * STRUCTURES
 */
typedef struct _replace__data
{
    char label;
    char mode;
    void* p_value;
} replace_data;


/*
 * FUNCTION DECLARATIONS
 */
#ifdef __cplusplus
extern "C" {
#endif
    int str_replace(char* buf, int len, char* format,
	    replace_data* p_repdat);
#ifdef __cplusplus
}
#endif

#endif /* _INC_STR_REPL */
/* vi:ts=8 sts=4 sw=4 tw=0
 */
