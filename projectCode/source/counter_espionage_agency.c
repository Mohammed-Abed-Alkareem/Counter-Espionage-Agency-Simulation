#include "counter_espionage_agency.h"
#include "two_way_hash_index.h"
// Global variables
AGENCY_MEMBER *MEMBERS;
Config config;

// Shared memory for enemy attacks
int *shared_mem_attack;
int shm_id;
sem_t *sem_ter_cond;
int sem_data_id = -1;
SharedData *shared_data;
int msg_agency_to_people_id;
int msg_agency_to_resistance_group_id;
/*
    * define the hash table for the people info and resistance member info
    * define the node for people and resistance member 
    
*/






#define CONTACT_RECORED_LEN 100



typedef struct {
    int id ; 
    STATE state;
    int sever_injury_count;
    int light_injury_count;
    int captured_count;
    float spy_probability;
    int contact_record[CONTACT_RECORED_LEN][2];
    int contact_count;
    int attacked_count;
    int group_id;
    pthread_mutex_t lock;

} resistance_member_info_node;




typedef struct {
    int id;
    STATE state;
    float spy_probability;
    pthread_mutex_t lock;
    int contact_count;

} PeopleInfoNode;
typedef PeopleInfoNode people_info_node;






// define two way hash for People info , resistance member info 
HashTable *people_info_hash;
HashTable *resistance_member_info_hash;











// create the node for the people info
void *create_people_info_node(){
    PeopleInfoNode *node = (PeopleInfoNode *)malloc(sizeof(PeopleInfoNode));
    if (node == NULL) {
        return NULL;
    }

    node->id = -1;
    node->state = ACTIVE;
    node->spy_probability = 0.0;
    node->contact_count = 0;
    if (pthread_mutex_init(&node->lock, NULL) != 0) {
        free(node);
        return NULL;
    }
    return node;
}

// delete the node for the people info
void delete_people_info_node(void *data){
    PeopleInfoNode *node = (PeopleInfoNode *)data;
    pthread_mutex_destroy(&node->lock);
    free(node);

    }

// create the node for the resistance member info

void *create_resistance_member_info_node(){
    resistance_member_info_node *node = (resistance_member_info_node *)malloc(sizeof(resistance_member_info_node));
    if (node == NULL) {
        return NULL;
    }
    node->id = -1;
    node->state = ACTIVE;
    node->spy_probability = 0.0;
    node->sever_injury_count = 0;
    node->light_injury_count = 0;
    node->captured_count = 0;
    for (int i = 0; i < CONTACT_RECORED_LEN; i++) {
        node->contact_record[i][0] = -1;
        node->contact_record[i][1] = -1;
    }
    node->contact_count = 0;
    node->attacked_count = 0;
    node->group_id = -1;

    if (pthread_mutex_init(&node->lock, NULL) != 0) {
        free(node);
        return NULL;
    }

    return node;
}

// delete the node for the resistance member info
void delete_resistance_member_info_node(void *data){
    resistance_member_info_node *node = (resistance_member_info_node *)data;
    pthread_mutex_destroy(&node->lock);
    free(node);
    free(data);
}


// function to reset people info node 
void reset_people_info_node(people_info_node *node){
    pthread_mutex_lock(&node->lock);
    node->state = ALIVE;
    node->spy_probability = 0.0;
    node->contact_count = 0;
    pthread_mutex_unlock(&node->lock);

}

// function to reset resistance member info node
void reset_resistance_member_info_node(resistance_member_info_node *node){
    pthread_mutex_lock(&node->lock);
    node->state = ACTIVE;
    node->spy_probability = 0.0;
    node->sever_injury_count = 0;
    node->light_injury_count = 0;
    node->captured_count = 0;
    for (int i = 0; i < CONTACT_RECORED_LEN; i++) {
        node->contact_record[i][0] = -1;
        node->contact_record[i][1] = -1;
    }
    node->contact_count = 0;
    node->attacked_count = 0;
    node->group_id = -1;
    pthread_mutex_unlock(&node->lock);
}


// function to send state update to people using agency to people message queue
void send_state_update_to_people(int people_id, STATE state){
    AgencyToPeopleStateMessage message;
    message.type = people_id;
    message.state = state;
    if (msgsnd(msg_agency_to_people_id, &message, sizeof(AgencyToPeopleStateMessage), 0) == -1) {
        perror("Error sending message to people");
    }
}


