"""
Routine Generator GUI — Fixed & Upgraded
Writes data.txt in the exact format the C parser expects, then launches routine.exe
"""
import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext
import subprocess
import os
import webbrowser

# ── Data stores ─────────────────────────────────────────────────────────────
courses  = []   # "name|TYPE|credit"
sections = []   # "A"
teachers = []   # "name|Rank|subject"


# ── Utilities ────────────────────────────────────────────────────────────────
def course_names():
    return [c.split("|")[0] for c in courses]


def refresh_listbox(lb, data):
    lb.delete(0, tk.END)
    for item in data:
        lb.insert(tk.END, item)


def save_data_txt():
    """Write data.txt exactly matching the C parser's expected format."""
    with open("data.txt", "w") as f:
        f.write("Teachers:\n")
        f.write(",".join(teachers) + "\n\n")
        f.write("Courses:\n")
        f.write(",".join(courses) + "\n\n")
        f.write("Sections:\n")
        f.write(",".join(sections) + "\n")


# ── Step navigation ──────────────────────────────────────────────────────────
def show_step(n):
    frame1.pack_forget()
    frame2.pack_forget()
    frame3.pack_forget()
    if n == 1: frame1.pack(fill="both", expand=True, padx=20, pady=10)
    if n == 2: frame2.pack(fill="both", expand=True, padx=20, pady=10)
    if n == 3: frame3.pack(fill="both", expand=True, padx=20, pady=10)
    update_step_indicator(n)


def update_step_indicator(active):
    for i, lbl in enumerate(step_labels, 1):
        lbl.config(fg="#2563EB" if i == active else "#94A3B8",
                   font=("Segoe UI", 10, "bold" if i == active else "normal"))


# ════════════════════════════════════════════════════════════════════════════
# STEP 1 — Courses & Sections
# ════════════════════════════════════════════════════════════════════════════
def add_course():
    name   = e_cname.get().strip()
    ctype  = v_ctype.get()
    credit = e_credit.get().strip()

    if not name or not credit:
        messagebox.showerror("Missing fields", "Fill Course Name and Credit."); return
    if not credit.isdigit() or int(credit) < 1:
        messagebox.showerror("Invalid credit", "Credit must be a positive integer."); return
    if name.lower() in [c.split("|")[0].lower() for c in courses]:
        messagebox.showerror("Duplicate", f"Course '{name}' already added."); return

    entry = f"{name.lower()}|{ctype}|{credit}"
    courses.append(entry)
    refresh_listbox(lb_courses, courses)
    e_cname.delete(0, tk.END); e_credit.delete(0, tk.END)
    e_cname.focus()


def remove_course():
    sel = lb_courses.curselection()
    if not sel: return
    courses.pop(sel[0]); refresh_listbox(lb_courses, courses)


def add_section():
    name = e_sec.get().strip()
    if not name:
        messagebox.showerror("Missing", "Enter a section name."); return
    if name in sections:
        messagebox.showerror("Duplicate", f"Section '{name}' already added."); return
    sections.append(name)
    refresh_listbox(lb_sections, sections)
    e_sec.delete(0, tk.END); e_sec.focus()


def remove_section():
    sel = lb_sections.curselection()
    if not sel: return
    sections.pop(sel[0]); refresh_listbox(lb_sections, sections)


def go_step2():
    if not courses:
        messagebox.showerror("No courses", "Add at least one course."); return
    if not sections:
        messagebox.showerror("No sections", "Add at least one section."); return
    refresh_subject_menu()
    show_step(2)


# ════════════════════════════════════════════════════════════════════════════
# STEP 2 — Teachers
# ════════════════════════════════════════════════════════════════════════════
def refresh_subject_menu():
    names = course_names()
    if not names: return
    v_subject.set(names[0])
    menu = om_subject["menu"]
    menu.delete(0, "end")
    for n in names:
        menu.add_command(label=n, command=lambda val=n: v_subject.set(val))


def add_teacher():
    name    = e_tname.get().strip()
    rank    = v_rank.get()
    subject = v_subject.get()

    if not name:
        messagebox.showerror("Missing", "Enter teacher name."); return
    if not subject:
        messagebox.showerror("Missing", "Select a subject."); return

    entry = f"{name}|{rank}|{subject}"
    teachers.append(entry)
    refresh_listbox(lb_teachers, teachers)
    e_tname.delete(0, tk.END); e_tname.focus()


def remove_teacher():
    sel = lb_teachers.curselection()
    if not sel: return
    teachers.pop(sel[0]); refresh_listbox(lb_teachers, teachers)


