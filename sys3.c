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

int main(void) {
    tprintf("I am parent process\n");
    pid_t pid1 = fork();
    if (pid1 == 0) {
        tprintf("child process 1\n");
        return 0;
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        tprintf("child process 2\n");
        return 0;
    }

    tprintf("waiting child process to complete\n");

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    tprintf("all child processes complete\n");

    return 0;
}