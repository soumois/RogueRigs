"""task-282: generic base colour clean-up of a Meshy robot texture. Geometry and UV are only read, never written. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_texture_clean.py -- <config.json> [--work <short dir>]
Config (Tools/art/blender/texture_clean/<robot>.json, paths relative to the repo root): see heavy.json.
Diagnosis: metallic and roughness maps are summarised inside and outside the defect texels, to say where the defect lies.
Treated classes (base colour only):
  blue    texels of the blue panels: flattened to the flat sheet blue (edge texels towards the dark keep their mix, by luminance)
  residue dark/mixed ink texels inside blue faces within residue_radius px of a blotch (the outlines of the blotches): flat blue
  blotch  low-saturation grey/tan texels of mid luminance (the camouflage and glass look): repainted with the flat colour of the panel
          (class of the mesh face they belong to: majority of its non-blotch texels, else a vote of the faces next to it in 3D, welded by position)
          then dilated into texels owned by no face. Blotches on a red (eye) face are left alone.
Every other texel (yellow, black, dark charcoal, red, antialiased mixes) is unchanged; the report proves it.
Needs Blender (numpy included). Nothing is imported in Unreal."""
import bpy, sys, os, json, shutil, hashlib
import numpy as np
from collections import defaultdict

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
CFG_PATH = os.path.abspath(argv[0])
WORK = argv[argv.index("--work") + 1] if "--work" in argv else "C:/Users/soumo/AppData/Local/Temp/rr-t282/work"
os.makedirs(WORK, exist_ok=True)
CFG = json.load(open(CFG_PATH, encoding="utf-8"))
ROOT = os.path.abspath(os.path.join(os.path.dirname(CFG_PATH), "..", "..", "..", ".."))
P = lambda k: os.path.join(ROOT, CFG[k])
RU = CFG["rules"]
BLUE = np.array(CFG["colours"]["blue"], float)
CHAR = np.array(CFG["colours"]["charcoal"], float)


def stage(p):
    d = os.path.join(WORK, "in_" + os.path.basename(p))
    shutil.copyfile(p, d)
    return d


def sha(p):
    return hashlib.sha256(open(p, "rb").read()).hexdigest()


def load_png(path):
    img = bpy.data.images.load(stage(path), check_existing=False)
    img.colorspace_settings.name = "Non-Color"
    w, h = img.size
    a = np.array(img.pixels[:], dtype=np.float32).reshape(h, w, 4)
    bpy.data.images.remove(img)
    return a


def save_png(path, arr):
    h, w = arr.shape[:2]
    img = bpy.data.images.new("o", w, h, alpha=False)
    img.colorspace_settings.name = "Non-Color"
    a = np.ones((h, w, 4), np.float32)
    a[..., :3] = arr[..., :3]
    img.pixels = a.ravel()
    img.filepath_raw = path
    img.file_format = "PNG"
    img.save()
    bpy.data.images.remove(img)


def to8(a):
    return np.clip(np.round(a[..., :3] * 255.0), 0, 255).astype(np.int16)


def raster(uvs, w, h):
    out = []
    for t in uvs:
        p = t * np.array([w, h])
        x0, x1 = max(int(np.floor(p[:, 0].min())) - 1, 0), min(int(np.ceil(p[:, 0].max())) + 1, w - 1)
        y0, y1 = max(int(np.floor(p[:, 1].min())) - 1, 0), min(int(np.ceil(p[:, 1].max())) + 1, h - 1)
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
        ins = (u >= 0) & (v >= 0) & (u + v <= 1)
        out.append((ys.ravel()[ins], xs.ravel()[ins]))
    return out


# ------------------------------------------------------------ mesh (read only)
FBX = P("in_fbx")
FBX_SHA_BEFORE = sha(FBX)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=stage(FBX))
ob = [o for o in bpy.data.objects if o.type == "MESH"][0]
m = ob.data
NF = len(m.polygons)
uvl = m.uv_layers[0].data
UVS = np.zeros((NF, 3, 2))
vco = np.array([tuple(v.co) for v in m.vertices])
scale = max(np.ptp(vco, 0).max(), 1e-9)
key = [tuple(np.round(c / scale * 20000).astype(int)) for c in vco]      # weld by position: neighbours across UV seams
ef = defaultdict(set)
for p in m.polygons:
    assert p.loop_total == 3
    for k, li in enumerate(p.loop_indices):
        UVS[p.index, k] = uvl[li].uv
    vs = [key[m.loops[li].vertex_index] for li in p.loop_indices]
    for j in range(3):
        ef[tuple(sorted((vs[j], vs[(j + 1) % 3])))].add(p.index)
