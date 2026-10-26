"""Report of the CSVs of the profiling harness (tools/optimization/godot/profile.gd).

Usage: python profrep.py [--skip N] a.csv [b.csv ...]
One CSV: the average / median / p95 of every column. More CSVs: the columns side by side
(median), the first CSV is the base for the difference.
"""
import csv
import statistics
import sys

ORDER = ["frame_ms", "process", "between", "render_gpu", "mbrun", "other_process",
         "run", "engine", "e_draw_2d", "e_minimap", "e_top_bar", "e_palette", "e_input",
         "e_entities", "e_lighting", "e_objectives", "screen_image",
         "gd_input", "gd_camera", "gd_player", "entities_get", "ent_loop", "ent_multimesh",
         "ent_show_hide", "ent_lights", "ent_particles", "ent_fill_pool", "terrain", "gd_misc",
         "sounds", "gd_fps_print", "gd_engine_post",
         "draw_calls", "primitives", "objects", "nodes", "pooled_active", "multimesh_inst", "lights"]


def load(path, skip):
    with open(path, newline="") as f:
        rows = list(csv.DictReader(f))[skip:]
    cols = {}
    for k in rows[0]:
        if k != "frame":
            cols[k] = [float(r[k]) for r in rows]
    return cols


def pct(v, p):
    s = sorted(v)
    return s[min(len(s) - 1, int(len(s) * p))]


def main():
    args = sys.argv[1:]
    skip = 60
    if args and args[0] == "--skip":
        skip = int(args[1])
        args = args[2:]
    data = [(p, load(p, skip)) for p in args]
    keys = [k for k in ORDER if any(k in d for _, d in data)]
    if len(data) == 1:
        d = data[0][1]
        print(f"{'':16s} {'avg':>10s} {'p50':>10s} {'p95':>10s} {'p99':>10s}   frames {len(d['frame_ms'])}")
        for k in keys:
            if k in d:
                v = d[k]
                print(f"{k:16s} {statistics.mean(v):10.2f} {statistics.median(v):10.2f} {pct(v, .95):10.2f} {pct(v, .99):10.2f}")
        return
    names = [p.rsplit("/", 1)[-1].rsplit("\\", 1)[-1].removesuffix(".csv") for p, _ in data]
    print(f"{'p50':16s}" + "".join(f"{n:>14s}" for n in names))
    for k in keys:
        line = f"{k:16s}"
        base = statistics.median(data[0][1][k]) if k in data[0][1] else None
        for i, (_, d) in enumerate(data):
            if k not in d:
                line += f"{'-':>14s}"
                continue
            m = statistics.median(d[k])
            if i == 0 or base is None:
                line += f"{m:14.2f}"
            else:
                line += f"{m:8.2f}({m - base:+5.1f})" if abs(m) < 1e5 else f"{m:14.3g}"
        print(line)
    print(f"{'avg frame_ms':16s}" + "".join(f"{statistics.mean(d['frame_ms']):14.2f}" for _, d in data))
    if all("render_gpu" in d for _, d in data):
        print(f"{'avg render_gpu':16s}" + "".join(f"{statistics.mean(d['render_gpu']):14.2f}" for _, d in data))


if __name__ == "__main__":
    main()
