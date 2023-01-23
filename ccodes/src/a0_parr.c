#include "cheaders.h"

void a0_parr() {
    char *name[] = {"Miller", "Anderson", "Jones"};
    int i;
    for (i = 0; i < 4; i++) {
        printf("%s\n", name[i]);
    }
}