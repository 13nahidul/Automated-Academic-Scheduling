#include <stdio.h>
#include "scheduler.h"
#include "display.h"
#include "export_html.h"
#include "export_teacher.h"

int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║   Automated Class Routine Generator  ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    initializeSystem();
    generateSchedule();
    printSchedule();
    exportHTML();
    exportTeacherHTML();

    return 0;
}

/* Compile:
   gcc main.c scheduler.c display.c export_html.c export_teacher.c -o routine.exe
*/
