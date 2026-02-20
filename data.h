#ifndef DATA_H
#define DATA_H

#define MAX_TEACHERS 40
#define MAX_SECTIONS 12
#define MAX_COURSES_PER_YEAR 8
#define MAX_CLASSROOMS 12
#define MAX_SCHEDULE 2000

#define YEARS 4
#define SECTIONS_PER_YEAR 3

#define DAYS 5
#define SLOTS_PER_DAY 6   // 5 theory + 1 lab

typedef enum {
    THEORY,
    LAB
} CourseType;

typedef struct {
    char name[20];
    int seniority;
    int maxLoad;
    int currentLoad;
    int occupied[DAYS][SLOTS_PER_DAY];
} Teacher;

typedef struct {
    char name[20];
    int capacity;
    int occupied[DAYS][SLOTS_PER_DAY];
} Classroom;

typedef struct {
    char name[50];
    CourseType type;
    int credit;   // NEW
} Course;

typedef struct {
    char name[10];
    int year;
    int students;
    int classroomIndex;
    Course courses[MAX_COURSES_PER_YEAR];
    int occupied[DAYS][SLOTS_PER_DAY];
} Section;

typedef struct {
    int section;
    int day;
    int slot;
    int teacher;
    int courseIndex;
} ScheduleEntry;

#endif