#include <stdio.h>
#include <string.h>
#include "data.h"

/* ================= GLOBAL DATA ================= */

Teacher teachers[MAX_TEACHERS];
Section sections[MAX_SECTIONS];
Classroom classrooms[MAX_CLASSROOMS];
ScheduleEntry schedule[MAX_SCHEDULE];

int teacherCount = 0;
int sectionCount = 0;
int classroomCount = 0;
int scheduleCount = 0;

/* ================= TEACHER GENERATION ================= */

void generateTeachers() {

    int index = 0;

    /* Professors – 18 */
    for (int i = 0; i < 18; i++) {
        sprintf(teachers[index].name, "T%d", index + 1);
        teachers[index].seniority = 1;
        teachers[index].maxLoad = 5;
        teachers[index].currentLoad = 0;
        memset(teachers[index].occupied, 0, sizeof teachers[index].occupied);
        index++;
    }

    /* Associate Professors – 5 */
    for (int i = 0; i < 5; i++) {
        sprintf(teachers[index].name, "T%d", index + 1);
        teachers[index].seniority = 2;
        teachers[index].maxLoad = 7;
        teachers[index].currentLoad = 0;
        memset(teachers[index].occupied, 0, sizeof teachers[index].occupied);
        index++;
    }

    /* Assistant Professors – 6 */
    for (int i = 0; i < 6; i++) {
        sprintf(teachers[index].name, "T%d", index + 1);
        teachers[index].seniority = 3;
        teachers[index].maxLoad = 9;
        teachers[index].currentLoad = 0;
        memset(teachers[index].occupied, 0, sizeof teachers[index].occupied);
        index++;
    }

    /* Lecturers – 11 */
    for (int i = 0; i < 11; i++) {
        sprintf(teachers[index].name, "T%d", index + 1);
        teachers[index].seniority = 4;
        teachers[index].maxLoad = 12;
        teachers[index].currentLoad = 0;
        memset(teachers[index].occupied, 0, sizeof teachers[index].occupied);
        index++;
    }

    teacherCount = index;
}

/* ================= SECTIONS + CLASSROOMS ================= */

void generateSectionsAndClassrooms() {

    char labels[3] = {'A','B','C'};
    int index = 0;

    for (int year = 1; year <= YEARS; year++) {

        for (int s = 0; s < SECTIONS_PER_YEAR; s++) {

            sprintf(sections[index].name, "Y%d%c", year, labels[s]);
            sections[index].year = year;
            sections[index].students = 60;

            sprintf(classrooms[index].name, "Room_Y%d%c", year, labels[s]);
            classrooms[index].capacity = 70;

            sections[index].classroomIndex = index;

            memset(sections[index].occupied, 0,
                   sizeof sections[index].occupied);

            memset(classrooms[index].occupied, 0,
                   sizeof classrooms[index].occupied);

            index++;
        }
    }

    sectionCount = index;
    classroomCount = index;
}

/* ================= COURSE ASSIGNMENT ================= */

void assignCoursesToSection(Section *sec) {

    if (sec->year == 1) {

        strcpy(sec->courses[0].name, "Mathematics I");
        sec->courses[0].type = THEORY;
        sec->courses[0].credit = 4;

        strcpy(sec->courses[1].name, "Physics I");
        sec->courses[1].type = THEORY;
        sec->courses[1].credit = 3;

        strcpy(sec->courses[2].name, "Chemistry");
        sec->courses[2].type = THEORY;
        sec->courses[2].credit = 3;

        strcpy(sec->courses[3].name, "Engineering Drawing");
        sec->courses[3].type = THEORY;
        sec->courses[3].credit = 3;

        strcpy(sec->courses[4].name, "Basic Mechanics");
        sec->courses[4].type = THEORY;
        sec->courses[4].credit = 3;

        strcpy(sec->courses[5].name, "Physics Lab");
        sec->courses[5].type = LAB;
        sec->courses[5].credit = 1;

        strcpy(sec->courses[6].name, "Chemistry Lab");
        sec->courses[6].type = LAB;
        sec->courses[6].credit = 1;

        strcpy(sec->courses[7].name, "Workshop Practice");
        sec->courses[7].type = LAB;
        sec->courses[7].credit = 1;
    }

    else {

        char *theoryNames[3][5] = {
            {"Thermodynamics","Fluid Mechanics","Strength of Materials","Manufacturing Process","Engineering Mathematics II"},
            {"Heat Transfer","Machine Design","Theory of Machines","Industrial Engineering","Mechatronics"},
            {"Power Plant Engineering","Refrigeration & AC","Automobile Engineering","Robotics","Project"}
        };

        int idx = sec->year - 2;

        for (int i = 0; i < 5; i++) {
            strcpy(sec->courses[i].name, theoryNames[idx][i]);
            sec->courses[i].type = THEORY;
            sec->courses[i].credit = 3;
        }

        strcpy(sec->courses[5].name, "Lab I");
        sec->courses[5].type = LAB;
        sec->courses[5].credit = 1;

        strcpy(sec->courses[6].name, "Lab II");
        sec->courses[6].type = LAB;
        sec->courses[6].credit = 1;

        strcpy(sec->courses[7].name, "Lab III");
        sec->courses[7].type = LAB;
        sec->courses[7].credit = 1;
    }
}

