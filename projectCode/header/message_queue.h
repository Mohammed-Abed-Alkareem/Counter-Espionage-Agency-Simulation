#include "common.h"
#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H


typedef struct message_queue
{   
    // other att 
    char text [100];
    size_t type ; 
} general_message  ;




// Function to create a message queue
int create_message_queue(key_t key);

// Function to send a message to the queue
int send_message(int msgid, void *msg, size_t msg_size, int no_wait);

// Function to receive a message from the queue
int receive_message(int msgid, void *msg, size_t msg_size, long msg_type, int no_wait);

// Function to delete the message queue
void delete_message_queue(int msgid);

#endif // MESSAGE_QUEUE_H