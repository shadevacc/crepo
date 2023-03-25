#include "cheaders.h"

static int sum(int v1, int v2) {
    return v1+v2;
}

int a2_callgrind() {
    int v1 = 2, v2 = 3, res;
    char input_buffer[50]="one";
    char output_buffer[5]; // including null terminator
    
    printf("Enter a string:\n");
    // fgets(input_buffer, 50, stdin); // read input from user
    
    // copy 4 bytes from input buffer to output buffer
    strncpy(output_buffer, input_buffer, 4);
    output_buffer[4] = '\0'; // add null terminator to output buffer

    res = sum(v1, v2);
    
    printf("Output: %s\n", output_buffer); // print output buffer
    printf("result: %d\n", res);
    
    return 0;
}