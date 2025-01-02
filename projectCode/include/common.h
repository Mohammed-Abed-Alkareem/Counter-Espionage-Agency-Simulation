#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>   
#include <string.h>
#include "config.h"
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <sys/shm.h>
#include <sys/msg.h>


typedef struct {
    // Shared data
    int number_killed_members;
    int number_injured_members;
    int number_captured_members;

} SharedData;

#endif // COMMON_H