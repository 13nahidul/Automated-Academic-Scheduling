#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

Teacher       teachers[MAX_TEACHERS];
Section       sections[MAX_SECTIONS];
Course        courseList[MAX_COURSES];
ScheduleEntry schedule[MAX_SCHEDULE];

/* Global course-slot lock for rule 6 */
int courseSlotLock[MAX_COURSES][DAYS][TOTAL_SLOTS];

int teacherCount  = 0;
int sectionCount  = 0;
int courseCount   = 0;
int scheduleCount = 0;

/* ================================================================
   HELPERS
   ================================================================ */
void toLower(char *s) {
    for (; *s; s++)
        if (*s >= 'A' && *s <= 'Z') *s += 32;
}

void trim(char *s) {
    int start = 0;
    while (s[start]==' '||s[start]=='\t'||s[start]=='\n'||s[start]=='\r') start++;
    if (start) memmove(s, s+start, strlen(s)-start+1);
    int end = (int)strlen(s)-1;
    while (end>=0 && (s[end]==' '||s[end]=='\t'||s[end]=='\n'||s[end]=='\r'))
        s[end--] = '\0';
}

/* ================================================================
   PARSERS
   ================================================================ */
static void parseTeacherToken(char *token, int idx) {
    char tmp[200];
    strncpy(tmp, token, sizeof(tmp)-1); tmp[sizeof(tmp)-1]='\0';

    char *p1 = strchr(tmp, '|'); if (!p1) return; *p1='\0';
    char *name = tmp; trim(name);
    strcpy(teachers[idx].name, name);

    char sn[20]; strncpy(sn, name, sizeof(sn)-1); sn[sizeof(sn)-1]='\0';
    char *sp = strchr(sn,' '); if (sp) *sp='\0';
    toLower(sn); strcpy(teachers[idx].shortName, sn);

    char *p2 = strchr(p1+1, '|'); if (!p2) return; *p2='\0';
    char *rankStr = p1+1; trim(rankStr);
    char *subject = p2+1; trim(subject); toLower(subject);
    strcpy(teachers[idx].subject, subject);

    if      (strncmp(rankStr,"Professor",9)==0) teachers[idx].rank=PROFESSOR;
    else if (strncmp(rankStr,"Associate",9)==0) teachers[idx].rank=ASSOCIATE;
    else if (strncmp(rankStr,"Assistant",9)==0) teachers[idx].rank=ASSISTANT;
    else                                         teachers[idx].rank=LECTURER;

    teachers[idx].maxLoad     = rankLoad(teachers[idx].rank);
    teachers[idx].currentLoad = 0;
    memset(teachers[idx].occupied, 0, sizeof(teachers[idx].occupied));
}

static void parseCourseToken(char *token, int idx) {
    char tmp[200];
    strncpy(tmp, token, sizeof(tmp)-1); tmp[sizeof(tmp)-1]='\0';

    char *p1 = strchr(tmp,'|'); if (!p1) return; *p1='\0';
    char *name = tmp; trim(name);
    char lower[50]; strncpy(lower,name,sizeof(lower)-1); lower[sizeof(lower)-1]='\0';
    toLower(lower); strcpy(courseList[idx].name, lower);

    char sn[20]; strncpy(sn,lower,sizeof(sn)-1); sn[sizeof(sn)-1]='\0';
    char *sp=strchr(sn,' '); if(sp)*sp='\0';
    strcpy(courseList[idx].shortName, sn);

    char *p2=strchr(p1+1,'|'); if(!p2) return; *p2='\0';
    char *typeStr=p1+1; trim(typeStr);
    char *creditStr=p2+1; trim(creditStr);
    courseList[idx].credit = atoi(creditStr);
    { char lname[50]; strncpy(lname,lower,sizeof(lname)-1); lname[sizeof(lname)-1]="\0"; if (strstr(lname,"lab")!=NULL) courseList[idx].type=LAB; else courseList[idx].type=(strcmp(typeStr,"LAB")==0)?LAB:THEORY; }
}

/* ================================================================
   LOAD DATA
   ================================================================ */
