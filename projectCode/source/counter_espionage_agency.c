#include "counter_espionage_agency.h"



void cleanUp() {
    // Clean up code here when the process is terminated

    printf("Counter espionage agency process terminated\n");
}

int main(int argc, char *argv[]) {

    atexit(cleanUp);



    printf("Counter espionage agency process created\n");
    return 0;
}