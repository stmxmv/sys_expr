#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_SIZE (256 * 1024 * 1024)
#define PAGE_SIZE (4 * 1024)
int main()
{
    void* mem_ptr;
    long i;
    long sum = 0;

    mem_ptr = malloc(MEM_SIZE);
    if (mem_ptr == NULL) {
        printf("Failed to allocate memory\n");
        return 1;
    }

    FILE* status_file = fopen("/proc/self/status", "r");
    char line[256];
    while (fgets(line, 256, status_file) != NULL) {
        if (sscanf(line, "VmSize: %ld kB", &i) == 1) {
            printf("Virtual memory usage before read/write: %ld KB\n", i);
        }
        if (sscanf(line, "VmRSS: %ld kB", &i) == 1) {
            printf("Physical memory usage before read/write: %ld KB\n", i);
        }
    }
    fclose(status_file);

    for (i = 0; i < MEM_SIZE; i += PAGE_SIZE) {
        sum += *(volatile char*)(mem_ptr + i);
    }

    status_file = fopen("/proc/self/status", "r");
    while (fgets(line, 256, status_file) != NULL) {
        if (sscanf(line, "VmSize: %ld kB", &i) == 1) {
            printf("Virtual memory usage after read: %ld KB\n", i);
        }
        if (sscanf(line, "VmRSS: %ld kB", &i) == 1) {
            printf("Physical memory usage after read: %ld KB\n", i);
        }
    }
    fclose(status_file);

    for (i = 0; i < MEM_SIZE; i += PAGE_SIZE) {
        *(volatile char*)(mem_ptr + i) = i % 256;
    }

    status_file = fopen("/proc/self/status", "r");
    while (fgets(line, 256, status_file) != NULL) {
        if (sscanf(line, "VmSize: %ld kB", &i) == 1) {
            printf("Virtual memory usage after write: %ld KB\n", i);
        }
        if (sscanf(line, "VmRSS: %ld kB", &i) == 1) {
            printf("Physical memory usage after write: %ld KB\n", i);
        }
    }
    fclose(status_file);

    free(mem_ptr);

    return 0;
}
