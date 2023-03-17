#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <cstring>

#include <string>
#include <iostream>
#include <mutex>

#include "typedef.h"





int main(int argc, const char * argv[]) {
    int shmid;
    void *shm;

    struct s_shared_mem *shared_mem;

    sem_t *sem_queue, *sem_empty, *sem_full;

    // Create the semaphore
    sem_queue = guard_sem_open(QUEUE_MUTEX_SEM, O_CREAT, 0644, 1);
    sem_empty = guard_sem_open(QUEUE_EMPTY_SEM, O_CREAT, 0644, 0);
    sem_full = guard_sem_open(QUEUE_FULL_SEM, O_CREAT, 0644, NUM_LINE);

    key_t key;

    // Generate a unique key for the shared memory segment
    if ((key = ftok(SHARED_MEM_KEY, 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Create the shared memory segment
    if ((shmid = shmget(key, sizeof(struct s_shared_mem), IPC_CREAT | 0666)) == -1) {
        perror("shmget");
        exit(1);
    }

    // Attach the shared memory segment to our data space
    if ((shm = shmat(shmid, nullptr, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    shared_mem = (struct s_shared_mem *) shm;

    memset(shared_mem, 0, sizeof(*shared_mem));

    shared_mem->running = true;

    std::string line;
    for (;;) {
        printf("Enter you text ('quit' for exit) : ");

        std::getline(std::cin, line);

        if (line == "quit") {
            {
                sem_wrapper semWrapper{sem_queue};
                std::scoped_lock lock(semWrapper);
                shared_mem->running = false;
            }

            for (int i = 0; i < NUM_LINE; ++i) {
                sem_post(sem_empty);
            }

            break;
        }

        sem_wait(sem_full);


        {
            sem_wrapper semWrapper{sem_queue};
            std::scoped_lock lock(semWrapper);
            strncpy(shared_mem->buffer[shared_mem->line_write], line.c_str(), LINE_SIZE);
            shared_mem->line_write = (shared_mem->line_write + 1) % NUM_LINE;
        }


        sem_post(sem_empty);
    }

    sem_close(sem_queue);
    sem_close(sem_empty);
    sem_close(sem_full);

    sem_unlink(QUEUE_MUTEX_SEM);
    sem_unlink(QUEUE_EMPTY_SEM);
    sem_unlink(QUEUE_FULL_SEM);

    // release shared memory
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}