#include "main.h"


// --- Globals ---
pid_t *resistance_group_pid;
pid_t counter_espionage_agency_pid;
pid_t civilian_pid, enemy_pid;

pthread_t thread_fork_resistance_group;

int resistance_group_counter = 0;

SharedData *shared_data;

Config config;
 
// int alarm_triggered = 0; 

// --- IPC keys ---

// ----- Shared memory -----
key_t shm_data_key; // Shared memory key between all processes to share data

// --- Message queues ---
int msg_queue_ids[7]; // Message queue ids

// ------IPC ids------
int shm_data_id = -1;
int msg_resistance_agency_id = -1;



// main function
int main(int argc, char *argv[]) {

    atexit(cleanup);//register the cleanup function to be called at the end of the program

    // validate arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Load config file
    if (load_config(argv[1], &config) == -1) {
        fprintf(stderr, "Error loading config file\n");
        exit(EXIT_FAILURE);
    }
 
    signal(SIGINT, handle_interrupt);//register the signal handler for interrupt signal

// ============shared memories==================

    // Shared memory for data
    shm_data_key = key_generator('A');

    shm_data_id = shmget(shm_data_key, sizeof(SharedData), IPC_CREAT | 0666);//create shared memory
    if (shm_data_id == -1) {
        perror("Shared memory creation failed");
        cleanup();
        exit(1);
    }

    // Attach shared memory to shared_data
    shared_data = (SharedData *)shmat(shm_data_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Shared memory attach failed");
        cleanup();
        exit(1);
    }

    // initalize the shared data
    shared_data->number_killed_members = 0;
    shared_data->number_injured_members = 0;    
    shared_data->number_captured_members = 0;


    // --- Message queues ---
    
    //create message queue key between resistance groups and counter espionage agency
    // send reports from resistance member to agency about people contact with 
    key_t resistance_to_agency_people_contact_report_key = key_generator('C');
    // send reports from resistance member to agency about his state regularly
    key_t resisitance_to_agency_member_state_report_key = key_generator('D');
    // send reports from agency to people about their state when arrested , caught , killed ....
    key_t agency_to_people_state_key = key_generator('E');
    // send reports from agency to resistance member state when arrested , caught , killed ....
    key_t agency_to_resistance_member_state_key = key_generator('I');
    // send reports from spy to enemy when he has info 
    key_t spy_to_enemy_report_key = key_generator('F');
    // enemy to resistance group attack message 
    key_t enemy_to_resistance_group_attack_key = key_generator('G');
    // resistance to people contact messge 
    key_t resistance_to_people_contact_key = key_generator('H');

    //create message queues for the keys
    msg_queue_ids[0] =  create_message_queue(resistance_to_agency_people_contact_report_key);
    msg_queue_ids[1] =  create_message_queue(resisitance_to_agency_member_state_report_key);
    msg_queue_ids[2] =  create_message_queue(agency_to_people_state_key);
    msg_queue_ids[3] =  create_message_queue(agency_to_resistance_member_state_key);
    msg_queue_ids[4] =  create_message_queue(spy_to_enemy_report_key);
    msg_queue_ids[5] =  create_message_queue(enemy_to_resistance_group_attack_key);
    msg_queue_ids[6] =  create_message_queue(resistance_to_people_contact_key);

    // ============converting keys to string==================
    // shared memory key 
    char shm_data_key_str[20];

    snprintf(shm_data_key_str, sizeof(shm_data_key_str), "%d", shm_data_key);

    setenv("SHM_DATA_KEY", shm_data_key_str, 1);

    // message queue keys
    char resistance_to_agency_people_contact_report_key_str[20];
    char resisitance_to_agency_member_state_report_key_str[20];
    char agency_to_people_state_key_str[20];
    char agency_to_resistance_member_state_key_str[20];
    char spy_to_enemy_report_key_str[20];
    char enemy_to_resistance_group_attack_key_str[20];
    char resistance_to_people_contact_key_str[20];

    snprintf(resistance_to_agency_people_contact_report_key_str, sizeof(resistance_to_agency_people_contact_report_key_str), "%d", resistance_to_agency_people_contact_report_key);
    snprintf(resisitance_to_agency_member_state_report_key_str, sizeof(resisitance_to_agency_member_state_report_key_str), "%d", resisitance_to_agency_member_state_report_key);
    snprintf(agency_to_people_state_key_str, sizeof(agency_to_people_state_key_str), "%d", agency_to_people_state_key);
    snprintf(agency_to_resistance_member_state_key_str, sizeof(agency_to_resistance_member_state_key_str), "%d", agency_to_resistance_member_state_key);
    snprintf(spy_to_enemy_report_key_str, sizeof(spy_to_enemy_report_key_str), "%d", spy_to_enemy_report_key);
    snprintf(enemy_to_resistance_group_attack_key_str, sizeof(enemy_to_resistance_group_attack_key_str), "%d", enemy_to_resistance_group_attack_key);
    snprintf(resistance_to_people_contact_key_str, sizeof(resistance_to_people_contact_key_str), "%d", resistance_to_people_contact_key);

    setenv("RESISTANCE_TO_AGENCY_PEOPLE_CONTACT_REPORT_KEY", resistance_to_agency_people_contact_report_key_str, 1);
    setenv("RESISTANCE_TO_AGENCY_MEMBER_STATE_REPORT_KEY", resisitance_to_agency_member_state_report_key_str, 1);
    setenv("AGENCY_TO_PEOPLE_STATE_KEY", agency_to_people_state_key_str, 1);
    setenv("AGENCY_TO_RESISTANCE_MEMBER_STATE_KEY", agency_to_resistance_member_state_key_str, 1);
    setenv("SPY_TO_ENEMY_REPORT_KEY", spy_to_enemy_report_key_str, 1);
    setenv("ENEMY_TO_RESISTANCE_GROUP_ATTACK_KEY", enemy_to_resistance_group_attack_key_str, 1);
    setenv("RESISTANCE_TO_PEOPLE_CONTACT_KEY", resistance_to_people_contact_key_str, 1);




// ============= Forking processes ===========

    // Fork counter espionage agency
    if ((counter_espionage_agency_pid = fork()) == 0) {
        // execl("./bin/counter_espionage_agency", "counter_espionage_agency", argv[1], NULL);
        execl("/home/adduser/ENCS4330/Projects/Project3/Counter-Espionage-Agency-Simulation/projectCode/bin/counter_espionage_agency", "counter_espionage_agency", argv[1], NULL);
        perror("Counter espionage agency process failed");
        exit(1);
    }
    
    //  Fork civilians
   if (civilian_pid = fork() == 0) {
        // execl("./bin/civilian", "civilian", argv[1], NULL);
        execl("/home/adduser/ENCS4330/Projects/Project3/Counter-Espionage-Agency-Simulation/projectCode/bin/civilian", "civilian", argv[1], NULL);
        perror("Civilian process failed");
        exit(1);
    }

    //fork enemy
    if (enemy_pid = fork() == 0) {
        // execl("./bin/enemy", "enemy", argv[1], NULL);
        execl("/home/adduser/ENCS4330/Projects/Project3/Counter-Espionage-Agency-Simulation/projectCode/bin/enemy", "enemy", argv[1], NULL);
        perror("Enemy process failed");
        exit(1);
    }


    // Fork resistance groups
    resistance_group_pid = (pid_t *)malloc(config.RESISTANCE_GROUP_MAX * sizeof(pid_t));
    if (resistance_group_pid == NULL) {
        perror("Memory allocation failed");
        cleanup();
        exit(1);
    }
    char resistance_group_id_str[20];
    for (int i = 0; i < config.RESISTANCE_GROUP_INITIAL; i++) {
        if ((resistance_group_pid[i] = fork()) == 0) {
            snprintf(resistance_group_id_str, sizeof(resistance_group_id_str), "%d", resistance_group_counter + 1);
            // execl("./bin/resistance_group", "resistance_group", argv[1], resistance_group_id_str, NULL);
            execl("/home/adduser/ENCS4330/Projects/Project3/Counter-Espionage-Agency-Simulation/projectCode/bin/resistance_group", "resistance_group", argv[1], resistance_group_id_str, NULL);
            perror("Resistance group process failed");
            exit(1);
        }
        resistance_group_counter++;
    }

    //make a thread for creating resistance group every specified interval
    

    pthread_create(&thread_fork_resistance_group, NULL ,fork_resistance_group, (void *)argv[1]);

    
    while (1) {
        // check for exit conditions
        if (shared_data->number_killed_members >= config.MAX_KILLED_MEMBERS) {
            exit_program();
        }

        //
        //
        // Add more exit conditions here
    }


    // // ithink no need for waiting for children processes to finish

    // int all_children = config.CIVILIAN_NUMBER + resistance_group_counter + 1 + 1;//all children processes
    // // Wait for all children to finish
    // for (int i = 0; i < all_children; i++) {
    //     wait(NULL);
    // }


    exit(0);
}


