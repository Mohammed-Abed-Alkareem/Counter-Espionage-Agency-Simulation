#ifndef COUNTER_ESPIONAGE_AGENCY_H
#define COUNTER_ESPIONAGE_AGENCY_H

#include "common.h"
//#include "shared_memory.h"

int main(int argc, char *argv[]);
void* member_function(void* arg);




typedef struct {
    int id;
    pthread_t thread_id;
    int health;
    STATUS status;
    int time_with_agency;
    float target_probability;
} AGENCY_MEMBER;







#endif // COUNTER_ESPIONAGE_AGENCY_H