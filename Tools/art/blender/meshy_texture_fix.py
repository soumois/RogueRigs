"""task-155-01: two texture corrections on the optimised Meshy chaser. Geometry and UV are never touched. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_texture_fix.py -- --in-fbx <chaser_meshy_opt.fbx> --in-tex <basecolor.png> --logo <T_ChaserLogo.png> --out-dir Art/Meshes/chaser-meshy [--work <short dir>] [--no-render]
Fix 1: the faces of the shoulder-to-torso joints (both sides) whose texture is yellow or mixed and whose mesh neighbours are dark are painted with the dark grey sampled
       from the fully dark faces of the same joints, with a 2 px dilation into texels owned by no other face.
Fix 2: the five faces of the black plate on top of the hood get the company emblem (yellow rock outline + yellow KV on black), redrawn from T_ChaserLogo.png at 1024 px,
       area-downscaled and mapped texel by texel through the 3D position of each texel (4x4 supersampling).
Writes chaser_meshy_fixed_basecolor.png, chaser_meshy_fixed.fbx, logo_source_1024.png, report.json, renders/*.png in --out-dir.
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, sys, os, json, math, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix
from collections import defaultdict

T0 = time.time()
TIM = {}


def tick(name, t):
    TIM[name] = round(time.time() - t, 2)


argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
FL = {"--no-render": "--no-render" in argv}
argv = [a for a in argv if a != "--no-render"]
opt = {"--in-fbx": "", "--in-tex": "", "--logo": "", "--out-dir": "", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--in-tex", "--logo", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
os.makedirs(OUT, exist_ok=True)
os.makedirs(WORK, exist_ok=True)
WOUT = os.path.join(WORK, "out")
os.makedirs(os.path.join(WOUT, "renders"), exist_ok=True)


def stage(p):
    """copy an input to the short work dir (Blender fails on very long Windows paths)"""
    d = os.path.join(WORK, "in_" + os.path.basename(p))
    shutil.copyfile(p, d)
    return d


IN_FBX, IN_TEX, IN_LOGO = stage(opt["--in-fbx"]), stage(opt["--in-tex"]), stage(opt["--logo"])
W = H = 2048
YELLOW_DEF = (248, 200, 40)
BLACK = (10, 10, 10)

# ---------------------------------------------------------------- generic helpers


def load_png(path):
    img = bpy.data.images.load(path, check_existing=False)
    img.colorspace_settings.name = "Non-Color"
    w, h = img.size
    a = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)
    bpy.data.images.remove(img)
    return a


def save_png(path, arr):
    h, w = arr.shape[:2]
    img = bpy.data.images.new("o_" + os.path.basename(path), w, h, alpha=False)
    img.colorspace_settings.name = "Non-Color"
    a = np.ones((h, w, 4), np.float32)
    a[..., :arr.shape[2]] = arr
    img.pixels = a.ravel()
    img.filepath_raw = path
    img.file_format = "PNG"
    img.save()
    bpy.data.images.remove(img)


def to8(a):
    return np.clip(np.round(a[..., :3] * 255.0), 0, 255).astype(np.int16)


def raster(uvs, w, h, pad=0.0):
    """per face (ys, xs) of the texels whose centre lies inside the UV triangle (y=0 is v=0)"""
    out = []
    for t in uvs:
        p = t * np.array([w, h])
        x0 = max(int(np.floor(p[:, 0].min())) - 1, 0)
        x1 = min(int(np.ceil(p[:, 0].max())) + 1, w - 1)
        y0 = max(int(np.floor(p[:, 1].min())) - 1, 0)
        y1 = min(int(np.ceil(p[:, 1].max())) + 1, h - 1)
        xs, ys = np.meshgrid(np.arange(x0, x1 + 1), np.arange(y0, y1 + 1))
        c = np.stack([xs.ravel() + 0.5, ys.ravel() + 0.5], 1)
        a, b, d = p
        v0, v1, v2 = b - a, d - a, c - a
        den = v0[0] * v1[1] - v1[0] * v0[1]
        if abs(den) < 1e-12:
            out.append((np.zeros(0, int), np.zeros(0, int)))
            continue
        u = (v2[:, 0] * v1[1] - v1[0] * v2[:, 1]) / den
        v = (v0[0] * v2[:, 1] - v2[:, 0] * v0[1]) / den
        ins = (u >= -pad) & (v >= -pad) & (u + v <= 1 + pad)
        out.append((ys.ravel()[ins], xs.ravel()[ins]))
    return out


def import_fbx(path, tex=None):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    ob = [o for o in bpy.data.objects if o.type == "MESH"][0]
    if tex:
        img = bpy.data.images.load(tex, check_existing=False)
        img.colorspace_settings.name = "sRGB"
        for n in ob.data.materials[0].node_tree.nodes:
            if n.type == "TEX_IMAGE" and n.name == "Image Texture":
                n.image = img
    return ob


def mesh_data(ob):
    m = ob.data
    uv = m.uv_layers[0].data
    nf = len(m.polygons)
    cen = np.zeros((nf, 3))
    nor = np.zeros((nf, 3))
    area = np.zeros(nf)
    uvs = np.zeros((nf, 3, 2))
    vidx = np.zeros((nf, 3), int)
    for p in m.polygons:
        assert p.loop_total == 3
        cen[p.index] = p.center
        nor[p.index] = p.normal
        area[p.index] = p.area
        vidx[p.index] = [m.loops[li].vertex_index for li in p.loop_indices]
        for k, li in enumerate(p.loop_indices):
            uvs[p.index, k] = uv[li].uv
    ef = defaultdict(list)
    for p in m.polygons:
        for k in p.edge_keys:
            ef[k].append(p.index)
    nb = -np.ones((nf, 3), int)
    for p in m.polygons:
        for j, k in enumerate(p.edge_keys):
            o = [f for f in ef[k] if f != p.index]
            if o:
                nb[p.index, j] = o[0]
    verts = np.array([tuple(v.co) for v in m.vertices])
    return dict(cen=cen, nor=nor, area=area, uvs=uvs, vidx=vidx, nb=nb, verts=verts, nv=len(m.vertices), nf=nf)


# ---------------------------------------------------------------- inputs
t = time.time()
ob_in = import_fbx(IN_FBX)
assert ob_in.matrix_world == Matrix.Identity(4) or True
MD = mesh_data(ob_in)
TEX = load_png(IN_TEX)
assert TEX.shape[0] == H and TEX.shape[1] == W
RGB = to8(TEX)
UVS = MD["uvs"]
NF = MD["nf"]
tick("load", t)

t = time.time()
RAST = raster(UVS, W, H)
OWN = np.zeros((H, W), np.int32)          # number of faces whose triangle covers a texel
for ys, xs in RAST:
    np.add.at(OWN, (ys, xs), 1)


def face_frac(rast):
    fr = np.zeros((NF, 2))
    for i, (ys, xs) in enumerate(rast):
        c = RGB[ys, xs]
        R, G, B = c[:, 0], c[:, 1], c[:, 2]
        dark = (R < 90) & (G < 90) & (B < 90)
        yel = (R > 170) & (G > 130) & (B < 120) & (R - B > 100)
        fr[i] = (dark.mean(), yel.mean())
    return fr


FR = face_frac(RAST)
DARK = FR[:, 0] > 0.9      # dark faces, for the neighbour test
PURE = FR[:, 0] > 0.995      # faces with no visible non-dark texel
MIXED = (FR[:, 0] > 0.02) & ~PURE
NDK = lambda m: np.array([sum(1 for j in range(3) if MD["nb"][i, j] >= 0 and m[MD["nb"][i, j]]) for i in range(NF)])
tick("raster", t)

# ---------------------------------------------------------------- fix 1: shoulder-to-torso joints
t = time.time()
cen = MD["cen"]
ZONE = {}
SEL = {}
for s, name in ((-1, "L(-x)"), (1, "R(+x)")):
    z = (s * cen[:, 0] > 0.185) & (s * cen[:, 0] < 0.335) & (cen[:, 1] > -0.05) & (cen[:, 1] < 0.27) & (cen[:, 2] > 0.5) & (cen[:, 2] < 0.9)
    nd = NDK(DARK)
    A = z & ~PURE & ((((FR[:, 1] > 0.02) | MIXED) & (nd >= 2)) | (MIXED & (nd >= 1)))
    for it in range(8):      # yellow faces enclosed by dark or already selected faces
        inn = DARK | A
        n2 = NDK(inn)
        add = z & ~PURE & ~A & (n2 >= 2)
        if not add.any():
            break
        A = A | add
    ZONE[name] = z
    SEL[name] = A
SEL_ALL = np.zeros(NF, bool)
for a in SEL.values():
    SEL_ALL |= a
zone_all = ZONE["L(-x)"] | ZONE["R(+x)"]
dk_zone = zone_all & (FR[:, 0] > 0.95)
dk_tex = np.concatenate([RGB[RAST[i][0], RAST[i][1]] for i in np.where(dk_zone)[0]])
DARK_RGB = tuple(int(v) for v in np.median(dk_tex, axis=0))

mask1 = np.zeros((H, W), bool)
for i in np.where(SEL_ALL)[0]:
    mask1[RAST[i][0], RAST[i][1]] = True
core1 = int(mask1.sum())
others = np.zeros((H, W), bool)
for i in range(NF):
    if not SEL_ALL[i]:
        others[RAST[i][0], RAST[i][1]] = True
shared1 = int((mask1 & others).sum())      # texels of a selected face also covered by a non-selected face
grow = mask1.copy()
for _ in range(2):      # 2 px dilation (8-neighbourhood) into texels owned by no other face
    g = grow.copy()
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            g |= np.roll(np.roll(grow, dy, 0), dx, 1)
    grow = (g & ~others) | mask1
mask1 = grow
dil1 = int(mask1.sum()) - core1
tick("fix1_select", t)

# ---------------------------------------------------------------- fix 2: emblem


def resample_matrix(n_out, start, length, n_src, kernel="cubic"):
    """rows: weights over n_src source samples for each of n_out outputs covering [start, start+length) in source pixel units (centres at +0.5)"""
    pos = start + (np.arange(n_out) + 0.5) * (length / n_out) - 0.5
    M = np.zeros((n_out, n_src), np.float64)
    i0 = np.floor(pos).astype(int)
    f = pos - i0
    for k in (-1, 0, 1, 2):
        x = np.abs(k - f)
        w = np.where(x < 1, 1.5 * x ** 3 - 2.5 * x ** 2 + 1, np.where(x < 2, -0.5 * x ** 3 + 2.5 * x ** 2 - 4 * x + 2, 0.0))      # Catmull-Rom
        idx = np.clip(i0 + k, 0, n_src - 1)
        for r in range(n_out):
            M[r, idx[r]] += w[r]
    return M


def area_matrix(n_out, n_src):
    M = np.zeros((n_out, n_src))
    sc = n_src / n_out
    for r in range(n_out):
        a, b = r * sc, (r + 1) * sc
        for j in range(int(np.floor(a)), min(int(np.ceil(b)), n_src)):
            M[r, j] = max(0.0, min(b, j + 1) - max(a, j))
        M[r] /= M[r].sum()
    return M


def label_components(mask):
    h, w = mask.shape
    lab = np.zeros((h, w), np.int32)
    n = 0
    for y in range(h):
        for x in range(w):
            if mask[y, x] and lab[y, x] == 0:
                n += 1
                st = [(y, x)]
                lab[y, x] = n
                while st:
                    cy, cx = st.pop()
                    for ny, nx in ((cy + 1, cx), (cy - 1, cx), (cy, cx + 1), (cy, cx - 1)):
                        if 0 <= ny < h and 0 <= nx < w and mask[ny, nx] and lab[ny, nx] == 0:
                            lab[ny, nx] = n
                            st.append((ny, nx))
    return lab, n


def disk_shift(mask, r, op):
    """binary erosion (op='and') or dilation (op='or') by a disc of radius r px"""
    out = mask.copy()
    ri = int(math.ceil(r))
    for dy in range(-ri, ri + 1):
        for dx in range(-ri, ri + 1):
            if dy * dy + dx * dx <= r * r and (dy or dx):
                sh = np.roll(np.roll(mask, dy, 0), dx, 1)
                out = (out & sh) if op == "and" else (out | sh)
    return out


def blur(a, sigma):
    r = int(math.ceil(sigma * 3))
    k = np.exp(-0.5 * (np.arange(-r, r + 1) / sigma) ** 2)
    k /= k.sum()
    for ax in (0, 1):
        pad = np.pad(a, [(r, r) if i == ax else (0, 0) for i in range(2)], mode="edge")
        a = sum(k[j] * np.take(pad, np.arange(j, j + a.shape[ax]), axis=ax) for j in range(len(k)))
    return a


def build_emblem(logo_path, yellow, black, size=1024, stroke=0.05, rock_radius=0.46):
    """yellow rock outline + yellow KV on a black square, size px. The rock is centred and its bounding circle has radius rock_radius * size."""
    L = load_png(logo_path)
    lum = L[..., :3].mean(2)[::-1].copy()      # row 0 = top of the image
    h, w = lum.shape
    fr_ = 8      # the source has a dark frame along its border: replaced by the background yellow
    yl = float(np.median(lum[fr_:fr_ + 6, w // 2 - 20:w // 2 + 20]))
    lum[:fr_], lum[-fr_:], lum[:, :fr_], lum[:, -fr_:] = yl, yl, yl, yl
    dark = lum < 0.38
    # background = yellowish area connected to the border
    lab, n = label_components(~dark)
    border = set(np.unique(np.concatenate([lab[0], lab[-1], lab[:, 0], lab[:, -1]]))) - {0}
    bg = np.isin(lab, list(border))
    rock = ~bg
    ys, xs = np.where(rock)
    cx, cy = (xs.min() + xs.max() + 1) / 2.0, (ys.min() + ys.max() + 1) / 2.0
    r_rock = max(np.hypot(xs - cx + 0.5, ys - cy + 0.5).max(), 1)      # bounding circle about the bbox centre
    half = r_rock / (2 * rock_radius)      # source px covered by half the output square (the rock circle has radius rock_radius * size)
    # letters: large yellow components inside the rock (K and V); thin cracks are dropped
    inner = rock & ~dark
    lab2, n2 = label_components(inner)
    areas = np.bincount(lab2.ravel())
    keep = [i for i in range(1, n2 + 1) if areas[i] >= 150]
    letters = np.isin(lab2, keep)
    Mx = resample_matrix(size, cx - half, 2 * half, w)
    My = resample_matrix(size, cy - half, 2 * half, h)
    up = lambda a: np.clip(My @ a @ Mx.T, 0.0, 1.0)
    lum_up = up(lum)
    dark_a = np.clip((0.38 - lum_up) / 0.24 + 0.5, 0, 1)
    holes_up = disk_shift(up((rock & ~dark).astype(np.float64)) > 0.5, 4, "or").astype(np.float64)      # dilated so no seam is left between the rock and the letters
    rock_a = np.maximum(dark_a, holes_up)
    let_up = disk_shift(up(letters.astype(np.float64)) > 0.5, 9, "or")
    let_a = (1 - dark_a) * let_up
    er = disk_shift(rock_a > 0.5, stroke * size, "and")
    outline = np.clip(rock_a - blur(er.astype(np.float64), 1.2), 0, 1)
    a = np.maximum(outline, let_a)
    img = np.zeros((size, size, 3))
    for c in range(3):
        img[..., c] = black[c] + (yellow[c] - black[c]) * a
    info = dict(src_rock_bbox_px=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())], src_rock_radius_px=round(float(r_rock), 1),
                letters_components=len(keep), letters_areas_px=[int(areas[i]) for i in keep], dropped_components=int(n2 - len(keep)),
                stroke_px_1024=round(stroke * size, 1))
    return np.clip(np.round(img), 0, 255).astype(np.uint8), info


t = time.time()
yel_faces = np.where(FR[:, 1] > 0.97)[0]
yt = np.concatenate([RGB[RAST[i][0], RAST[i][1]] for i in yel_faces[:400]])
YELLOW = tuple(int(v) for v in np.median(yt, axis=0))
EMB, EMB_INFO = build_emblem(IN_LOGO, YELLOW, BLACK)
save_png(os.path.join(WOUT, "logo_source_1024.png"), EMB[::-1].astype(np.float32) / 255.0)      # EMB rows run top to bottom, save_png wants bottom to top
tick("emblem", t)

t = time.time()
# plate faces: top front of the hood, by 3D position, then those whose texture is dark
box = (np.abs(cen[:, 0]) < 0.07) & (cen[:, 1] > -0.22) & (cen[:, 1] < -0.09) & (cen[:, 2] > 0.77)
PLATE = np.where(box & (FR[:, 0] > 0.3))[0]
V = MD["verts"]
vi = MD["vidx"]
nrm = sum(MD["nor"][i] * MD["area"][i] for i in PLATE)
nrm = nrm / np.linalg.norm(nrm)
Yv = np.array([0.0, 1.0, 0.0])
u_ax = Yv - Yv.dot(nrm) * nrm
u_ax /= np.linalg.norm(u_ax)
r_ax = np.cross(u_ax, nrm)
pverts = sorted(set(vi[PLATE].ravel().tolist()))
C0 = V[pverts].mean(0)
pl = lambda P: np.stack([(P - C0) @ r_ax, (P - C0) @ u_ax], -1)
poly_pts = pl(V[pverts])
edge_count = defaultdict(int)
for i in PLATE:
    for k in range(3):
        a_, b_ = vi[i][k], vi[i][(k + 1) % 3]
        edge_count[tuple(sorted((a_, b_)))] += 1
bedges = [e for e, c in edge_count.items() if c == 1]
seg = np.array([[pl(V[a_]), pl(V[b_])] for a_, b_ in bedges])      # (n,2,2)


def in_poly(pts):
    inside = np.zeros(len(pts), bool)
    for (x1, y1), (x2, y2) in seg:
        cond = ((y1 > pts[:, 1]) != (y2 > pts[:, 1])) & (pts[:, 0] < (x2 - x1) * (pts[:, 1] - y1) / (y2 - y1 + 1e-30) + x1)
        inside ^= cond
    return inside


def dist_seg(pts):
    d = np.full(len(pts), 1e9)
    for (p, q) in seg:
        pq = q - p
        tt = np.clip(((pts - p) @ pq) / (pq @ pq), 0, 1)
        d = np.minimum(d, np.linalg.norm(pts - (p + tt[:, None] * pq), axis=1))
    return d


gx, gy = np.meshgrid(np.linspace(poly_pts[:, 0].min(), poly_pts[:, 0].max(), 241), np.linspace(poly_pts[:, 1].min(), poly_pts[:, 1].max(), 241))
G = np.stack([gx.ravel(), gy.ravel()], 1)
G = G[in_poly(G)]
dg = dist_seg(G)
ib = int(np.argmax(dg))
P0 = G[ib]
R_IN = float(dg[ib])
S_EMB = 2 * R_IN      # emblem square side in metres (rock radius = 0.46 * S = 0.92 R_in)
# texel density in the plate (texels per metre)
def tri_area_px(t):
    p = t * np.array([W, H])
    return 0.5 * abs((p[1, 0] - p[0, 0]) * (p[2, 1] - p[0, 1]) - (p[2, 0] - p[0, 0]) * (p[1, 1] - p[0, 1]))


uv_area_px = sum(tri_area_px(UVS[i]) for i in PLATE)
RHO = math.sqrt(uv_area_px / sum(MD["area"][i] for i in PLATE))
N_EMB = S_EMB * RHO
ND = max(16, int(round(N_EMB * 2)))
Ad = area_matrix(ND, 1024)
EMB_DS = np.stack([Ad @ EMB[..., c].astype(np.float64) @ Ad.T for c in range(3)], -1)      # area-filter downscale (to twice the final size)


def sample_bilinear(img, s, tt):
    """s right, tt up in [0,1] of the emblem square; black outside"""
    n = img.shape[0]
    fx = s * n - 0.5
    fy = (1 - tt) * n - 0.5
    x0 = np.floor(fx).astype(int)
    y0 = np.floor(fy).astype(int)
    ax, ay = fx - x0, fy - y0
    out = np.zeros(s.shape + (3,))
    for dy, wy in ((0, 1 - ay), (1, ay)):
        for dx, wx in ((0, 1 - ax), (1, ax)):
            xx, yy = x0 + dx, y0 + dy
            ok = (xx >= 0) & (xx < n) & (yy >= 0) & (yy < n)
            v = img[np.clip(yy, 0, n - 1), np.clip(xx, 0, n - 1)]
            v = np.where(ok[..., None], v, np.array(BLACK, float))
            out += (wx * wy)[..., None] * v
    return out


mask2_core = np.zeros((H, W), bool)
for i in PLATE:
    mask2_core[RAST[i][0], RAST[i][1]] = True
PLATE_SET = set(PLATE.tolist())
others2 = np.zeros((H, W), bool)
for i in range(NF):
    if i not in PLATE_SET:
        others2[RAST[i][0], RAST[i][1]] = True
shared2 = int((mask2_core & others2).sum())
shared1_with_plate = int((mask1 & mask2_core).sum())
out_tex = TEX.copy()
SS = 4
sub = (np.arange(SS) + 0.5) / SS
logo_px = 0


def plate_colours(i, ys, xs):
    """emblem colour of the texels (ys, xs) through the 3D position of the plate face i (barycentric extrapolation outside the triangle), 4x4 supersampling"""
    p = UVS[i] * np.array([W, H])
    a_, b_, c_ = p
    den = (b_[0] - a_[0]) * (c_[1] - a_[1]) - (c_[0] - a_[0]) * (b_[1] - a_[1])
    acc = np.zeros((len(ys), 3))
    for sy in sub:
        for sx in sub:
            q = np.stack([xs + sx, ys + sy], 1) - a_
            u = (q[:, 0] * (c_[1] - a_[1]) - (c_[0] - a_[0]) * q[:, 1]) / den
            v = ((b_[0] - a_[0]) * q[:, 1] - q[:, 0] * (b_[1] - a_[1])) / den
            P3 = V[vi[i][0]] + u[:, None] * (V[vi[i][1]] - V[vi[i][0]]) + v[:, None] * (V[vi[i][2]] - V[vi[i][0]])
            ab = pl(P3)
            acc += sample_bilinear(EMB_DS, (ab[:, 0] - P0[0]) / S_EMB + 0.5, (ab[:, 1] - P0[1]) / S_EMB + 0.5)
    return np.clip(np.round(acc / (SS * SS)), 0, 255) / 255.0


for i in PLATE:
    ys, xs = RAST[i]
    if len(ys):
        out_tex[ys, xs, :3] = plate_colours(i, ys, xs)
        logo_px += len(ys)
mask2 = mask2_core.copy()
g = mask2_core.copy()
for _ in range(2):      # 2 px dilation of the plate: black, into texels owned by no other face
    g2 = g.copy()
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            g2 |= np.roll(np.roll(g, dy, 0), dx, 1)
    g = (g2 & ~others2) | mask2_core
ring2 = g & ~mask2_core
ry, rx = np.where(ring2)      # padding texels carry the emblem too (nearest plate face, extrapolated) so the seam between the two islands stays closed
pen = []
for i in PLATE:
    p = UVS[i] * np.array([W, H])
    a_, b_, c_ = p
    den = (b_[0] - a_[0]) * (c_[1] - a_[1]) - (c_[0] - a_[0]) * (b_[1] - a_[1])
    q = np.stack([rx + 0.5, ry + 0.5], 1) - a_
    u = (q[:, 0] * (c_[1] - a_[1]) - (c_[0] - a_[0]) * q[:, 1]) / den
    v = ((b_[0] - a_[0]) * q[:, 1] - q[:, 0] * (b_[1] - a_[1])) / den
    pen.append(np.maximum.reduce([np.zeros_like(u), -u, -v, u + v - 1]))
near = np.argmin(np.array(pen), 0)
for k, i in enumerate(PLATE):
    sel_ = near == k
    if sel_.any():
        out_tex[ry[sel_], rx[sel_], :3] = plate_colours(i, ry[sel_], rx[sel_])
mask2 = g
tick("fix2_logo", t)

# fix 1 painting (after fix 2 so the joints stay independent of the plate)
t = time.time()
out_tex[mask1, :3] = np.array(DARK_RGB) / 255.0
FIXED_PNG = os.path.join(WOUT, "chaser_meshy_fixed_basecolor.png")
save_png(FIXED_PNG, out_tex)
back = load_png(FIXED_PNG)
RGB_F = to8(back)
changed = (RGB_F != RGB).any(2)
allowed = mask1 | mask2
outside = int((changed & ~allowed).sum())
tick("write_png", t)

# ---------------------------------------------------------------- FBX export and geometry check
t = time.time()
ob = import_fbx(IN_FBX)
img = bpy.data.images.load(FIXED_PNG, check_existing=False)
img.colorspace_settings.name = "sRGB"
img.name = "chaser_meshy_fixed_basecolor.png"
img.pack()
for n in ob.data.materials[0].node_tree.nodes:
    if n.type == "TEX_IMAGE" and n.name == "Image Texture":
        n.image = img
bpy.context.scene.unit_settings.scale_length = 1.0
bpy.ops.object.select_all(action="DESELECT")
ob.select_set(True)
FIXED_FBX = os.path.join(WOUT, "chaser_meshy_fixed.fbx")
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_NONE", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="COPY", embed_textures=True)
bpy.ops.export_scene.fbx(filepath=FIXED_FBX, **EXPORT)
ob2 = import_fbx(FIXED_FBX)
MD2 = mesh_data(ob2)
geom = dict(vertices_in=MD["nv"], vertices_out=MD2["nv"], faces_in=NF, faces_out=MD2["nf"],
            max_abs_vertex_diff=float(np.abs(MD["verts"] - MD2["verts"]).max()) if MD["nv"] == MD2["nv"] else None,
            vertex_index_order_equal=bool((MD["vidx"] == MD2["vidx"]).all()) if MD["nf"] == MD2["nf"] else None,
            max_abs_uv_diff=float(np.abs(UVS - MD2["uvs"]).max()) if MD["nf"] == MD2["nf"] else None,
            dimensions_in=[round(x, 5) for x in ob_in.dimensions] if False else None)
emb_img = [n.image for n in ob2.data.materials[0].node_tree.nodes if n.type == "TEX_IMAGE" and n.name == "Image Texture"][0]
emb_name = emb_img.name
emb_px = np.array(emb_img.pixels[:], dtype=np.float32).reshape(emb_img.size[1], emb_img.size[0], 4) if emb_img.size[0] else None
geom["embedded_texture_name"] = emb_name
geom["embedded_texture_size"] = list(emb_img.size)
usf = None
b = open(FIXED_FBX, "rb").read()
j = b.find(b"UnitScaleFactor")
if j >= 0:
    k = b.find(b"D", j + 20)
    usf = struct.unpack("<d", b[k + 1:k + 9])[0]
tick("export", t)

# ---------------------------------------------------------------- renders (EEVEE sun + AO, same lighting as chaser/part_preview.py --lit, scene in metres)


def setup_scene():
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            pass
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    sc.render.resolution_percentage = 100
    try:
        sc.eevee.use_raytracing = True
        sc.eevee.fast_gi_method = "AMBIENT_OCCLUSION_ONLY"
        sc.eevee.fast_gi_distance = 0.06
    except Exception as ex_:
        print("lit: no raytracing/AO", ex_)
    for a_, v_ in (("use_gtao", True), ("gtao_distance", 0.06), ("gtao_factor", 1.4), ("use_soft_shadows", True)):
        if hasattr(sc.eevee, a_):
            setattr(sc.eevee, a_, v_)
    w = bpy.data.worlds.new("W")
    sc.world = w
    w.use_nodes = True
    bg = w.node_tree.nodes["Background"]
    bg.inputs["Color"].default_value = (0.62, 0.62, 0.62, 1)
    bg.inputs["Strength"].default_value = 0.55
    sun = bpy.data.lights.new("sun", "SUN")
    sun.energy = 4.5
    sun.angle = math.radians(14)
    so = bpy.data.objects.new("sun", sun)
    sc.collection.objects.link(so)
    cam = bpy.data.cameras.new("cam")
    cam.type = "ORTHO"
    cam.clip_start, cam.clip_end = 0.01, 100
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    sc.camera = co
    return sc, co, so


def shot(sc, co, so, d, centre, size, path, px, up=(0, 0, 1)):
    """camera on the side d (unit vector from the target to the camera), screen up = projection of `up`, same key light as part_preview --lit"""
    d = Vector(d).normalized()
    fwd = -d
    upv = Vector(up)
    r = fwd.cross(upv).normalized()
    u = r.cross(fwd).normalized()
    co.location = Vector(centre) + d * 10
    co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
    co.data.ortho_scale = size
    bpy.context.view_layer.update()
    key = (fwd + r * 0.75 + Vector((0, 0, -1.1))).normalized()
    so.rotation_euler = key.to_track_quat("-Z", "Y").to_euler()
    sc.render.resolution_x = sc.render.resolution_y = px
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)


LOGO_C = (V[pverts].mean(0)).tolist()
VIEWS = []
for sgn, nm in ((-1, "L"), (1, "R")):
    c = (0.27 * sgn, 0.12, 0.68)
    VIEWS += [("joint%s_front" % nm, (0, -1, 0), c, 0.35, 900, (0, 0, 1)),
              ("joint%s_threequarter" % nm, (0.6 * sgn, -0.6, 0.4), c, 0.35, 900, (0, 0, 1)),
              ("joint%s_back" % nm, (0, 1, 0), c, 0.35, 900, (0, 0, 1))]
VIEWS += [("logo_topdown", (0, 0, 1), LOGO_C, 0.18, 900, (0, 1, 0)),
          ("logo_threequarter", (0.55, -0.6, 0.55), LOGO_C, 0.18, 900, (0, 0, 1)),
          ("logo_onplate", tuple(nrm), LOGO_C, 0.18, 900, tuple(u_ax)),
          ("robot_front", (0, -1, 0), (0, 0, 0), 2.2, 1100, (0, 0, 1)),
          ("robot_side", (1, 0, 0), (0, 0, 0), 2.2, 1100, (0, 0, 1)),
          ("robot_back", (0, 1, 0), (0, 0, 0), 2.2, 1100, (0, 0, 1)),
          ("robot_threequarter", (-0.6, -0.6, 0.45), (0, 0, 0), 2.2, 1100, (0, 0, 1))]


def render_all(fbx, tex, tag, only=None):
    obr = import_fbx(fbx, tex)
    sc, co, so = setup_scene()
    for nm, d, c, size, px, up in VIEWS:
        if only and nm not in only:
            continue
        shot(sc, co, so, d, c, size, os.path.join(WOUT, "renders", "%s_%s.png" % (nm, tag)), px, up)


RENDER_STATS = {}
CONTROL = {}
if not FL["--no-render"]:
    t = time.time()
    render_all(IN_FBX, IN_TEX, "before")
    render_all(FIXED_FBX, None, "after")
    CTRL = ("robot_front", "jointL_front", "logo_topdown")      # control: exported FBX with the ORIGINAL texture vs the input FBX = noise floor of the renderer and shading round trip
    render_all(FIXED_FBX, IN_TEX, "control", CTRL)
    for nm in CTRL:
        pb = load_png(os.path.join(WOUT, "renders", "%s_before.png" % nm))
        pc = load_png(os.path.join(WOUT, "renders", "%s_control.png" % nm))
        d_ = np.abs(pb[..., :3] - pc[..., :3]).max(2) * 255
        CONTROL[nm] = dict(differing_pixels_gt2=int((d_ > 2).sum()), max_diff_8bit=round(float(d_.max()), 1))
        os.remove(os.path.join(WOUT, "renders", "%s_control.png" % nm))
    tick("renders", t)
    t = time.time()
    # side by side before | after, and the pixel difference
    for nm, *_ in VIEWS:
        pb = load_png(os.path.join(WOUT, "renders", "%s_before.png" % nm))
        pa = load_png(os.path.join(WOUT, "renders", "%s_after.png" % nm))
        save_png(os.path.join(WOUT, "renders", "%s_compare.png" % nm), np.concatenate([pb, pa], 1))
        dif = (np.abs(pb[..., :3] - pa[..., :3]).max(2) * 255) > 2
        ys, xs = np.where(dif)
        RENDER_STATS[nm] = dict(differing_pixels=int(dif.sum()), bbox_xyxy=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None, size_px=int(pb.shape[0]))
    tick("compare", t)

# ---------------------------------------------------------------- report and copy
sel_ids = {k: np.where(v)[0].tolist() for k, v in SEL.items()}
unclass = {}
for k, z in ZONE.items():
    rest = np.where(z & ~PURE & ~SEL[k])[0]
    unclass[k] = dict(count=int(len(rest)), mixed_not_selected=[int(i) for i in rest if MIXED[i]],
                      yellow_not_selected=int(sum(1 for i in rest if FR[i, 1] > 0.6)))
plate_px_a = sorted(set(poly_pts[:, 0].round(5).tolist()))
rep = dict(
    inputs=dict(fbx=os.path.basename(opt["--in-fbx"]), texture=os.path.basename(opt["--in-tex"]), logo=os.path.basename(opt["--logo"])),
    outputs=dict(texture="chaser_meshy_fixed_basecolor.png", fbx="chaser_meshy_fixed.fbx", logo_source="logo_source_1024.png", renders_dir="renders"),
    geometry_check=geom,
    export_settings=dict(**{k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, blender=bpy.app.version_string, scene_unit_scale_length=1.0, fbx_unit_scale_factor_in_file=usf,
                         note="coordinates identical to the input FBX (UnitScaleFactor 1.0 = cm as written by Meshy: the mesh is 1.9 units tall, so Unreal imports it 1.9 cm tall unless the import uniform scale is 100 or the mesh is scaled to 190 cm); the Blender-side axes are Z up, front toward -Y"),
    fix1_joints=dict(
        rule="zone x 0.185..0.335 (each side), y -0.05..0.27, z 0.5..0.9 m; dark face = more than 90 percent dark texels (neighbour test), pure dark = more than 99.5 percent; selected = faces that are not pure dark and are mixed or yellow with >=2 dark neighbours, or mixed with >=1 dark neighbour, then yellow faces with >=2 neighbours in dark-or-selected (8 passes max)",
        dark_rgb=list(DARK_RGB), dark_sample_faces=int(dk_zone.sum()), dark_sample_texels=int(len(dk_tex)),
        faces_selected={k: len(v) for k, v in sel_ids.items()}, face_ids=sel_ids,
        texels_in_selected_faces=core1, texels_added_by_2px_dilation=dil1, texels_painted_total=int(mask1.sum()),
        texels_shared_with_non_selected_faces=shared1, zone_faces_not_selected=unclass),
    fix2_logo=dict(
        plate_faces=PLATE.tolist(), plate_islands_uv_bbox_px=[[int(v) for v in (UVS[i] * [W, H]).min(0).tolist() + (UVS[i] * [W, H]).max(0).tolist()] for i in PLATE],
        plate_normal=[round(x, 4) for x in nrm.tolist()], plate_up_axis_logo=[round(x, 4) for x in u_ax.tolist()], plate_right_axis_logo=[round(x, 4) for x in r_ax.tolist()],
        orientation="logo up = +Y (robot back) projected on the plate plane, logo right = +X; read upright in a top-down view where the robot front (-Y) points to the bottom edge of the view",
        plate_pentagon_m=dict(width_a=round(float(np.ptp(poly_pts[:, 0])), 4), height_b=round(float(np.ptp(poly_pts[:, 1])), 4), inscribed_circle_radius=round(R_IN, 4), n_boundary_edges=len(bedges)),
        texel_density_texels_per_m=round(RHO, 1),
        plate_size_px=dict(width=round(float(np.ptp(poly_pts[:, 0])) * RHO, 1), height=round(float(np.ptp(poly_pts[:, 1])) * RHO, 1), inscribed_diameter=round(2 * R_IN * RHO, 1)),
        emblem_square_px=round(N_EMB, 1), rock_diameter_px=round(0.92 * 2 * R_IN * RHO, 1), emblem_downscale=dict(source=1024, area_filter_to=ND, then="4x4 bilinear supersampling per texel"),
        under_96px_across=bool(N_EMB < 96), emblem=EMB_INFO, colours=dict(yellow=list(YELLOW), black=list(BLACK)),
        texels_in_plate_faces=int(mask2_core.sum()), texels_added_by_2px_dilation=int(ring2.sum()), texels_shared_with_other_faces=shared2, texels_shared_with_fix1=shared1_with_plate),
    texels=dict(total=W * H, changed_total=int(changed.sum()), changed_inside_fix1=int((changed & mask1).sum()), changed_inside_fix2=int((changed & mask2).sum()),
                changed_outside_the_two_fixes=outside, allowed_mask_texels=int(allowed.sum())),
    render_pixel_differences_before_after=RENDER_STATS,
    render_control_exported_fbx_with_original_texture_vs_input=CONTROL,
    timings_s=dict(TIM, total=round(time.time() - T0, 2)),
)
with open(os.path.join(WOUT, "report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
for root, _, files in os.walk(WOUT):
    rel = os.path.relpath(root, WOUT)
    os.makedirs(os.path.join(OUT, rel), exist_ok=True)
    for fn in files:
        shutil.copyfile(os.path.join(root, fn), os.path.join(OUT, rel, fn))
print("DONE", json.dumps({k: rep[k] for k in ("texels", "geometry_check")}))
