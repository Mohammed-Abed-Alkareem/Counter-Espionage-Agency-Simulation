#include "named_semaphore.h"
#include <fcntl.h>  
#include <sys/stat.h>


// Function to create a named semaphore
sem_t *create_named_semaphore(const char *name, unsigned int value) {
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0644, value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    return sem;
}

// Function to delete a named semaphore
int delete_named_semaphore(const char *name, sem_t *sem) {
    if (sem_close(sem) == -1) {
        perror("sem_close");
        return -1;
    }
    if (sem_unlink(name) == -1) {
        perror("sem_unlink");
        return -1;
    }
    return 0;
}


