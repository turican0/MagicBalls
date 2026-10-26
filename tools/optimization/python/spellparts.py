"""Finds the parts of a replay where a spell is on (by the k_* columns of the profiling harness)
and compares the frame times in them with the rest.

Usage: python spellparts.py run.csv [column=k_fair] [threshold=50]
"""
import csv
import statistics
import sys

path = sys.argv[1]
column = sys.argv[2] if len(sys.argv) > 2 else "k_fair"
threshold = float(sys.argv[3]) if len(sys.argv) > 3 else 50
rows = list(csv.DictReader(open(path, newline="")))[60:]


def pct(v, p):
    s = sorted(v)
    return s[min(len(s) - 1, int(len(s) * p))]


# parts: runs of frames with the column over the threshold (gaps under 30 frames joined)
parts = []
for r in rows:
    f = int(r["frame"])
    if float(r[column]) >= threshold:
        if parts and f - parts[-1][1] <= 30:
            parts[-1][1] = f
        else:
            parts.append([f, f])
print(f"{path}: {len(rows)} frames, {len(parts)} parts with {column} >= {threshold}")
inside = set()
for a, b in parts:
    seg = [r for r in rows if a <= int(r["frame"]) <= b]
    inside.update(int(r["frame"]) for r in seg)
    fm = [float(r["frame_ms"]) for r in seg]
    print(f"  frames {a}-{b} ({b - a + 1}): {column} max {max(float(r[column]) for r in seg):.0f}, meteor max "
          f"{max(float(r['k_meteor']) for r in seg):.0f}, frame p50 {statistics.median(fm):.1f} p95 {pct(fm, .95):.1f} "
          f"max {max(fm):.1f}, >33 ms {sum(1 for x in fm if x > 33)}")
KEYS = ["frame_ms", "process", "between", "mbrun", "other_process", "run", "entities_get", "ent_loop", "ent_show_hide",
        "ent_lights", "ent_particles", "ent_fill_pool", "sounds", "draw_calls", "objects", "pooled_active",
        "multimesh_inst", "lights", "k_meteor", "k_fair", "k_fire", "k_smoke"]
for name, sel in (("in the parts", [r for r in rows if int(r["frame"]) in inside]),
                  ("rest", [r for r in rows if int(r["frame"]) not in inside])):
    if not sel:
        continue
    print(f"-- {name}: {len(sel)} frames")
    for k in KEYS:
        if k in sel[0]:
            v = [float(r[k]) for r in sel]
            print(f"   {k:14s} p50 {statistics.median(v):9.2f} p95 {pct(v, .95):9.2f} max {max(v):9.2f}")
