#ifndef RESISTANCE_GROUP_H
#define RESISTANCE_GROUP_H

#include "common.h"


int main(int argc, char *argv[]);

void cleanUp();

//create a struct
typedef struct {

    //add member info
    int id;
    pthread_t thread_id;
    int health;
    STATUS status;
    MEMBER_TYPE type;
    int is_spy;

} RESISTANCE_MEMBER;


//enum
typedef enum {
    ALIVE,
    DEAD
} STATUS;

///another enum for member type

typedef enum {
    SOCIALIST,
    MILITARY,
} MEMBER_TYPE;

#endif // RESISTANCE_GROUP_H