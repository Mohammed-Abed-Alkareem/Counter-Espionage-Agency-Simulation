#include "resistance_group.h"

//--Globals--
int NUM_OF_MEMBERS = 0;
int NUM_OF_EXISTING_MEMBERS = 0;
int spy_exist = 0;

RESISTANCE_MEMBER *MEMBERS = NULL;

Config CONFIG;

// Mutexes
int general_attack = 0;
int general_attack_counter = 0;
int member_attack = -1;
pthread_mutex_t general_attack_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t general_attack_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t member_attack_mutex = PTHREAD_MUTEX_INITIALIZER;

// message queue vars 
int msg_regular_report_id = -1;
int msg_resistance_group_to_people_id = -1;
int msg_resistance_group_to_agency_communication_report_id = -1;
int msg_resistance_group_to_enemy_id = -1;
int msg_enemy_to_resistance_group_id = -1;
int msg_agency_to_resistance_group_id = -1;



// new functions 

// send contact message to people and agency
void send_contact_message(RESISTANCE_MEMBER *member){
    
            // send communication message to people and send the communcation report to the agency
            ResistanceMemberToPeopleContactMessage contact_message;
            contact_message.type = random_integer(1, CONFIG.CIVILIAN_NUMBER);
            contact_message.num_of_sec = random_integer(CONFIG.MIN_CONTACT_TIME, CONFIG.MAX_CONTACT_TIME);
            contact_message.group_id = member->group_id;
            contact_message.group_type = member->type;
            contact_message.member_id = member->id;

            // Send the message to the people
            if (msgsnd(msg_resistance_group_to_people_id, &contact_message, sizeof(ResistanceMemberToPeopleContactMessage), 0) == -1) {
                perror("Error sending message to people");
            }

            // Send the communication report to the agency
            PeopleContactReportMessage report_message;
            report_message.type = 1;
            report_message.group_id = member->group_id;
            report_message.member_num = member->id;
            report_message.people_number = contact_message.type;
            report_message.contact_time = contact_message.num_of_sec;

            // Send the message to the agency
            if (msgsnd(msg_resistance_group_to_agency_communication_report_id, &report_message, sizeof(PeopleContactReportMessage), 0) == -1) {
                perror("Error sending message to agency");
            }

            char console_message[200];
            sprintf(console_message, "Member %d contacted people %d for %d seconds\n", member->id, contact_message.type, contact_message.num_of_sec);
            print_color(console_message, YELLOW);

}


// handle attack by enemy
void handle_attack (RESISTANCE_MEMBER *member){

    pthread_mutex_lock(&member_attack_mutex);
    //check if the member is targeted by the enemy from the global var 
    if (member->id == member_attack) {
        // Member is targeted by the enemy
        member_attack = -1;
        pthread_mutex_unlock(&member_attack_mutex);

        being_attacked(member);
        return;

    }else {
        pthread_mutex_unlock(&member_attack_mutex);
    }
    
    pthread_mutex_lock(&general_attack_mutex);
    if (general_attack) {
        pthread_mutex_unlock(&general_attack_mutex);
        being_attacked(member);
        pthread_mutex_lock(&general_attack_counter_mutex);
        general_attack_counter++;
        if (general_attack_counter == NUM_OF_EXISTING_MEMBERS) {
            general_attack = 0;
            general_attack_counter = 0;
        }
        pthread_mutex_unlock(&general_attack_counter_mutex);
        return;
    }else {
        pthread_mutex_unlock(&general_attack_mutex);
    }

    // read the message queue from the enemy to check if the member is targeted
    EnemyToResistanceGroupAttackMessage enemy_attack_message;
    if (msgrcv(msg_enemy_to_resistance_group_id, &enemy_attack_message, sizeof(EnemyToResistanceGroupAttackMessage), member->group_id, IPC_NOWAIT) != -1) {
        // Check if the message is for the member
        if (enemy_attack_message.isGeneral){
            pthread_mutex_lock(&general_attack_mutex);
            general_attack=1;
            pthread_mutex_unlock(&general_attack_mutex);
            return;
        }else{
            pthread_mutex_lock(&member_attack_mutex);
            if(member_attack!= -1 ){
                // write the message  back to the message queue 
                if (msgsnd(msg_enemy_to_resistance_group_id, &enemy_attack_message, sizeof(EnemyToResistanceGroupAttackMessage), 0) == -1) {
                    perror("Error sending message back to enemy queue");
                }
                pthread_mutex_unlock(&member_attack_mutex);
                return;
            }
            member_attack = member->id;
            pthread_mutex_unlock(&member_attack_mutex);
            return;
        }
    }       
}


