#include <stdio.h>
#include "data.h"

/* EXTERN GLOBALS */
extern Section sections[];
extern Teacher teachers[];
extern ScheduleEntry schedule[];
extern int sectionCount;
extern int scheduleCount;

char *daysHTML[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday"};
char *timesHTML[] = {"8-9", "9-10", "10-11", "11-12", "12-1", "2-5"};

void exportScheduleToHTML() {

    FILE *fp = fopen("routine.html", "w");
    if (!fp) {
        printf("Error creating HTML file\n");
        return;
    }

    fprintf(fp, "<html><head><title>University Routine</title>\n");
    fprintf(fp, "<style>\n");
    fprintf(fp, "table{border-collapse:collapse;width:100%%;margin-bottom:40px;}\n");
    fprintf(fp, "th,td{border:1px solid black;padding:8px;text-align:center;}\n");
    fprintf(fp, "th{background:#e0e0e0;}\n");
    fprintf(fp, "h2{margin-top:50px;}\n");
    fprintf(fp, "</style></head><body>\n");

    fprintf(fp, "<h1>Mechanical Engineering Routine</h1>\n");

    for (int sec = 0; sec < sectionCount; sec++) {

        fprintf(fp, "<h2>Year %d - Section %s</h2>\n",
                sections[sec].year,
                sections[sec].name);

        fprintf(fp, "<table>\n");
        fprintf(fp, "<tr><th>Time</th>");

        for (int d = 0; d < DAYS; d++)
            fprintf(fp, "<th>%s</th>", daysHTML[d]);

        fprintf(fp, "</tr>\n");

        for (int s = 0; s < SLOTS_PER_DAY; s++) {

            fprintf(fp, "<tr><td>%s</td>", timesHTML[s]);

            for (int d = 0; d < DAYS; d++) {

                int found = 0;

                for (int i = 0; i < scheduleCount; i++) {

                    if (schedule[i].section == sec &&
                        schedule[i].day == d &&
                        schedule[i].slot == s) {

                        fprintf(fp,
                                "<td>%s<br><b>%s</b></td>",
                                sections[sec].courses[schedule[i].courseIndex].name,
                                teachers[schedule[i].teacher].name);

                        found = 1;
                        break;
                    }
                }

                if (!found)
                    fprintf(fp, "<td>-</td>");
            }

            fprintf(fp, "</tr>\n");
        }

        fprintf(fp, "</table>\n");
    }

    fprintf(fp, "</body></html>");
    fclose(fp);

    printf("HTML routine generated: routine.html\n");
}