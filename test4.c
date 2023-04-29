#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MEM_SIZE (256 * 1024 * 1024)
#define CHUNK_SIZE (4 * 1024)

void printInfo(void) {
    char filename[256];
    pid_t pid = getpid();
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
}

int main()
{
    void* mem_ptr;
    long i;
    long sum = 0;

    puts("****** before allocation");
    printInfo();

    mem_ptr = malloc(MEM_SIZE);
    if (mem_ptr == NULL) {
        printf("Failed to allocate memory\n");
        return 1;
    }

    puts("\n****** after allocation");
    printInfo();

    for (i = 0; i < MEM_SIZE; i += CHUNK_SIZE) {
        sum += *(volatile char*)(mem_ptr + i);
    }

    puts("\n****** after read");
    printInfo();

    for (i = 0; i < MEM_SIZE; i += CHUNK_SIZE) {
        *(volatile char*)(mem_ptr + i) = i % 256;
    }

    puts("\n****** after written");
    printInfo();

    free(mem_ptr);

    return 0;
}
