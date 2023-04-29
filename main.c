#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define MB (1024 * 1024)
#define NUM_CHUNKS 6

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

    sprintf(filename, "/proc/%d/maps", pid);
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open file %s.\n", filename);
        return;
    }
    n = read(fd, buf, sizeof(buf));
    buf[n] = '\0';
    printf("memory map:\n%s", buf);
    close(fd);
}

int main(void) {
    freopen("test1.txt", "w", stdout);

    int i;
    void *chunks[NUM_CHUNKS];

    printf("****** before allocation\n");
    printInfo();
    puts("******");
    puts("");

    puts("start allocation 6 * 128 MB memory\n");
    for (i = 0; i < NUM_CHUNKS; i++) {
        chunks[i] = malloc(128 * MB);
        if (chunks[i] == NULL) {
            printf("Failed to allocate memory.\n");
            return 1;
        }
        printf("allocate 128MB at %p ~ %p\n", chunks[i], (char *)chunks[i] + 128 * MB);
    }

    printf("******** after 6 * 128MB allocation \n");
    printInfo();
    puts("******");


    free(chunks[1]);
    free(chunks[2]);
    free(chunks[4]);
    printf("******** free 2, 3, 5 memory");
    printInfo();
    puts("******");

    void *large_chunk = malloc(1024 * MB);
    if (large_chunk == NULL) {
        printf("Failed to allocate memory.\n");
        return 1;
    }
    printf("****** Allocated 1024MB at address %p.\n", large_chunk);
    printInfo();
    puts("******");

    /// allocate 64 MB
    void *chunk = malloc(64 * MB);
    printf("***** allocate 64 MB at %p ~ %p\n", chunk, (char *)chunk + 64 * MB);
    printInfo();
    puts("*****");

    return 0;
}
