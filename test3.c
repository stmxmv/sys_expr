#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


void printInfo(void) {
    char filename[256];
    pid_t pid = getpid();
    // 检查/proc/pid/status文件中关于内存的情况
    sprintf(filename, "/proc/%d/status", pid);
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open file %s.\n", filename);
        return;
    }
    char buf[4096];
    int n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("process status %s\n", buf);
    close(fd);

    // 检查/proc/pid/maps文件中关于内存的情况
    sprintf(filename, "/proc/%d/maps", pid);
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open file %s.\n", filename);
        return;
    }
    n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("Virtual memory map:\n%s", buf);
    close(fd);
}

int main(void) {
    long step_size = 128 * 1024 * 1024; // allocate 128MB each time
    long allocated_memory = 0;
    void* ptr;

    for (int i = 0; i < 14; ++i) {
        ptr = malloc(step_size);
        if (ptr == NULL) {
            printf("Failed to allocate %ld MB memory\n", allocated_memory/1024/1024);
            break;
        }

        // use the memroy
        memset(ptr, 1, step_size);

        allocated_memory += step_size;
        printf("allocated memory size: %ldMB, %p ~ %p\n", allocated_memory / 1024 / 1024, ptr, (char *)ptr + step_size);
    }

    printf("Maximum memory allocated: %ld MB\n", allocated_memory/1024/1024);

    printInfo();

    sleep(1000);

    return 0;
}
