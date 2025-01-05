#include "civilian.h"


void cleanUp() {
    // Clean up code here when the process is terminated

    printf("Civilian process terminated\n");
}



int main(int argc, char *argv[]) {

    atexit(cleanUp);

    printf("Civilian process created\n");
    return 0;
}