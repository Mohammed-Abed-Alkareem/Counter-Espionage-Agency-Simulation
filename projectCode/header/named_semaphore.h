#include "common.h"

#ifndef NAMED_SEMAPHORE_H
#define NAMED_SEMAPHORE_H

#include <semaphore.h>

// Function to create a named semaphore
sem_t *create_named_semaphore(const char *name, unsigned int value);

// Function to delete a named semaphore
int delete_named_semaphore(const char *name, sem_t *sem);



#endif // SEMAPHORE_H