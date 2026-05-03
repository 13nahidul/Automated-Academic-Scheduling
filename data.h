#ifndef DATA_H
#define DATA_H

#define MAX_TEACHERS 50
#define MAX_SECTIONS 20
#define MAX_COURSES  20
#define MAX_SCHEDULE 5000

/*
 * DAYS  : 5  (Saturday=0 ... Wednesday=4)
 * SLOTS : 8  total per day
 *   Theory slots  0-4  ->  8:00, 9:00, 10:00, 11:00, 12:00  (each 1 hour)
 *   Lab    slots  5-7  ->  2:00, 3:00, 4:00                  (each 1 hour, 2-5 pm)
 */
#define DAYS          5
#define THEORY_SLOTS  5
#define LAB_SLOTS     3
#define TOTAL_SLOTS   8    /* 5 theory + 3 lab */

#define THEORY_START  0
#define LAB_START     5

/* Seniority -- lower value = more senior */
typedef enum { PROFESSOR=1, ASSOCIATE=2, ASSISTANT=3, LECTURER=4 } Rank;

typedef enum { THEORY, LAB } CourseType;

typedef struct {
    char name[50];
    char shortName[20];
    char subject[50];
    Rank rank;
    int  maxLoad;
    int  currentLoad;
    int  occupied[DAYS][TOTAL_SLOTS];
} Teacher;

typedef struct {
    char       name[50];
    char       shortName[20];
    CourseType type;
    int        credit;
} Course;

typedef struct {
    char   name[10];
    Course courses[MAX_COURSES];
    int    courseCount;
    int    occupied[DAYS][TOTAL_SLOTS];
} Section;

typedef struct {
    int section;
    int day;
    int slot;
    int teacher;
    int course;
} ScheduleEntry;

/* Prevents same course from being scheduled in multiple sections
   at the exact same day+slot (rule 6) */
extern int courseSlotLock[MAX_COURSES][DAYS][TOTAL_SLOTS];

static inline int rankLoad(Rank r) {
    switch(r) {
        case PROFESSOR:  return 12;
        case ASSOCIATE:  return 14;
        case ASSISTANT:  return 16;
        default:         return 18;
    }
}

#endif