#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
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
    UNIX_CHECK(sem_queue = sem_open(QUEUE_MUTEX_SEM, 0));
    UNIX_CHECK(sem_empty = sem_open(QUEUE_EMPTY_SEM, 0));
    UNIX_CHECK(sem_full = sem_open(QUEUE_FULL_SEM, 0));

    sem_wrapper semQueueWrapper{sem_queue};

    key_t key;

    // Generate a unique key for the shared memory segment
    UNIX_CHECK((key = ftok(SHARED_MEM_KEY, SHARED_MEM_ID)));

    // Create the shared memory segment
    UNIX_CHECK((shmid = shmget(key, sizeof(struct s_shared_mem), 0)));

    // Attach the shared memory segment to our data space
    UNIX_CHECK(shm = shmat(shmid, nullptr, 0));

    shared_mem = (struct s_shared_mem *) shm;

    pid_t pid;

    UNIX_CHECK(pid = fork());


    auto mainLoop = [=] {
        for (;;) {
            UNIX_CHECK(sem_wait(sem_full));

            {
                std::scoped_lock lock(semQueueWrapper);

                if (!shared_mem->running) {
                    UNIX_CHECK(sem_post(sem_full));
                    break;
                }

                const char *msg = shared_mem->buffer[shared_mem->line_read];
                printf("%s from pid %d\n", msg, getpid());
                shared_mem->line_read = (shared_mem->line_read + 1) % NUM_LINE;
            }

            UNIX_CHECK(sem_post(sem_empty));
        }
    };

    if (pid == 0) {
        // child
        mainLoop();
    } else {
        // parent
        mainLoop();
        // wait for child
        waitpid(pid, nullptr, 0);
    }

    // unmap shared memory
    UNIX_CHECK(shmdt(shm));

    UNIX_CHECK(sem_close(sem_queue));
    UNIX_CHECK(sem_close(sem_empty));
    UNIX_CHECK(sem_close(sem_full));


    return 0;
}