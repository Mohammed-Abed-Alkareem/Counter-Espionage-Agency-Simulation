#include "enemy.h"

// Global variables
Config config;
int msg_enemy_to_resistance_group_id = -1;
int msg_civilian_to_enemy_id = -1;
int enemy_id = 0;
SpyToEnemyReportMessage message_from_spy;

// Function to clean up the process
void cleanUp() {
    printf("Enemy process terminated\n");
}

// Function to handle messages from civilians and spies
void handle_messages() {
    // Non-blocking check for messages from civilians
    if (msgrcv(msg_civilian_to_enemy_id, &message_from_spy, sizeof(SpyToEnemyReportMessage), 0, IPC_NOWAIT) != -1) {
        printf("Received contact message from civilian ");
        handle_attacks();
        
    } else {
        // No message received, continue processing
        printf("No message received from civilians\n");
    }
    // Non-blocking check for messages from spies
    ////////////////////////////////////////////////////////////////////////
}




// Function to handle attacks
void handle_attacks() {

    if (message_from_spy.isCounterAttack == 0){//if the message is to attack the resistance group
        float randomFloat = random_float(-1, 1);
        //using sigmoid function to calculate the probability of being targeted
        float target_probability = 1 / (1 + exp(-(randomFloat * (message_from_spy.num_of_sec + message_from_spy.enroll_data))));//sigmoid function
        if (target_probability > config.ENEMY_ATTACK_PROBABILITY) {//if the probability is greater than the attack probability then attack
            int is_general_attack = random_float(0, 1) < config.GENERAL_ATTACK_PROBABILITY;
            if (is_general_attack){
                // Perform a general attack on a resistance group
                EnemyToResistanceGroupAttackMessage attack_message;
                attack_message.type = message_from_spy.group_id;
                attack_message.isGeneral = 1;
                attack_message.time_sent = time(NULL);
                attack_message.attack_type = random_integer(1, 3);//randomly select the attack type
                if (msgsnd(msg_enemy_to_resistance_group_id, &attack_message, sizeof(EnemyToResistanceGroupAttackMessage), 0) == -1) {
                    perror("Error sending general attack message");
                }
            } else {//if the attack is not general
                // Target a specific member of a resistance group
                EnemyToResistanceGroupAttackMessage attack_message;
                attack_message.type = message_from_spy.group_id;
                attack_message.isGeneral = 0;
                attack_message.member_id = message_from_spy.group_member;
                attack_message.time_sent = time(NULL);
                if (msgsnd(msg_enemy_to_resistance_group_id, &attack_message, sizeof(EnemyToResistanceGroupAttackMessage), 0) == -1) {
                    perror("Error sending targeted attack message");
                }
            }
        }    
            
    } else {//if the message is to attack the agency
        float randomFloat = random_float(-1, 1);
        //using sigmoid function to calculate the probability of being targeted
        float target_probability = 1 / (1 + exp(-(randomFloat * (message_from_spy.num_of_sec + message_from_spy.enroll_data))));//sigmoid function
        if (target_probability > config.ENEMY_ATTACK_PROBABILITY) {//if the probability is greater than the attack probability then attack
            // Perform a general attack on the agency
            int is_general_attack = random_float(0, 1) < config.GENERAL_ATTACK_PROBABILITY;
            if (is_general_attack){
                // Perform a general attack on a resistance group
                EnemyToResistanceGroupAttackMessage attack_message;
                attack_message.type = message_from_spy.group_id;
                attack_message.isGeneral = 1;
                attack_message.time_sent = time(NULL);
                attack_message.attack_type = random_integer(1, 3);//randomly select the attack type
                if (msgsnd(msg_enemy_to_resistance_group_id, &attack_message, sizeof(EnemyToResistanceGroupAttackMessage), 0) == -1) {
                    perror("Error sending general attack message");
                }
            } else {//if the attack is not general
                // Target a specific member of a resistance group
                EnemyToResistanceGroupAttackMessage attack_message;
                attack_message.type = message_from_spy.group_id;
                attack_message.isGeneral = 0;
                attack_message.member_id = message_from_spy.group_member;
                attack_message.time_sent = time(NULL);
                attack_message.attack_type = random_integer(1, 3);//randomly select the attack type
                if (msgsnd(msg_enemy_to_resistance_group_id, &attack_message, sizeof(EnemyToResistanceGroupAttackMessage), 0) == -1) {
                    perror("Error sending targeted attack message");
                }
            }
        }
    }
}



// Main function
int main(int argc, char *argv[]) {
    atexit(cleanUp);

    // Validate arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <config file> <enemy_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Load the configuration file
    if (load_config(argv[1], &config) == -1) {
        fprintf(stderr, "Error loading config file\n");
        exit(EXIT_FAILURE);
    }

    // Get the enemy ID
    enemy_id = atoi(argv[2]);

    // Get the message queue keys from environment variables
    char *key_str_enemy_to_resistance_group = getenv("ENEMY_TO_RESISTANCE_GROUP_ATTACK_KEY");
    if (key_str_enemy_to_resistance_group == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_enemy_to_resistance_group_id = atoi(key_str_enemy_to_resistance_group);

    char *key_str_civilian_to_enemy = getenv("CIVILIAN_TO_ENEMY_REPORT_KEY");
    if (key_str_civilian_to_enemy == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_civilian_to_enemy_id = atoi(key_str_civilian_to_enemy);

    printf("Enemy process created\n");

    while (1){
        handle_messages(); 
    }

    return 0;
}