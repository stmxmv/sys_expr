#ifndef SYS_TYPEDEF_H
#define SYS_TYPEDEF_H

#define SHARED_MEM_KEY "./producer"
#define QUEUE_MUTEX_SEM "an.queue.mutex"
#define QUEUE_EMPTY_SEM "an.queue.empty"
#define QUEUE_FULL_SEM "an.queue.full"

#define LINE_SIZE 256
#define NUM_LINE 16

struct s_shared_mem {
    bool running;
    char buffer[NUM_LINE][LINE_SIZE];
    int line_write;
    int line_read;
};


template<typename ...Args>
static inline sem_t *guard_sem_open(const char *name, int oflags, Args... args) {
    sem_t *sem;
    if ((sem = sem_open(name, oflags, args...)) == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    return sem;
}

class sem_wrapper {
    sem_t *sem;

public:

    explicit sem_wrapper(sem_t *sem) : sem(sem) {}

    void lock() {
        sem_wait(sem);
    }

    void unlock() {
        sem_post(sem);
    }

};

#endif //SYS_TYPEDEF_H
