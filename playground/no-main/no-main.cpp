#include <stdio.h>
#include <stdlib.h>

/*
 The 199711L stands for Year=1997, Month=11 -- That is the date when
 the committee approved the standard that the rest of the ISO approved in early 1998
 */
#if __cplusplus > 199711L
#define register // Deprecated in C++11.
#endif

// Linkage specification to make C++ functions have a C likage
extern "C"
{
    extern void _exit(register int);

    int _start()
    {
        printf("Hello World\n");
        _exit(0);
        return EXIT_SUCCESS;
    }
}
