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

            if (isdigit(value_str[0]) || (value_str[0] == '-' && isdigit(value_str[1]))) {
                int value = atoi(value_str); // Parse as integer
                if (strcmp(key, "simulation_duration") == 0) config->SIMULATION_DURATION = value;
                else if (strcmp(key, "counter_espionage_agency_member") == 0) config->COUNTER_ESPIONAGE_AGENCY_MEMBER = value;
                else if (strcmp(key, "resistance_group_creation_interval") == 0) config->RESISTANCE_GROUP_CREATION_INTERVAL = value;
                else if (strcmp(key, "resistance_member_min") == 0) config->RESISTANCE_MEMBER_MIN = value;
                else if (strcmp(key, "resistance_member_max") == 0) config->RESISTANCE_MEMBER_MAX = value;
                else if (strcmp(key, "max_killed_members") == 0) config->MAX_KILLED_MEMBERS = value;
                else if (strcmp(key, "max_injured_members") == 0) config->MAX_INJURED_MEMBERS = value;
                else if (strcmp(key, "light_injury_recovery_time") == 0) config->LIGHT_INJURY_RECOVERY_TIME = value;
                else if (strcmp(key, "agency_inactivity_limit") == 0) config->AGENCY_INACTIVITY_LIMIT = value;
                else {
                    fprintf(stderr, "Unexpected integer key: %s\n", key);
                }
            }

            
            // Check for float value
            else {
                float value = atof(value_str); // Parse as float
                if (strcmp(key, "military_group_probability") == 0) config->MILITARY_GROUP_PROBABILITY = value;
                else if (strcmp(key, "spy_probability") == 0) config->SPY_PROBABILITY = value;
                else if (strcmp(key, "suspicion_threshold") == 0) config->SUSPICION_THRESHOLD = value;
                else if (strcmp(key, "spy_confirmation_threshold") == 0) config->SPY_CONFIRMATION_THRESHOLD = value;
                else if (strcmp(key, "full_group_killed_probability") == 0) config->FULL_GROUP_KILLED_PROBABILITY = value;
                else if (strcmp(key, "mixed_outcome_probability") == 0) config->MIXED_OUTCOME_PROBABILITY = value;
                else if (strcmp(key, "kill_probability") == 0) config->KILL_PROBABILITY = value;
                else if (strcmp(key, "injury_probability") == 0) config->INJURY_PROBABILITY = value;
                else if (strcmp(key, "capture_probability") == 0) config->CAPTURE_PROBABILITY = value;
                else if (strcmp(key, "severe_injury_probability") == 0) config->SEVERE_INJURY_PROBABILITY = value;
                else if (strcmp(key, "enemy_attack_probability") == 0) config->ENEMY_ATTACK_PROBABILITY = value;
                else if (strcmp(key, "agency_member_target_probability") == 0) config->AGENCY_MEMBER_TARGET_PROBABILITY = value;
                else {
                    fprintf(stderr, "Unexpected float key: %s\n", key);
                }
            }
        }
    }

    fclose(file);
    return 0;
}