void being_attacked (RESISTANCE_MEMBER * member ){
        if (member->is_spy){
            member->health -= CONFIG.MIN_ATTACK_DAMAGE;
        }else {
            member->health -= random_integer(CONFIG.MIN_ATTACK_DAMAGE, CONFIG.MAX_ATTACK_DAMAGE);
        }
        
         // perform the attack
        if (member->health <= 0) {
            member->status = KILLED;
            NUM_OF_EXISTING_MEMBERS--;
            if (member->is_spy) {
            spy_exist--;
            }
            char console_message[200];
            sprintf(console_message, "Member %d was killed in an attack\n", member->id);
            print_color(console_message, RED);
        } else if (member->health < CONFIG.MIN_HEALTH) {
            member->status = SERIOUSLYINJURED;
            char console_message[200];
            sprintf(console_message, "Member %d was seriously injured in an attack\n", member->id);
            print_color(console_message, ORANGE);
        }else {
            member->status = LIGHTINJURED;
            char console_message[200];
            sprintf(console_message, "Member %d was lightly injured in an attack\n", member->id);
            print_color(console_message, YELLOW);
        }
        return;
}


void cleanUp() {
    // Clean up code here when the process is terminated

    // Free the memory allocated for the members
    free(MEMBERS);

    printf("Resistance group process terminated\n");
}


