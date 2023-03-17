#ifndef SYS_TYPEDEF_H
#define SYS_TYPEDEF_H

#define SHARED_MEM_KEY "./producer"
#define SHARED_MEM_ID 13
#define QUEUE_MUTEX_SEM "an.queue.mutex"
#define QUEUE_EMPTY_SEM "an.queue.empty"
#define QUEUE_FULL_SEM "an.queue.full"

#define LINE_SIZE 256
#define NUM_LINE 16

#define UNIX_CHECK(stmt) \
    do {\
    if ((long long int)(stmt) < 0) {    \
        perror(#stmt);   \
        exit(-1); \
    } } while (0)\


struct s_shared_mem {
    bool running;
    char buffer[NUM_LINE][LINE_SIZE];
    int line_write;
    int line_read;
};


class sem_wrapper {
    sem_t *sem;

public:

    explicit sem_wrapper(sem_t *sem) : sem(sem) {}

    void lock() const {
        UNIX_CHECK(sem_wait(sem));
    }

    void unlock() const {
        UNIX_CHECK(sem_post(sem));
    }

};

#endif //SYS_TYPEDEF_H
