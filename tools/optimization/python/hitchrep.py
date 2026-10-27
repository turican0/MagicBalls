# Summary of a play log with --debug-log --debug-hitches (scenes/hitch_log.gd): the phases of the
# game (engine states, level loading, quit) and the long frames between them, runs of long frames
# of the same kind merged.
#   python hitchrep.py <stderr or debug.log>
import re
import sys

PHASE = re.compile(r"engine thread: (\w+)|level loading|level loaded|exit game.*|REMC2EndGame.*|MBEX freed|"
                   r"extension uninitialized|hitch log freed.*|window close requested|end: .*|engine run result \d+")
HITCH = re.compile(r"\[mb\s+([\d.]+) \w+\] HITCH frame (\d+) ([\d.]+) ms: process ([\d.]+), draw ([\d.]+) \| (.*?) \| "
                   r"engine (.*?) \| pipelines (\d+), nodes ([+-]\d+) \| (.*)")
SKIP = {"IN_GAME_LOOP", "FADEIN_FADEOUT_LOOP", "INTRO_BEGIN", "PLAY_INFO_FLV_LOOP"}

run = []  # long frames in a row: [start t, end t, count, max ms, total ms, kinds]


def flush():
    if run:
        t0, t1, n, mx, tot, what, _ = run
        print(f"  {t0:8.2f}-{t1:8.2f} s  {n:3d} long frame(s), longest {mx:7.1f} ms, total {tot:8.1f} ms  {what}")
        run.clear()


for line in open(sys.argv[1], encoding="utf-8", errors="replace"):
    m = HITCH.search(line)
    if m:
        t, ms = float(m.group(1)), float(m.group(3))
        parts, engine, events = m.group(6), m.group(7), m.group(10)
        what = parts.split(",")[0].rsplit(" ", 1)[0] if parts != "-" else "draw/other"
        detail = f"[{parts}] [engine {engine}] pipelines {m.group(8)} nodes {m.group(9)}"
        if events != "-":
            detail += " | " + events[:300]
        kind = re.sub(r"[\d.+-]+", "", parts + engine)  # the parts that took the time, without numbers
        if run and run[6] == kind and t - run[1] < 0.5 and events == "-":
            run[1], run[2], run[3], run[4] = t, run[2] + 1, max(run[3], ms), run[4] + ms
        else:
            flush()
            run.extend([t, t, 1, ms, ms, detail, kind])
        continue
    p = PHASE.search(line)
    if p and not (p.group(1) in SKIP):
        flush()
        t = re.search(r"\[mb\s+([\d.]+)", line)
        print(f"{float(t.group(1)) if t else 0:8.2f} s  == {p.group(0)}")
flush()