def go_step3():
    if not teachers:
        messagebox.showerror("No teachers", "Add at least one teacher."); return
    # Warn if any course has no teacher
    covered = {t.split("|")[2] for t in teachers}
    missing = [c.split("|")[0] for c in courses if c.split("|")[0] not in covered]
    if missing:
        ans = messagebox.askyesno(
            "Warning",
            f"These courses have no assigned teacher:\n{', '.join(missing)}\n\n"
            "The scheduler will use a fallback teacher.\nContinue?")
        if not ans: return
    show_step(3)
    populate_preview()


# ════════════════════════════════════════════════════════════════════════════
# STEP 3 — Preview & Generate
# ════════════════════════════════════════════════════════════════════════════
def populate_preview():
    txt_preview.config(state="normal")
    txt_preview.delete("1.0", tk.END)

    txt_preview.insert(tk.END, "── COURSES ──────────────────────\n", "heading")
    for c in courses:
        parts = c.split("|")
        txt_preview.insert(tk.END, f"  {parts[0]:15} {parts[1]:8} {parts[2]} cr\n")

    txt_preview.insert(tk.END, "\n── SECTIONS ─────────────────────\n", "heading")
    txt_preview.insert(tk.END, "  " + "  ".join(sections) + "\n")

    txt_preview.insert(tk.END, "\n── TEACHERS ─────────────────────\n", "heading")
    for t in teachers:
        parts = t.split("|")
        txt_preview.insert(tk.END, f"  {parts[0]:20} {parts[1]:12} → {parts[2]}\n")

    txt_preview.config(state="disabled")


def generate():
    try:
        save_data_txt()
    except Exception as e:
        messagebox.showerror("Save Error", str(e)); return

    exe = "routine.exe" if os.name == "nt" else "./routine"
    if not os.path.exists(exe):
        messagebox.showerror(
            "Executable not found",
            f"'{exe}' not found.\n\n"
            "Compile first with:\n"
            "  gcc main.c scheduler.c display.c export_html.c export_teacher.c -o routine.exe")
        return

    try:
        result = subprocess.run(
            [exe],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=15
        )
        stdout = result.stdout.decode("utf-8", errors="replace") if result.stdout else ""
        stderr = result.stderr.decode("utf-8", errors="replace") if result.stderr else ""
        output = stdout + stderr
    except subprocess.TimeoutExpired:
        messagebox.showerror("Timeout", "Scheduler took too long."); return
    except Exception as e:
        messagebox.showerror("Error", str(e)); return

    txt_output.config(state="normal")
    txt_output.delete("1.0", tk.END)
    txt_output.insert(tk.END, output)
    txt_output.config(state="disabled")

    if os.path.exists("routine.html"):
        btn_open_html.config(state="normal")
    if os.path.exists("teacher_routine.html"):
        btn_open_teacher.config(state="normal")
    if os.path.exists("routine.html") or os.path.exists("teacher_routine.html"):
        messagebox.showinfo("Done!", "Routine generated!\n• Section Routine → routine.html\n• Teacher Routine → teacher_routine.html\n\nUse PDF buttons inside each page to save/print.")
    else:
        messagebox.showwarning("Done (no HTML)", "Scheduler ran but routine.html not found.")


def open_html():
    path = os.path.abspath("routine.html")
    webbrowser.open(f"file://{path}")

def open_teacher_html():
    path = os.path.abspath("teacher_routine.html")
    webbrowser.open(f"file://{path}")


# ════════════════════════════════════════════════════════════════════════════
# UI SETUP
# ════════════════════════════════════════════════════════════════════════════
root = tk.Tk()
root.title("Class Routine Generator")
root.geometry("780x600")
root.configure(bg="#F1F5F9")
root.resizable(True, True)

# ── Step indicator ───────────────────────────────────────────────────────────
top_bar = tk.Frame(root, bg="#1E293B", height=50)
top_bar.pack(fill="x")

tk.Label(top_bar, text="Class Routine Generator",
         bg="#1E293B", fg="white",
         font=("Segoe UI", 13, "bold")).pack(side="left", padx=15, pady=10)

steps_frame = tk.Frame(top_bar, bg="#1E293B")
steps_frame.pack(side="right", padx=15)

step_labels = []
for i, label in enumerate(["① Courses & Sections", "② Teachers", "③ Preview & Generate"], 1):
    lbl = tk.Label(steps_frame, text=label, bg="#1E293B", fg="#94A3B8",
                   font=("Segoe UI", 10), cursor="hand2")
    lbl.pack(side="left", padx=10)
    step_labels.append(lbl)


