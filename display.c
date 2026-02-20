#include <stdio.h>
#include "data.h"

/* EXTERN GLOBALS */
extern Section sections[];
extern Teacher teachers[];
extern ScheduleEntry schedule[];
extern int sectionCount;
extern int scheduleCount;

char *days[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday"};
char *times[] = {"8-9", "9-10", "10-11", "11-12", "12-1", "2-5"};

void displaySchedule() {

    for (int sec = 0; sec < sectionCount; sec++) {

        printf("\n============================================\n");
        printf("Year %d - Section %s Routine\n",
               sections[sec].year,
               sections[sec].name);
        printf("============================================\n");

        for (int d = 0; d < DAYS; d++) {
            for (int s = 0; s < SLOTS_PER_DAY; s++) {

                for (int i = 0; i < scheduleCount; i++) {

                    if (schedule[i].section == sec &&
                        schedule[i].day == d &&
                        schedule[i].slot == s) {

                        printf("%s %s | %s | Teacher: %s\n",
                               days[d],
                               times[s],
                               sections[sec].courses[schedule[i].courseIndex].name,
                               teachers[schedule[i].teacher].name);
                    }
                }
            }
        }
    }
}