void cleanup() {
    if (shm_data_id != -1) shmctl(shm_data_id, IPC_RMID, NULL);
    if (msg_resistance_agency_id != -1) msgctl(msg_resistance_agency_id, IPC_RMID, NULL);

    free(resistance_group_pid);


    // terminate the thread thread_fork_resistance_group
    pthread_cancel(thread_fork_resistance_group);



    for (int i = 0; i < 7; i++) {
        delete_message_queue(msg_queue_ids[i]);
    }



    printf("Resources cleaned up.\n");
}

// Fork resistance group every specified interval using sleep
void* fork_resistance_group(void *arg) {
    char *argv = (char *)arg;
    printf("Fork resistance group thread created\n");    
    char resistance_group_id_str[20];
    while (1) {
        sleep(config.RESISTANCE_GROUP_CREATION_INTERVAL);
        if (resistance_group_counter < config.RESISTANCE_GROUP_MAX) {
            printf("Forking resistance group\n");
            if ((resistance_group_pid[resistance_group_counter] = fork()) == 0) {
                snprintf(resistance_group_id_str, sizeof(resistance_group_id_str), "%d", resistance_group_counter + 1 );
                // execl("./bin/resistance_group", "resistance_group", argv, resistance_group_id_str, NULL);
                execl("/home/adduser/ENCS4330/Projects/Project3/Counter-Espionage-Agency-Simulation/projectCode/bin/resistance_group", "resistance_group", argv, resistance_group_id_str, NULL);
                perror("Resistance group process failed");
                exit(1);
            }
            resistance_group_counter++;
        }
    }
}

// // Alarm signal handler
// void alarm_handler(int signum) {
//     alarm_triggered = 1;
//     printf("Alarm triggered\n");
// }

// Exit program
void exit_program() {
    //send kill signal to all children processes
    kill(counter_espionage_agency_pid, SIGKILL);
    kill(civilian_pid, SIGKILL);
    kill(enemy_pid, SIGKILL);

    for (int i = 0; i < resistance_group_counter; i++) {
        kill(resistance_group_pid[i], SIGKILL);
    }

    exit(0);//it will call the cleanup function automatically
}

// Signal handler for interrupt signal
void handle_interrupt(int signal) {
    exit_program();
}