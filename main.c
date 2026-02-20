#include <stdio.h>
#include "data.h"

/* function prototypes from scheduler.c */
void initializeSystem();
void generateSchedule();
void displaySchedule();
void exportScheduleToHTML();

int main() {

    initializeSystem();
    generateSchedule();

    displaySchedule();
    exportScheduleToHTML();

    return 0;
}