// function to send state update to resistance group using agency to resistance group message queue
void send_state_update_to_resistance_group(int member_id, int group_id, STATE state){
    
    AgencyToResistanceStateMessage message;
    message.member_number = member_id;
    message.type = group_id;
    message.state = state;
    message.time_sent = time(NULL);
    if (msgsnd(msg_agency_to_resistance_group_id, &message, sizeof(AgencyToResistanceStateMessage), 0) == -1) {
        perror("Error sending message to resistance group");
    }
}

void analyze_contact_recored(resistance_member_info_node *member_info){
    pthread_mutex_lock(&member_info->lock);
    // from the people hash get the people info node that the member has contact with and update the spy probability
    for (int i = 0; i < member_info->contact_count; i++) {

        people_info_node *people_info = (people_info_node *)get_or_create_node(people_info_hash, &member_info->contact_record[i][0], 1);
        pthread_mutex_lock(&people_info->lock);
        switch (member_info->state)
        
        {
        case KILLED:
            people_info->spy_probability = 0.05;
            break;

        case ATTACKED:
            people_info->spy_probability = 0.08;
            break;
        default:
            break;
        }
        
        if (people_info->spy_probability >= config.SPY_CONFIRMATION_THRESHOLD) {
            people_info->state = SPY;
            send_state_update_to_people(people_info->id, SPY);
            send_state_update_to_people(people_info->id, INVISTIGATED);
            send_state_update_to_people(people_info->id , KILLED);
            


        }else if (people_info->spy_probability >= config.SPY_CONFIRMATION_THRESHOLD/2){
            people_info->state = ARRESTED;
            send_state_update_to_people(people_info->id, ARRESTED);
            
        }
        pthread_mutex_unlock(&people_info->lock);
    }
    pthread_mutex_unlock(&member_info->lock);
}





/*
    * end of the hash table and node definition and function for it 
*/





/*
    * functions ::::

*/




// function to read messages from the resistance group

void read_message_from_resistance_group(AGENCY_MEMBER *member){
    // read the message from the resistance group
    ResistanceMemberStateReportMessage message;
    if (msgrcv(msg_agency_to_resistance_group_id, &message, sizeof(ResistanceMemberStateReportMessage), 0 , IPC_NOWAIT) == -1) {
        if (errno != ENOMSG) {
            perror("Error receiving message from resistance group");
        }
    } else {
        // update the state of the member
        int member_id = message.member_num;
        int group_id = message.type;
        resistance_member_info_node *member_info = (resistance_member_info_node *)get_or_create_node(resistance_member_info_hash, &member_id, &group_id);
        member_info->id = member_id;
        member_info->group_id = group_id;
        switch (message.state)
        {
        case KILLED:
            pthread_mutex_lock(&member_info->lock);
            member_info->state = DEAD;
            pthread_mutex_unlock(&member_info->lock);
            analyze_contact_recored(member_info);
            sem_wait(sem_ter_cond);
            shared_data->number_killed_members++;
            sem_post(sem_ter_cond);

            
            break;
        case ATTACKED:
            if (member_info->state != ATTACKED){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = ATTACKED;
                member_info->attacked_count++;
                pthread_mutex_unlock(&member_info->lock);
                analyze_contact_recored(member_info);
                sem_wait(sem_ter_cond);
                shared_data->number_injured_members++;
                sem_post(sem_ter_cond);
            }
            break;
        case CAPTURED:
            if (member_info->state != CAPTURED){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = CAPTURED;
                member_info->captured_count++;
                pthread_mutex_unlock(&member_info->lock);
                analyze_contact_recored(member_info);
                sem_wait(sem_ter_cond);
                shared_data->number_captured_members++;
                sem_post(sem_ter_cond);
            }
            break;
        case SERIOUSLYINJURED:
            if (member_info->state != SERIOUSLYINJURED){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = SERIOUSLYINJURED;
                member_info->sever_injury_count++;
                pthread_mutex_unlock(&member_info->lock);
                analyze_contact_recored(member_info);
            }
            break;
        case LIGHTINJURED:
            if (member_info->state != LIGHTINJURED){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = LIGHTINJURED;
                member_info->light_injury_count++;
                pthread_mutex_unlock(&member_info->lock);
                analyze_contact_recored(member_info);
            }
            break;
        case ACTIVE:
            pthread_mutex_lock(&member_info->lock);
            member_info->state = ACTIVE;
            pthread_mutex_unlock(&member_info->lock);
            
            break;
        case SPY:
            pthread_mutex_lock(&member_info->lock);
            member_info->spy_probability = 1.0;
            pthread_mutex_unlock(&member_info->lock);
            send_state_update_to_resistance_group(member_info->id, member_info->group_id, KILLED);
            break;
        break;
        case INVISTIGATED:
            if(random_float(0,1) < config.KILL_PROBABILITY){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = KILLED;
                pthread_mutex_unlock(&member_info->lock);
                send_state_update_to_resistance_group(member_info->id, member_info->group_id, KILLED);
            }
        break;
            case ARRESTED:
            if(random_float(0,1) < config.KILL_PROBABILITY){
                pthread_mutex_lock(&member_info->lock);
                member_info->state = KILLED;
                pthread_mutex_unlock(&member_info->lock);
                send_state_update_to_resistance_group(member_info->id, member_info->group_id, KILLED);
            }
            break;
            case ALIVE:
                read_message_from_resistance_group(member);
                //! mabey analyze people recored here
            break;
            case DEAD:
            reset_resistance_member_info_node(member_info);
            break;
        default:
            break;
        }
        // char console_message[200];
        // sprintf(console_message, "Member %d has been updated to state %d\n", member->id, message.state);
        // print_color(console_message, YELLOW);
    }
}


