#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define M_WRITE _IOW('a', 'a', int32_t*)
#define M_READ _IOR('b', 'b', int32_t*)

int main() {
    int fd;
    int32_t value, num;

    fd = open("/dev/dev_sha", O_RDWR);
    if (fd < 0) {
        printf("open failed\n");
        return EXIT_FAILURE;
    }

    printf("Enter value to send\n");
    scanf("%d", &num);
    ioctl(fd, M_WRITE, (int32_t *) &num);
    printf("%d sent\n", num);
    ioctl(fd, M_READ, (int32_t *) &value);
    printf("%d read\n", value);
    close(fd);
    return EXIT_SUCCESS;
}
