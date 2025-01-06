#ifndef RESISTANCE_GROUP_H
#define RESISTANCE_GROUP_H

#include "common.h"


int main(int argc, char *argv[]);

void cleanUp();



///another enum for member type

typedef enum {
    SOCIALIST,
    MILITARY,
} MEMBER_TYPE;



//enum
typedef enum {
    ALIVE,
    DEAD
} STATUS;



//create a struct
typedef struct {

    //add member info
    int id;
    pthread_t thread_id;
    int health;
    STATUS status;
    MEMBER_TYPE type;
    int is_spy;
    int group_id;

} RESISTANCE_MEMBER;





#endif // RESISTANCE_GROUP_H