
// shared_memory.c
#include "shared_memory.h"


int create_shared_memory(key_t key, size_t size) {
    int shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid == -1) {
        perror("Failed to create shared memory");
    }
    return shmid;
}

int acquire_shared_memory(key_t key) {
    int shmid = shmget(key, 0, 0666);
    if (shmid == -1) {
        perror("Failed to acquire shared memory");
    }
    return shmid;
}

int delete_shared_memory(int shmid, const void* shmaddr) {
    // Detach the shared memory for the calling process
    if (shmaddr != NULL && shmdt(shmaddr) == -1) {
        perror("Failed to detach shared memory");
        return -1;
    }

    // Mark the shared memory for deletion
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Failed to delete shared memory");
        return -1;
    }

    return 0;
}

void* attach_shared_memory(int shmid) {
    void* shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void*)-1) {
        perror("Failed to attach to shared memory");
    }
    return shmaddr;
}

int detach_shared_memory(const void* shmaddr) {
    if (shmdt(shmaddr) == -1) {
        perror("Failed to detach from shared memory");
        return -1;
    }
    return 0;
}

// copy a memory block to the shared memory segment
void copy_to_shared_memory(void *dest /*shared memory*/, const void *src /* what to copy */ , size_t size /* size of the memory block */) {
    memcpy(dest, src, size);
}
