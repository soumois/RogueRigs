"""Shared wear mask: 1024x1024 grayscale, tileable, hard-edged. White = clean paint, dark = worn metal.
Run: "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/wear_mask.py
"""
import json, os, sys
import numpy as np
import bpy

SIZE = 1024
SEED = 106
OUT_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "Art", "Textures")
OUT_DIR = os.path.normpath(OUT_DIR)
if "--" in sys.argv:
    a = sys.argv[sys.argv.index("--") + 1:]
    if "--outdir" in a:
        OUT_DIR = a[a.index("--outdir") + 1]

def periodic_noise(rng, cells, size=SIZE):
    """Value noise on a wrapped lattice (period = cells), smoothstep interpolation."""
    g = rng.random((cells, cells))
    xs = np.arange(size) * cells / size
    i0 = np.floor(xs).astype(int); f = xs - i0
    f = f * f * (3 - 2 * f)
    i1 = (i0 + 1) % cells; i0 = i0 % cells
    top = g[np.ix_(i0, i0)] * (1 - f)[None, :] + g[np.ix_(i0, i1)] * f[None, :]
    bot = g[np.ix_(i1, i0)] * (1 - f)[None, :] + g[np.ix_(i1, i1)] * f[None, :]
    return top * (1 - f)[:, None] + bot * f[:, None]

def fbm(rng, base, octaves=3):
    n = np.zeros((SIZE, SIZE)); amp = 1.0; tot = 0
    for o in range(octaves):
        n += amp * periodic_noise(rng, base * 2 ** o); tot += amp; amp *= 0.5
    return n / tot

def scratches(rng, count):
    m = np.zeros((SIZE, SIZE), bool)
    for _ in range(count):
        x, y = rng.random(2) * SIZE
        ang = rng.random() * np.pi
        length = rng.integers(60, 260)
        w = rng.integers(1, 3)
        dx, dy = np.cos(ang), np.sin(ang)
        for t in range(length):
            px = int(x + dx * t) % SIZE; py = int(y + dy * t) % SIZE
            for k in range(w):
                m[(py + k) % SIZE, px] = True
    return m

rng = np.random.default_rng(SEED)
img = np.ones((SIZE, SIZE))

# grime: large flat-faceted tone patch, very light
grime = fbm(rng, 3, 2)
img[grime < np.quantile(grime, 0.30)] = 0.90

# scuffs: medium grey, hard threshold
scuff = fbm(rng, 5, 3)
img[scuff < np.quantile(scuff, 0.07)] = 0.55

# chips: dark patches, small blobby hard-edged
chip = fbm(rng, 9, 3)
img[chip < np.quantile(chip, 0.07)] = 0.22

# scratches
img[scratches(rng, 28)] = 0.40

img = np.clip(img, 0, 1)
g8 = np.round(img * 255).astype(np.uint8)
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

# verify from saved file
chk = bpy.data.images.load(png)
buf = np.empty(SIZE * SIZE * 4, np.float32); chk.pixels.foreach_get(buf)
r = buf.reshape(SIZE, SIZE, 4)[::-1, :, :]
gray = r[..., 0]
lr = float(np.mean(np.abs(gray[:, 0] - gray[:, -1])))
tb = float(np.mean(np.abs(gray[0, :] - gray[-1, :])))
inner_lr = float(np.mean(np.abs(gray[:, 1:] - gray[:, :-1])))
report = {
    "file": "Art/Textures/T_WearMask.png", "width": SIZE, "height": SIZE,
    "channels": "RGB equal (grayscale), 8 bit", "seed": SEED,
    "seeds_note": "numpy default_rng(106); single stream, fixed order grime, scuff, chip, scratches",
    "layers": {"grime": "tone 0.90 on 30 percent", "scuff": "tone 0.55 on 7 percent",
               "chip": "tone 0.22 on 7 percent", "scratches": "28 lines tone 0.40, wrapped"},
    "mean_grey": round(float(gray.mean()), 4),
    "dark_fraction_below_0.6": round(float((gray < 0.6).mean()), 4),
    "dark_fraction_below_0.5": round(float((gray < 0.5).mean()), 4),
    "tileable_check": {"left_right_mean_abs_diff": round(lr, 5), "top_bottom_mean_abs_diff": round(tb, 5),
                       "adjacent_column_mean_abs_diff_reference": round(inner_lr, 5),
                       "threshold": 0.02, "pass": bool(lr < 0.02 and tb < 0.02)},
    "applied_to_assets": [],
}
with open(os.path.join(OUT_DIR, "T_WearMask.report.json"), "w") as fh:
    json.dump(report, fh, indent=2)
print(json.dumps(report, indent=2))
