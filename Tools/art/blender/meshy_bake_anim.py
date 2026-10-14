"""task-157-02: author the chaser RUN (16 frame loop) and ATTACK (15 frames) procedurally on the ten rigid parts and bake one transform per part and frame into a float texture.
Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_bake_anim.py -- --in-fbx Art/Meshes/chaser-meshy/chaser_meshy_parts.fbx --in-tex Art/Meshes/chaser-meshy/chaser_meshy_fixed_basecolor.png --out-dir Art/Meshes/chaser-meshy [--work <short dir>] [--no-render]
Space: robot space, cm: X forward, Y left, Z up. Right-handed rotations. A positive rotation about Y swings a hanging limb BACKWARD (and leans a standing torso FORWARD); a negative one swings it forward.
Each part is a Blender object at its joint (origin = pivot), parent offsets plain (identity parent inverse), rest rotation identity. A pose = per part a local Euler XYZ (R = Rz Ry Rx, applied about the part pivot, in the parent's frame) plus a local pelvis translation.
World transform per part comes from evaluating matrix_world of the posed objects (the parent chain is included: forearm follows upper arm, torso, pelvis).
Bake (world, relative to the rest pose, mesh space of the combined static mesh): for a part with rest pivot p, world rotation R (relative to rest), world pivot position q:
    new = R (v - p) + q   =>   new = R v + t   with   t = q - R p      (v = rest vertex in the combined mesh space)
Texture T_ChaserAnim.exr: RGBA float16, 20 x 31; texel (x = 2*part, row=frame) = quaternion x y z w of R; texel (x = 2*part+1, row=frame) = rgb t in cm, a = 0.
Rows: frame f is stored at the image row f counted from the TOP of the EXR (scanline f, first scanline in the file); v = (f + 0.5) / 31. Frames: run 0..15, attack 16..30.
Blender only (numpy included). Nothing is imported in Unreal."""
import bpy, sys, os, json, math, time, shutil, struct
import numpy as np
from mathutils import Vector, Matrix, Euler

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
NORENDER = "--no-render" in argv
argv = [a for a in argv if a != "--no-render"]
opt = {"--in-fbx": "", "--in-tex": "", "--out-dir": "", "--work": "C:/Users/soumo/AppData/Local/Temp/rr-meshy2/work", "--height-cm": "123",
       "--thigh-deg": "35", "--knee-deg": "55", "--arm-deg": "30", "--elbow-deg": "40", "--lean-deg": "8", "--twist-deg": "5", "--speed-cm-s": "180"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
for k in ("--in-fbx", "--in-tex", "--out-dir"):
    if not opt[k]:
        raise SystemExit("missing " + k)
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
os.makedirs(WORK, exist_ok=True)
HEIGHT = float(opt["--height-cm"])
A_THIGH, A_KNEE, A_ARM, A_ELBOW, A_LEAN, A_TWIST = (float(opt[k]) for k in ("--thigh-deg", "--knee-deg", "--arm-deg", "--elbow-deg", "--lean-deg", "--twist-deg"))
SPEED = float(opt["--speed-cm-s"])
FPS = 30
IN_FBX = os.path.join(WORK, "anim_in_parts.fbx")
shutil.copyfile(opt["--in-fbx"], IN_FBX)
TEX = os.path.join(WORK, os.path.basename(opt["--in-tex"]))
shutil.copyfile(opt["--in-tex"], TEX)

PARTS = ["Pelvis", "HeadTorso", "UpperArm_L", "UpperArm_R", "ForearmHand_L", "ForearmHand_R", "Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"]
PARENT = {"Pelvis": None, "HeadTorso": "Pelvis", "UpperArm_L": "HeadTorso", "UpperArm_R": "HeadTorso", "ForearmHand_L": "UpperArm_L", "ForearmHand_R": "UpperArm_R",
          "Thigh_L": "Pelvis", "Thigh_R": "Pelvis", "ShinFoot_L": "Thigh_L", "ShinFoot_R": "Thigh_R"}
NRUN, NATT = 16, 15
NF = NRUN + NATT
TEXW = 2 * len(PARTS)

# ---------------------------------------------------------------- import
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=IN_FBX, colors_type="LINEAR")
OB = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
assert sorted(OB) == sorted(PARTS), sorted(OB)
bpy.context.view_layer.update()
WARN = []
REST_LOC = {}
REST_P = {}
REST_V = {}
for n in PARTS:
    o = OB[n]
    o.rotation_mode = "XYZ"
    m = o.matrix_world
    if max(abs(m[i][j] - (1.0 if i == j else 0.0)) for i in range(3) for j in range(3)) > 1e-5:
        raise SystemExit("rest matrix of %s is not a pure translation: %s" % (n, m))
    assert max(abs(x) for x in o.rotation_euler) < 1e-6 and max(abs(s - 1) for s in o.scale) < 1e-6
    REST_LOC[n] = o.location.copy()
    REST_P[n] = np.array(m.translation[:], float)
    REST_V[n] = np.array([v.co[:] for v in o.data.vertices], float) + REST_P[n]     # rest vertices, combined mesh space (world)