// function to read contact report of people to resistance group from resistance group and recored it 
void read_contact_report_from_resistance_group(){
    // read the message from the resistance group
    PeopleContactReportMessage message;
    if (msgrcv(msg_agency_to_resistance_group_id, &message, sizeof(PeopleContactReportMessage), 0 , IPC_NOWAIT) == -1) {
        if (errno != ENOMSG) {
            perror("Error receiving message from resistance group");
        }
    } else {
        // update the state of the member
        int member_id = message.member_num;
        int group_id = message.type;
        resistance_member_info_node *member_info = (resistance_member_info_node *)get_or_create_node(resistance_member_info_hash, &member_id, &group_id);
        pthread_mutex_lock(&member_info->lock);
        if (member_info->contact_count < CONTACT_RECORED_LEN) {
            member_info->contact_record[member_info->contact_count][0] = message.people_number;
            member_info->contact_record[member_info->contact_count][1] = message.contact_time;

            member_info->id = member_id;
            member_info->group_id = group_id;
            member_info->contact_count++;
            people_info_node* people_info = (people_info_node *)get_or_create_node(people_info_hash, &message.people_number, 1);
            pthread_mutex_lock(&people_info->lock);
            people_info->contact_count++;
            people_info->id = message.people_number;
            pthread_mutex_unlock(&people_info->lock);

        }
        pthread_mutex_unlock(&member_info->lock);
    }
}





void examine_people_member_state(void* data) {
    people_info_node* node = (people_info_node*)data;
    pthread_mutex_lock(&node->lock);
    if(node->spy_probability >= config.SPY_CONFIRMATION_THRESHOLD){
        node->state = SPY;
        send_state_update_to_people(node->id, SPY);
    }
    switch (node->state) {
        case ARRESTED:
            if (random_float(0, 1) < config.KILL_PROBABILITY) {
                node->state = KILLED;
                send_state_update_to_people(node->id, KILLED);
            } else {
                node->state = INVISTIGATED;
                send_state_update_to_people(node->id, INVISTIGATED);
                if(random_float(0,1) < 1 - config.KILL_PROBABILITY-node->spy_probability){
                    node->state = ALIVE;
                    send_state_update_to_people(node->id, ALIVE);
                }
            }
            break;
        case INVISTIGATED:
            if (random_float(0, 1) < config.KILL_PROBABILITY) {
                node->state = KILLED;
                send_state_update_to_people(node->id, KILLED);
            } else {
                node->state = ALIVE;
                send_state_update_to_people(node->id, ALIVE);
            }
            break;
        case SPY:
            node->state = KILLED;
            send_state_update_to_people(node->id, KILLED);
            pthread_mutex_unlock(&node->lock);
            reset_people_info_node(node);
            return;
            break;
        default:
            break;
    }

    pthread_mutex_unlock(&node->lock);
}

