# Floor texture maps from the owner image (task-273-02). Run:
# blender -b --factory-startup --python floor_texture_maps.py -- <dir> <source image name>
# Writes PNG conversion, D_asis, D, N (DirectX), H (16-bit), 3x3 previews; prints numbers.
import sys, os, numpy as np, bpy
d, src = sys.argv[sys.argv.index('--')+1:][:2]
OUT = 2048; BAKE = 12.0
TARGET = np.array([95, 45, 95]) / 255.0
bpy.context.scene.view_settings.view_transform = 'Raw'

def load(path):
    im = bpy.data.images.load(path); im.colorspace_settings.name = 'Non-Color'
    w, h = im.size; a = np.array(im.pixels[:], dtype=np.float32).reshape(h, w, 4)
    return np.flipud(a[..., :3]).copy()          # top row first, raw values

def save(a, path, bits=8):
    h, w = a.shape[:2]
    rgba = np.ones((h, w, 4), np.float32); c = a if a.ndim == 3 else np.repeat(a[..., None], 3, 2)
    rgba[..., :3] = np.flipud(c)
    im = bpy.data.images.new(os.path.basename(path), w, h, alpha=False, float_buffer=(bits == 16))
    im.colorspace_settings.name = 'Non-Color'; im.pixels.foreach_set(rgba.ravel())
    s = bpy.context.scene.render.image_settings
    s.file_format = 'PNG'; s.color_mode = 'RGB'; s.color_depth = str(bits)
    im.save_render(path, scene=bpy.context.scene)

def q8(a): return np.round(np.clip(a, 0, 1) * 255) / 255
def s2l(c): return np.where(c <= .04045, c/12.92, ((c+.055)/1.055)**2.4)
def l2s(c): return np.where(c <= .0031308, c*12.92, 1.055*np.clip(c, 0, None)**(1/2.4)-.055)
def lum(c): return c @ np.array([.2126, .7152, .0722])

def wrap_step(a):
    a = a*255 if a.max() <= 1.0001 else a
    if a.ndim == 2: a = a[..., None]
    out = []
    for ax in (1, 0):  # x (columns), y (rows)
        wr = np.abs(np.take(a, 0, ax) - np.take(a, -1, ax)).mean()
        inner = np.abs(np.diff(a, axis=ax)).mean()
        out.append((round(float(wr), 2), round(float(inner), 2)))
    return {'x_wrap_vs_inner': out[0], 'y_wrap_vs_inner': out[1]}

def lanczos_w(n_in, n_out, a=3):
    s = n_in / n_out; W = np.zeros((n_out, n_in))
    for i in range(n_out):
        c = (i+.5)*s - .5; f = int(np.floor(c))
        for k in range(f-a+1, f+a+1):
            x = k - c; w = np.sinc(x)*np.sinc(x/a) if abs(x) < a else 0.0
            W[i, k % n_in] += w
        W[i] /= W[i].sum()
    return W.astype(np.float32)

def blur(a, sigma):
    h, w = a.shape; fy = np.fft.fftfreq(h)[:, None]; fx = np.fft.fftfreq(w)[None, :]
    k = np.exp(-2*(np.pi*sigma)**2*(fx**2+fy**2))
    return np.fft.ifft2(np.fft.fft2(a)*k).real.astype(np.float32)

def stats(c):
    L = lum(c)
    return {'mean_srgb255': [round(float(x), 1) for x in c.reshape(-1, 3).mean(0)*255],
            'mean_lum255': round(float(L.mean()*255), 1),
            'p1_lum255': round(float(np.percentile(L, 1)*255), 1),
            'p99_lum255': round(float(np.percentile(L, 99)*255), 1)}