print("REST_P", {n: [round(x, 3) for x in REST_P[n]] for n in PARTS})
print("HEIGHT", max(v[:, 2].max() for v in REST_V.values()), min(v[:, 2].min() for v in REST_V.values()))
FOOT = ["ShinFoot_L", "ShinFoot_R"]


def apply_pose(pose):
    """pose = dict part -> dict(rot=(rx, ry, rz) degrees Euler XYZ, loc=(dx, dy, dz) cm added to the rest local location)"""
    for n in PARTS:
        o = OB[n]
        p = pose.get(n, {})
        r = p.get("rot", (0, 0, 0))
        d = p.get("loc", (0, 0, 0))
        o.rotation_euler = Euler([math.radians(a) for a in r], "XYZ")
        o.location = REST_LOC[n] + Vector(d)
    bpy.context.view_layer.update()


def world_state():
    """(R 3x3 numpy relative to rest, q world pivot position) per part, from matrix_world"""
    out = {}
    for n in PARTS:
        m = OB[n].matrix_world
        out[n] = (np.array(m.to_3x3())[:], np.array(m.translation[:], float))
    return out


def world_verts(n):
    m = np.array(OB[n].matrix_world)
    loc = np.array([v.co[:] for v in OB[n].data.vertices], float)
    return loc @ m[:3, :3].T + m[:3, 3]


def foot_min():
    return [float(world_verts(f)[:, 2].min()) for f in FOOT]


def fit_ground(pose):
    """lower/raise the pelvis so the lowest foot vertex of both feet is at z = 0"""
    pose.setdefault("Pelvis", {})
    pose["Pelvis"]["loc"] = tuple(pose["Pelvis"].get("loc", (0, 0, 0)))[:2] + (0.0,)
    apply_pose(pose)
    dz = -min(foot_min())
    pose["Pelvis"]["loc"] = pose["Pelvis"]["loc"][:2] + (dz,)
    apply_pose(pose)
    return dz


# ---------------------------------------------------------------- clips
def run_pose(phase):
    th = 2 * math.pi * phase
    s = math.sin(th)
    pose = {}
    pose["HeadTorso"] = dict(rot=(0, A_LEAN, A_TWIST * s))                    # lean forward about +Y; twist about Z: right shoulder forward when the left leg is forward
    pose["Thigh_L"] = dict(rot=(0, -A_THIGH * s, 0))                          # forward = negative Y rotation
    pose["Thigh_R"] = dict(rot=(0, A_THIGH * s, 0))
    pose["ShinFoot_L"] = dict(rot=(0, A_KNEE * max(0.0, math.cos(th)), 0))    # shin rotates backward (+Y) only while the leg swings forward (d angle / d phase > 0)
    pose["ShinFoot_R"] = dict(rot=(0, A_KNEE * max(0.0, -math.cos(th)), 0))
    pose["UpperArm_L"] = dict(rot=(0, A_ARM * s, 0))                          # left arm forward when the right leg is forward (opposite to the left leg)
    pose["UpperArm_R"] = dict(rot=(0, -A_ARM * s, 0))
    pose["ForearmHand_L"] = dict(rot=(0, -A_ELBOW, 0))                        # elbow bent forward, constant
    pose["ForearmHand_R"] = dict(rot=(0, -A_ELBOW, 0))
    return pose


def smooth(x):
    x = min(1.0, max(0.0, x))
    return x * x * (3 - 2 * x)


WIND = {"HeadTorso": dict(rot=(0, -10, 0)), "UpperArm_R": dict(rot=(0, 60, 0)), "ForearmHand_R": dict(rot=(0, -70, 0))}
STRIKE = {"HeadTorso": dict(rot=(0, 15, 0)), "UpperArm_R": dict(rot=(0, -70, 0)), "ForearmHand_R": dict(rot=(0, -30, 0)),
          "Pelvis": dict(rot=(0, 0, 0), loc=(8, 0, 0)), "Thigh_L": dict(rot=(0, -20, 0)), "Thigh_R": dict(rot=(0, 6, 0))}
RESTP = {}


def blend(pa, pb, w):
    out = {}
    for n in set(pa) | set(pb):
        a, b = pa.get(n, {}), pb.get(n, {})
        ra, rb = np.array(a.get("rot", (0, 0, 0)), float), np.array(b.get("rot", (0, 0, 0)), float)
        la, lb = np.array(a.get("loc", (0, 0, 0)), float), np.array(b.get("loc", (0, 0, 0)), float)
        out[n] = dict(rot=tuple(ra + (rb - ra) * w), loc=tuple(la + (lb - la) * w))
    return out


