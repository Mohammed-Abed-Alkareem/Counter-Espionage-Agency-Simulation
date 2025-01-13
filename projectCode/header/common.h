#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>   
#include <string.h>
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
#include "config.h"




//state 
typedef enum {
    ACTIVE,
    INACTIVE,
    CAPTURED,
    ESCAPED,
    KILLED,
    LIGHTINJURED,
    SERIOUSLYINJURED,
    DEAD,
    SPY,
    INVISTIGATED,
    ARRESTED,
    ALIVE, 
    ATTACKED
} STATE;


typedef struct {
    // Already existing fields:
    int number_killed_members;
    int number_injured_members;
    int number_captured_members;

    // Add an agency inactivity time (if you want to track that):
    float agency_inactivity_time;

    // For the “boxes”:
    // Let’s assume up to 32 groups, each can have up to 10 members.
    // If group i is not used, set group_active[i] = 0.
    int  group_active[32];          // 1 if group is active, 0 if not.
    int  group_member_count[32];    // How many members in this group.
    int  group_member_killed[32][10]; // For each member in group i, is it killed? (1 = yes, 0 = no)
    // You could store more advanced info like x,y positions, etc., if you want dynamic positions.
} SharedData;

//********************* */
// Message Structers 
//********************* */

typedef struct  {
    long type;
    int group_id;
    int member_num;
    int people_number; // like ID
    int contact_time;
} PeopleContactReportMessage; // report message from people to resistance group for agency to know

typedef struct  {
    long type; // The group ID
    int member_num;
    STATE state; // e.g., killed, severely injured, lightly injured
} ResistanceMemberStateReportMessage; // report message from resistance group of his state to agency;


typedef struct  {
    long type; // The people number
    int state; // e.g., arrested, killed, caught, released
    int member_id;
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
//<<<<<<< adham_branch
    int num_of_sec; // Number of seconds communicated
    int enroll_data; // Enrolment data
    int isCounterAttack; // to differentiate between the counter espionage and the resistance group attack
//=======
 //   int enroll_date ; 
 ///   int isCounterAttack; // For counter attack
//>>>>>>> MohammedQady11
}SpyToEnemyReportMessage; // report message from spy to enemy


typedef struct  {
    long type; // Group number for resistance group and agency member number for agency member
    int member_id; // For specific attack
    int attack_type; // e.g., target, kill, definite kill
    int isGeneral; // For general attack
    int time_sent;
} EnemyToResistanceGroupAttackMessage ;

typedef EnemyToResistanceGroupAttackMessage EnemyToAgencyAttackMessage ; // enemy to agency attack message structure


typedef struct  {
    long type; // Represents the people ID
    int num_of_sec; // Number of seconds communicated
    int group_id; // Group ID
    int group_type ; // e.g., military, civilian
    int member_id; // Member ID
}ResistanceMemberToPeopleContactMessage ; // resistance group member contac with people message structure 


typedef struct {
    long type; // Represents the people ID
    int member_id; // Member ID
    int num_of_sec; // Number of seconds communicated
    int enroll_date; // Enrolment date
    int group_id;
    int group_type;

} AgencyMemberToPeopleContactMessage; // agency member to people contact message structure



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
    int enroll_date;

} RESISTANCE_MEMBER;



#endif //COMMON_H