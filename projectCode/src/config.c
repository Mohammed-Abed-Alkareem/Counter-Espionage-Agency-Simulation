#include "config.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// Function to load configuration settings from a specified file
int load_config(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening config file");
        return -1;
    }

    // Initialize all configuration values to default values
    config->SIMULATION_DURATION = 300;
    config->COUNTER_ESPIONAGE_AGENCY_MEMBER = 5;
    config->RESISTANCE_GROUP_CREATION_INTERVAL = 10;
    config->RESISTANCE_MEMBER_MIN = 3;
    config->RESISTANCE_MEMBER_MAX = 10;
    config->MILITARY_GROUP_PROBABILITY = 0.5f;
    config->SPY_PROBABILITY = 0.2f;
    config->SUSPICION_THRESHOLD = 0.7f;
    config->SPY_CONFIRMATION_THRESHOLD = 0.8f;
    config->FULL_GROUP_KILLED_PROBABILITY = 0.2f;
    config->MIXED_OUTCOME_PROBABILITY = 0.8f;
    config->KILL_PROBABILITY = 0.5f;
    config->INJURY_PROBABILITY = 0.3f;
    config->CAPTURE_PROBABILITY = 0.2f;
    config->LIGHT_INJURY_RECOVERY_TIME = 10;
    config->SEVERE_INJURY_PROBABILITY = 0.3f;
    config->ENEMY_ATTACK_PROBABILITY = 0.4f;
    config->AGENCY_MEMBER_TARGET_PROBABILITY = 0.2f;
    config->MAX_KILLED_MEMBERS = 50;
    config->MAX_INJURED_MEMBERS = 30;
    config->AGENCY_INACTIVITY_LIMIT = 20;


    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Ignore comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[50];
        char value_str[50];
        if (sscanf(line, "%49[^=]=%49s", key, value_str) == 2) {

            // Check for integer value

            if (atof(value_str) == round(atof(value_str))) { // Check if value is an integer
                int value = atoi(value_str); // Parse as integer
                if (strcmp(key, "SIMULATION_DURATION") == 0) config->SIMULATION_DURATION = value;
                else if (strcmp(key, "COUNTER_ESPIONAGE_AGENCY_MEMBER") == 0) config->COUNTER_ESPIONAGE_AGENCY_MEMBER = value;
                else if (strcmp(key, "RESISTANCE_GROUP_CREATION_INTERVAL") == 0) config->RESISTANCE_GROUP_CREATION_INTERVAL = value;
                else if (strcmp(key, "RESISTANCE_MEMBER_MIN") == 0) config->RESISTANCE_MEMBER_MIN = value;
                else if (strcmp(key, "RESISTANCE_MEMBER_MAX") == 0) config->RESISTANCE_MEMBER_MAX = value;
                else if (strcmp(key, "MAX_KILLED_MEMBERS") == 0) config->MAX_KILLED_MEMBERS = value;
                else if (strcmp(key, "MAX_INJURED_MEMBERS") == 0) config->MAX_INJURED_MEMBERS = value;
                else if (strcmp(key, "LIGHT_INJURY_RECOVERY_TIME") == 0) config->LIGHT_INJURY_RECOVERY_TIME = value;
                else if (strcmp(key, "AGENCY_INACTIVITY_LIMIT") == 0) config->AGENCY_INACTIVITY_LIMIT = value;
                else if(strcmp(key,"CIVILIAN_NUMBER") == 0) config->CIVILIAN_NUMBER = value;
                else if(strcmp(key,"RESISTANCE_GROUP_MAX") == 0) config->RESISTANCE_GROUP_MAX = value;
                else {
                    fprintf(stderr, "Unexpected integer key: %s\n", key);
                }
            }

            
            // Check for float value
            else {
                float value = atof(value_str); // Parse as float
                if (strcmp(key, "MILITARY_GROUP_PROBABILITY") == 0) config->MILITARY_GROUP_PROBABILITY = value;
                else if (strcmp(key, "SPY_PROBABILITY") == 0) config->SPY_PROBABILITY = value;
                else if (strcmp(key, "SUSPICION_THRESHOLD") == 0) config->SUSPICION_THRESHOLD = value;
                else if (strcmp(key, "SPY_CONFIRMATION_THRESHOLD") == 0) config->SPY_CONFIRMATION_THRESHOLD = value;
                else if (strcmp(key, "FULL_GROUP_KILLED_PROBABILITY") == 0) config->FULL_GROUP_KILLED_PROBABILITY = value;
                else if (strcmp(key, "MIXED_OUTCOME_PROBABILITY") == 0) config->MIXED_OUTCOME_PROBABILITY = value;
                else if (strcmp(key, "KILL_PROBABILITY") == 0) config->KILL_PROBABILITY = value;
                else if (strcmp(key, "INJURY_PROBABILITY") == 0) config->INJURY_PROBABILITY = value;
                else if (strcmp(key, "CAPTURE_PROBABILITY") == 0) config->CAPTURE_PROBABILITY = value;
                else if (strcmp(key, "SEVERE_INJURY_PROBABILITY") == 0) config->SEVERE_INJURY_PROBABILITY = value;
                else if (strcmp(key, "ENEMY_ATTACK_PROBABILITY") == 0) config->ENEMY_ATTACK_PROBABILITY = value;
                else if (strcmp(key, "AGENCY_MEMBER_TARGET_PROBABILITY") == 0) config->AGENCY_MEMBER_TARGET_PROBABILITY = value;
                else {
                    fprintf(stderr, "Unexpected float key: %s\n", key);
                }
            }
        }
    }

    fclose(file);
    return 0;
}