def attack_pose(i):
    """i = 0..14: 0..2 wind-up (full wind-up at i = 2), 3..6 strike (full strike at i = 6), 7..14 recovery (rest at i = 14)"""
    if i <= 2:
        return blend(RESTP, WIND, smooth((i + 1) / 3.0))
    if i <= 6:
        return blend(WIND, STRIKE, smooth((i - 2) / 4.0))
    return blend(STRIKE, RESTP, smooth((i - 6) / 8.0))


# ---------------------------------------------------------------- evaluate all frames
POSES = []
DZ = []
FOOTZ = []
WORLD = []       # per frame: dict part -> (R, q)
VERTS = []       # per frame: dict part -> world verts (independent path: matrix_world @ local co)
for f in range(NF):
    pose = run_pose(f / NRUN) if f < NRUN else attack_pose(f - NRUN)
    dz = fit_ground(pose)
    POSES.append(pose)
    DZ.append(dz)
    FOOTZ.append(foot_min())
    WORLD.append(world_state())
    VERTS.append({n: world_verts(n) for n in PARTS})
# loop closure: pose at phase 1.0 against frame 0
pose1 = run_pose(1.0)
fit_ground(pose1)
V_NEXT = {n: world_verts(n) for n in PARTS}
loop_err = max(float(np.linalg.norm(V_NEXT[n] - VERTS[0][n], axis=1).max()) for n in PARTS)
apply_pose({})
V_REST_CHECK = max(float(np.linalg.norm(world_verts(n) - REST_V[n], axis=1).max()) for n in PARTS)
att14_err = max(float(np.linalg.norm(VERTS[NRUN + 14][n] - REST_V[n], axis=1).max()) for n in PARTS)


def quat_from(R):
    q = Matrix(R.tolist()).to_quaternion()       # w x y z
    return np.array([q.x, q.y, q.z, q.w], float)


def qmat(q):
    x, y, z, w = q / np.linalg.norm(q)
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


# ---------------------------------------------------------------- bake arrays
Q = np.zeros((NF, len(PARTS), 4))
T = np.zeros((NF, len(PARTS), 3))
for f in range(NF):
    for pi, n in enumerate(PARTS):
        R, q = WORLD[f][n]
        Q[f, pi] = quat_from(R)
        T[f, pi] = q - R @ REST_P[n]
for lo, hi in ((0, NRUN), (NRUN, NF)):          # sign continuity inside each clip
    for pi in range(len(PARTS)):
        if Q[lo, pi, 3] < 0:
            Q[lo, pi] *= -1
        for f in range(lo + 1, hi):
            if np.dot(Q[f, pi], Q[f - 1, pi]) < 0:
                Q[f, pi] *= -1
loop_dot_min = min(float(np.dot(Q[NRUN - 1, pi], Q[0, pi])) for pi in range(len(PARTS)))


def qmat_raw(q):
    x, y, z, w = q          # no normalisation: exact for unit q; a shader that does not renormalise sees this
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def half_neighbours(x):
    h = np.float16(x)
    return [float(np.nextafter(h, np.float16(-np.inf))), float(h), float(np.nextafter(h, np.float16(np.inf)))]


# float16 storage: pick, per frame and part, the float16 quaternion (each component within one half step of the exact one) and the float16 t (least squares on the part vertices
# for that quaternion) that give the smallest max vertex error with the quaternion used as stored (not renormalised). Plain rounding costs up to ~0.06 cm on the attack.
import itertools
Qh = np.zeros_like(Q)
Th = np.zeros_like(T)
for f in range(NF):
    for pi, n in enumerate(PARTS):
        tgt = VERTS[f][n]
        best = None
        for c in itertools.product(*[half_neighbours(Q[f, pi, k]) for k in range(4)]):
            qc = np.array(c, float)
            Rr, Rn = qmat_raw(qc), qmat(qc)
            br, bn = REST_V[n] @ Rr.T, REST_V[n] @ Rn.T
            for tl in ((tgt - br).mean(0), (tgt - bn).mean(0), (tgt - 0.5 * (br + bn)).mean(0)):
                tq = np.array([float(np.float16(x)) for x in tl])
                err = max(float(np.linalg.norm(br + tq - tgt, axis=1).max()), float(np.linalg.norm(bn + tq - tgt, axis=1).max()))
                if best is None or err < best[0]:
                    best = (err, qc, tq)
        Qh[f, pi], Th[f, pi] = best[1], best[2]