/* ================= INITIALIZE ================= */

void initializeSystem() {

    generateTeachers();
    generateSectionsAndClassrooms();

    for (int i = 0; i < sectionCount; i++) {
        assignCoursesToSection(&sections[i]);
    }
}

/* ================= BREAK CHECK ================= */

int canPlaceTheory(int sec, int day, int slot) {

    int count = 0;

    for (int s = slot - 1; s >= 0 && s >= slot - 2; s--) {
        if (sections[sec].occupied[day][s] == 1)
            count++;
    }

    if (count >= 2)
        return 0;

    return 1;
}

/* ================= SCHEDULING ENGINE ================= */

void generateSchedule() {

    scheduleCount = 0;

    for (int year = 1; year <= YEARS; year++) {

        /* Collect 3 sections of this year */
        int yearSections[3];
        int idx = 0;

        for (int s = 0; s < sectionCount; s++)
            if (sections[s].year == year)
                yearSections[idx++] = s;

        /* ---------------- THEORY COURSES ---------------- */
        for (int c = 0; c < 5; c++) {

            Course *course = &sections[yearSections[0]].courses[c];

            /* Assign teacher (least load) */
            int teacher = -1;
            int minLoad = 9999;
            int requiredLoad = course->credit * 3;

            for (int t = 0; t < teacherCount; t++) {
                if (teachers[t].currentLoad + requiredLoad <= teachers[t].maxLoad) {
                    if (teachers[t].currentLoad < minLoad) {
                        minLoad = teachers[t].currentLoad;
                        teacher = t;
                    }
                }
            }

            if (teacher == -1) continue;

            /* Spread sessions across different days */
            for (int session = 0; session < course->credit; session++) {

                int day = (c + session) % DAYS;

                for (int secOffset = 0; secOffset < 3; secOffset++) {

                    int sec = yearSections[secOffset];

                    /* rotate slot per section */
                    int slot = (session + secOffset) % 5;

                    if (sections[sec].occupied[day][slot] == 0 &&
                        teachers[teacher].occupied[day][slot] == 0 &&
                        canPlaceTheory(sec, day, slot)) {

                        sections[sec].occupied[day][slot] = 1;
                        teachers[teacher].occupied[day][slot] = 1;
                        teachers[teacher].currentLoad++;

                        schedule[scheduleCount].section = sec;
                        schedule[scheduleCount].day = day;
                        schedule[scheduleCount].slot = slot;
                        schedule[scheduleCount].teacher = teacher;
                        schedule[scheduleCount].courseIndex = c;

                        scheduleCount++;
                    }
                }
            }
        }

        /* ---------------- LAB COURSES ---------------- */
        for (int c = 5; c < 8; c++) {

            Course *course = &sections[yearSections[0]].courses[c];

            int teacher = -1;
            int minLoad = 9999;

            for (int t = 0; t < teacherCount; t++) {
                if (teachers[t].currentLoad + 3 <= teachers[t].maxLoad) {
                    if (teachers[t].currentLoad < minLoad) {
                        minLoad = teachers[t].currentLoad;
                        teacher = t;
                    }
                }
            }

            if (teacher == -1) continue;

            /* Place labs on different days */
            for (int secOffset = 0; secOffset < 3; secOffset++) {

                int sec = yearSections[secOffset];

                int day = (c + secOffset) % DAYS;
                int slot = 5;

                if (sections[sec].occupied[day][slot] == 0 &&
                    teachers[teacher].occupied[day][slot] == 0) {

                    sections[sec].occupied[day][slot] = 1;
                    teachers[teacher].occupied[day][slot] = 1;
                    teachers[teacher].currentLoad++;

                    schedule[scheduleCount].section = sec;
                    schedule[scheduleCount].day = day;
                    schedule[scheduleCount].slot = slot;
                    schedule[scheduleCount].teacher = teacher;
                    schedule[scheduleCount].courseIndex = c;

                    scheduleCount++;
                }
            }
        }
    }
}