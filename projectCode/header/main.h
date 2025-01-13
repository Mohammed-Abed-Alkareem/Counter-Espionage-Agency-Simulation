#ifndef MAIN_H
#define MAIN_H

#include "common.h"



int main(int argc, char *argv[]);
key_t key_generator(char letter);
void cleanup();
void handle_alarm(int signal);
void exit_program();
void alarm_handler(int signal);
void fork_resistance_group(int signal);
void handle_interrupt(int signal);



#endif // MAIN_H