tex = np.zeros((NF, TEXW, 4), np.float32)        # tex[frame, x] frame 0 = top row of the image file
for pi in range(len(PARTS)):
    tex[:, 2 * pi, :] = Qh[:, pi, :]
    tex[:, 2 * pi + 1, :3] = Th[:, pi, :]

# ---------------------------------------------------------------- EXR write (float16, raw) and read back
EXR_W = os.path.join(WORK, "T_ChaserAnim.exr")
sc = bpy.context.scene
ims = sc.render.image_settings
ims.file_format = "OPEN_EXR"
ims.color_depth = "16"
ims.color_mode = "RGBA"
ims.exr_codec = "NONE"
img = bpy.data.images.new("T_ChaserAnim", TEXW, NF, alpha=True, float_buffer=True)
img.colorspace_settings.name = "Non-Color"
buf = np.flipud(tex)             # Blender row 0 = bottom of the image: frame 0 goes to the last Blender row = top of the saved file
img.pixels.foreach_set(buf.astype(np.float32).ravel())
img.pixels.update()
img.save_render(EXR_W, scene=sc)
img2 = bpy.data.images.load(EXR_W, check_existing=False)
img2.colorspace_settings.name = "Non-Color"
assert tuple(img2.size) == (TEXW, NF), tuple(img2.size)
back = np.array(img2.pixels[:], np.float32).reshape(NF, TEXW, 4)
back = np.flipud(back)            # back[frame]
half_ref = tex.astype(np.float16).astype(np.float32)
exr_vs_half = float(np.abs(back - half_ref).max())
exr_vs_f32_rel = float((np.abs(back - tex) / np.maximum(np.abs(tex), 1e-3)).max())
exr_vs_f32_abs = float(np.abs(back - tex).max())


def read_exr_raw(path):
    """independent parse of the uncompressed EXR: returns (channels, width, height, array [rows from the first scanline in file order][W][channels sorted as in the file])"""
    d = open(path, "rb").read()
    assert d[:4] == b"v/1\x01"
    pos = 8
    attrs = {}
    while d[pos] != 0:
        e = d.index(b"\0", pos)
        name = d[pos:e].decode()
        e2 = d.index(b"\0", e + 1)
        size = struct.unpack("<i", d[e2 + 1:e2 + 5])[0]
        attrs[name] = d[e2 + 5:e2 + 5 + size]
        pos = e2 + 5 + size
    pos += 1
    ch = []
    cd = attrs["channels"]
    p = 0
    while cd[p] != 0:
        e = cd.index(b"\0", p)
        nm = cd[p:e].decode()
        ptype = struct.unpack("<i", cd[e + 1:e + 5])[0]
        ch.append((nm, ptype))
        p = e + 1 + 16
    dw = struct.unpack("<iiii", attrs["dataWindow"])
    w, h = dw[2] - dw[0] + 1, dw[3] - dw[1] + 1
    comp = attrs["compression"][0]
    lo = attrs.get("lineOrder", b"\0")[0]
    offs = struct.unpack("<%dq" % h, d[pos:pos + 8 * h])
    rows = []
    for o in offs:
        y, sz = struct.unpack("<ii", d[o:o + 8])
        data = d[o + 8:o + 8 + sz]
        a = np.zeros((w, len(ch)), np.float32)
        q = 0
        for ci, (nm, pt) in enumerate(ch):
            if pt == 1:
                a[:, ci] = np.frombuffer(data, np.float16, w, q)
                q += 2 * w
            else:
                a[:, ci] = np.frombuffer(data, np.float32, w, q)
                q += 4 * w
        rows.append((y, a))
    return [c[0] for c in ch], [c[1] for c in ch], w, h, comp, lo, rows


ch_names, ch_types, rw, rh, rcomp, rline, rrows = read_exr_raw(EXR_W)
cidx = {nm: i for i, nm in enumerate(ch_names)}
raw_top_first = np.stack([r[1] for r in sorted(rrows, key=lambda r: r[0])])          # scanline y = 0 first (the TOP row of the picture)
raw_rgba = raw_top_first[:, :, [cidx["R"], cidx["G"], cidx["B"], cidx["A"]]]
raw_vs_arrays = float(np.abs(raw_rgba - half_ref).max())
SAVED = back.copy()      # what Unreal will read, rows by frame

# ---------------------------------------------------------------- bake verification (baked quaternion + t on the rest mesh against the posed hierarchy)


def bake_err(QQ, TT, raw=False):
    per = {}
    for f in range(NF):
        for pi, n in enumerate(PARTS):
            Rm = qmat_raw(QQ[f, pi]) if raw else qmat(QQ[f, pi])
            new = REST_V[n] @ Rm.T + TT[f, pi]
            e = float(np.linalg.norm(new - VERTS[f][n], axis=1).max())
            per[f] = max(per.get(f, 0.0), e)
    return per