NB = [set() for _ in range(NF)]
for fs in ef.values():
    for a_ in fs:
        NB[a_] |= fs - {a_}

# ------------------------------------------------------------ textures
TEX = load_png(P("in_basecolor"))
H, W = TEX.shape[:2]
RGB = to8(TEX)
R, G, B = RGB[..., 0].astype(float), RGB[..., 1].astype(float), RGB[..., 2].astype(float)
mx, mn = RGB.max(2).astype(float), RGB.min(2).astype(float)
sat = (mx - mn) / np.maximum(mx, 1)
lum = (R + G + B) / 3
yb, rb, bb, kb = RU["yellow"], RU["red"], RU["blue"], RU["blotch"]
RED = (R > rb["r_min"]) & (G < rb["g_max"]) & (B < rb["b_max"])
YEL = (R > yb["r_min"]) & (G > yb["g_min"]) & (B < yb["b_max"]) & (R - B > yb["r_minus_b_min"]) & ~RED
BLU = (B - R > bb["b_minus_r_min"]) & (sat > bb["sat_min"]) & ~RED & ~YEL
if bb.get("mode") == "green":      # optional (shooter): the treated main colour is green instead of blue
    BLU = (G - R > bb["g_minus_r_min"]) & (G - B > bb["g_minus_b_min"]) & (sat > bb["sat_min"]) & ~RED & ~YEL
BLO = (sat < kb["sat_max"]) & (lum >= kb["lum_min"]) & (lum <= kb["lum_max"]) & ~RED & ~YEL & ~BLU
DRK = (mx < RU["dark_max_channel"]) & ~RED & ~YEL & ~BLU & ~BLO
CLS = np.full((H, W), 4, np.int8)      # 0 blue 1 yellow 2 red 3 dark 4 other, 5 blotch
CLS[DRK], CLS[RED], CLS[YEL], CLS[BLU] = 3, 2, 1, 0
CLS[BLO] = 5
NAMES = ["blue", "yellow", "red", "dark", "other"]

RAST = raster(UVS, W, H)

# ------------------------------------------------------------ face classes
FC = np.full(NF, -1)
for i, (ys, xs) in enumerate(RAST):
    c = CLS[ys, xs]
    n = len(c)
    if n == 0:
        continue
    cnt = np.array([(c == k).sum() for k in range(5)])
    if cnt[:4].sum() / n >= RU["face_resolve_min_known_fraction"] and cnt[:4].sum() > 0:
        FC[i] = int(np.argmax(cnt[:4]))
n_direct = int((FC >= 0).sum())
for it in range(60):
    todo = np.where(FC < 0)[0]
    if len(todo) == 0:
        break
    new = {}
    for i in todo:
        votes = [FC[j] for j in NB[i] if FC[j] >= 0 and FC[j] not in CFG.get("vote_exclude_classes", [])]
        if votes:
            new[i] = int(np.bincount(votes, minlength=4).argmax())
    if not new:
        break
    for i, v in new.items():
        FC[i] = v
n_voted = int((FC >= 0).sum()) - n_direct
n_default = int((FC < 0).sum())
FC[FC < 0] = 3
YEL_RGB = np.median(RGB[YEL], 0)
TARGET = {0: BLUE, 1: YEL_RGB, 3: CHAR}

# ------------------------------------------------------------ paint
out = RGB.astype(np.float64).copy()
blue_mask = CLS == 0
al = np.clip((lum - bb["dark_lum"]) / (bb["flatten_full_lum"] - bb["dark_lum"]), 0, 1)
flat_blue = CHAR[None, None, :] + al[..., None] * (BLUE - CHAR)[None, None, :]
out[blue_mask] = flat_blue[blue_mask]

LAB = np.full((H, W), -1, np.int8)      # target class for blotch texels
for i, (ys, xs) in enumerate(RAST):
    sel = CLS[ys, xs] == 5
    if sel.any():
        LAB[ys[sel], xs[sel]] = FC[i]
blot = CLS == 5
for _ in range(RU["dilate_px"]):      # blotch texels owned by no face: take the label of a neighbouring labelled texel
    cand = blot & (LAB < 0)
    if not cand.any():
        break
    best = np.full((H, W), -1, np.int8)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            sh = np.roll(np.roll(LAB, dy, 0), dx, 1)
            best = np.where((best < 0) & (sh >= 0), sh, best)
    LAB = np.where(cand & (best >= 0), best, LAB)