def preview(a, path):
    t = np.tile(a if a.ndim == 3 else a[..., None], (3, 3, 1))
    h, w, c = t.shape; t = t.reshape(h//4, 4, w//4, 4, c).mean((1, 3))
    save(t if c == 3 else t[..., 0], path)

# 1 source png
S = load(os.path.join(d, src)); print('source', S.shape)
save(q8(S), os.path.join(d, os.path.splitext(src)[0] + '.png'))
print('SEAM source', wrap_step(S))
# 2b wrap-step correction: spread the border mismatch d over BW px each side (cosine ramp), x then y
BW = 16
def fix(a, ax):
    a = np.moveaxis(a, ax, 1).copy()           # work on columns
    d = a[:, 0] - a[:, -1]                      # mismatch across the wrap
    r = 0.5*(1+np.cos(np.pi*(np.arange(BW)+.5)/BW))   # 1 at the border -> 0 inside
    a[:, :BW] -= (d/2)[:, None, :]*r[None, :, None]
    a[:, -BW:] += (d/2)[:, None, :]*r[::-1][None, :, None]
    return np.moveaxis(a, 1, ax)
S = np.clip(fix(fix(S, 1), 0), 0, 1)
print("SEAM source after correction", wrap_step(S))
# 3 resample, wrap-aware (circular taps)
Wy = lanczos_w(S.shape[0], OUT); Wx = lanczos_w(S.shape[1], OUT)
R = np.stack([Wy @ S[..., c] @ Wx.T for c in range(3)], -1)
R = np.clip(R, 0, 1)
A = q8(R)
# 4 albedo
save(A, os.path.join(d, 'T_Floor_Plates_D_asis.png')); print('ASIS', stats(A), wrap_step(A))
lin = s2l(A); L0 = lin @ np.array([.2126, .7152, .0722])
SAT = 0.80   # saturation x0.8 in linear
lin = L0[..., None] + SAT*(lin - L0[..., None])
for c in range(3):                      # per-channel linear gain so mean sRGB hits target
    lo, hi = 1e-4, 4.0
    for _ in range(40):
        g = (lo+hi)/2
        m = l2s(np.clip(lin[..., c]*g, 0, 1)).mean()
        lo, hi = (g, hi) if m < TARGET[c] else (lo, g)
    lin[..., c] *= g; print('gain', c, round(g, 4))
D = q8(l2s(np.clip(lin, 0, 1)))
save(D, os.path.join(d, 'T_Floor_Plates_D.png')); print('D', stats(D), wrap_step(D))
# 5 height + normal
L = lum(A)
low = blur(L, 20); low = (low - low.mean())/ (low.std()+1e-6)
loc = blur(L, 8)
crack = np.clip(loc - L, 0, None); crack = np.clip(crack/np.percentile(crack, 99.5), 0, 1)
rim = np.clip(L - loc, 0, None); rim = np.clip(rim/np.percentile(rim, 99.5), 0, 1)
crack = blur(crack, 1.0); rim = blur(rim, 1.5)
H = np.clip(0.5 + 0.04*low - 0.38*crack + 0.12*rim, 0, 1)
H16 = np.round(H*65535)/65535
save(H16.astype(np.float32), os.path.join(d, 'T_Floor_Plates_H.png'), bits=16)
dx = (np.roll(H16, -1, 1) - np.roll(H16, 1, 1))/2
dr = (np.roll(H16, -1, 0) - np.roll(H16, 1, 0))/2     # rows grow downward
n = np.stack([-dx*BAKE, -(dr*BAKE), np.ones_like(H16)], -1)   # DirectX: green = -dH/drow... see below
n /= np.linalg.norm(n, axis=-1, keepdims=True)
N = q8(n*.5+.5)
save(N, os.path.join(d, 'T_Floor_Plates_N.png'))
Nd = N*2-1
print('N mean bytes', (N.reshape(-1, 3).mean(0)*255).round(1), 'len min/max',
      np.linalg.norm(Nd, axis=-1).min().round(3), np.linalg.norm(Nd, axis=-1).max().round(3),
      'wrap', wrap_step(N), 'Hwrap', wrap_step(H16), 'Hmean', H16.mean().round(3))
preview(D, os.path.join(d, 'T_Floor_Plates_D_tiled3x3.png'))
preview(N, os.path.join(d, 'T_Floor_Plates_N_tiled3x3.png'))
