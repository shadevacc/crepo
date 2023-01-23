#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 1024

int8_t write_buf[MAX];
int8_t read_buf[MAX];

int main() {
    char option;
    int fd;

    fd = open("/dev/dev_sha", O_RDWR);
    if (fd < 0) {
        printf("ERROR: Could not open file %s\n", "/dev/dev_sha");
        return EXIT_FAILURE;
    }

    while(1) {
        printf("Please choose one of below options\n");
        printf("1.write\n");
        printf("2.read\n");
        printf("3.exit\n");
        printf("option:\n");
        getchar();
        scanf("%c", &option);
        printf("option: %c\n", option);

        switch(option) {
            case '1':
                printf("Enter string to write data to driver:\n");
                getchar();
                scanf("%[^\t\n]s", write_buf);
                // fgets(write_buf, sizeof(write_buf), stdin);
                printf("Data writing...\n");
                write(fd, write_buf, strlen(write_buf)+1);
                printf("Data written\n");
                break;
            case '2':
                printf("Data reading...\n");
                read(fd, read_buf, MAX);
                printf("Data successfully read\n");
                printf("Data = %s\n", read_buf);
                break;
            case '3':
                close(fd);
                exit(1);
                break;
            default:
                printf("ERROR: Unknown option, try again\n");
                break;
        }
    }
    close(fd);
    return EXIT_SUCCESS;
}