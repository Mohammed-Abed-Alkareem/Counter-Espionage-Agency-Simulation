#include "enemy.h"



// Function to clean up the process
void cleanUp() {
    // Clean up code here when the process is terminated

    printf("Enemy process terminated\n");
}

// Main function
int main(int argc, char *argv[]) {

    // Register the cleanUp function to be called when the process is terminated
    atexit(cleanUp);

    return 0;
}