void *member_function(void *arg) {
    RESISTANCE_MEMBER *member = (RESISTANCE_MEMBER *)arg;
    char console_message [200];
    // Print the member information
    printf("Member %d created\n", member->id);

    // Infinite loop to simulate the member's life
    // while (1) {

        // 

        switch (member->status) {
            case ALIVE:
            
            // send contac message to people and report it to agency 
            if (random_float(0, 1) < CONFIG.PEOPLE_INTERACTION_RATE)    
                send_contact_message(member);
            wait_random_time_ms(1000 , 5000);



            // Member is alive, perform regular duties
            printf("Member %d is alive and performing duties\n", member->id);
            break;

            case ARRESTED:
            // Member is arrested, handle arrest scenario
            printf("Member %d is arrested\n", member->id);
            // Perform actions related to arrest
            break;
            case CAPTURED:
            // Member is captured, handle capture scenario  
            
            break;

            case SERIOUSLYINJURED:
            // Member is injured, handle injury scenario
            printf("Member %d is injured\n", member->id);
            break;

            case LIGHTINJURED:
            break;

            case DEAD:

            break;

            case ESCAPED:
            // Member has escaped, handle escape scenario

            break;

            case SPY:

            break;

            case INVISTIGATED:

            break;

            case ATTACKED:

            break;
            case KILLED:
            
            sprintf(console_message, "Member %d is killed From group %d\n", member->id, member->group_id);
            print_color(console_message, RED);
            break;

        // Wait for a random amount of time
        wait_random_time(CONFIG.RESISTANCE_MEMBER_MIN, CONFIG.RESISTANCE_MEMBER_MAX);

        // Check if the member is alive
        if (member->status == ALIVE) {
            // Print the member's health
            printf("Member %d health: %d\n", member->id, member->health);

            // Check if the member's health is below 50
            if (member->health < 50) {
                // Print a message that the member is injured
                printf("Member %d is injured\n", member->id);
            }

            // Check if the member's health is below 0
            if (member->health <= 0) {
                // Print a message that the member is dead
                printf("Member %d is dead\n", member->id);
                member->status = KILLED;
                NUM_OF_EXISTING_MEMBERS--;
                if (member->is_spy) {
                    spy_exist--;
                }
            }
        }
    // }

    return NULL;
}


int main(int argc, char *argv[]) {

    atexit(cleanUp);

    // check if the number of arguments is correct
    if (argc != 4) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    // Load the configuration file
    if (load_config(argv[1], &CONFIG) != 0) {
        printf("Failed to load the configuration file\n");
        return 1;
    }


    // get env for message queue keys
    char *key_str_regular_report = getenv("RESISTANCE_GROUP_TO_PEOPLE_KEY");
    if (key_str_regular_report == NULL) {
        perror("Error getting environment variable");
        return 1;
    }   
    msg_resistance_group_to_people_id = atoi(key_str_regular_report);

    char *key_str_communication_report = getenv("RESISTANCE_GROUP_TO_AGENCY_COMMUNICATION_REPORT_KEY");
    if (key_str_communication_report == NULL) {
        perror("Error getting environment variable");
        return 1;
    }   
    msg_resistance_group_to_agency_communication_report_id = atoi(key_str_communication_report);

    char *key_str_enemy = getenv("RESISTANCE_GROUP_TO_ENEMY_KEY");
    if (key_str_enemy == NULL) {
        perror("Error getting environment variable");
        return 1;
    }
    msg_resistance_group_to_enemy_id = atoi(key_str_enemy);

    char *key_str_enemy_to_resistance = getenv("ENEMY_TO_RESISTANCE_GROUP_KEY");
    if (key_str_enemy_to_resistance == NULL) {
        perror("Error getting environment variable");
        return 1;
    }
    msg_enemy_to_resistance_group_id = atoi(key_str_enemy_to_resistance);

    char *key_str_agency_to_resistance = getenv("AGENCY_TO_RESISTANCE_GROUP_KEY");
    if (key_str_agency_to_resistance == NULL) {
        perror("Error getting environment variable");
        return 1;
    }
    msg_agency_to_resistance_group_id = atoi(key_str_agency_to_resistance);




    // end of setting up message queues ids

    // Seed the random number generator with pid and time
    srand(time(NULL) ^ getpid());

    NUM_OF_MEMBERS = NUM_OF_EXISTING_MEMBERS = random_integer(CONFIG.RESISTANCE_MEMBER_MIN, CONFIG.RESISTANCE_MEMBER_MAX);

    // Allocate memory for the members
    MEMBERS = (RESISTANCE_MEMBER *)malloc(NUM_OF_MEMBERS * sizeof(RESISTANCE_MEMBER));

    // Check if memory allocation was successful
    if (MEMBERS == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    int group_id = atoi(argv[2]);

    int group_type = atoi(argv[3]);

    // Initialize the members information
    for (int i = 0; i < NUM_OF_MEMBERS; i++) {
        // Initialize the member
        MEMBERS[i].id = i + 1;
        MEMBERS[i].health = random_integer(CONFIG.MIN_HEALTH, CONFIG.MAX_HEALTH);
        MEMBERS[i].status = ALIVE;
        MEMBERS[i].type = propability_choice(CONFIG.MILITARY_GROUP_PROBABILITY) ? MILITARY : SOCIALIST;
        MEMBERS[i].is_spy = (spy_exist < 1) ? propability_choice(CONFIG.SPY_PROBABILITY) : 0;
        if (MEMBERS[i].is_spy) {
            spy_exist++;
        }
        MEMBERS[i].group_id = group_id;//get the group id from the command line
    }
    
    printf("Resistance group process created %d\n", group_id);


    // create a thread for each member
    for (int i = 0; i < NUM_OF_MEMBERS; i++) {
        pthread_create(&MEMBERS[i].thread_id, NULL, member_function, (void *)&MEMBERS[i]);

        //check for success
        if (MEMBERS[i].thread_id == 0) {
            printf("Failed to create thread for member %d\n", MEMBERS[i].id);
            return 1;
        }

    }

    // Wait for all threads to finish if a thread terminates then crate another one
    while (NUM_OF_EXISTING_MEMBERS > 0){

        for (int i = 0; i < NUM_OF_MEMBERS; i++) {
            if (MEMBERS[i].status == KILLED) {
                pthread_join(MEMBERS[i].thread_id, NULL);
                // Create a new thread for the dead member

                int wait_time = random_integer(CONFIG.RESISTANCE_MEMBER_MIN, CONFIG.RESISTANCE_MEMBER_MAX);

                MEMBERS[i].health = random_integer(CONFIG.MIN_HEALTH, CONFIG.MAX_HEALTH);
                
                MEMBERS[i].type = group_type;
                MEMBERS[i].is_spy = (spy_exist < 1) ? propability_choice(CONFIG.SPY_PROBABILITY) : 0;
                if (MEMBERS[i].is_spy) {
                    spy_exist++;
                }


                pthread_create(&MEMBERS[i].thread_id, NULL, member_function, (void *)&MEMBERS[i]);

                //check for success
                if (MEMBERS[i].thread_id == 0) {
                    printf("Failed to create thread for member %d\n", MEMBERS[i].id);
                    return 1;
                }
                MEMBERS[i].status = ALIVE;
            }
        }





    }

    return 0;
}