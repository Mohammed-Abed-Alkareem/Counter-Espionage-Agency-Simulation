// shared_memory.h
#include "common.h"

#ifndef SHARED_MEMORY_H

#include <sys/ipc.h>
#include <sys/shm.h>



#define SHARED_MEMORY_H
// Creates a shared memory segment of the given size and key.
// Returns the shared memory ID (shmid) on success, or -1 on failure.
int create_shared_memory(key_t key, size_t size);

// Acquires access to an existing shared memory segment with the given key.
// Returns the shared memory ID (shmid) on success, or -1 on failure.
int acquire_shared_memory(key_t key);

// Deletes a shared memory segment identified by the given shared memory ID.
// Ensures the calling process detaches the shared memory first.
// Returns 0 on success, or -1 on failure.
int delete_shared_memory(int shmid, const void* shmaddr);

// Attaches to the shared memory segment identified by the given shared memory ID.
// Returns a pointer to the shared memory on success, or (void *)-1 on failure.
void* attach_shared_memory(int shmid);

// Detaches from the shared memory segment at the given address.
// Returns 0 on success, or -1 on failure.
int detach_shared_memory(const void* shmaddr);

void copy_to_shared_memory(void *dest /*shared memory*/, const void *src /* what to copy */ , size_t size /* size of the memory block */);
#endif //SHARED_MEMORY_H
