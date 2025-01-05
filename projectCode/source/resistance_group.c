#include "resistance_group.h"

//--Globals--
int NUM_OF_MEMBERS = 0;
int NUM_OF_EXISTING_MEMBERS = 0;
int spy_exist = 0;

RESISTANCE_MEMBER *MEMBERS = NULL;

Config CONFIG;




void cleanUp() {
    // Clean up code here when the process is terminated

    printf("Resistance group process terminated\n");
}


void *member_function(void *arg) {
    RESISTANCE_MEMBER *member = (RESISTANCE_MEMBER *)arg;

    // Print the member information
    printf("Member %d created\n", member->id);

    // Infinite loop to simulate the member's life
    while (1) {
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
                member->status = DEAD;
                NUM_OF_EXISTING_MEMBERS--;
                if (member->is_spy) {
                    spy_exist--;
                }
            }
        }
    }

    return NULL;
}


int main(int argc, char *argv[]) {

    atexit(cleanUp);

    // check if the number of arguments is correct
    if (argc != 2) {
        printf("Usage: %s <config_file>\n", argv[0]);
        return 1;
    }

    // Load the configuration file
    if (load_config(argv[1], &CONFIG) != 0) {
        printf("Failed to load the configuration file\n");
        return 1;
    }

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

    // Initialize the members information
    for (int i = 0; i < NUM_OF_MEMBERS; i++) {
        // Initialize the member
        MEMBERS[i].id = i + 1;
        MEMBERS[i].health = random_integer(CONFIG.RESISTANCE_MEMBER_HEALTH_MIN, CONFIG.RESISTANCE_MEMBER_HEALTH_MAX);
        MEMBERS[i].status = ALIVE;
        MEMBERS[i].type = propability_choice(CONFIG.MILITARY_GROUP_PROBABILITY) ? MILITARY : SOCIALIST;
        MEMBERS[i].is_spy = (spy_exist < 1) ? propability_choice(CONFIG.SPY_PROBABILITY) : 0;
        if (MEMBERS[i].is_spy) {
            spy_exist++;
        }
    }

    printf("Resistance group process created\n");


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
            if (MEMBERS[i].status == DEAD) {
                pthread_join(MEMBERS[i].thread_id, NULL);
                // Create a new thread for the dead member

                int wait_time = random_integer(CONFIG.RESISTANCE_MEMBER_MIN, CONFIG.RESISTANCE_MEMBER_MAX);

                MEMBERS[i].health = random_integer(CONFIG.RESISTANCE_MEMBER_HEALTH_MIN, CONFIG.RESISTANCE_MEMBER_HEALTH_MAX);
                
                MEMBERS[i].type = propability_choice(CONFIG.MILITARY_GROUP_PROBABILITY) ? MILITARY : SOCIALIST;
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