
#include "common.h"
#ifndef UTILS_H

#include <dirent.h>
#include <sys/stat.h>

#define UTILS_H


typedef enum {
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
} Color;

void print_color(const char * , Color);



void create_folder(const char *path);
void create_file(const char *path);
void read_file_line_by_line(const char *path);
void remove_file(const char *path);
void move_file(const char *source, const char *destination);
void remove_directory(const char *path);
void print_color(const char *string, Color color);
float random_float(float min, float max);
int random_integer(int min, int max);
void wait_random_time_ms(int min, int max);
void wait_random_time(int min, int max); 
key_t key_generator(char letter);


#endif