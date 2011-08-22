#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

    int
main(void)
{
    int cnt = 0;
    FILE *fp = fopen("teststrm.log", "wt");
    while (1)
    {
	fprintf(fp, "Test line:%d\n", ++cnt);
	fflush(fp);
	Sleep(5000);
    }
    return 0;
}
