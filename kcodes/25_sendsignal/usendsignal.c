#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

#define M_SIG_TX 44
#define MREG_CURRENT_TASK _IOW('a', 'a', int32_t *)

static int done = 0;
int check = 0;

void ctrl_c_handler(int n, siginfo_t *si, void *context);
void sign_event_handler(int n, siginfo_t *si, void *context);
void ctrl_c_handler(int n, siginfo_t *si, void *context) {
    if (n == SIGINT) {
        printf("%s: ctrl-c received\n", __FUNCTION__);
        done = 1;
    }
}

void sig_event_handler(int n, siginfo_t *si, void *context) {
    if (n == M_SIG_TX) {
        printf("%s: M_SIG_TX received\n", __FUNCTION__);
        check = si->si_code;
    }
}

int main(void) {
    int fd; /* File descriptor is needed to open a file */
    int32_t no;
    struct sigaction act;
/*
act.sa_flags = (SA_SIGINFO | SA_RESETHAND);:
This sets the flags for the sigaction struct act.
SA_SIGINFO specifies that the sa_sigaction field of act should be used as the
signal handler, and
SA_RESETHAND specifies that the signal handler should be reset to the default
after it is executed.

act.sa_sigaction = ctrl_c_handler;:
This sets the sa_sigaction field of act to the ctrl_c_handler function, which
will be called when a SIGINT signal is received.

sigaction(SIGINT, &act, NULL);:
This calls the sigaction function with SIGINT as the signal to handle, &act as
the sigaction struct to use, and NULL as the old sigaction struct (which is not
needed in this case). This installs the signal handler specified in act for the
SIGINT signal.
*/
    /* We need to install ctrl+c interrupt handler to cleanup at exit */
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESETHAND);
    act.sa_sigaction = ctrl_c_handler;
    sigaction(SIGINT, &act, NULL);

    /* We need to install custom signal handler */
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(M_SIG_TX, &act, NULL);

    printf("Installed signale handler for M_SIG_TX: %d\n", M_SIG_TX);

    printf("Opening device file\n");
    fd = open("/dev/sha_dev_2", O_RDWR);
    if (fd < 0) {
        printf("Error opening device file\n");
        return EXIT_FAILURE;
    }

    printf("Register application with device driver...\n");
    /* Register this process with kernel for receiving signal       */
    if (ioctl(fd, MREG_CURRENT_TASK, (int32_t *) &no)) {
        printf("Failed to register application with device driver\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    while(!done) {
        printf("Waiting for signal...\n");
        //blocking check
        while(!done && !check);
        check = 0;
    }

    printf("closing the driver file...\n");
    close(fd);
    return EXIT_SUCCESS;
}