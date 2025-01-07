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

// struct for group resistance
typedef struct {
    int group_number;
    int is_military;
    int number_members;
    int number_killed_members;
    int number_injured_members;
    int number_captured_members;
} ResistanceGroup;

// struct for civilian
typedef struct {
    int civilian_number;
    int is_spy;
    int is_injured;
    int is_killed;
    int is_captured;
} Civilian;

// struct for message
typedef struct {
    long mtype;
    char mtext[100];
} Message;

//struct
// struct for counter espionage agency member
typedef struct {
    int member_id;
    int is_alive;
    int is_captured;
    int is_injured;
    time_t time_joined;
} AgencyMember;

// struct for resistance group member
typedef struct {
    int member_id;
    int is_spy;
    int is_alive;
    int is_captured;
    int is_injured;
    time_t time_joined;
} ResistanceMember;

#endif // COMMON_H