SQ = np.zeros((NF, len(PARTS), 4))
ST = np.zeros((NF, len(PARTS), 3))
for pi in range(len(PARTS)):
    SQ[:, pi] = SAVED[:, 2 * pi]
    ST[:, pi] = SAVED[:, 2 * pi + 1, :3]
e32 = bake_err(Q, T)
e16 = bake_err(SQ, ST)
e16raw = bake_err(SQ, ST, raw=True)
bake = dict(float32=dict(run_max_cm=max(e32[f] for f in range(NRUN)), attack_max_cm=max(e32[f] for f in range(NRUN, NF))),
            from_saved_exr_float16=dict(run_max_cm=max(e16[f] for f in range(NRUN)), attack_max_cm=max(e16[f] for f in range(NRUN, NF))),
            from_saved_exr_float16_quaternion_not_renormalised=dict(run_max_cm=max(e16raw[f] for f in range(NRUN)), attack_max_cm=max(e16raw[f] for f in range(NRUN, NF))))
# the half-float step of t grows with |t|; report the shader-side normalisation too (quaternion renormalised after sampling, which qmat does)

# ---------------------------------------------------------------- numbers per clip


def angle_deg(q):
    return math.degrees(2 * math.acos(min(1.0, abs(float(q[3])))))


def clip_stats(lo, hi):
    ang = {n: max(angle_deg(Q[f, pi]) for f in range(lo, hi)) for pi, n in enumerate(PARTS)}
    tmax = {n: float(max(np.linalg.norm(T[f, pi]) for f in range(lo, hi))) for pi, n in enumerate(PARTS)}
    disp = {n: float(max(np.linalg.norm(WORLD[f][n][1] - REST_P[n]) for f in range(lo, hi))) for pi, n in enumerate(PARTS)}
    # local channel extents (what was keyed)
    ext = {}
    for n in PARTS:
        arr = np.array([list(POSES[f].get(n, {}).get("rot", (0, 0, 0))) for f in range(lo, hi)])
        ext[n] = dict(rot_min_deg=[round(float(x), 2) for x in arr.min(0)], rot_max_deg=[round(float(x), 2) for x in arr.max(0)])
    pel = np.array([POSES[f].get("Pelvis", {}).get("loc", (0, 0, 0)) for f in range(lo, hi)])
    return dict(max_world_rotation_deg_per_part={k: round(v, 2) for k, v in ang.items()}, max_world_rotation_deg=round(max(ang.values()), 2),
                max_abs_t_cm_per_part={k: round(v, 2) for k, v in tmax.items()}, max_abs_t_cm=round(max(tmax.values()), 2),
                max_pivot_displacement_cm_per_part={k: round(v, 2) for k, v in disp.items()}, keyed_local_euler_extents=ext,
                pelvis_local_translation_cm=dict(min=[round(float(x), 2) for x in pel.min(0)], max=[round(float(x), 2) for x in pel.max(0)]))


# feet: ground fit, floating of the higher foot, sliding of the lowest vertex against the floor
fz = np.array(FOOTZ)
run_dz = np.array(DZ[:NRUN])
att_dz = np.array(DZ[NRUN:])
slide = []
per_frame = SPEED / FPS
for f in range(NRUN):
    f2 = (f + 1) % NRUN
    best = None
    for n in FOOT:
        v = VERTS[f][n]
        i = int(v[:, 2].argmin())
        if best is None or v[i, 2] < best[0]:
            best = (v[i, 2], n, i)
    z0, n, i = best
    x0 = VERTS[f][n][i]
    x1 = VERTS[f2][n][i]
    if z0 <= 0.5 and x1[2] <= 1.0:
        slide.append(dict(frame=f, vertex=[n, i], z_cm=round(float(z0), 3), next_z_cm=round(float(x1[2]), 3), slide_cm_per_frame=round(float(x1[0] - x0[0] + per_frame), 2)))
    else:
        slide.append(dict(frame=f, vertex=[n, i], z_cm=round(float(z0), 3), next_z_cm=round(float(x1[2]), 3), slide_cm_per_frame=None))
sl = [s["slide_cm_per_frame"] for s in slide if s["slide_cm_per_frame"] is not None]
leg_len = float(REST_P["Thigh_L"][2])
ideal_amp = math.degrees(math.asin(min(1.0, (SPEED * NRUN / FPS / 2) / 2 / leg_len)))
stride_cm = 2 * leg_len * math.sin(math.radians(A_THIGH))
frame_step = []
for f in range(NRUN):
    f2 = (f + 1) % NRUN
    frame_step.append(max(float(np.linalg.norm(VERTS[f2][n] - VERTS[f][n], axis=1).max()) for n in PARTS))
