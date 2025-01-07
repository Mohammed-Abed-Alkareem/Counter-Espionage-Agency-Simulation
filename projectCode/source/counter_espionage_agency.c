#include "counter_espionage_agency.h"

// Global variables
AGENCY_MEMBER *MEMBERS;
pthread_mutex_t agency_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t analysis_thread, investigator_thread;
Config config;

// Clean-up function called at exit
void cleanUp() {
    free(MEMBERS);
    printf("Counter espionage agency process terminated\n");
}

// Function for analyzing data regularly
void* analyze_data(void* arg) {
    Config *config = (Config *)arg;

    while (1) {
        sleep(5); // Analyze data at regular intervals
        analyze_reports(); // Analyze reports from resistance groups
    }

    return NULL;
}

// Function to analyze reports from resistance groups
void analyze_reports() {
    printf("Analyzing reports from resistance groups...\n");
}

// Function to replace a killed or captured member
void replace_member(int index) {
    MEMBERS[index].id = index + 1;
    MEMBERS[index].health = random_integer(50, 100); // Example health range
    MEMBERS[index].status = ALIVE;
    MEMBERS[index].time_with_agency = time(NULL);

    // Restart thread for new member
    if (pthread_create(&MEMBERS[index].thread_id, NULL, member_function, (void *)&MEMBERS[index]) != 0) {
        printf("Failed to create thread for new member %d\n", MEMBERS[index].id);
    } else {
        printf("New member %d has joined the agency.\n", MEMBERS[index].id);
    }
}

void update_enrollment_time_target_probability(AGENCY_MEMBER *member) {
    member->time_with_agency = time(NULL) - member->time_with_agency;
    // Update the target probability related to the time with the agency, ensuring it stays between 0 and 1
    float value = log(member->time_with_agency + 1) * random_float(0.1, 0.5);
    member->target_probability = 1.0 / (1.0 + exp(-value));//sigmoid function
    printf("Member %d target probability: %f\n", member->id, member->target_probability);
}

// Function to simulate agency member behavior
void* member_function(void* arg) {
    AGENCY_MEMBER *member = (AGENCY_MEMBER *)arg;

    while (1) {
        sleep(1); // Simulate member activity interval

        pthread_mutex_lock(&agency_lock);
        if (member->status == KILLED || member->status == CAPTURED || member->status == INJURED) {
            // Replace the member
            replace_member(member->id - 1);
            pthread_mutex_unlock(&agency_lock);
            pthread_exit(NULL);
        }
        update_enrollment_time_target_probability(member);

        pthread_mutex_unlock(&agency_lock);
    }

    return NULL;
}

// Function to investigate and arrest suspicious members
void* investigator_function(void* arg) {
    Config *config = (Config *)arg;

    while (1) {
        sleep(10); // Investigate at regular intervals
        // Implement investigation logic here
        printf("Investigating suspicious members...\n");
    }

    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    // Register clean-up function
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

    // Allocate memory for agency members
    MEMBERS = (AGENCY_MEMBER *)malloc(config.COUNTER_ESPIONAGE_AGENCY_MEMBER * sizeof(AGENCY_MEMBER));
    if (MEMBERS == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    // Initialize agency members
    for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
        MEMBERS[i].id = i + 1;
        MEMBERS[i].health = random_integer(config.MIN_HEALTH, config.MAX_HEALTH);
        MEMBERS[i].status = ALIVE;
        MEMBERS[i].time_with_agency = time(NULL);
        MEMBERS[i].target_probability = 0;

        // Create a thread for each member
        if (pthread_create(&MEMBERS[i].thread_id, NULL, member_function, (void *)&MEMBERS[i]) != 0) {
            printf("Failed to create thread for member %d\n", MEMBERS[i].id);
            return 1;
        }
    }

    // Get the key to send message to enemy about the target probability
    char *key_str = getenv("AGENCY_TO_ENEMY_TARGET_PROBABILITY_KEY");
    if (key_str == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    key_t key_target_prob = atoi(key_str);

    // Create the analysis thread
    if (pthread_create(&analysis_thread, NULL, analyze_data, (void *)&config) != 0) {
        perror("Failed to create analysis thread");
        return 1;
    }

    // Create the investigator thread
    if (pthread_create(&investigator_thread, NULL, investigator_function, (void *)&config) != 0) {
        perror("Failed to create investigator thread");
        return 1;
    }

    AgencyToEnemyTargetProbabilityMessage target_prob_msg;
    // Every regular time send the target probability to the enemy
    while (1) {
        sleep(5); // Send the target probability to the enemy every 5 seconds
        for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
            // Send the target probability to the enemy
            target_prob_msg.member_id = MEMBERS[i].id;
            target_prob_msg.target_probability = MEMBERS[i].target_probability;
            if (msgsnd(key_target_prob, &target_prob_msg, sizeof(target_prob_msg.target_probability) + sizeof(target_prob_msg.member_id), 0) == -1) {
                perror("Error sending target probability message to enemy");
            }
        }
    }

    return 0;
}
