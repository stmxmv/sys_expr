#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int tprintf(const char *fmt, ...) {
    va_list args;
    struct tm *tstruct;
    time_t tsec;
    tsec = time(NULL);
    tstruct = localtime(&tsec);
    printf("[%02d:%02d:%02d| %5d] ", tstruct->tm_hour, tstruct->tm_min, tstruct->tm_sec, getpid());
    va_start(args, fmt);
    return vprintf(fmt, args);
}

int main() {
    int pid, status;

    tprintf("I am parent process %d\n", getpid());
    printf("spawn child process num : ");

    int process_num;

    scanf("%d", &process_num);

    /// create multiple child processes
    for (int i = 0; i < process_num; i++) {
        pid = fork();
        if (pid == 0) {
            sleep(1);
            tprintf("I am child process %d, my parent is %d\n", getpid(), getppid());
            tprintf("Child process %d is running\n", getpid());
            tprintf("Child process %d is done\n", getpid());
            return 0;
        }
        if (pid < 0) {
            tprintf("Error creating child process\n");
            return -1;
        }
    }

    tprintf("wait all child processes to complete\n");
    // waiting all child processes to complete
    for (int i = 0; i < process_num; i++) {
        wait(NULL);
    }

    tprintf("All child processes are done\n");

    return 0;
}
