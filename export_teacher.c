#include <stdio.h>
#include "data.h"

extern Teacher       teachers[];
extern Section       sections[];
extern ScheduleEntry schedule[];
extern int teacherCount, sectionCount, scheduleCount;

static const char *DAY_NAMES[]  = {"Saturday","Sunday","Monday","Tuesday","Wednesday"};
static const char *SLOT_TIMES[] = {
    "8:00-9:00","9:00-10:00","10:00-11:00","11:00-12:00","12:00-1:00",
    "2:00-3:00","3:00-4:00","4:00-5:00"
};

/* ── Shared CSS (green accent for teacher page) ─────────── */
static void writeCSS(FILE *fp) {
    fprintf(fp,
"<style>\n"
"@import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap');\n"
"*{box-sizing:border-box;margin:0;padding:0}\n"
"body{font-family:'Inter',sans-serif;background:#EDF7F1;padding:28px;color:#1a202c;min-width:900px}\n"
"h1{text-align:center;font-size:1.9rem;color:#14532d;margin-bottom:5px}\n"
".sub{text-align:center;color:#64748b;margin-bottom:26px;font-size:.85rem}\n"
/* nav */
".nav{display:flex;gap:10px;flex-wrap:wrap;margin-bottom:22px;justify-content:center;align-items:center}\n"
".nav a{padding:7px 20px;border-radius:20px;text-decoration:none;background:#fff;"
"color:#334155;font-size:.82rem;font-weight:600;border:1.5px solid #cbd5e1;transition:.15s}\n"
".nav a:hover{background:#166534;color:#fff;border-color:#166534}\n"
".nav a.active{background:#166534;color:#fff;border-color:#166534}\n"
/* block */
".block{background:#fff;border-radius:12px;box-shadow:0 2px 14px rgba(0,0,0,.07);"
"margin-bottom:30px;overflow:hidden;page-break-inside:avoid}\n"
".block-title{background:#14532d;color:#fff;padding:12px 20px;font-size:1rem;"
"font-weight:700;display:flex;justify-content:space-between;align-items:center}\n"
".block-title .subj{opacity:.7;font-weight:400;font-size:.88rem;margin-left:8px}\n"
".block-title .badge{font-size:.72rem;background:rgba(255,255,255,.2);padding:3px 10px;"
"border-radius:12px;font-weight:400;white-space:nowrap}\n"
/* table */
"table{width:100%%;border-collapse:collapse}\n"
"th{background:#166534;color:#fff;padding:9px 6px;font-size:.74rem;white-space:nowrap;text-align:center}\n"
"th.sep,td.sep{background:#bbf7d0;width:5px;padding:0;border:none}\n"
"td{padding:8px 4px;text-align:center;border:1px solid #e2e8f0;font-size:.74rem;min-width:80px}\n"
"td.day{font-weight:700;background:#F0FDF4;white-space:nowrap;color:#14532d;min-width:90px;padding:8px 10px}\n"
"td.empty{color:#c0cce0}\n"
"td.theory{background:#ECFDF5;border-left:3px solid #16a34a}\n"
"td.lab{background:#FEF9E7;border-left:3px solid #f0a500}\n"
".cn{font-weight:700;color:#14532d;display:block}\n"
".sn{color:#607d8b;font-size:.67rem;display:block;margin-top:2px}\n"
/* buttons */
".btn-bar{padding:10px 18px 14px;display:flex;justify-content:flex-end;gap:8px}\n"
".pdf-btn{display:inline-flex;align-items:center;gap:5px;background:#c0392b;"
"color:#fff;border:none;padding:7px 15px;border-radius:8px;font-size:.78rem;"
"font-weight:600;cursor:pointer;font-family:inherit;transition:.15s}\n"
".pdf-btn:hover{background:#96281b}\n"
".all-btn{display:inline-flex;align-items:center;gap:5px;background:#14532d;"
"color:#fff;border:none;padding:7px 16px;border-radius:20px;font-size:.82rem;"
"font-weight:600;cursor:pointer;font-family:inherit;transition:.15s}\n"
".all-btn:hover{background:#166534}\n"
/* print */
"@media print{\n"
"  body{background:#fff;padding:10px}\n"
"  .no-print{display:none!important}\n"
"  .block{box-shadow:none;border:1px solid #ccc}\n"
"  .block-title{background:#14532d!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  th{background:#166534!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  td.theory{background:#ECFDF5!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  td.lab{background:#FEF9E7!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"}\n"
"</style>\n");
}

/* ── PDF script ─────────────────────────────────────────── */
static void writePDFScript(FILE *fp) {
    fprintf(fp,
"<script>\n"
"function dlBlock(id,title){\n"
"  var el=document.getElementById(id);\n"
"  var w=window.open('','_blank','width=1000,height=750');\n"
"  var styles='';\n"
"  document.querySelectorAll('style').forEach(function(s){styles+=s.outerHTML;});\n"
"  w.document.write('<!DOCTYPE html><html><head><meta charset=UTF-8>'+"
"'<title>'+title+'</title>');\n"
"  w.document.write(styles);\n"
"  w.document.write('</head><body style=\"padding:20px;background:#fff\">');\n"
"  w.document.write(el.outerHTML);\n"
"  w.document.write('</body></html>');\n"
"  w.document.close();\n"
"  w.focus();\n"
"  setTimeout(function(){w.print();},450);\n"
"}\n"
"function dlAll(){window.print();}\n"
"</script>\n");
}

