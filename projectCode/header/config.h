// Updated config.h
#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

typedef struct {
    int SIMULATION_DURATION;
    int COUNTER_ESPIONAGE_AGENCY_MEMBER;
    int RESISTANCE_GROUP_CREATION_INTERVAL;
    int RESISTANCE_MEMBER_MIN;
    int RESISTANCE_MEMBER_MAX;
    int RESISTANCE_GROUP_MAX;
    float MILITARY_GROUP_PROBABILITY;
    float SPY_PROBABILITY;
    float SUSPICION_THRESHOLD;
    float SPY_CONFIRMATION_THRESHOLD;
    float FULL_GROUP_KILLED_PROBABILITY;
    float MIXED_OUTCOME_PROBABILITY;
    float KILL_PROBABILITY;
    float INJURY_PROBABILITY;
    float CAPTURE_PROBABILITY;
    int LIGHT_INJURY_RECOVERY_TIME;
    float SEVERE_INJURY_PROBABILITY;
    float ENEMY_ATTACK_PROBABILITY;
    float AGENCY_MEMBER_TARGET_PROBABILITY;
    int MAX_KILLED_MEMBERS;
    int MAX_INJURED_MEMBERS;
    int AGENCY_INACTIVITY_LIMIT;
    int CIVILIAN_NUMBER;
    int ENEMY_NUMBER;

    // New parameters
    float RATE_KEEP_ARRESTING_RELEASING;
    float RATE_INNOCENT_OR_GUILTY;
    int ATTACK_DURATION_INCREASE;
    float PEOPLE_INTERACTION_RATE;
    int PEOPLE_INTERACTION_RANGE;
    float RECOVERY_RATE;
    float MILITARY_GROUP_ATTACK_RATE;
    float CIVILIAN_GROUP_ATTACK_RATE;
    float RATIO_SPY;
    int MAX_SURVIVAL_TIME;
    int RESISTANCE_GROUP_INITIAL;
    int MAX_HEALTH;
    int MIN_HEALTH;
    int MIN_CONTACT_TIME;
    int MAX_CONTACT_TIME;
    
    int MAX_ATTACK_DAMAGE;
    int MIN_ATTACK_DAMAGE;
} Config;


int load_config(const char *filename, Config *config);

#endif // CONFIG_H
