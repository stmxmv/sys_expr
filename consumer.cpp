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

#include "typedef.h"


int main(int argc, const char * argv[]) {

    int shmid;
    void *shm;

    struct s_shared_mem *shared_mem;

    sem_t *sem_queue, *sem_empty, *sem_full;

    // Create the semaphore
    sem_queue = guard_sem_open(QUEUE_MUTEX_SEM, 0);
    sem_empty = guard_sem_open(QUEUE_EMPTY_SEM, 0);
    sem_full = guard_sem_open(QUEUE_FULL_SEM, 0);

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

    pid_t pid;

    pid = fork();


    auto mainLoop = [=] {
        for (;;) {
            sem_wait(sem_empty);

            {
                sem_wrapper semWrapper{sem_queue};
                std::scoped_lock lock(semWrapper);

                if (!shared_mem->running) {
                    break;
                }

                const char *msg = shared_mem->buffer[shared_mem->line_read];
                printf("%s from pid %d\n", msg, getpid());
                shared_mem->line_read = (shared_mem->line_read + 1) % NUM_LINE;
            }

            sem_post(sem_full);
        }
    };

    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // child
        mainLoop();
    } else {
        // parent
        mainLoop();
        // wait for child
        waitpid(pid, NULL, 0);
    }

    sem_close(sem_queue);
    sem_close(sem_empty);
    sem_close(sem_full);


    return 0;
}