att_step = [max(float(np.linalg.norm(VERTS[f + 1][n] - VERTS[f][n], axis=1).max()) for n in PARTS) for f in range(NRUN, NF - 1)]

# ---------------------------------------------------------------- renders
RENDERS = []
if not NORENDER:
    import colorsys
    img_tex = bpy.data.images.load(TEX, check_existing=False)
    img_tex.colorspace_settings.name = "sRGB"
    for o in OB.values():
        for m in o.data.materials:
            if m and m.node_tree:
                for nd in m.node_tree.nodes:
                    if nd.type == "TEX_IMAGE":
                        nd.image = img_tex
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            pass
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    try:
        sc.eevee.use_raytracing = True
        sc.eevee.fast_gi_method = "AMBIENT_OCCLUSION_ONLY"
        sc.eevee.fast_gi_distance = 6.0
    except Exception as ex_:
        print("lit: no raytracing/AO", ex_)
    w = bpy.data.worlds.new("W")
    sc.world = w
    w.use_nodes = True
    bgn = w.node_tree.nodes["Background"]
    bgn.inputs["Color"].default_value = (0.62, 0.62, 0.62, 1)
    bgn.inputs["Strength"].default_value = 0.55
    sun = bpy.data.lights.new("sun", "SUN")
    sun.energy = 4.5
    sun.angle = math.radians(14)
    so = bpy.data.objects.new("sun", sun)
    sc.collection.objects.link(so)
    cam = bpy.data.cameras.new("cam")
    cam.type = "ORTHO"
    cam.clip_start, cam.clip_end = 1, 4000
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    sc.camera = co
    # ground slab, top face at z = 0 (a dark line in the side and front views)
    gme = bpy.data.meshes.new("floor")
    gv = [(-75, -40, -0.6), (75, -40, -0.6), (75, 40, -0.6), (-75, 40, -0.6), (-75, -40, 0), (75, -40, 0), (75, 40, 0), (-75, 40, 0)]
    gme.from_pydata(gv, [], [(0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)])
    gm = bpy.data.materials.new("floor")
    gm.use_nodes = True
    gm.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.08, 0.08, 0.1, 1)
    gme.materials.append(gm)
    go = bpy.data.objects.new("floor", gme)
    sc.collection.objects.link(go)
    VIEWS = {"side": (0, -1, 0), "front": (1, 0, 0)}
    CENTRE = (0, 0, HEIGHT / 2)
    PX = 420

    def aim(d, centre, size, px, up=(0, 0, 1)):
        d = Vector(d).normalized()
        fwd = -d
        r = fwd.cross(Vector(up)).normalized()
        u = r.cross(fwd).normalized()
        co.location = Vector(centre) + d * 500
        co.rotation_euler = Matrix((r, u, -fwd)).transposed().to_euler()
        co.data.ortho_scale = size
        sc.render.resolution_x = sc.render.resolution_y = px
        bpy.context.view_layer.update()
        return fwd, r, u

    def shot(d, path):
        fwd, r, u = aim(d, CENTRE, 150, PX)
        key = (fwd + r * 0.75 + Vector((0, 0, -1.1))).normalized()
        so.rotation_euler = key.to_track_quat("-Z", "Y").to_euler()
        sc.render.filepath = path
        ims.file_format = "PNG"
        ims.color_mode = "RGB"
        ims.color_depth = "8"
        bpy.ops.render.render(write_still=True)
        im = bpy.data.images.load(path, check_existing=False)
        a = np.array(im.pixels[:], np.float32).reshape(PX, PX, 4)
        bpy.data.images.remove(im)
        return a

    def sheet(frames, view, fname):
        tiles = []
        for f in frames:
            apply_pose(POSES[f])
            tiles.append(shot(VIEWS[view], os.path.join(WORK, "tile.png")))
        a = np.concatenate(tiles, axis=1)
        h, wd = a.shape[:2]
        o = bpy.data.images.new("sheet", wd, h, alpha=False)
        o.colorspace_settings.name = "sRGB"
        o.pixels.foreach_set(a.ravel())
        o.filepath_raw = os.path.join(WORK, fname)
        o.file_format = "PNG"
        o.save()
        bpy.data.images.remove(o)
        RENDERS.append(fname)

    sheet(list(range(0, 16, 2)), "side", "run_contact.png")
    sheet(list(range(NRUN, NF, 2)), "side", "attack_contact.png")
    sheet(list(range(0, 16, 2)), "front", "run_contact_front.png")

# ---------------------------------------------------------------- report
if any(abs(x) > 0.05 for x in (bake["float32"]["run_max_cm"], bake["float32"]["attack_max_cm"], bake["from_saved_exr_float16"]["run_max_cm"], bake["from_saved_exr_float16"]["attack_max_cm"], bake["from_saved_exr_float16_quaternion_not_renormalised"]["run_max_cm"], bake["from_saved_exr_float16_quaternion_not_renormalised"]["attack_max_cm"], loop_err)):
    WARN.append("a bake / loop error is above 0.05 cm (see bake_verification)")