# ── Shared style helpers ──────────────────────────────────────────────────────
def card(parent, **kw):
    f = tk.Frame(parent, bg="white", relief="flat",
                 highlightthickness=1, highlightbackground="#E2E8F0", **kw)
    return f

def lbl(parent, text, bold=False, color="#374151"):
    return tk.Label(parent, text=text, bg="white",
                    fg=color, font=("Segoe UI", 9, "bold" if bold else "normal"))

def entry(parent, width=28):
    e = tk.Entry(parent, width=width, relief="flat",
                 bg="#F8FAFC", fg="#1E293B",
                 highlightthickness=1, highlightbackground="#CBD5E1",
                 font=("Segoe UI", 10))
    return e

def btn(parent, text, cmd, color="#2563EB", fg="white"):
    return tk.Button(parent, text=text, command=cmd,
                     bg=color, fg=fg, relief="flat", cursor="hand2",
                     font=("Segoe UI", 9, "bold"), padx=10, pady=5,
                     activebackground="#1D4ED8", activeforeground="white")

def listbox(parent, height=6):
    lb = tk.Listbox(parent, height=height, relief="flat",
                    bg="#F8FAFC", fg="#1E293B",
                    selectbackground="#BFDBFE", selectforeground="#1E40AF",
                    font=("Segoe UI", 9), highlightthickness=1,
                    highlightbackground="#CBD5E1")
    return lb


# ════════════════════════════════════════════════════════════════════════════
# FRAME 1
# ════════════════════════════════════════════════════════════════════════════
frame1 = tk.Frame(root, bg="#F1F5F9")

# ── Course card ──────────────────────────────────────────────────────────────
c_card = card(frame1)
c_card.grid(row=0, column=0, sticky="nsew", padx=(0,8), pady=4)

lbl(c_card, "Courses", bold=True, color="#1E293B").grid(
    row=0, column=0, columnspan=3, sticky="w", padx=10, pady=(10,4))

lbl(c_card, "Course Name").grid(row=1, column=0, sticky="w", padx=10)
e_cname = entry(c_card, 20)
e_cname.grid(row=1, column=1, padx=6, pady=3)

lbl(c_card, "Type").grid(row=2, column=0, sticky="w", padx=10)
v_ctype = tk.StringVar(value="THEORY")
ttk.Combobox(c_card, textvariable=v_ctype, values=["THEORY", "LAB"],
             state="readonly", width=18).grid(row=2, column=1, padx=6, pady=3)

lbl(c_card, "Credit Hours").grid(row=3, column=0, sticky="w", padx=10)
e_credit = entry(c_card, 20)
e_credit.grid(row=3, column=1, padx=6, pady=3)

bf1 = tk.Frame(c_card, bg="white")
bf1.grid(row=4, column=0, columnspan=3, pady=6, padx=10, sticky="w")
btn(bf1, "+ Add Course", add_course).pack(side="left", padx=(0,6))
btn(bf1, "Remove", remove_course, color="#EF4444").pack(side="left")

lb_courses = listbox(c_card, height=7)
lb_courses.grid(row=5, column=0, columnspan=3, sticky="ew", padx=10, pady=(0,10))

# ── Section card ─────────────────────────────────────────────────────────────
s_card = card(frame1)
s_card.grid(row=0, column=1, sticky="nsew", padx=(8,0), pady=4)

lbl(s_card, "Sections", bold=True, color="#1E293B").grid(
    row=0, column=0, columnspan=2, sticky="w", padx=10, pady=(10,4))

lbl(s_card, "Section Name").grid(row=1, column=0, sticky="w", padx=10)
e_sec = entry(s_card, 20)
e_sec.grid(row=1, column=1, padx=6, pady=3)

bf2 = tk.Frame(s_card, bg="white")
bf2.grid(row=2, column=0, columnspan=2, pady=6, padx=10, sticky="w")
btn(bf2, "+ Add Section", add_section).pack(side="left", padx=(0,6))
btn(bf2, "Remove", remove_section, color="#EF4444").pack(side="left")

lb_sections = listbox(s_card, height=7)
lb_sections.grid(row=3, column=0, columnspan=2, sticky="ew", padx=10)

btn(s_card, "Next → Assign Teachers", go_step2, color="#059669").grid(
    row=4, column=0, columnspan=2, pady=12, padx=10, sticky="ew")

frame1.columnconfigure(0, weight=1)
frame1.columnconfigure(1, weight=1)
frame1.rowconfigure(0, weight=1)


