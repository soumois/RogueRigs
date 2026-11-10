"""task-499-01: robot emblem decal textures from the Kessler-Voss emblem cut-out (variation 4).
Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_emblem_texture.py -- [--src <cutout.png>] [--out-dir Art/Meshes/robot-emblem] [--size 512] [--fill 0.92]
Writes (absolute or repo-relative paths):
  T_RobotEmblem_Dark.png  the full emblem: charcoal faceted rock with the yellow KV and facets, transparent outside the rock
                          (chaser v2 and shield chaser v2: yellow on dark so it reads on the yellow body, as the chaser v1 hood emblem).
  robot_emblem_texture.report.json  sizes, colours, alpha coverage.
The yellow-only emblem of the heavy and the kamikaze is the existing shield emblem T_ChaserShield_Emblem (Art/Meshes/ChaserShield/T_ChaserShield_Emblem.png, 512 px, KV and facets only), not rebuilt here.
Resampling: area (box) filter on premultiplied colour, then un-premultiplied; the rock is centred and scaled so its larger side fills --fill of the square."""
import bpy, sys, os, json
import numpy as np

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--src": "Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4-cutout.png", "--out-dir": "Art/Meshes/robot-emblem", "--size": "512", "--fill": "0.92"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
ab = lambda p: p if os.path.isabs(p) else os.path.join(REPO, p)
SRC, OUT, N, FILL = ab(opt["--src"]), ab(opt["--out-dir"]), int(opt["--size"]), float(opt["--fill"])
os.makedirs(OUT, exist_ok=True)

img = bpy.data.images.load(SRC)
img.colorspace_settings.name = "Non-Color"       # raw sRGB bytes, no conversion
w, h = img.size
px = np.array(img.pixels[:], np.float32).reshape(h, w, 4)[::-1]      # rows top to bottom
a = px[..., 3]
ys, xs = np.where(a > 0.5)
y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
crop = px[y0:y1, x0:x1].copy()
ch, cw = crop.shape[:2]
side = int(round(max(ch, cw) / FILL))
canvas = np.zeros((side, side, 4), np.float32)
oy, ox = (side - ch) // 2, (side - cw) // 2
canvas[oy:oy + ch, ox:ox + cw] = crop
pm = canvas.copy()
pm[..., :3] *= pm[..., 3:4]


def box_resize(arr, n):
    """area average of an (S, S, C) array to (n, n, C)"""
    s = arr.shape[0]
    edges = np.linspace(0, s, n + 1)
    c = np.zeros((s + 1, s + 1, arr.shape[2]), np.float64)
    c[1:, 1:] = arr.cumsum(0).cumsum(1)

    def integ(y, x):        # integral over [0, y) x [0, x) with fractional bounds, bilinear on the summed table
        yi, xi = np.floor(y).astype(int), np.floor(x).astype(int)
        yi1, xi1 = np.minimum(yi + 1, s), np.minimum(xi + 1, s)
        fy, fx = (y - yi)[:, None, None], (x - xi)[None, :, None]
        g = lambda a_, b_: c[a_][:, b_]
        return (g(yi, xi) * (1 - fy) * (1 - fx) + g(yi1, xi) * fy * (1 - fx) + g(yi, xi1) * (1 - fy) * fx + g(yi1, xi1) * fy * fx)

    I = integ(edges, edges)
    area = (s / n) ** 2
    return ((I[1:, 1:] - I[:-1, 1:] - I[1:, :-1] + I[:-1, :-1]) / area).astype(np.float32)


r = box_resize(pm, N)
al = r[..., 3:4]
rgb = np.where(al > 1e-4, r[..., :3] / np.maximum(al, 1e-4), 0.0)
outpx = np.concatenate([np.clip(rgb, 0, 1), np.clip(al, 0, 1)], 2)
o = bpy.data.images.new("T_RobotEmblem_Dark", N, N, alpha=True)
o.colorspace_settings.name = "Non-Color"
o.pixels.foreach_set(outpx[::-1].ravel())
o.filepath_raw = os.path.join(OUT, "T_RobotEmblem_Dark.png")
o.file_format = "PNG"
o.save()

solid = outpx[outpx[..., 3] > 0.99]
yel = solid[(solid[:, 0] > 0.7) & (solid[:, 2] < 0.3)]
dark = solid[(solid[:, 0] < 0.3)]
rep = dict(source=os.path.relpath(SRC, REPO).replace("\\", "/"), source_size=[w, h], rock_bbox_px=[int(x0), int(y0), int(x1), int(y1)], out="T_RobotEmblem_Dark.png", size=N, fill=FILL,
           alpha_coverage=round(float((outpx[..., 3] > 0.5).mean()), 4),
           yellow_mean_srgb8=[int(round(v * 255)) for v in yel[:, :3].mean(0)] if len(yel) else None,
           dark_mean_srgb8=[int(round(v * 255)) for v in dark[:, :3].mean(0)] if len(dark) else None,
           yellow_fraction_of_rock=round(len(yel) / max(1, len(solid)), 4))
json.dump(rep, open(os.path.join(OUT, "robot_emblem_texture.report.json"), "w"), indent=1)
print("EMBLEMTEX", json.dumps(rep))