unlabelled = int((blot & (LAB < 0)).sum())

# residue: dark and mixed ink texels (outlines of the former blotches) inside blue faces, within residue_radius px of a blotch -> flat blue
FCT = np.full((H, W), -1, np.int8)      # face class of each owned texel
for i, (ys, xs) in enumerate(RAST):
    FCT[ys, xs] = FC[i]
for _ in range(RU["dilate_px"]):
    cand = FCT < 0
    best = np.full((H, W), -1, np.int8)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            sh = np.roll(np.roll(FCT, dy, 0), dx, 1)
            best = np.where((best < 0) & (sh >= 0), sh, best)
    FCT = np.where(cand & (best >= 0), best, FCT)
RES = CFG.get("residue", {"face_class": 0, "target": "blue", "zone": "blotch"})
zone = (blot | YEL) if RES["zone"] == "blotch_yellow" else blot.copy()
for _ in range(2):      # square dilation by residue_radius px (separable)
    r_ = int(RU["residue_radius"])
    for ax in (0, 1):
        acc = zone.copy()
        for d in range(1, r_ + 1):
            acc |= np.roll(zone, d, ax) | np.roll(zone, -d, ax)
        zone = acc
    break
residue = zone & (FCT == RES["face_class"]) & ~blot & ~YEL & ~RED & np.isin(CLS, RES.get("classes", [3, 4, 0]))

if "full_yellow_face_fraction" in RES:      # faces whose owned texels are mostly yellow: their remaining dark/mixed texels become yellow too (clean tip, no dark smear)
    for i, (ys, xs) in enumerate(RAST):
        if len(ys) and YEL[ys, xs].mean() >= RES["full_yellow_face_fraction"]:
            sel = ~YEL[ys, xs] & ~RED[ys, xs]
            residue[ys[sel], xs[sel]] = True
STAIN = CFG.get("stain")      # optional (shooter): texels of the main colour with a low luminance inside faces of another class (dark green stains in charcoal) -> that class colour
stain = np.zeros((H, W), bool)
if STAIN:
    stain = blue_mask & (FCT == STAIN["face_class"]) & (lum <= STAIN["lum_max"])
painted = {}
for k, nm in ((0, "blue"), (1, "yellow"), (2, "red"), (3, "charcoal")):
    sel = blot & (LAB == k)
    painted[nm] = int(sel.sum())
    if k in TARGET:
        out[sel] = TARGET[k]
out[residue] = YEL_RGB if RES["target"] == "yellow" else BLUE
out[stain] = CHAR
logo = np.zeros((H, W), bool)
for LP in CFG.get("logo_patches", []):      # optional (shooter): muddy logo plate repainted: yellow ring texels -> yellow, red or orange centre texels -> dark; inside a disc (centre x, y from the top)
    yy, xx = np.mgrid[0:H, 0:W]
    disc = ((xx - LP["cx"]) ** 2 + (H - 1 - yy - LP["cy"]) ** 2) <= LP["r"] ** 2
    ye = disc & (R > LP["yellow_r_min"]) & (B < LP["yellow_b_max"]) & (G - R < LP["yellow_g_minus_r_max"])
    ce = disc & (R > 90) & (G < R * 0.8) & (B < 90)
    out[ye] = YEL_RGB
    out[ce] = np.array(LP["centre_rgb"], float)
    logo |= ye | ce
final = np.clip(np.round(out), 0, 255).astype(np.float32) / 255.0

OUT_PNG = P("out_basecolor")
os.makedirs(os.path.dirname(OUT_PNG), exist_ok=True)
save_png(OUT_PNG, final)
back = to8(load_png(OUT_PNG))
changed = (back != RGB).any(2)
treated = blue_mask | blot | residue | logo
outside = int((changed & ~treated).sum())
changed_by = dict(blue_flattened=int((changed & blue_mask & ~residue).sum()), ink_residue_in_blue_faces_to_blue=int((changed & residue).sum()),
                  blotch_to_blue=int((changed & blot & (LAB == 0)).sum()), blotch_to_yellow=int((changed & blot & (LAB == 1)).sum()),
                  blotch_to_charcoal=int((changed & blot & (LAB == 3)).sum()), stain_to_charcoal=int((changed & stain).sum()), logo_repainted=int((changed & logo).sum()), blotch_left_in_red_faces=int((blot & (LAB == 2)).sum()))

