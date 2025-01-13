#include "civilian.h"

// Global variables
Config config;
int msg_resistance_group_to_people_id = -1;
int msg_people_to_enemy_id = -1;
int is_spy = 0;
int civilian_id = 0;
int msg_agency_to_people_id;
int msg_agency_to_people_state_id;
// Function to clean up the process
void cleanUp() {
    printf("Civilian process terminated\n");
}

// Function to handle messages from resistance groups
void handle_contact_messages() {
    ResistanceMemberToPeopleContactMessage contact_message_from_resistance_group;
    // Non-blocking check for messages from resistance groups
    if (msgrcv(msg_resistance_group_to_people_id, &contact_message_from_resistance_group, sizeof(ResistanceMemberToPeopleContactMessage), 0, IPC_NOWAIT) != -1) {
        printf("Received contact message from resistance group %d, member %d, contact time %d seconds\n",
               contact_message_from_resistance_group.group_id, contact_message_from_resistance_group.member_id, contact_message_from_resistance_group.num_of_sec);
        // Send the message to the enemy if the civilian is a spy
        if (is_spy) {
            // Send the message to the enemy
            SpyToEnemyReportMessage report_message;
            report_message.type = random_integer(1, config.ENEMY_NUMBER);
            report_message.group_id = contact_message_from_resistance_group.group_id;
            report_message.group_type = contact_message_from_resistance_group.group_type;
            report_message.group_member = contact_message_from_resistance_group.member_id;
            report_message.num_of_sec = contact_message_from_resistance_group.num_of_sec;
            report_message.enroll_data = 0;//not used here
            report_message.isCounterAttack = 0;//to attack the resistance group
            if (msgsnd(msg_people_to_enemy_id, &report_message, sizeof(SpyToEnemyReportMessage), 0) == -1) {
                perror("Error sending message to enemy");
            }
            
        }
    } else {
        // No message received, continue processing
        printf("No message received from resistance groups\n");
    }
    // Non-blocking check for messages from the agency members (contact with people)
    AgencyMemberToPeopleContactMessage contact_message_from_agency;
    if (msgrcv(msg_agency_to_people_id, &contact_message_from_agency, sizeof(AgencyMemberToPeopleContactMessage), 0, IPC_NOWAIT) != -1) {
        printf("Received contact message from agency member %d, contact time %d seconds\n",
               contact_message_from_agency.member_id, contact_message_from_agency.num_of_sec);
        if (is_spy) {
            // Send the message to the enemy
            SpyToEnemyReportMessage report_message;
            report_message.type = random_integer(1, config.ENEMY_NUMBER);
            report_message.group_id = contact_message_from_agency.group_id;
            report_message.group_type = contact_message_from_agency.group_type;
            report_message.group_member = contact_message_from_agency.member_id;
            report_message.num_of_sec = contact_message_from_agency.num_of_sec;
            report_message.enroll_data = contact_message_from_agency.enroll_date;
            report_message.isCounterAttack = 1;//to attack the agency
            if (msgsnd(msg_people_to_enemy_id, &report_message, sizeof(SpyToEnemyReportMessage), 0) == -1) {
                perror("Error sending message to enemy");
            }
        }
    } else {
        // No message received, continue processing
        printf("No message received from agency members\n");
    }
    // Non-blocking check for messages from the agency members (State)
    AgencyToPeopleStateMessage state_message_from_agency;
    if (msgrcv(msg_agency_to_people_state_id, &state_message_from_agency, sizeof(AgencyToPeopleStateMessage), 0, IPC_NOWAIT) != -1) {
        printf("Received state message from agency member %d, state %d\n",
               state_message_from_agency.member_id, state_message_from_agency.state);

    } else {
        // No message received, continue processing
        printf("No message received from agency members\n");
    }


}


// Main function
int main(int argc, char *argv[]) {
    atexit(cleanUp);

    // Validate arguments
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <config file> <is_spy>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Load the configuration file
    if (load_config(argv[1], &config) == -1) {
        fprintf(stderr, "Error loading config file\n");
        exit(EXIT_FAILURE);
    }

    // Determine if the civilian is a spy
    is_spy = atoi(argv[2]);

    // get the civilian ID
    civilian_id = atoi(argv[3]);

    // Get the message queue keys from environment variables
    char *key_str_resistance_group_to_people = getenv("RESISTANCE_TO_PEOPLE_CONTACT_KEY");
    if (key_str_resistance_group_to_people == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_resistance_group_to_people_id = atoi(key_str_resistance_group_to_people);


    if (is_spy){
        char *key_str_people_to_enemy = getenv("CIVILIAN_TO_ENEMY_REPORT_KEY");
        if (key_str_people_to_enemy == NULL) {
            perror("Error getting environment variable");
            exit(1);
        }
        msg_people_to_enemy_id = atoi(key_str_people_to_enemy);
    }

    char *key_str_agency_to_people = getenv("AGENCY_TO_CIVILIAN_CONTACT_REPORT_KEY");
    if (key_str_agency_to_people == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_agency_to_people_id = atoi(key_str_agency_to_people);
    msg_agency_to_people_id = msgget(msg_agency_to_people_id, 0666 | IPC_CREAT);


    char *key_str_agency_to_people_state = getenv("AGENCY_TO_PEOPLE_STATE_KEY");
    if (key_str_agency_to_people_state == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_agency_to_people_state_id = atoi(key_str_agency_to_people_state);
    msg_agency_to_people_state_id = msgget(msg_agency_to_people_state_id, 0666 | IPC_CREAT);
    

    printf("Civilian process created\n");

    // Handle messages from resistance groups and agency members
    while (1)
    {
        handle_contact_messages();
    }
    

    return 0;
}