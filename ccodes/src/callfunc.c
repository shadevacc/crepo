#include "main.h"
#include "callfunc.h"

void callfunc()
{
    //printf("SETUP READY\n");
#if 0
    a0_parr();
    ds_stack();
    a2_callgrind();
    l0_merge(); // Seeing compilation errors and to be fixed
#endif
    dsandalgs_call();
    cpointers_call();
    leetcode_call();
}