# ------------------------------------------------------------ cause: metallic and roughness vs the defect
MET = load_png(P("in_metallic"))[..., 0]
ROU = load_png(P("in_roughness"))[..., 0]


def down(a):
    f = a.shape[0] // H
    return a.reshape(H, f, W, f).mean((1, 3)) if f > 1 else a


MET, ROU = down(MET), down(ROU)
stat = lambda v, mk: dict(n=0) if not mk.any() else dict(mean=round(float(v[mk].mean()), 4), std=round(float(v[mk].std()), 4), p5=round(float(np.percentile(v[mk], 5)), 4), p95=round(float(np.percentile(v[mk], 95)), 4))
cause = dict(
    metallic=dict(all=dict(mean=round(float(MET.mean()), 5), max=round(float(MET.max()), 5)), in_blotch=stat(MET, blot), in_blue=stat(MET, blue_mask)),
    roughness=dict(all=stat(ROU, np.ones((H, W), bool)), in_blotch=stat(ROU, blot), in_blue=stat(ROU, blue_mask), in_dark=stat(ROU, CLS == 3), in_yellow=stat(ROU, CLS == 1)),
    blotch_texels_fraction=round(float(blot.mean()), 4),
)
cause["verdict"] = ("base colour: the grey and tan blotches and the washed blue are painted in the base colour map itself; the metallic map is zero everywhere (max %.4f, nothing to clean) "
                    "and the roughness map is smooth, mean %.2f, nearly the same inside blotches (%.2f) and in blue panels (%.2f), so it does not hold the defect. "
                    "Cleaned metallic and roughness maps are therefore not written (and are unused in game under the owner default: base colour only, constant roughness, metallic 0)."
                    % (float(MET.max()), float(ROU.mean()), float(ROU[blot].mean()), float(ROU[blue_mask].mean())))

if "verdict_template" in CFG:      # per-robot wording: %(met_max)s %(rou_mean)s %(rou_blot)s %(rou_dark)s %(rou_yel)s
    cause["verdict"] = CFG["verdict_template"] % dict(met_max="%.4f" % float(MET.max()), rou_mean="%.2f" % float(ROU.mean()), rou_blot="%.2f" % float(ROU[blot].mean()),
                                                      rou_dark="%.2f" % float(ROU[CLS == 3].mean()), rou_yel="%.2f" % float(ROU[CLS == 1].mean()))

rep = dict(
    robot=CFG["robot"], inputs={k: CFG[k] for k in ("in_fbx", "in_basecolor", "in_metallic", "in_roughness")},
    outputs=dict(basecolor=CFG["out_basecolor"], size_px=[W, H]),
    cause=cause,
    colours_used=dict(blue_rgb=BLUE.astype(int).tolist(), charcoal_rgb=CHAR.astype(int).tolist(), yellow_rgb_median_of_raw_yellow_texels=YEL_RGB.astype(int).tolist(), source=CFG["colour_source"]),
    rules=RU,
    residue_texels=int(residue.sum()),
    texel_classes_raw=dict(total=W * H, **{n: int((CLS == k).sum()) for k, n in enumerate(NAMES)}, blotch=int(blot.sum())),
    face_classes=dict(faces=NF, resolved_directly=n_direct, resolved_by_neighbour_vote=n_voted, defaulted_to_dark=n_default, final_counts={n: int((FC == k).sum()) for k, n in enumerate(NAMES[:4])}),
    texels_changed_per_class=changed_by, blotch_texels_by_target_class=painted, blotch_texels_unlabelled=unlabelled,
    texels_changed_total=int(changed.sum()),
    outside_check=dict(treated_class_texels=int(treated.sum()), texels_outside_treated_classes=int((~treated).sum()), changed_outside_treated_classes=outside,
                       note="8-bit read back of the written PNG compared with the raw PNG"),
    geometry_uv=dict(fbx=CFG["in_fbx"], sha256_before=FBX_SHA_BEFORE, sha256_after=sha(FBX), unchanged=FBX_SHA_BEFORE == sha(FBX), note="the FBX is only read; not re-exported"),
    unused_in_game_note="Metallic, roughness and normal maps are unused in game under the owner default of 2026-10-22 (base colour only, constant roughness, metallic 0).",
)
with open(P("out_report"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps(dict(changed=changed_by, outside=outside, painted=painted, faces=rep["face_classes"])))
