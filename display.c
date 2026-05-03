#include <stdio.h>
#include "data.h"

extern Teacher      teachers[];
extern Section      sections[];
extern ScheduleEntry schedule[];
extern int sectionCount, scheduleCount, teacherCount;

static const char *DAY_NAMES[]  = {"Sat","Sun","Mon","Tue","Wed"};
static const char *SLOT_TIMES[] = {
    "8-9","9-10","10-11","11-12","12-1",    /* theory slots 0-4 */
    "2-3","3-4","4-5"                        /* lab slots 5-7    */
};

void printSchedule() {
    printf("\n========== CLASS ROUTINE ==========\n");

    for (int sec=0; sec<sectionCount; sec++) {
        printf("\n--- Section %s ---\n", sections[sec].name);

        /* Header */
        printf("%-5s", "Day");
        for (int s=0; s<TOTAL_SLOTS; s++) {
            if (s == LAB_START) printf(" | "); /* visual separator */
            printf(" %-12s", SLOT_TIMES[s]);
        }
        printf("\n");

        /* Divider */
        printf("-----");
        for (int s=0; s<TOTAL_SLOTS; s++) {
            if (s == LAB_START) printf("---");
            printf(" ------------");
        }
        printf("\n");

        for (int d=0; d<DAYS; d++) {
            printf("%-5s", DAY_NAMES[d]);
            for (int s=0; s<TOTAL_SLOTS; s++) {
                if (s == LAB_START) printf(" | ");
                int found=-1;
                for (int i=0; i<scheduleCount; i++) {
                    if (schedule[i].section==sec &&
                        schedule[i].day==d &&
                        schedule[i].slot==s) { found=i; break; }
                }
                if (found!=-1) {
                    char cell[30];
                    snprintf(cell,sizeof(cell),"%s/%s",
                             sections[sec].courses[schedule[found].course].name,
                             teachers[schedule[found].teacher].shortName);
                    printf(" %-12s", cell);
                } else {
                    printf(" %-12s", "---");
                }
            }
            printf("\n");
        }
    }

    /* Teacher load */
    printf("\n--- Teacher Load Summary ---\n");
    printf("  %-20s %-14s %-10s %s\n","Name","Subject","Rank","Load");
    const char *rankName[] = {"","Professor","Associate","Assistant","Lecturer"};
    for (int t=0; t<teacherCount; t++) {
        printf("  %-20s %-14s %-10s %d/%d\n",
               teachers[t].name,
               teachers[t].subject,
               rankName[teachers[t].rank],
               teachers[t].currentLoad,
               teachers[t].maxLoad);
    }
}