/* ── Table header ───────────────────────────────────────── */
static void writeTableHeader(FILE *fp) {
    fprintf(fp,"<table><tr><th>Day</th>");
    for (int s=0; s<TOTAL_SLOTS; s++) {
        if (s==LAB_START) fprintf(fp,"<th class='sep'></th>");
        fprintf(fp,"<th>%s</th>", SLOT_TIMES[s]);
    }
    fprintf(fp,"</tr>\n");
}

/* ══════════════════════════════════════════════════════════
   TEACHER ROUTINE  →  teacher_routine.html
   ══════════════════════════════════════════════════════════ */
void exportTeacherHTML() {
    FILE *fp = fopen("teacher_routine.html","w");
    if (!fp) { printf("[ERROR] Cannot write teacher_routine.html\n"); return; }

    const char *rankNames[]={"","Professor","Associate","Assistant","Lecturer"};

    fprintf(fp,"<!DOCTYPE html><html lang='en'><head>"
               "<meta charset='UTF-8'>"
               "<title>Class Routine &mdash; Teacher View</title>\n");
    writeCSS(fp);
    writePDFScript(fp);
    fprintf(fp,"</head><body>\n");

    fprintf(fp,"<h1>&#127979; Teacher Routines</h1>\n"
               "<p class='sub'>Saturday &ndash; Wednesday &nbsp;&bull;&nbsp; "
               "Theory: 8:00 AM &ndash; 1:00 PM &nbsp;&bull;&nbsp; "
               "Lab: 2:00 PM &ndash; 5:00 PM</p>\n");

    /* Nav */
    fprintf(fp,"<div class='nav no-print'>\n"
               "  <a href='routine.html'>&#128100; Section View</a>\n"
               "  <a href='teacher_routine.html' class='active'>&#127979; Teacher View</a>\n"
               "  <button class='all-btn' onclick='dlAll()'>&#128438; Print / Save All as PDF</button>\n"
               "</div>\n");

    /* One block per teacher (skip teachers with zero classes) */
    for (int t=0; t<teacherCount; t++) {

        int total=0;
        for (int i=0;i<scheduleCount;i++)
            if (schedule[i].teacher==t) total++;
        if (total==0) continue;

        fprintf(fp,
            "<div class='block' id='teacher_%d'>\n"
            "<div class='block-title'>"
            "&#127979; %s"
            "<span class='subj'>(%s)</span>"
            "<span class='badge'>%s &mdash; %d / %d classes</span>"
            "</div>\n",
            t,
            teachers[t].name,
            teachers[t].subject,
            rankNames[teachers[t].rank],
            teachers[t].currentLoad,
            teachers[t].maxLoad);

        writeTableHeader(fp);

        for (int d=0; d<DAYS; d++) {
            fprintf(fp,"<tr><td class='day'>%s</td>", DAY_NAMES[d]);
            for (int s=0; s<TOTAL_SLOTS; s++) {
                if (s==LAB_START) fprintf(fp,"<td class='sep'></td>");

                /* Collect ALL entries for this teacher at [d][s]
                   (same teacher could cover multiple sections in same slot via fallback,
                    but normally only 1 — we show the first and append extras) */
                int entries[MAX_SECTIONS];
                int eCount=0;
                for (int i=0;i<scheduleCount;i++) {
                    if (schedule[i].teacher==t &&
                        schedule[i].day==d &&
                        schedule[i].slot==s) {
                        entries[eCount++]=i;
                        if (eCount>=MAX_SECTIONS) break;
                    }
                }

                if (eCount>0) {
                    const char *cls=(s>=LAB_START)?"lab":"theory";
                    fprintf(fp,"<td class='%s'>", cls);
                    for (int e=0; e<eCount; e++) {
                        int idx=entries[e];
                        int sec=schedule[idx].section;
                        int crs=schedule[idx].course;
                        fprintf(fp,"<span class='cn'>%s</span>"
                                   "<span class='sn'>Sec %s</span>",
                                sections[sec].courses[crs].name,
                                sections[sec].name);
                    }
                    fprintf(fp,"</td>");
                } else {
                    fprintf(fp,"<td class='empty'>&ndash;</td>");
                }
            }
            fprintf(fp,"</tr>\n");
        }
        fprintf(fp,"</table>\n");

        /* Per-teacher PDF button */
        fprintf(fp,
            "<div class='btn-bar no-print'>"
            "<button class='pdf-btn' "
            "onclick='dlBlock(\"teacher_%d\",\"Routine - %s\")'>"
            "&#128438; Download PDF &mdash; %s"
            "</button></div>\n",
            t, teachers[t].name, teachers[t].name);

        fprintf(fp,"</div>\n");
    }

    fprintf(fp,"</body></html>\n");
    fclose(fp);
    printf("[INFO] teacher_routine.html generated\n");
}
