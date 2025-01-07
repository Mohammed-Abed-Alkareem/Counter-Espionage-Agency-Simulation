#include "enemy.h"

Config config;


// Function to clean up the process
void cleanUp() {
    // Clean up code here when the process is terminated

    printf("Enemy process terminated\n");
}

// Main function
int main(int argc, char *argv[]) {

    // Register the cleanUp function to be called when the process is terminated
    atexit(cleanUp);
    // Check for correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Load the configuration file
    if (load_config(argv[1], &config) == -1) {
        fprintf(stderr, "Error loading config file\n");
        exit(EXIT_FAILURE);
    }
        
    char *key_str_target_prob = getenv("AGENCY_TO_ENEMY_TARGET_PROBABILITY_KEY");
    if (key_str_target_prob == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    char *key_str_attack_agency = getenv("ENEMY_TO_AGENCY_ATTACK_KEY");
    if (key_str_attack_agency == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    key_t key_target_prob = atoi(key_str_target_prob);
    key_t key_attack_agency = atoi(key_str_attack_agency);
    
    AgencyToEnemyTargetProbabilityMessage target_prob_msg;
   
    EnemyToAgencyAttackMessage attack_msg;

    while (1) {
        // Check for messages from the counter espionage agency
        if (msgrcv(key_target_prob, &target_prob_msg, sizeof(target_prob_msg), 0, IPC_NOWAIT) != -1) {
            // Process the message
            printf("Received target probability message from counter espionage agency\n");
            printf("Target probability: %f\n", target_prob_msg.target_probability);
            printf("Member ID: %d\n", target_prob_msg.member_id);
            // call a function that will decide if the enemy will attack or not
            if (target_prob_msg.target_probability > config.ENEMY_ATTACK_PROBABILITY) {
                printf("Enemy will attack the agency member %d\n", target_prob_msg.member_id);
                // send message to the agency to attack the member
                attack_msg.type = target_prob_msg.member_id;
                // attack_msg.attack_type = 1;//kill
                if (msgsnd(key_attack_agency, &attack_msg, sizeof(attack_msg), 0) == -1) {
                    perror("Error sending attack message to agency");
                }
            } else {
                printf("Enemy will not attack the agency member %d\n", target_prob_msg.member_id);
            }
        }
        // Check for message from the spy
        
    }
    

    return 0;
}
