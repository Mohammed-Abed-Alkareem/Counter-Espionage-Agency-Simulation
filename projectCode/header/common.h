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
#include <errno.h>
#include <pthread.h>

#include "message_queue.h"
#include "utils.h"



//state 
typedef enum {
    ACTIVE,
    INACTIVE,
    CAPTURED,
    ESCAPED,
    KILLED,
    LightINJURED,
    SERIOUSLYINJURED,
    DEAD,
    SPY,
    INVISTIGATED,
    ARREST,
    ALIVE
} STATE;


typedef struct {
    // Shared data
    int number_killed_members;
    int number_injured_members;
    int number_captured_members;

} SharedData;

//********************* */
// Message Structers 
//********************* */

typedef struct  {
    long type;
    int group_id;
    int member_num;
    int people_process_num;
    int people_number; // like ID
    int num_of_sec_communcated_with;
} PeopleContactReportMessage; // report message from people to resistance group for agency to know

typedef struct  {
    long type; // The group ID
    int member_num;
    int state; // e.g., killed, severely injured, lightly injured
} ResistanceMemberStateReportMessage; // report message from resistance group of his state to agency;


typedef struct  {
    long type; // The people number
    int state; // e.g., arrested, killed, caught, released
} AgencyToPeopleStateMessage; // from agency to people to know their state

typedef struct  {
    long type; // The group number
    int member_number;
    STATE state; // e.g., arrested, killed, caught, released
    int time_sent;
} AgencyToResistanceStateMessage ;

typedef struct  {
    long type;
    int process_id; // For general attack on all groups
    int group_id; // Specific attack on a member
    int group_type; // e.g., military, civilian
    int group_member; // Specific attack on a member
}SpyToEnemyReportMessage; // report message from spy to enemy


typedef struct  {
    long type; // Group number
    int member_id; // For specific attack
    int attack_type; // e.g., target, kill, definite kill
    int isGeneral; // For general attack
    int time_sent;
} EnemyToResistanceGroupAttackMessage ;

typedef struct  {
    int type; // Represents the member ID
    // int attack_type; // e.g., target, kill, definite kill
}EnemyToAgencyAttackMessage ; // enemy to agency attack message structure


typedef struct  {
    long type; // Represents the people ID
    int num_of_sec; // Number of seconds communicated
    int group_id; // Group ID
    int group_type ; // e.g., military, civilian
    int member_id; // Member ID
}ResistanceMemberToPeopleContactMessage ; // resistance group member contac with people message structure 

typedef struct  {
    int member_id; // Represents the people ID
    float target_probability; // Probability of being targeted
} AgencyToEnemyTargetProbabilityMessage; // agency to enemy target probability message structure


////////////////

typedef enum {
    SOCIALIST,
    MILITARY,
} MEMBER_TYPE;


typedef STATE STATUS;

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



#endif //COMMON_H