rep = dict(
    task="task-157-02-chaser-clips-bake",
    source=dict(fbx=os.path.basename(opt["--in-fbx"]), blender=bpy.app.version_string, rest_matches_import_check_max_cm=round(V_REST_CHECK, 6)),
    space="robot space, cm: X forward, Y left, Z up, right-handed; positive rotation about +Y swings a hanging limb backward and leans a torso forward; rest rotation of every part is identity",
    frame_rate_fps=FPS,
    clips=dict(run=dict(start_frame=0, length=NRUN, loop=True, seconds=round(NRUN / FPS, 4)), attack=dict(start_frame=NRUN, length=NATT, loop=False, seconds=round(NATT / FPS, 4))),
    parts=[dict(index=i, name=n, parent=PARENT[n], rest_pivot_cm=[round(float(x), 3) for x in REST_P[n]]) for i, n in enumerate(PARTS)],
    formula=dict(
        text="for a part with rest-pose world pivot p, current world rotation R (relative to rest, includes the whole parent chain) and current world pivot position q: new_position = R (v - p) + q = R v + t, with t = q - R p; v = rest vertex in the mesh space of the combined static mesh (the same space as the rest parts FBX, cm).",
        composition_order="world matrix of a part = parent world matrix @ Translation(rest local offset + keyed local translation) @ Rz Ry Rx (keyed Euler XYZ, about the part pivot). R = world matrix 3x3, q = world matrix translation. The pose is evaluated with Blender object matrix_world (hierarchy), the quaternion is taken from R, t = q - R p is computed from them. In the material: rotate v by the quaternion (x y z w, unit length) then add t.",
        quaternion_layout="texel x = 2*part_index: RGBA = (qx, qy, qz, qw); texel x = 2*part_index+1: RGB = t in cm, A = 0",
        sign_continuity="per part, the sign of q is flipped when needed so that dot(q[f], q[f-1]) >= 0 inside each clip; frame 0 and frame 16 start with w >= 0; min dot between run frame 15 and frame 0 = %.5f" % loop_dot_min),
    texture=dict(file="T_ChaserAnim.exr", width=TEXW, height=NF, format="OpenEXR RGBA half float (16 bit), compression NONE, colourspace Non-Color (raw linear data), written by Blender save_render",
                 row_mapping="frame f is the f-th scanline of the file counted from the TOP of the image (Unreal row 0 = frame 0 = run start); Blender in-memory row = (height - 1 - f); sample at u = (x + 0.5)/20, v = (f + 0.5)/31",
                 frame_index=dict(run="0..15", attack="16..30", attack_phases=dict(wind_up="16..18", strike="19..22", recovery="23..30")),
                 raw_exr_parse=dict(channels=ch_names, channel_types_1_is_half=ch_types, compression_code=rcomp, line_order=rline, width=rw, height=rh,
                                    first_scanline_equals_frame0_max_abs_diff=float(np.abs(raw_rgba[0] - half_ref[0]).max()), all_scanlines_in_frame_order_max_abs_diff=raw_vs_arrays),
                 reload_vs_half_rounded_arrays_max_abs=exr_vs_half, reload_vs_float32_arrays_max_abs=exr_vs_f32_abs, reload_vs_float32_arrays_max_relative=exr_vs_f32_rel),
    bake_verification=dict(method="for every frame and part: R(q_baked) * v_rest + t_baked against matrix_world @ local vertex of the keyed objects; max over all vertices (cm). float32 = arrays before the EXR; from_saved_exr = values re-read from the half float file (half float step: plain rounding of q and t costs up to ~0.06 cm; the script therefore stores, per frame and part, the float16 q (within one half step of exact) and float16 t (least squares) with the smallest max error, taking the worse of the normalised and not-renormalised quaternion use)",
                           **bake, limit_cm=0.05, per_frame_float32_max_cm=[round(e32[f], 5) for f in range(NF)], per_frame_saved_exr_max_cm=[round(e16[f], 5) for f in range(NF)]),
    run_loop_closure=dict(pose_at_phase_1_vs_frame_0_max_vertex_error_cm=loop_err, note="the next loop's first frame is the pose at phase 16/16; the texture has no frame for it", wrap_step_frame15_to_0_max_vertex_move_cm=round(frame_step[NRUN - 1], 3),
                          max_vertex_move_between_consecutive_frames_cm=[round(x, 3) for x in frame_step]),
    attack_end=dict(frame_30_vs_rest_max_vertex_error_cm=att14_err, frame_30_max_world_rotation_deg=round(max(angle_deg(Q[NF - 1, pi]) for pi in range(len(PARTS))), 4),
                    attack_consecutive_frame_max_vertex_move_cm=[round(x, 3) for x in att_step]),
    parameters=dict(thigh_swing_deg=A_THIGH, knee_flexion_max_deg=A_KNEE, arm_swing_deg=A_ARM, elbow_bend_deg=A_ELBOW, torso_lean_deg=A_LEAN, torso_twist_deg=A_TWIST, speed_cm_s=SPEED, pelvis_z_rule="per frame, pelvis local z set so that the lowest vertex of both feet is at z = 0 (no separate 2.5 cm bob term: the ground fit sets the pelvis height)",
                    attack_keys=dict(wind_up=WIND, strike=STRIKE, interpolation="smoothstep per phase; wind-up frames 16..18 reach WIND at frame 18; strike 19..22 reaches STRIKE at frame 22; recovery 23..30 reaches rest at frame 30")),
    run=clip_stats(0, NRUN),
    attack=clip_stats(NRUN, NF),
    feet=dict(
        run=dict(pelvis_z_offset_from_ground_fit_cm=dict(min=round(float(run_dz.min()), 3), max=round(float(run_dz.max()), 3), range=round(float(run_dz.max() - run_dz.min()), 3), per_frame=[round(float(x), 3) for x in run_dz]),
                 lowest_foot_vertex_z_per_frame_cm_L_R=[[round(a, 3), round(b, 3)] for a, b in FOOTZ[:NRUN]],
                 min_foot_vertex_z_over_cycle_cm=round(float(fz[:NRUN].min()), 4), max_penetration_below_floor_cm=round(float(max(0.0, -fz[:NRUN].min())), 4),
                 higher_foot_lowest_vertex_z_max_cm=round(float(fz[:NRUN].max()), 3),
                 slide=dict(speed_cm_s=SPEED, travel_per_frame_cm=per_frame, per_frame=slide, max_abs_cm_per_frame=(round(float(max(abs(x) for x in sl)), 2) if sl else None),
                            mean_abs_cm_per_frame=(round(float(np.mean([abs(x) for x in sl])), 2) if sl else None), frames_in_contact=len(sl),
                            note="slide of the lowest foot vertex (same vertex tracked to the next frame, loop wraps) = body-space x change per frame + travel per frame; 0 = planted on the floor. Counted only when the vertex is within 0.5 cm of the floor at the frame and within 1 cm at the next"),
                 geometry=dict(leg_length_hip_to_floor_cm=leg_len, foot_excursion_cm_two_legs_straight_at_thigh_swing=round(stride_cm, 2), cycle_travel_cm=round(SPEED * NRUN / FPS, 2),
                               thigh_amplitude_deg_for_zero_slide_straight_leg_estimate=round(ideal_amp, 2))),
        attack=dict(pelvis_z_offset_from_ground_fit_cm=dict(min=round(float(att_dz.min()), 3), max=round(float(att_dz.max()), 3), per_frame=[round(float(x), 3) for x in att_dz]),
                    lowest_foot_vertex_z_per_frame_cm_L_R=[[round(a, 3), round(b, 3)] for a, b in FOOTZ[NRUN:]],
                    min_foot_vertex_z_cm=round(float(fz[NRUN:].min()), 4), max_penetration_below_floor_cm=round(float(max(0.0, -fz[NRUN:].min())), 4), higher_foot_lowest_vertex_z_max_cm=round(float(fz[NRUN:].max()), 3))),
    contact_sheets=dict(files=RENDERS, run_frames=list(range(0, 16, 2)), attack_frames=list(range(NRUN, NF, 2)), camera="orthographic 150 cm frame, 420 px per panel, 8 panels in a row; side = camera at -Y looking +Y (robot faces image right), front = camera at +X; lit rig of meshy_texture_fix.py (EEVEE, sun 4.5, world grey 0.62 strength 0.55), chaser_meshy_fixed_basecolor.png, dark floor slab top at z = 0"),
    warnings=WARN)
rep_path = os.path.join(WORK, "anim_report.json")
with open(rep_path, "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
os.makedirs(OUT, exist_ok=True)
shutil.copyfile(EXR_W, os.path.join(OUT, "T_ChaserAnim.exr"))
shutil.copyfile(rep_path, os.path.join(OUT, "anim_report.json"))
os.makedirs(os.path.join(OUT, "renders", "anim"), exist_ok=True)
for fn in RENDERS:
    shutil.copyfile(os.path.join(WORK, fn), os.path.join(OUT, "renders", "anim", fn))
print("DONE", json.dumps(dict(bake=bake, loop_err=loop_err, att_end_err=att14_err, exr=dict(raw=raw_vs_arrays, half=exr_vs_half), warnings=WARN)))