void examine_people_states_in_group(const void* group_id) {
    iterate_group_members(people_info_hash, group_id, examine_people_member_state);
}





// regural check for resistance group member state and update it....


void examine_resistance_member_state(void* data) {
    resistance_member_info_node* node = (resistance_member_info_node*)data;
    pthread_mutex_lock(&node->lock);
    if(node->spy_probability >= config.SPY_CONFIRMATION_THRESHOLD){
        node->state = SPY;
        send_state_update_to_resistance_group(node->id, node->group_id, SPY);
    }
    switch (node->state) {
        case ARRESTED:
            if (random_float(0, 1) < config.KILL_PROBABILITY) {
                node->state = KILLED;
                send_state_update_to_resistance_group(node->id, node->group_id, KILLED);
            } else {
                node->state = INVISTIGATED;
                send_state_update_to_resistance_group(node->id, node->group_id, INVISTIGATED);
                if(random_float(0,1) < 1 - config.KILL_PROBABILITY-node->spy_probability){
                    node->state = ACTIVE;
                    send_state_update_to_resistance_group(node->id, node->group_id, ACTIVE);
                }
            }
            break;
        case INVISTIGATED:
            if (random_float(0, 1) < config.KILL_PROBABILITY) {
                node->state = KILLED;
                send_state_update_to_resistance_group(node->id, node->group_id, KILLED);
            } else {
                node->state = ACTIVE;
                send_state_update_to_resistance_group(node->id, node->group_id, ACTIVE);
            }
            break;
        case SPY:
            node->state = KILLED;
            send_state_update_to_resistance_group(node->id, node->group_id, KILLED);
            pthread_mutex_unlock(&node->lock);
            reset_resistance_member_info_node(node);
            return;
            break;
        default:
            break;
    }

    pthread_mutex_unlock(&node->lock);
}

void examine_resistance_member_states_in_group(const void* group_id) {
    iterate_group_members(resistance_member_info_hash, group_id, examine_resistance_member_state);
}

/*
    * end of the functions
*/



// Define the message queue IDs
int msg_agency_to_people_id;
int msg_agency_to_resistance_group_id;
int enemy_to_agency_attack_id;
int resistance_group_to_agency_id;

// Clean-up function called at exit
void cleanUp() {
    free(MEMBERS);
    // remove the shared memory >> this should be done by the main process...
    // keep the detach only
    shmdt(shared_mem_attack);
    shmctl(shm_id, IPC_RMID, NULL);
    
    // free hash tables
    delete_hash_table(people_info_hash);
    delete_hash_table(resistance_member_info_hash);


    printf("Counter espionage agency process terminated\n");
}





// Function to replace a killed or captured member
void replace_member(int index) {
    wait_random_time_ms(1000, 10000); // Wait for random time before replacing member
    MEMBERS[index].id = index + 1;
    MEMBERS[index].health = config.MAX_HEALTH;
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
    char console_message[200];
    int people_group_id = 1;
    while (1) {
        sleep(1); // Simulate member activity interval

        switch (member->status) {
            case ALIVE:
            case ACTIVE:
                // Perform actions for alive members
                send_contact_message(member);
                read_message_from_resistance_group(member);
                read_contact_report_from_resistance_group();
                if (random_float(0, 1) < config.SPY_PROBABILITY) {
                    examine_people_states_in_group(&people_group_id);
                    for (int i = 1; i <= config.RESISTANCE_MEMBER_MAX; i++) {
                        if(random_float(0,1) < config.SPY_PROBABILITY){
                            examine_resistance_member_states_in_group(&i);
                        }
                        
                    }
                }
                handle_attack(member);
                break;
            case SERIOUSLYINJURED:
                // Perform actions for injured members
                recover_from_injury(member);
                handle_attack(member);
                break;
            case LIGHTINJURED:
                // Perform actions for injured members
                recover_from_injury(member);
                handle_attack(member);
                break;
            case CAPTURED:
                // Perform actions for captured members
                if(random_float(0 , 1) < config.ENEMY_ATTACK_PROBABILITY){
                    shared_mem_attack[member->id-1] = 1;
                }
                handle_attack(member);
                recover_from_injury(member);
                if(random_integer(0,10) < 1 )
                    member->status = ESCAPED;
                

            case ESCAPED:
                shared_mem_attack[member->id-1] = 0;
                // Perform actions for escaped members
                if(member->health < config.MIN_HEALTH )
                    member->status = SERIOUSLYINJURED;
                else 
                    member->status = LIGHTINJURED;
                break;
            case KILLED:
                // Perform actions for killed members
                replace_member(member->id - 1);

                sprintf(console_message, "Member from Counter Espoinage Agency %d has been Killed\n", member->id);
                print_color(console_message, RED);
                return NULL;
                break;
            // Add more states as needed
            default:
                break;
        }
    }

    return NULL;
}