void loadData() {
    FILE *fp = fopen("data.txt","r");
    if (!fp) { printf("[ERROR] Cannot open data.txt\n"); return; }

    char line[2000];
    int mode = 0;

    while (fgets(line, sizeof(line), fp)) {
        trim(line);
        if (strlen(line)==0) continue;
        if (strncmp(line,"Teachers:",9)==0) { mode=1; continue; }
        if (strncmp(line,"Courses:", 8)==0) { mode=2; continue; }
        if (strncmp(line,"Sections:",9)==0) { mode=3; continue; }

        char buf[2000];
        strncpy(buf,line,sizeof(buf)-1); buf[sizeof(buf)-1]='\0';
        char *tok = strtok(buf,",");

        if (mode==1) {
            while (tok && teacherCount<MAX_TEACHERS) {
                trim(tok);
                if (strlen(tok)>0) { parseTeacherToken(tok,teacherCount++); }
                tok=strtok(NULL,",");
            }
        } else if (mode==2) {
            while (tok && courseCount<MAX_COURSES) {
                trim(tok);
                if (strlen(tok)>0) { parseCourseToken(tok,courseCount++); }
                tok=strtok(NULL,",");
            }
        } else if (mode==3) {
            while (tok && sectionCount<MAX_SECTIONS) {
                trim(tok);
                if (strlen(tok)>0) {
                    strcpy(sections[sectionCount].name, tok);
                    sections[sectionCount].courseCount = courseCount;
                    for (int i=0;i<courseCount;i++)
                        sections[sectionCount].courses[i]=courseList[i];
                    memset(sections[sectionCount].occupied,0,
                           sizeof(sections[sectionCount].occupied));
                    sectionCount++;
                }
                tok=strtok(NULL,",");
            }
        }
    }
    fclose(fp);
    memset(courseSlotLock,0,sizeof(courseSlotLock));
    printf("[INFO] Loaded %d teachers, %d courses, %d sections\n",
           teacherCount, courseCount, sectionCount);
}

void initializeSystem() { loadData(); }

/* ================================================================
   FIND TEACHER
   Exact subject match first (senior priority), then fallback.
   ================================================================ */
int findTeacher(const char *subject, int d, int s) {
    int best=-1;
    for (int t=0;t<teacherCount;t++) {
        if (strcmp(teachers[t].subject,subject)==0 &&
            !teachers[t].occupied[d][s] &&
            teachers[t].currentLoad < teachers[t].maxLoad) {
            if (best==-1 || teachers[t].rank < teachers[best].rank) best=t;
        }
    }
    if (best!=-1) return best;
    for (int t=0;t<teacherCount;t++) {
        if (!teachers[t].occupied[d][s] &&
            teachers[t].currentLoad < teachers[t].maxLoad) {
            if (best==-1 || teachers[t].rank < teachers[best].rank) best=t;
        }
    }
    return best;
}

/* ================================================================
   BREAK CHECK  (Rule 4)
   Returns 1 if placing a class at [sec][d][s] would create 3
   consecutive occupied slots with no gap.
   ================================================================ */
static int wouldMake3Consecutive(int sec, int d, int s) {
    /* only applies within theory block (slots 0-4) */
    if (s < THEORY_START || s >= THEORY_START+THEORY_SLOTS) return 0;

    /* check if s-2, s-1, s are all occupied (adding s) */
    int before2 = (s>=2) ? sections[sec].occupied[d][s-2] : 0;
    int before1 = (s>=1) ? sections[sec].occupied[d][s-1] : 0;
    if (before2 && before1) return 1;

    /* check if s, s+1, s+2 would all be occupied */
    int after1  = (s+1 < THEORY_START+THEORY_SLOTS) ? sections[sec].occupied[d][s+1] : 0;
    int after2  = (s+2 < THEORY_START+THEORY_SLOTS) ? sections[sec].occupied[d][s+2] : 0;
    if (after1 && after2) return 1;

    /* check s-1, s, s+1 */
    if (before1 && after1) return 1;

    return 0;
}

