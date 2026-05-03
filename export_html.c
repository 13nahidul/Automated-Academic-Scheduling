#include <stdio.h>
#include "data.h"

extern Teacher       teachers[];
extern Section       sections[];
extern ScheduleEntry schedule[];
extern int sectionCount, scheduleCount, teacherCount;

static const char *DAY_NAMES[]  = {"Saturday","Sunday","Monday","Tuesday","Wednesday"};
static const char *SLOT_TIMES[] = {
    "8:00-9:00","9:00-10:00","10:00-11:00","11:00-12:00","12:00-1:00",
    "2:00-3:00","3:00-4:00","4:00-5:00"
};

/* ── Shared CSS ─────────────────────────────────────────── */
static void writeCSS(FILE *fp) {
    fprintf(fp,
"<style>\n"
"@import url('https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap');\n"
"*{box-sizing:border-box;margin:0;padding:0}\n"
"body{font-family:'Inter',sans-serif;background:#EEF2F7;padding:28px;color:#1a202c;min-width:900px}\n"
"h1{text-align:center;font-size:1.9rem;color:#1e3a5f;margin-bottom:5px}\n"
".sub{text-align:center;color:#64748b;margin-bottom:26px;font-size:.85rem}\n"
/* nav */
".nav{display:flex;gap:10px;flex-wrap:wrap;margin-bottom:22px;justify-content:center;align-items:center}\n"
".nav a{padding:7px 20px;border-radius:20px;text-decoration:none;background:#fff;"
"color:#334155;font-size:.82rem;font-weight:600;border:1.5px solid #cbd5e1;transition:.15s}\n"
".nav a:hover{background:#2d5282;color:#fff;border-color:#2d5282}\n"
".nav a.active{background:#2d5282;color:#fff;border-color:#2d5282}\n"
/* block */
".block{background:#fff;border-radius:12px;box-shadow:0 2px 14px rgba(0,0,0,.07);"
"margin-bottom:30px;overflow:hidden;page-break-inside:avoid}\n"
".block-title{background:#1e3a5f;color:#fff;padding:12px 20px;font-size:1rem;"
"font-weight:700;display:flex;justify-content:space-between;align-items:center}\n"
".block-title .badge{font-size:.72rem;background:rgba(255,255,255,.2);padding:3px 10px;"
"border-radius:12px;font-weight:400}\n"
/* table */
"table{width:100%%;border-collapse:collapse}\n"
"th{background:#2d5282;color:#fff;padding:9px 6px;font-size:.74rem;white-space:nowrap;text-align:center}\n"
"th.sep,td.sep{background:#c8d8ee;width:5px;padding:0;border:none}\n"
"td{padding:8px 4px;text-align:center;border:1px solid #e2e8f0;font-size:.74rem;min-width:80px}\n"
"td.day{font-weight:700;background:#F7FAFC;white-space:nowrap;color:#2d3748;min-width:90px;padding:8px 10px}\n"
"td.empty{color:#c0cce0}\n"
"td.theory{background:#EBF5FB;border-left:3px solid #2980b9}\n"
"td.lab{background:#FEF9E7;border-left:3px solid #f0a500}\n"
".cn{font-weight:700;color:#1a4a7a;display:block}\n"
".sn{color:#607d8b;font-size:.67rem;display:block;margin-top:2px}\n"
/* pdf buttons */
".btn-bar{padding:10px 18px 14px;display:flex;justify-content:flex-end;gap:8px}\n"
".pdf-btn{display:inline-flex;align-items:center;gap:5px;background:#c0392b;"
"color:#fff;border:none;padding:7px 15px;border-radius:8px;font-size:.78rem;"
"font-weight:600;cursor:pointer;font-family:inherit;transition:.15s;text-decoration:none}\n"
".pdf-btn:hover{background:#96281b}\n"
".all-btn{display:inline-flex;align-items:center;gap:5px;background:#1e3a5f;"
"color:#fff;border:none;padding:7px 16px;border-radius:20px;font-size:.82rem;"
"font-weight:600;cursor:pointer;font-family:inherit;transition:.15s}\n"
".all-btn:hover{background:#2d5282}\n"
/* load table */
".load-section{max-width:720px;margin:10px auto 36px}\n"
".load-section h2{color:#1e3a5f;margin-bottom:10px;font-size:1.05rem}\n"
".lt{width:100%%;border-collapse:collapse;background:#fff;border-radius:8px;"
"overflow:hidden;box-shadow:0 1px 4px rgba(0,0,0,.07)}\n"
".lt th{background:#2d5282;color:#fff;padding:8px 14px;text-align:left;font-size:.8rem}\n"
".lt td{padding:8px 14px;border-bottom:1px solid #e8edf2;font-size:.8rem}\n"
".bar-bg{height:8px;background:#dce8f5;border-radius:4px;margin-top:3px;width:150px}\n"
".bar-fg{height:8px;background:#2980b9;border-radius:4px}\n"
/* print */
"@media print{\n"
"  body{background:#fff;padding:10px}\n"
"  .no-print{display:none!important}\n"
"  .block{box-shadow:none;border:1px solid #ccc}\n"
"  .block-title{background:#1e3a5f!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  th{background:#2d5282!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  td.theory{background:#EBF5FB!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"  td.lab{background:#FEF9E7!important;-webkit-print-color-adjust:exact;print-color-adjust:exact}\n"
"}\n"
"</style>\n");
}

/* ── PDF JS ─────────────────────────────────────────────── */
static void writePDFScript(FILE *fp) {
    fprintf(fp,
"<script>\n"
"function dlBlock(id,title){\n"
"  var el=document.getElementById(id);\n"
"  var w=window.open('','_blank','width=1000,height=750');\n"
"  var styles='';\n"
"  document.querySelectorAll('style').forEach(function(s){styles+=s.outerHTML;});\n"
"  w.document.write('<!DOCTYPE html><html><head><meta charset=UTF-8><title>'+title+'</title>');\n"
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

/* ── Table header row ────────────────────────────────────── */
static void writeTableHeader(FILE *fp) {
    fprintf(fp,"<table><tr><th>Day</th>");
    for (int s=0; s<TOTAL_SLOTS; s++) {
        if (s==LAB_START) fprintf(fp,"<th class='sep'></th>");
        fprintf(fp,"<th>%s</th>", SLOT_TIMES[s]);
    }
    fprintf(fp,"</tr>\n");
}

/* ══════════════════════════════════════════════════════════
   SECTION ROUTINE  →  routine.html
   ══════════════════════════════════════════════════════════ */
void exportHTML() {
    FILE *fp = fopen("routine.html","w");
    if (!fp) { printf("[ERROR] Cannot write routine.html\n"); return; }

    fprintf(fp,"<!DOCTYPE html><html lang='en'><head>"
               "<meta charset='UTF-8'><title>Class Routine &mdash; Section View</title>\n");
    writeCSS(fp);
    writePDFScript(fp);
    fprintf(fp,"</head><body>\n");

    fprintf(fp,"<h1>&#128197; Class Routine</h1>\n"
               "<p class='sub'>Saturday &ndash; Wednesday &nbsp;&bull;&nbsp; "
               "Theory: 8:00 AM &ndash; 1:00 PM &nbsp;&bull;&nbsp; "
               "Lab: 2:00 PM &ndash; 5:00 PM</p>\n");

    /* Nav */
    fprintf(fp,"<div class='nav no-print'>\n"
               "  <a href='routine.html' class='active'>&#128100; Section View</a>\n"
               "  <a href='teacher_routine.html'>&#127979; Teacher View</a>\n"
               "  <button class='all-btn' onclick='dlAll()'>&#128438; Print / Save All as PDF</button>\n"
               "</div>\n");

    const char *rankNames[]={"","Professor","Associate","Assistant","Lecturer"};

    /* One block per section */
    for (int sec=0; sec<sectionCount; sec++) {

        /* Count total classes for summary badge */
        int total=0;
        for (int i=0;i<scheduleCount;i++)
            if (schedule[i].section==sec) total++;

        fprintf(fp,
            "<div class='block' id='sec_%d'>\n"
            "<div class='block-title'>"
            "&#128100; Section %s"
            "<span class='badge'>%d classes/week</span>"
            "</div>\n",
            sec, sections[sec].name, total);

        writeTableHeader(fp);

        for (int d=0; d<DAYS; d++) {
            fprintf(fp,"<tr><td class='day'>%s</td>", DAY_NAMES[d]);
            for (int s=0; s<TOTAL_SLOTS; s++) {
                if (s==LAB_START) fprintf(fp,"<td class='sep'></td>");

                int found=-1;
                for (int i=0;i<scheduleCount;i++) {
                    if (schedule[i].section==sec &&
                        schedule[i].day==d &&
                        schedule[i].slot==s) { found=i; break; }
                }
                if (found!=-1) {
                    const char *cls=(s>=LAB_START)?"lab":"theory";
                    fprintf(fp,"<td class='%s'>"
                               "<span class='cn'>%s</span>"
                               "<span class='sn'>%s</span>"
                               "</td>",
                            cls,
                            sections[sec].courses[schedule[found].course].name,
                            teachers[schedule[found].teacher].name);
                } else {
                    fprintf(fp,"<td class='empty'>&ndash;</td>");
                }
            }
            fprintf(fp,"</tr>\n");
        }
        fprintf(fp,"</table>\n");

        /* Per-section PDF button */
        fprintf(fp,
            "<div class='btn-bar no-print'>"
            "<button class='pdf-btn' "
            "onclick='dlBlock(\"sec_%d\",\"Routine - Section %s\")'>"
            "&#128438; Download PDF &mdash; Section %s"
            "</button></div>\n",
            sec, sections[sec].name, sections[sec].name);

        fprintf(fp,"</div>\n");
    }

    /* Teacher load summary */
    fprintf(fp,"<div class='load-section no-print'>\n"
               "<h2>&#128203; Teacher Load Summary</h2>\n"
               "<table class='lt'><tr>"
               "<th>Teacher</th><th>Rank</th><th>Subject</th>"
               "<th>Load (used / max)</th></tr>\n");
    for (int t=0; t<teacherCount; t++) {
        int pct=(teachers[t].maxLoad>0)
                ?(teachers[t].currentLoad*100/teachers[t].maxLoad):0;
        if (pct>100) pct=100;
        fprintf(fp,"<tr><td><strong>%s</strong></td><td>%s</td><td>%s</td>"
                   "<td>%d / %d"
                   "<div class='bar-bg'><div class='bar-fg' style='width:%d%%'></div></div>"
                   "</td></tr>\n",
                teachers[t].name,
                rankNames[teachers[t].rank],
                teachers[t].subject,
                teachers[t].currentLoad,
                teachers[t].maxLoad,
                pct);
    }
    fprintf(fp,"</table></div>\n");

    fprintf(fp,"</body></html>\n");
    fclose(fp);
    printf("[INFO] routine.html generated\n");
}
