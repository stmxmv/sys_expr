#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <cstring>

#include <string>
#include <iostream>
#include <mutex>

#include "typedef.h"


int main(int argc, const char *argv[]) {
    int shmid;
    void *shm;

    struct s_shared_mem *shared_mem;

    sem_t *sem_queue, *sem_empty, *sem_full;

    // Create the semaphore
    UNIX_CHECK(sem_queue = sem_open(QUEUE_MUTEX_SEM, O_CREAT, 0644, 1));
    UNIX_CHECK(sem_empty = sem_open(QUEUE_EMPTY_SEM, O_CREAT, 0644, NUM_LINE));
    UNIX_CHECK(sem_full = sem_open(QUEUE_FULL_SEM, O_CREAT, 0644, 0));

    sem_wrapper semQueueWrapper{sem_queue};

    key_t key;

    // Generate a unique key for the shared memory segment
    UNIX_CHECK(key = ftok(SHARED_MEM_KEY, SHARED_MEM_ID));

    // Create the shared memory segment
    UNIX_CHECK(shmid = shmget(key, sizeof(struct s_shared_mem), IPC_CREAT | 0666));

    // Attach the shared memory segment to our data space
    UNIX_CHECK((shm = shmat(shmid, nullptr, 0)));

    shared_mem = (struct s_shared_mem *) shm;

    memset(shared_mem, 0, sizeof(*shared_mem));

    shared_mem->running = true;

    std::string line;
    for (;;) {
        printf("Enter you text ('quit' for exit) : ");

        std::getline(std::cin, line);

        if (line == "quit") {

            {
                std::scoped_lock lock(semQueueWrapper);
                shared_mem->running = false;
            }

            UNIX_CHECK(sem_post(sem_full));

            break;
        }

        UNIX_CHECK(sem_wait(sem_empty));


        {
            std::scoped_lock lock(semQueueWrapper);
            strncpy(shared_mem->buffer[shared_mem->line_write], line.c_str(), LINE_SIZE);
            shared_mem->line_write = (shared_mem->line_write + 1) % NUM_LINE;
        }


        UNIX_CHECK(sem_post(sem_full));
    }

    // unmap shared memory
    UNIX_CHECK(shmdt(shm));

    // remove the shared memory segment
    struct shmid_ds buf;
    UNIX_CHECK(shmctl(shmid, IPC_RMID, &buf));

    UNIX_CHECK(sem_close(sem_queue));
    UNIX_CHECK(sem_close(sem_empty));
    UNIX_CHECK(sem_close(sem_full));

    UNIX_CHECK(sem_unlink(QUEUE_MUTEX_SEM));
    UNIX_CHECK(sem_unlink(QUEUE_EMPTY_SEM));
    UNIX_CHECK(sem_unlink(QUEUE_FULL_SEM));

    return 0;
}