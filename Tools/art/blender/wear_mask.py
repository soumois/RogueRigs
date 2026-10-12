"""Shared wear mask (scratch-only): 1024x1024 grayscale, tileable, hard-edged.
White = clean paint, dark grey = thin scratch lines. No blobs, no grime, no gradients.
Run: "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/wear_mask.py
"""
import json, os, sys
import numpy as np
import bpy

SIZE = 1024
SEED = 108
N_SINGLE = 26
N_CLUSTERS = 3      # small clusters of 3 near-parallel lines (blows on a lid)
OUT_DIR = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "Art", "Textures"))
if "--" in sys.argv:
    a = sys.argv[sys.argv.index("--") + 1:]
    if "--outdir" in a:
        OUT_DIR = a[a.index("--outdir") + 1]

def shallow_angle(rng):
    """Mostly shallow (near horizontal) angles, some steeper; radians."""
    if rng.random() < 0.75:
        return np.radians(rng.uniform(-25, 25))
    return np.radians(rng.uniform(25, 155))

def draw_line(m, rng, x, y, ang, length, w, tone):
    """Slightly bent polyline (quadratic bend), stamped as hard discs, wrapped on both axes."""
    bend = rng.uniform(-0.04, 0.04) * length
    n = int(length * 2)
    r = w / 2.0
    ri = int(np.ceil(r))
    offs = [(i, j) for i in range(-ri, ri + 1) for j in range(-ri, ri + 1) if i * i + j * j <= r * r + 0.25]
    dx, dy = np.cos(ang), np.sin(ang)
    for k in range(n + 1):
        t = k / n
        px = x + dx * t * length - dy * bend * 4 * t * (1 - t)
        py = y + dy * t * length + dx * bend * 4 * t * (1 - t)
        for i, j in offs:
            m[int(round(py + j)) % SIZE, int(round(px + i)) % SIZE] = tone

rng = np.random.default_rng(SEED)
img = np.ones((SIZE, SIZE))
lines = 0
for _ in range(N_SINGLE):
    L = SIZE * rng.uniform(0.05, 0.30)
    draw_line(img, rng, rng.uniform(0, SIZE), rng.uniform(0, SIZE), shallow_angle(rng), L,
              int(rng.integers(1, 4)), rng.uniform(0.35, 0.5))
    lines += 1
for _ in range(N_CLUSTERS):
    cx, cy = rng.uniform(0, SIZE, 2)
    base = shallow_angle(rng)
    for _k in range(int(rng.integers(3, 5))):
        L = SIZE * rng.uniform(0.05, 0.18)
        draw_line(img, rng, cx + rng.uniform(-35, 35), cy + rng.uniform(-35, 35),
                  base + np.radians(rng.uniform(-15, 15)), L, int(rng.integers(1, 3)), rng.uniform(0.35, 0.5))
        lines += 1

g8 = np.round(np.clip(img, 0, 1) * 255).astype(np.uint8)
f = g8.astype(np.float32) / 255.0

os.makedirs(OUT_DIR, exist_ok=True)
png = os.path.join(OUT_DIR, "T_WearMask.png")
im = bpy.data.images.new("T_WearMask", SIZE, SIZE, alpha=False, float_buffer=False)
im.colorspace_settings.name = "Non-Color"
rgba = np.empty((SIZE, SIZE, 4), np.float32)
rgba[..., 0] = rgba[..., 1] = rgba[..., 2] = f[::-1]  # bpy origin is bottom-left
rgba[..., 3] = 1.0
im.pixels.foreach_set(rgba.ravel())
im.filepath_raw = png
im.file_format = "PNG"
im.save()

chk = bpy.data.images.load(png)
buf = np.empty(SIZE * SIZE * 4, np.float32); chk.pixels.foreach_get(buf)
gray = buf.reshape(SIZE, SIZE, 4)[::-1, :, :][..., 0]
lr = float(np.mean(np.abs(gray[:, 0] - gray[:, -1])))
tb = float(np.mean(np.abs(gray[0, :] - gray[-1, :])))
inner_lr = float(np.mean(np.abs(gray[:, 1:] - gray[:, :-1])))
inner_tb = float(np.mean(np.abs(gray[1:, :] - gray[:-1, :])))
dark = gray < 0.9
report = {
    "file": "Art/Textures/T_WearMask.png", "width": SIZE, "height": SIZE,
    "channels": "RGB equal (grayscale), 8 bit", "seed": SEED,
    "seeds_note": f"numpy default_rng({SEED}); single stream, fixed order: single lines then clusters",
    "content": "scratch lines only on white; hard-edged, 1 to 3 px wide, slightly bent, wrapped on both axes",
    "line_count": lines, "single_lines": N_SINGLE, "clusters": N_CLUSTERS,
    "line_length_percent_of_width": "5 to 30 (clusters 5 to 18)",
    "line_tone_range": "0.35 to 0.5",
    "unique_grey_levels": int(len(np.unique(g8))),
    "min_grey": round(float(gray.min()), 4),
    "mean_grey": round(float(gray.mean()), 4),
    "dark_fraction": round(float(dark.mean()), 4),
    "tileable_check": {"left_right_mean_abs_diff": round(lr, 5), "top_bottom_mean_abs_diff": round(tb, 5),
                       "adjacent_column_mean_abs_diff_reference": round(inner_lr, 5),
                       "adjacent_row_mean_abs_diff_reference": round(inner_tb, 5),
                       "threshold": 0.02, "pass": bool(lr < 0.02 and tb < 0.02)},
    "applied_to_assets": [],
}
with open(os.path.join(OUT_DIR, "T_WearMask.report.json"), "w") as fh:
    json.dump(report, fh, indent=2)
print(json.dumps(report, indent=2))
