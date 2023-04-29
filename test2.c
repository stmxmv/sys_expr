#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MB (1024 * 1024)

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
    char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;
    ptr1 = malloc(128 * MB);
    ptr2 = malloc(256 * MB);
    ptr3 = malloc(128 * MB);
    ptr4 = malloc(128 * MB);
    ptr5 = malloc(128 * MB);
    ptr6 = malloc(128 * MB);

    printf("128MB ptr1 %p ~ %p\n", ptr1, ptr1 + 128 * MB);
    printf("256MB ptr2 %p ~ %p\n", ptr2, ptr2 + 256 * MB);
    printf("128MB ptr3 %p ~ %p\n", ptr3, ptr3 + 128 * MB);
    printf("128MB ptr4 %p ~ %p\n", ptr4, ptr4 + 128 * MB);
    printf("128MB ptr5 %p ~ %p\n", ptr5, ptr5 + 128 * MB);
    printf("128MB ptr6 %p ~ %p\n", ptr6, ptr6 + 128 * MB);

    free(ptr2);
    free(ptr4);

    printInfo();

    puts("free ptr2 and ptr4");
    void *ptr = malloc(128 * MB);
    printf("128MB ptr5 %p ~ %p\n", ptr, ptr + 128 * MB);

    printInfo();

    return 0;
}