/* ================================================================
   COUNT classes scheduled for a section on a given day
   ================================================================ */
static int classesOnDay(int sec, int d) {
    int cnt=0;
    for (int s=0;s<TOTAL_SLOTS;s++)
        if (sections[sec].occupied[d][s]) cnt++;
    return cnt;
}

/* ================================================================
   GENERATE SCHEDULE
   Rules enforced:
   1. Days: Saturday(0) to Wednesday(4)
   2. Theory: slots 0-4 (8am-1pm, 1hr each)
   3. Lab:    slots 5-7 (2pm-5pm, 1hr each)   [3 consecutive slots]
   4. No 3 back-to-back theory classes (break rule)
   5. Equal distribution across 5 days
   6. Same course not in two sections at same time (courseSlotLock)
   7. Lab slots are used for labs only and teacher checked for overlap
   ================================================================ */
void generateSchedule() {
    scheduleCount = 0;

    for (int sec=0; sec<sectionCount; sec++) {
        for (int c=0; c<sections[sec].courseCount; c++) {

            int need    = sections[sec].courses[c].credit;
            CourseType ct = sections[sec].courses[c].type;
            if (need<=0) continue;

            /* Find the global index of this course in courseList */
            int courseIdx = -1;
            for (int ci=0; ci<courseCount; ci++) {
                if (strcmp(courseList[ci].name,
                           sections[sec].courses[c].name)==0) {
                    courseIdx=ci; break;
                }
            }

            /* ── LAB course: needs 'credit' consecutive lab slots ──
               Rule 3: always 2-5pm (slots 5,6,7).
               Rule 7: all lab slots must be free for teacher & section.
               Labs for same course must not overlap across sections (rule 6).  */
            if (ct == LAB) {
                int placed=0;
                for (int d=0; d<DAYS && !placed; d++) {
                    /* Check all 3 lab slots free for this section */
                    int secFree=1;
                    for (int ls=LAB_START; ls<LAB_START+LAB_SLOTS; ls++)
                        if (sections[sec].occupied[d][ls]) { secFree=0; break; }
                    if (!secFree) continue;

                    /* Rule 6: check courseSlotLock for all lab slots */
                    if (courseIdx>=0) {
                        int locked=0;
                        for (int ls=LAB_START; ls<LAB_START+LAB_SLOTS; ls++)
                            if (courseSlotLock[courseIdx][d][ls]) { locked=1; break; }
                        if (locked) continue;
                    }

                    /* Find teacher free for ALL lab slots on this day */
                    int t=-1;
                    for (int cand=0; cand<teacherCount; cand++) {
                        if (strcmp(teachers[cand].subject,
                                   sections[sec].courses[c].name)!=0) continue;
                        int free=1;
                        for (int ls=LAB_START; ls<LAB_START+LAB_SLOTS; ls++)
                            if (teachers[cand].occupied[d][ls]) { free=0; break; }
                        if (free && teachers[cand].currentLoad+LAB_SLOTS
                                    <= teachers[cand].maxLoad) {
                            if (t==-1 || teachers[cand].rank<teachers[t].rank) t=cand;
                        }
                    }
                    /* Fallback teacher */
                    if (t==-1) {
                        for (int cand=0; cand<teacherCount; cand++) {
                            int free=1;
                            for (int ls=LAB_START; ls<LAB_START+LAB_SLOTS; ls++)
                                if (teachers[cand].occupied[d][ls]) { free=0; break; }
                            if (free && teachers[cand].currentLoad+LAB_SLOTS
                                        <= teachers[cand].maxLoad) {
                                if (t==-1 || teachers[cand].rank<teachers[t].rank) t=cand;
                            }
                        }
                    }
                    if (t==-1) continue;

                    /* Place 3 consecutive lab slots */
                    for (int ls=LAB_START; ls<LAB_START+LAB_SLOTS; ls++) {
                        sections[sec].occupied[d][ls] = 1;
                        teachers[t].occupied[d][ls]   = 1;
                        if (courseIdx>=0)
                            courseSlotLock[courseIdx][d][ls] = 1;
                        schedule[scheduleCount++] = (ScheduleEntry){sec,d,ls,t,c};
                    }
                    teachers[t].currentLoad += LAB_SLOTS;
                    placed=1;
                }
                continue; /* done with this lab course */
            }

            /* ── THEORY course ──
               Rule 2: only slots 0-4 (8am-1pm).
               Rule 5: spread evenly — pick day with fewest classes first.
               Rule 4: avoid 3 consecutive slots.
               Rule 6: courseSlotLock prevents same course same slot in two sections. */

            /* Build day order sorted by current class count (fewest first) */
            int dayOrder[DAYS];
            for (int i=0;i<DAYS;i++) dayOrder[i]=i;
            /* Bubble sort by classesOnDay */
            for (int i=0;i<DAYS-1;i++)
                for (int j=i+1;j<DAYS;j++)
                    if (classesOnDay(sec,dayOrder[i]) >
                        classesOnDay(sec,dayOrder[j])) {
                        int tmp=dayOrder[i]; dayOrder[i]=dayOrder[j]; dayOrder[j]=tmp;
                    }

            /* Pass 1: one class per day, spread (respects break rule) */
            for (int di=0; di<DAYS && need>0; di++) {
                int d=dayOrder[di];
                for (int s=THEORY_START; s<THEORY_START+THEORY_SLOTS && need>0; s++) {
                    if (sections[sec].occupied[d][s]) continue;
                    if (wouldMake3Consecutive(sec,d,s)) continue;
                    if (courseIdx>=0 && courseSlotLock[courseIdx][d][s]) continue;

                    int t=findTeacher(sections[sec].courses[c].name,d,s);
                    if (t==-1) continue;

                    sections[sec].occupied[d][s] = 1;
                    teachers[t].occupied[d][s]   = 1;
                    teachers[t].currentLoad++;
                    if (courseIdx>=0) courseSlotLock[courseIdx][d][s]=1;
                    schedule[scheduleCount++]=(ScheduleEntry){sec,d,s,t,c};
                    need--;
                    break; /* only 1 per day in pass 1 */
                }
            }

            /* Pass 2: fill remaining credits (still respecting break & lock) */
            for (int di=0; di<DAYS && need>0; di++) {
                int d=dayOrder[di];
                for (int s=THEORY_START; s<THEORY_START+THEORY_SLOTS && need>0; s++) {
                    if (sections[sec].occupied[d][s]) continue;
                    if (wouldMake3Consecutive(sec,d,s)) continue;
                    if (courseIdx>=0 && courseSlotLock[courseIdx][d][s]) continue;

                    int t=findTeacher(sections[sec].courses[c].name,d,s);
                    if (t==-1) continue;

                    sections[sec].occupied[d][s] = 1;
                    teachers[t].occupied[d][s]   = 1;
                    teachers[t].currentLoad++;
                    if (courseIdx>=0) courseSlotLock[courseIdx][d][s]=1;
                    schedule[scheduleCount++]=(ScheduleEntry){sec,d,s,t,c};
                    need--;
                }
            }

            /* Pass 3: last resort — ignore break rule, still respect lock */
            for (int d=0; d<DAYS && need>0; d++) {
                for (int s=THEORY_START; s<THEORY_START+THEORY_SLOTS && need>0; s++) {
                    if (sections[sec].occupied[d][s]) continue;
                    if (courseIdx>=0 && courseSlotLock[courseIdx][d][s]) continue;

                    int t=findTeacher(sections[sec].courses[c].name,d,s);
                    if (t==-1) continue;

                    sections[sec].occupied[d][s] = 1;
                    teachers[t].occupied[d][s]   = 1;
                    teachers[t].currentLoad++;
                    if (courseIdx>=0) courseSlotLock[courseIdx][d][s]=1;
                    schedule[scheduleCount++]=(ScheduleEntry){sec,d,s,t,c};
                    need--;
                }
            }
        }
    }
    printf("[INFO] Scheduled %d classes total\n", scheduleCount);
}