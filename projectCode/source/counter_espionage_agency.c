#include "counter_espionage_agency.h"

// Global variables
AGENCY_MEMBER *MEMBERS;
pthread_mutex_t agency_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t analysis_thread, investigator_thread;
Config config;

// Shared memory for enemy attacks
typedef struct {
    int member_id;
    int attack_damage;
    int is_read;
} SharedMemoryAttack;

SharedMemoryAttack *shared_mem_attack;
int shm_id;

// Define the message queue IDs
int msg_agency_to_people_id;
int msg_agency_to_resistance_group_id;
int enemy_to_agency_attack_id;
int resistance_group_to_agency_id;

// Clean-up function called at exit
void cleanUp() {
    free(MEMBERS);
    shmdt(shared_mem_attack);
    shmctl(shm_id, IPC_RMID, NULL);
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

// Function to simulate agency member behavior
void* member_function(void* arg) {
    AGENCY_MEMBER *member = (AGENCY_MEMBER *)arg;

    while (1) {
        sleep(1); // Simulate member activity interval

        pthread_mutex_lock(&agency_lock);
        switch (member->status) {
            case ALIVE:
                // Perform actions for alive members
                update_enrollment_time_target_probability(member);
                send_contact_message(member);
                handle_attack(member);
                break;
            case SERIOUSLYINJURED:
                // Perform actions for injured members
                recover_from_injury(member);
                break;
            case LIGHTINJURED:
                // Perform actions for injured members
                recover_from_injury(member);
                break;
            case CAPTURED:
                // Perform actions for captured members
                handle_capture(member);
                break;
            case KILLED:
                // Perform actions for killed members
                handle_death(member);
                pthread_mutex_unlock(&agency_lock);
                pthread_exit(NULL);
                break;
            // Add more states as needed
            default:
                break;
        }

        // Check for attacks from the enemy
        if (shared_mem_attack[member->id - 1].is_read == 0 && shared_mem_attack[member->id - 1].member_id == member->id) {
            member->health -= shared_mem_attack[member->id - 1].attack_damage;
            shared_mem_attack[member->id - 1].is_read = 1;
            if (member->health <= 0) {
                member->status = KILLED;
            } else if (member->health < config.MIN_HEALTH) {
                member->status = SERIOUSLYINJURED;
            }
        }

        pthread_mutex_unlock(&agency_lock);
    }

    return NULL;
}

// Function to handle recovery from injury
void recover_from_injury(AGENCY_MEMBER *member) {
    if (member->status == INJURED) {
        member->health += random_integer((int)(config.RECOVERY_RATE * config.MIN_HEALTH), (int)(config.RECOVERY_RATE * member->health + config.MIN_HEALTH / 10));
        if (member->health >= config.MAX_HEALTH) {
            member->status = ALIVE;
            printf("Member %d has recovered from injury\n", member->id);
        }
    }
}

// Function to handle capture
void handle_capture(AGENCY_MEMBER *member) {
    // Implement capture logic here
}

// Function to handle death
void handle_death(AGENCY_MEMBER *member) {
    printf("Member %d is killed\n", member->id);
    // Implement additional death logic here
}

// Function to handle attacks
void handle_attack(AGENCY_MEMBER *member) {
    // Implement attack handling logic here
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

// Function to send contact message to people and agency
void send_contact_message(AGENCY_MEMBER *member) {
    // Send communication message to people and send the communication report to the agency
    AgencyMemberToPeopleContactMessage contact_message;
    contact_message.type = config.CIVILIAN_NUMBER + random_integer(1, config.CIVILIAN_NUMBER);
    contact_message.num_of_sec = random_integer(config.MIN_CONTACT_TIME, config.MAX_CONTACT_TIME);
    contact_message.member_id = member->id;

    // Send the message to the people
    if (msgsnd(msg_agency_to_people_id, &contact_message, sizeof(AgencyMemberToPeopleContactMessage), 0) == -1) {
        perror("Error sending message to people");
    }
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

    // Initialize shared memory for enemy attacks
    shm_id = shmget(IPC_PRIVATE, config.COUNTER_ESPIONAGE_AGENCY_MEMBER * sizeof(SharedMemoryAttack), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Shared memory creation failed");
        exit(1);
    }
    shared_mem_attack = (SharedMemoryAttack *)shmat(shm_id, NULL, 0);
    if (shared_mem_attack == (void *)-1) {
        perror("Shared memory attachment failed");
        exit(1);
    }
    for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
        shared_mem_attack[i].is_read = 1;
    }




    // Get the message queue IDs from environment variables
    char *key_str_agency_to_people = getenv("AGENCY_TO_PEOPLE_STATE_KEY");
    if (key_str_agency_to_people == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_agency_to_people_id = atoi(key_str_agency_to_people);

    char *key_str_agency_to_resistance = getenv("AGENCY_TO_RESISTANCE_MEMBER_STATE_KEY");
    if (key_str_agency_to_resistance == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    msg_agency_to_resistance_group_id = atoi(key_str_agency_to_resistance);

    char *key_str_enemy_to_agency = getenv("ENEMY_TO_AGENCY_ATTACK_KEY");
    if (key_str_enemy_to_agency == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    enemy_to_agency_attack_id = atoi(key_str_enemy_to_agency);

    char *key_str_resistance_to_agency = getenv("RESISTANCE_TO_AGENCY_MEMBER_STATE_REPORT_KEY");
    if (key_str_resistance_to_agency == NULL) {
        perror("Error getting environment variable");
        exit(1);
    }
    resistance_group_to_agency_id = atoi(key_str_resistance_to_agency);


    // get the message queue id 
    msg_agency_to_people_id = create_message_queue(msg_agency_to_people_id);
    msg_agency_to_resistance_group_id = create_message_queue(msg_agency_to_resistance_group_id);
    enemy_to_agency_attack_id = create_message_queue(enemy_to_agency_attack_id);
    resistance_group_to_agency_id = create_message_queue(resistance_group_to_agency_id);
    


    // Initialize agency members
    for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
        MEMBERS[i].id = i + 1;
        MEMBERS[i].health = config.MAX_HEALTH;
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
