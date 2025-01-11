
#include "utils.h"

void create_folder(const char *path) {
    mkdir(path, 0755);
}

void create_file(const char *path) {
    FILE *file = fopen(path, "w");
    if (file) {
        fclose(file);
    } else {
        perror("Failed to create file");
    }
}

void read_file_line_by_line(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        char *line = NULL;
        size_t len = 0;
        
        while (getline(&line, &len, file) != -1) {
            printf("%s", line);
        }
        // tokenization of the line by strtok 
        // char *token = strtok(line, " ");
        // while (token != NULL) {
        //     printf("%s\n", token);
            // convert token to integer
            //int number = atoi(token);
            //convert token to float
            //float number = atof(token);
            
        //     token = strtok(NULL, " ");
        // }

    
        free(line);
        fclose(file);
    } else {
        perror("Failed to open file");
    }
}

void remove_file(const char *path) {
    if (remove(path) != 0) {
        perror("Failed to remove file");
    }
}

void move_file(const char *source, const char *destination) {
    if (rename(source, destination) != 0) {
        perror("Failed to move file");
    }
}

void remove_directory(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
                if (entry->d_type == DT_DIR) {
                    remove_directory(full_path);
                } else {
                    remove_file(full_path);
                }
            }
        }
        closedir(dir);
        rmdir(path);
    } else {
        perror("Failed to open directory");
    }
}


// function that wait random amount of time between two values in seconds
void wait_random_time(int min, int max) {
    int wait_time = rand() % (max - min + 1) + min;
    sleep(wait_time);
}

// function that wait random amount of time between two values in milliseconds
void wait_random_time_ms(int min, int max) {
    int wait_time = rand() % (max - min + 1) + min;
    usleep(wait_time * 1000);
}

// function that create and random integer between two values
int random_integer(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int propability_choice(float propability) {
    return (rand() / (float)RAND_MAX) < propability;
}


// function that create and random float between two values
float random_float(float min, float max) {
    return (rand() / (float)RAND_MAX) * (max - min) + min;
}


void print_color(const char *string, Color color) {
    // if the string does not end with a new line add it 
    if (string[strlen(string) - 1] != '\n') {
        strcat(string, "\n");
    }

    switch (color) {
        case RED:
            printf("\033[0;31m%s\033[0m", string);
            break;
        case GREEN:
            printf("\033[0;32m%s\033[0m", string);
            break;
        case YELLOW:
            printf("\033[0;33m%s\033[0m", string);
            break;
        case BLUE:
            printf("\033[0;34m%s\033[0m", string);
            break;
        case MAGENTA:
            printf("\033[0;35m%s\033[0m", string);
            break;
        case CYAN:
            printf("\033[0;36m%s\033[0m", string);
            break;
        case WHITE:
            printf("\033[0;37m%s\033[0m", string);
            break;
        case ORANGE:
            printf("\033[0;38;5;208m%s\033[0m", string);
            break;
        case LIGHTRED:
            printf("\033[0;38;5;196m%s\033[0m", string);
            break;
        default:
            printf("%s", string);
            break;
    }
}


key_t key_generator(char letter) {
    key_t key = ftok(".", letter);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    return key;
}