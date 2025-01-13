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



void handel_message(){
    // Read message from the people process
    if (msgrcv(msg_civilian_to_enemy_id, &message_from_spy, sizeof(SpyToEnemyReportMessage) - sizeof(long), 0, 0) == -1) {
        perror("Error receiving message from civilian to enemy");
        exit(1);
    }else{
        
    // Decide whether to send an attack message based on the received message
    if (random_float(0,1)< config.ENEMY_ATTACK_PROBABILITY) {
        EnemyToResistanceGroupAttackMessage attack_message;
        attack_message.type = message_from_spy.group_id;
        attack_message.member_id = message_from_spy.group_member;
        attack_message.attack_type = 1; // Example attack type
        attack_message.isGeneral = 0; // Specific attack
        attack_message.time_sent = time(NULL);

        // Send the attack message to the resistance group
        if (msgsnd(msg_enemy_to_resistance_group_id, &attack_message, sizeof(EnemyToResistanceGroupAttackMessage) - sizeof(long), 0) == -1) {
            perror("Error sending attack message to resistance group");
            exit(1);
        }

        // printf("Sent attack message to resistance group:\n");
        // printf("Group ID: %ld\n", attack_message.type);
        // printf("Member ID: %d\n", attack_message.member_id);
        // printf("Attack Type: %d\n", attack_message.attack_type);
        // printf("Is General: %d\n", attack_message.isGeneral);
        // printf("Time Sent: %d\n", attack_message.time_sent);
    }
    }


    

    // printf("Received message from spy:\n");
    // printf("Process ID: %d\n", message_from_spy.process_id);
    // printf("Group ID: %d\n", message_from_spy.group_id);
    // printf("Group Type: %d\n", message_from_spy.group_type);
    // printf("Group Member: %d\n", message_from_spy.group_member);
    // printf("Number of Seconds: %d\n", message_from_spy.num_of_sec);
    // printf("Enroll Data: %d\n", message_from_spy.enroll_data);
    // printf("Is Counter Attack: %d\n", message_from_spy.isCounterAttack);


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
        handel_message(); 
    }

    return 0;
}