"""Explosion warning sign texture (task-101-08). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/warning_sign.py -- [--out Art/Meshes/barrel/T_Barrel_Warning.png]
Own drawing, deterministic, no text: 512 x 512 RGBA, transparent background, rounded yellow triangle (#FFD500),
thick rounded black border, black explosion burst (8 irregular spikes of different heights on a jagged base). Hard edges (binary alpha)."""
import bpy, os, sys
import numpy as np

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
OUT = os.path.abspath(argv[argv.index("--out") + 1]) if "--out" in argv else os.path.abspath("Art/Meshes/barrel/T_Barrel_Warning.png")
N = 512
ys, xs = np.mgrid[0:N, 0:N].astype(np.float64); xs += 0.5; ys += 0.5   # row 0 = top, y down
C = np.array([256.0, 300.0]); RIN0 = 130.5                              # incentre, inradius of the full equilateral triangle
CIRC = 2.0 * RIN0
V = [np.array([0.0, -CIRC]), np.array([np.sqrt(3) * RIN0, RIN0]), np.array([-np.sqrt(3) * RIN0, RIN0])]

def seg_dist(px, py, a, b):
    ab = b - a; t = np.clip(((px - a[0]) * ab[0] + (py - a[1]) * ab[1]) / (ab @ ab), 0, 1)
    return np.hypot(px - (a[0] + t * ab[0]), py - (a[1] + t * ab[1]))
def rounded_tri(rho, a):
    """triangle of inradius rho with corners rounded by radius a = Minkowski sum of the (rho - a) triangle and a disc"""
    s = (rho - a) / RIN0
    T = [C + v * s for v in V]
    inside = np.ones((N, N), bool)
    for i in range(3):
        p, q = T[i], T[(i + 1) % 3]
        cr = (q[0] - p[0]) * (ys - p[1]) - (q[1] - p[1]) * (xs - p[0])
        inside &= cr >= 0
    d = np.minimum.reduce([seg_dist(xs, ys, T[i], T[(i + 1) % 3]) for i in range(3)])
    return inside | (d <= a)

outer = rounded_tri(128.0, 34.0)        # yellow plate
black = rounded_tri(114.0, 26.0)        # black border outer edge
inner = rounded_tri(80.0, 10.0)         # yellow field inside the border
# burst polygon (px, y down): own shape, 8 spikes of different heights and widths on a jagged base
BURST = [(196, 378), (172, 324), (208, 336), (203, 262), (229, 320), (236, 222), (251, 301), (262, 250), (271, 309),
         (291, 212), (297, 292), (323, 260), (311, 320), (341, 298), (319, 342), (339, 352), (318, 378),
         (298, 364), (281, 378), (263, 362), (245, 378), (227, 364), (210, 378)]
def poly_mask(pts):
    m = np.zeros((N, N), bool); n = len(pts)
    for i in range(n):
        (x1, y1), (x2, y2) = pts[i], pts[(i + 1) % n]
        if y1 == y2: continue
        cond = ((y1 <= ys) & (ys < y2)) | ((y2 <= ys) & (ys < y1))
        xin = x1 + (ys - y1) * (x2 - x1) / (y2 - y1)
        m ^= cond & (xs < xin)
    return m
burst = poly_mask(BURST) & inner
img = np.zeros((N, N, 4), np.float32)
img[outer] = (1.0, 0.8353, 0.0, 1.0)     # #FFD500 (byte image: pixels are stored as given)
img[black & ~inner] = (0.0, 0.0, 0.0, 1.0)
img[burst] = (0.0, 0.0, 0.0, 1.0)
img[~outer, 3] = 0.0
# the colour of fully transparent pixels stays black, alpha 0 (hard edge, no partial alpha)
im = bpy.data.images.new("T_Barrel_Warning", N, N, alpha=True)
im.pixels = np.ascontiguousarray(img[::-1]).reshape(-1).tolist()
im.filepath_raw = OUT; im.file_format = "PNG"; im.save()
print("wrote", OUT, "burst px", int(burst.sum()), "clipped by inner field", int(poly_mask(BURST).sum() - burst.sum()))