# ════════════════════════════════════════════════════════════════════════════
# FRAME 2
# ════════════════════════════════════════════════════════════════════════════
frame2 = tk.Frame(root, bg="#F1F5F9")

t_card = card(frame2)
t_card.pack(fill="both", expand=True)

lbl(t_card, "Assign Teachers", bold=True, color="#1E293B").grid(
    row=0, column=0, columnspan=4, sticky="w", padx=10, pady=(10,4))

lbl(t_card, "Teacher Name").grid(row=1, column=0, sticky="w", padx=10)
e_tname = entry(t_card, 22)
e_tname.grid(row=1, column=1, padx=6, pady=3)

lbl(t_card, "Rank").grid(row=2, column=0, sticky="w", padx=10)
v_rank = tk.StringVar(value="Lecturer")
ttk.Combobox(t_card, textvariable=v_rank,
             values=["Professor", "Associate", "Assistant", "Lecturer"],
             state="readonly", width=20).grid(row=2, column=1, padx=6, pady=3)

lbl(t_card, "Subject").grid(row=3, column=0, sticky="w", padx=10)
v_subject = tk.StringVar()
om_subject = tk.OptionMenu(t_card, v_subject, "")
om_subject.config(relief="flat", bg="#F8FAFC", width=18)
om_subject.grid(row=3, column=1, padx=6, pady=3, sticky="w")

bf3 = tk.Frame(t_card, bg="white")
bf3.grid(row=4, column=0, columnspan=4, pady=6, padx=10, sticky="w")
btn(bf3, "+ Add Teacher", add_teacher).pack(side="left", padx=(0,6))
btn(bf3, "Remove", remove_teacher, color="#EF4444").pack(side="left")

lb_teachers = listbox(t_card, height=10)
lb_teachers.grid(row=5, column=0, columnspan=4, sticky="ew", padx=10)

nav2 = tk.Frame(t_card, bg="white")
nav2.grid(row=6, column=0, columnspan=4, sticky="ew", padx=10, pady=10)
btn(nav2, "← Back", lambda: show_step(1), color="#64748B").pack(side="left")
btn(nav2, "Next → Preview", go_step3, color="#059669").pack(side="right")

t_card.columnconfigure(1, weight=1)


# ════════════════════════════════════════════════════════════════════════════
# FRAME 3
# ════════════════════════════════════════════════════════════════════════════
frame3 = tk.Frame(root, bg="#F1F5F9")

top3 = tk.Frame(frame3, bg="#F1F5F9")
top3.pack(fill="both", expand=True)

# Left — data preview
pv_card = card(top3)
pv_card.pack(side="left", fill="both", expand=True, padx=(0,6))

lbl(pv_card, "Data Preview", bold=True, color="#1E293B").pack(anchor="w", padx=10, pady=(10,4))
txt_preview = scrolledtext.ScrolledText(pv_card, height=14, width=38,
                                        font=("Consolas", 9),
                                        bg="#F8FAFC", fg="#1E293B",
                                        state="disabled", relief="flat")
txt_preview.pack(fill="both", expand=True, padx=10, pady=(0,10))
txt_preview.tag_configure("heading", foreground="#2563EB", font=("Consolas", 9, "bold"))

# Right — console output
out_card = card(top3)
out_card.pack(side="right", fill="both", expand=True, padx=(6,0))

lbl(out_card, "Scheduler Output", bold=True, color="#1E293B").pack(anchor="w", padx=10, pady=(10,4))
txt_output = scrolledtext.ScrolledText(out_card, height=14, width=38,
                                       font=("Consolas", 9),
                                       bg="#0F172A", fg="#86EFAC",
                                       state="disabled", relief="flat")
txt_output.pack(fill="both", expand=True, padx=10, pady=(0,10))

# Bottom buttons
bot3 = tk.Frame(frame3, bg="#F1F5F9")
bot3.pack(fill="x", pady=8)
btn(bot3, "← Back", lambda: show_step(2), color="#64748B").pack(side="left", padx=6)
btn(bot3, "⚙ Generate Routine", generate, color="#2563EB").pack(side="left", padx=6)
btn_open_html = btn(bot3, "🌐 Section Routine", open_html, color="#059669")
btn_open_html.pack(side="left", padx=6)
btn_open_html.config(state="disabled")
btn_open_teacher = btn(bot3, "👩‍🏫 Teacher Routine", open_teacher_html, color="#7C3AED")
btn_open_teacher.pack(side="left", padx=6)
btn_open_teacher.config(state="disabled")


# ── Start ─────────────────────────────────────────────────────────────────────
show_step(1)
root.mainloop()