void recover_from_injury(AGENCY_MEMBER *member) {
    if (member->status == LIGHTINJURED) {
        member->health += random_integer((int)(config.RECOVERY_RATE*config.MIN_HEALTH),(int) (config.RECOVERY_RATE*member->health + config.MIN_HEALTH/10));
        if (member->health >= config.MAX_HEALTH) {
            member->status = ALIVE;
            char console_message[200];
            sprintf(console_message, "Agency Member %d has recovered from light injury\n", member->id);
            print_color(console_message, GREEN);
        }
    } else if (member->status == SERIOUSLYINJURED) {
        member->health += random_integer((int)(config.RECOVERY_RATE*config.MIN_HEALTH/100),(int) (config.RECOVERY_RATE*member->health + config.MIN_HEALTH/100));
        if (member->health >= config.MIN_HEALTH) {
            member->status = ALIVE;
            char console_message[200];
            sprintf(console_message, "Agency Member %d has recovered from serious injury and it become light injuer\n", member->id);
            print_color(console_message, GREEN);
        }
    }
}




// Function to handle attacks
void handle_attack(AGENCY_MEMBER *member) {

    
        // Check for attacks from the enemy
        if (shared_mem_attack[member->id-1] == 1) {
            if(random_float(0, 1) < config.ENEMY_ATTACK_PROBABILITY) {
                member->health -= config.MAX_HEALTH;
                member->status = KILLED;
                
            }else{
                member->health -= random_integer(config.MIN_HEALTH, config.MAX_HEALTH);
                shared_mem_attack[member->id - 1]=0;
                if (member->health <= 0) {
                    member->status = KILLED;
                } else if (member->health < config.MIN_HEALTH) {
                    member->status = SERIOUSLYINJURED;
                }else if (member->health < config.MIN_HEALTH) {
                    member->status = LIGHTINJURED;
                }
            }
        }
        shared_mem_attack[member->id-1] = 0 ;
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
//! add enrolment date to the contac message 
void send_contact_message(AGENCY_MEMBER *member) {
    // Send communication message to people and send the communication report to the agency
    AgencyMemberToPeopleContactMessage contact_message;
    contact_message.type = config.CIVILIAN_NUMBER + random_integer(1, config.CIVILIAN_NUMBER);
    contact_message.num_of_sec = random_integer(config.MIN_CONTACT_TIME, config.MAX_CONTACT_TIME);
    contact_message.member_id = member->id;
    contact_message.enroll_date = member->time_with_agency;

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

    // Create a shared semaphore for shared data
        // Create a shared semaphore for shared data
    sem_ter_cond = sem_open("/termination_cond_sem", O_CREAT, 0644, 1);
    if (sem_ter_cond == SEM_FAILED) {
    perror("sem_open failed");
    exit(EXIT_FAILURE);
}

    // Shared memory for data
    key_t shm_data_key = key_generator('A');

    int shm_data_id = shmget(shm_data_key, sizeof(SharedData), IPC_CREAT | 0666); // create shared memory
    if (shm_data_id == -1) {
        perror("Shared memory creation failed");
        cleanUp();
        exit(1);
    }

    // Attach shared memory to shared_data
    shared_data = (SharedData *)shmat(shm_data_id, NULL, 0);
    if (shared_data == (void *)-1) {
        perror("Shared memory attach failed");
        cleanUp();
        exit(1);
    }

    // Initialize the shared data
    shared_data->number_killed_members = 0;
    shared_data->number_injured_members = 0;
    shared_data->number_captured_members = 0;



    // creating and initializing the shared memory in the main process
    // Allocate memory for agency members
    MEMBERS = (AGENCY_MEMBER *)malloc(config.COUNTER_ESPIONAGE_AGENCY_MEMBER * sizeof(AGENCY_MEMBER));
    if (MEMBERS == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }
    // create a key and set it in the environment variable
    // Initialize shared memory for enemy attacks
    key_t shm_key = key_generator('Z');
    shm_id = shmget(shm_key, config.COUNTER_ESPIONAGE_AGENCY_MEMBER * sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Shared memory creation failed");
        exit(1);
    }
    
    shared_mem_attack = (int *)shmat(shm_id, NULL, 0);
    if (shared_mem_attack == (void *)-1) {
        perror("Shared memory attachment failed");
        exit(1);
    }
    for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
        shared_mem_attack[i] = 1;
    }


    // create the hash table for the people info and resistance member info
    people_info_hash = create_hash_table(create_people_info_node, delete_people_info_node , config.CIVILIAN_NUMBER , 2);
    resistance_member_info_hash = create_hash_table(create_resistance_member_info_node, delete_resistance_member_info_node,config.RESISTANCE_MEMBER_MAX , config.RESISTANCE_GROUP_MAX);




    // Get the message queue IDs from environment variables
    char *key_str_agency_to_people = getenv("AGENCY_TO_PEOPLE_STATE_KEY");
    if (key_str_agency_to_people == NULL) {
        perror("Error getting environment variable AGENCY_TO_PEOPLE_STATE_KEY");
        exit(1);
    }
    msg_agency_to_people_id = atoi(key_str_agency_to_people);

    char *key_str_agency_to_resistance = getenv("AGENCY_TO_RESISTANCE_MEMBER_STATE_KEY");
    if (key_str_agency_to_resistance == NULL) {
        perror("Error getting environment variable AGENCY_TO_RESISTANCE_MEMBER_STATE_KEY");
        exit(1);
    }
    msg_agency_to_resistance_group_id = atoi(key_str_agency_to_resistance);

    char *key_str_enemy_to_agency = getenv("ENEMY_TO_AGENCY_ATTACK_KEY");
    if (key_str_enemy_to_agency == NULL) {
        perror("Error getting environment variable ENEMY_TO_AGENCY_ATTACK_KEY");
        exit(1);
    }
    enemy_to_agency_attack_id = atoi(key_str_enemy_to_agency);

    char *key_str_resistance_to_agency = getenv("RESISTANCE_TO_AGENCY_MEMBER_STATE_REPORT_KEY");
    if (key_str_resistance_to_agency == NULL) {
        perror("Error getting environment variable RESISTANCE_TO_AGENCY_MEMBER_STATE_REPORT_KEY");
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

    
    // wait for the threads to finish
    for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
        pthread_join(MEMBERS[i].thread_id, NULL);
    }

    //! this is not needed but leave it for now (targert probability)
    // Get the key to send message to enemy about the target probability
    char *key_str = getenv("AGENCY_TO_ENEMY_TARGET_PROBABILITY_KEY");
    if (key_str == NULL) {
        perror("Error getting environment variable AGENCY_TO_ENEMY_TARGET_PROBABILITY_KEY");
        exit(1);
    }
    key_t key_target_prob = atoi(key_str);

    // // Create the analysis thread
    // if (pthread_create(&analysis_thread, NULL, analyze_data, (void *)&config) != 0) {
    //     perror("Failed to create analysis thread");
    //     return 1;
    // }

    // // Create the investigator thread
    // if (pthread_create(&investigator_thread, NULL, investigator_function, (void *)&config) != 0) {
    //     perror("Failed to create investigator thread");
    //     return 1;
    // }

    // AgencyToEnemyTargetProbabilityMessage target_prob_msg;
    // // Every regular time send the target probability to the enemy
    // while (1) {
    //     sleep(5); // Send the target probability to the enemy every 5 seconds
    //     for (int i = 0; i < config.COUNTER_ESPIONAGE_AGENCY_MEMBER; i++) {
    //         // Send the target probability to the enemy
    //         target_prob_msg.member_id = MEMBERS[i].id;
    //         target_prob_msg.target_probability = MEMBERS[i].target_probability;
    //         if (msgsnd(key_target_prob, &target_prob_msg, sizeof(target_prob_msg.target_probability) + sizeof(target_prob_msg.member_id), 0) == -1) {
    //             perror("Error sending target probability message to enemy");
    //         }
    //     }
    // }

    return 0;
}
