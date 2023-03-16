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

//void create_tree(int level) {
//    if (level == 0) {
//        printf("Leaf node %d created\n", getpid());
//        return;
//    }
//
//    printf("Node %d created\n", getpid());
//
//    pid_t pid1 = fork();
//    if (pid1 == 0) {
//        create_tree(level - 1);
//        return;
//    }
//
//    pid_t pid2 = fork();
//    if (pid2 == 0) {
//        create_tree(level - 1);
//        return;
//    }
//
//    waitpid(pid1, NULL, 0);
//    waitpid(pid2, NULL, 0);
//    printf("Node %d completed\n", getpid());
//}

int main(void) {
    const int level = 4;

    pid_t child1, child2;

    tprintf("root process start\n");

    int i;
    for (i = 0; i < level; ++i) {
        child1 = fork();
        if (child1) {
            // parent
            child2 = fork();
            if (child2) {
                tprintf("wait level %d child1 %d to complete\n", i, child1);
                waitpid(child1, NULL, 0);

                tprintf("wait level %d child2 %d to complete\n", i, child2);
                waitpid(child2, NULL, 0);
                break;
            }

            tprintf("level %d child2 running\n", i);

        } else {
            tprintf("level %d child1 running\n", i);
        }
    }

    tprintf("process complete\n");

//    sleep(100);
    return 0;
}
