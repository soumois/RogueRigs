"""Cut the task-548 shop panel frame into UI sprites (run from the repo root).

Source: Art/Source/shop-panel-frame/20261114-052340-shop-panel-frame-1.png (Kie.ai round 2).
Outputs (Art/Source/shop-panel-frame/):
  T_Shop_PanelFrame.png  484 x 484, 9-slice: corners 210 px (margin 210/484 = 0.4339 on each side),
                         a 64 px plain bar piece between them, dark interior in the middle.
  T_Shop_PanelRivet.png  32 x 32, one round rivet (soft circular mask), placed on the bars.
  T_Shop_PanelClamp.png  side clamp of the left bar (the right one is the same image mirrored).
Magenta FF00FF is keyed to transparent (only magenta-ish pixels; the dark outline keeps its colour).
"""
import numpy as np
from PIL import Image

SRC = 'Art/Source/shop-panel-frame/20261114-052340-shop-panel-frame-1.png'
OUT = 'Art/Source/shop-panel-frame/'

im = np.array(Image.open(SRC).convert('RGB')).astype(np.float32)


def key(rgb):
    r, g, b = rgb[..., 0], rgb[..., 1], rgb[..., 2]
    m = np.minimum(r - g, b - g)
    magentaish = (r > g + 40) & (b > g + 40)
    a = np.where(magentaish, 1.0 - np.clip(m / 230.0, 0, 1), 1.0)
    out = rgb.copy()
    # Un-mix the magenta from edge pixels: the frame outline is near black.
    mix = magentaish[..., None]
    out = np.where(mix, np.minimum(rgb, np.stack([g, g, g], -1)) * 0.5, rgb)
    rgba = np.dstack([out, a * 255.0])
    return np.clip(rgba, 0, 255).astype(np.uint8)


# Frame bounding box (outer corners) and slices.
X0, X1, Y0, Y1 = 16, 1656, 57, 857
C = 210                  # corner size in px (holds bracket, patch plate, weld and inner curve)
EX, EY = 563, 353        # start of a plain 64 px bar piece (between rivets) on top/bottom and left/right
E = 64
cols = [(X0, X0 + C), (EX, EX + E), (X1 - C, X1)]
rows = [(Y0, Y0 + C), (EY, EY + E), (Y1 - C, Y1)]
S = 2 * C + E
frame = np.zeros((S, S, 3), np.float32)
oy = 0
for (ya, yb) in rows:
    ox = 0
    for (xa, xb) in cols:
        frame[oy:oy + (yb - ya), ox:ox + (xb - xa)] = im[ya:yb, xa:xb]
        ox += xb - xa
    oy += yb - ya
Image.fromarray(key(frame)).save(OUT + 'T_Shop_PanelFrame.png')

# Rivet: centre (835.6, 93.7) on the top bar, radius 13 with a 1.5 px soft edge.
cx, cy, R = 835.6, 93.7, 13.0
half = 16
patch = im[int(cy) - half:int(cy) + half, int(cx) - half:int(cx) + half]
yy, xx = np.mgrid[0:2 * half, 0:2 * half] + 0.5
d = np.hypot(xx - (cx - (int(cx) - half)), yy - (cy - (int(cy) - half)))
alpha = np.clip((R + 1.5 - d) / 1.5, 0, 1) * 255
Image.fromarray(np.dstack([patch, alpha]).astype(np.uint8)).save(OUT + 'T_Shop_PanelRivet.png')

# Left side clamp, tight box.
clamp = im[415:494, 16:98]
Image.fromarray(key(clamp)).save(OUT + 'T_Shop_PanelClamp.png')
print('frame', S, 'margin', round(C / S, 4), 'clamp', clamp.shape[1], 'x', clamp.shape[0])
