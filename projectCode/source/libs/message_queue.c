
#include "message_queue.h"
#include <sys/ipc.h>
#include <sys/msg.h>


// Function to create a message queue
int create_message_queue(key_t key) {
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }
    return msgid;
}

// Function to send a message to the queue
int send_message(int msgid, void *msg, size_t msg_size, int no_wait) {
    int flags = no_wait ? IPC_NOWAIT : 0;
    if (msgsnd(msgid, msg, msg_size, flags) == -1) {
        if (errno == EAGAIN && no_wait) {
            return -1; // Queue is full
        } else {
            perror("msgsnd");
            exit(1);
        }
    }
    return 0;
}

// Function to receive a message from the queue
int receive_message(int msgid, void *msg, size_t msg_size, long msg_type, int no_wait) {
    int flags = no_wait ? IPC_NOWAIT : 0;
    if (msgrcv(msgid, msg, msg_size, msg_type, flags) == -1) {
        if (errno == ENOMSG && no_wait) {
            return -1; // No message of the desired type
        } else {
            perror("msgrcv");
            exit(1);
        }
    }
    return 0;
}

// Function to delete the message queue
void delete_message_queue(int msgid) {
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
}