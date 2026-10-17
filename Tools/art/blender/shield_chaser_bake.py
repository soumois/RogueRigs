"""task-219-01: shield chaser. Four clips baked into one float texture and one combined animated mesh (chaser rigid parts + the shield bound to ForearmHand_L).
Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/shield_chaser_bake.py -- [--out-dir Art/Meshes/shield-chaser] [--work <short dir>] [--no-render]
Inputs (repo relative defaults): Art/Meshes/chaser-meshy/{chaser_meshy_parts.fbx, T_ChaserAnim.exr, chaser_meshy_fixed_basecolor.png}, Art/Meshes/ChaserShield/{SM_ChaserShield.fbx, textures}.
Nothing in Art/Meshes/chaser-meshy is modified. Functions apply_pose / world_state / fit_ground / blend / bake / EXR read back are copied from meshy_bake_anim.py (same space, same bake formula new = R v + t).

Clips (rows from the top of T_ShieldChaserAnim.exr, 20 x 62 RGBA half float, texel 2p quaternion xyzw, 2p+1 translation cm; p = part index of PARTS):
  clip 0 shield run    rows  0..15  (16 frames, loop)
  clip 1 shield bash   rows 16..30  (15 frames: wind-up 16..18, strike 19..22, recovery to guard 23..30)
  clip 2 chaser run    rows 31..46  exact copy of T_ChaserAnim rows 0..15
  clip 3 chaser attack rows 47..61  exact copy of T_ChaserAnim rows 16..30
The shield has no texel of its own: it follows ForearmHand_L (part 4). Its vertices are stored in REST space: v_rest = R0^T (v_placed - t0), (R0, t0) = ForearmHand_L transform of clip 0 frame 0, so that new = R v_rest + t.
PartId (UV channel 2) of the shield = (4 + 0.5) / 10 = 0.45, PartIndex colour red = 4 / 9.
The left arm is solved with a small damped least squares (numeric Jacobian) on the real object hierarchy so that the hand reaches the guard / wind-up / strike hand targets (world space, cm)."""
import bpy, sys, os, json, math, time, shutil, struct, itertools, copy
import numpy as np
from mathutils import Vector, Matrix, Euler
from mathutils.bvhtree import BVHTree

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
NORENDER = "--no-render" in argv
argv = [a for a in argv if a != "--no-render"]
opt = {"--out-dir": os.path.join(REPO, "Art/Meshes/shield-chaser"), "--work": "C:/Users/soumo/AppData/Local/Temp/rr-sc/work",
       "--chaser-dir": os.path.join(REPO, "Art/Meshes/chaser-meshy"), "--shield-dir": os.path.join(REPO, "Art/Meshes/ChaserShield")}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out-dir"])
WORK = os.path.abspath(opt["--work"])
shutil.rmtree(WORK, ignore_errors=True)
os.makedirs(WORK, exist_ok=True)
os.makedirs(OUT, exist_ok=True)
CH = os.path.abspath(opt["--chaser-dir"])
SH = os.path.abspath(opt["--shield-dir"])
IN_FBX = os.path.join(WORK, "in_parts.fbx")
shutil.copyfile(os.path.join(CH, "chaser_meshy_parts.fbx"), IN_FBX)
CH_TEX = os.path.join(WORK, "chaser_meshy_fixed_basecolor.png")
shutil.copyfile(os.path.join(CH, "chaser_meshy_fixed_basecolor.png"), CH_TEX)
SH_WORK = os.path.join(WORK, "shield")
os.makedirs(SH_WORK, exist_ok=True)
for fn in ("SM_ChaserShield.fbx", "T_ChaserShield_BaseColor.png", "T_ChaserShield_Emblem.png"):
    shutil.copyfile(os.path.join(SH, fn), os.path.join(SH_WORK, fn))
SRC_EXR = os.path.join(CH, "T_ChaserAnim.exr")

PARTS = ["Pelvis", "HeadTorso", "UpperArm_L", "UpperArm_R", "ForearmHand_L", "ForearmHand_R", "Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"]
PARENT = {"Pelvis": None, "HeadTorso": "Pelvis", "UpperArm_L": "HeadTorso", "UpperArm_R": "HeadTorso", "ForearmHand_L": "UpperArm_L", "ForearmHand_R": "UpperArm_R",
          "Thigh_L": "Pelvis", "Thigh_R": "Pelvis", "ShinFoot_L": "Thigh_L", "ShinFoot_R": "Thigh_R"}
SHIELD_PART = "ForearmHand_L"
SHIELD_PART_INDEX = PARTS.index(SHIELD_PART)       # 4
NRUN, NATT = 16, 15
NCL = NRUN + NATT                                   # frames evaluated here (clips 0 and 1)
NROWS = 2 * NCL                                     # 62
TEXW = 2 * len(PARTS)
CLIPS = [dict(name="shield_run", start=0, length=16, loop=True), dict(name="shield_bash", start=16, length=15, loop=False),
         dict(name="chaser_run", start=31, length=16, loop=True), dict(name="chaser_attack", start=47, length=15, loop=False)]
FPS = 30
HEIGHT = 123.0

# ---------------------------------------------------------------- parameters
A_THIGH, A_KNEE, A_ARM_R, A_ELBOW_R, A_LEAN, A_TWIST = 35.0, 55.0, 20.0, 40.0, 8.0, 3.0      # chaser run values except the right arm swing (30 -> 20) and twist (5 -> 3)
BOB_DEG = 1.5
SHIELD_TILT_DEG = 8.0              # plate tilted back at clip 0 frame 0: normal pitched up by this angle
SHIELD_GRIP_CM = (-2.01, 4.6, 39.53)    # centre of the middle grip block, from the shield pivot (SM_ChaserShield.report.json grip_offset_cm, Unreal axes x, -y, z -> y mirrored back to Blender)
HAND_Z_MAX = 68.0                       # rest vertices of ForearmHand_L below this z are the hand
for k_, v_ in zip(argv[::2], argv[1::2]):
    pass
KEYS = dict(
    guard_run=dict(target=(27.0, 9.0, 92.0), tilt=None),
    guard_stand=dict(target=(27.0, 9.0, 92.0), tilt=SHIELD_TILT_DEG),
    wind=dict(target=(19.0, 5.0, 92.0), tilt=10.0),
    strike=dict(target=(46.0, 9.0, 94.0), tilt=3.0),
)
ENV = os.environ.get("SC_KEYS")
if ENV:
    for k_, v_ in json.loads(ENV).items():
        KEYS[k_].update(v_)

# ---------------------------------------------------------------- import
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=IN_FBX, colors_type="LINEAR")
OB = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
assert sorted(OB) == sorted(PARTS), sorted(OB)
PART_ORDER = [o.name for o in bpy.data.objects if o.type == "MESH"]
bpy.context.view_layer.update()
WARN = []
REST_LOC, REST_P, REST_V = {}, {}, {}
SRC_PART = {}
for n in PARTS:
    o = OB[n]
    o.rotation_mode = "XYZ"
    m = o.matrix_world
    assert max(abs(m[i][j] - (1.0 if i == j else 0.0)) for i in range(3) for j in range(3)) < 1e-5, n
    REST_LOC[n] = o.location.copy()
    REST_P[n] = np.array(m.translation[:], float)
    REST_V[n] = np.array([v.co[:] for v in o.data.vertices], float) + REST_P[n]
    me = o.data
    SRC_PART[n] = dict(tris=sum(len(p.vertices) - 2 for p in me.polygons), v=REST_V[n].copy(), uv=[u.name for u in me.uv_layers],
                       uv0=np.array([l.uv[:] for l in me.uv_layers[0].data]), uv1=np.array([l.uv[:] for l in me.uv_layers["LightMap"].data]),
                       uv2=np.array([l.uv[:] for l in me.uv_layers["PartId"].data]), col=np.array([c.color[:] for c in me.color_attributes["PartIndex"].data]),
                       mats=[mm.name for mm in me.materials])
CHASER_TRIS = sum(SRC_PART[n]["tris"] for n in PARTS)
CHASER_MAT = OB["Pelvis"].data.materials[0]
img_ch = bpy.data.images.load(CH_TEX, check_existing=False)
img_ch.colorspace_settings.name = "sRGB"
for nd in CHASER_MAT.node_tree.nodes:
    if nd.type == "TEX_IMAGE":
        nd.image = img_ch

# shield
bpy.ops.import_scene.fbx(filepath=os.path.join(SH_WORK, "SM_ChaserShield.fbx"), colors_type="LINEAR")
SHO = [o for o in bpy.data.objects if o.type == "MESH" and o.name not in PARTS]
assert len(SHO) == 1
SHIELD = SHO[0]
SHIELD.name = "Shield"
SHIELD.data.name = "Shield"
bpy.context.view_layer.update()
assert max(abs(SHIELD.matrix_world[i][j] - (1.0 if i == j else 0.0)) for i in range(4) for j in range(4)) < 1e-5
SHIELD_V0 = np.array([v.co[:] for v in SHIELD.data.vertices], float)      # shield space (pivot bottom centre, plate faces +X)
SHIELD_TRIS = sum(len(p.vertices) - 2 for p in SHIELD.data.polygons)
SHIELD_MATS = [m.name for m in SHIELD.data.materials]
SHIELD_UV_IN = [u.name for u in SHIELD.data.uv_layers]
for m in SHIELD.data.materials:
    for nd in m.node_tree.nodes:
        if nd.type == "TEX_IMAGE":
            nm = nd.image.name if nd.image else ""
            if "Emblem" in nm or "Emblem" in m.name:
                p_ = os.path.join(SH_WORK, "T_ChaserShield_Emblem.png")
            else:
                p_ = os.path.join(SH_WORK, "T_ChaserShield_BaseColor.png")
            im = bpy.data.images.load(p_, check_existing=True)
            im.colorspace_settings.name = "sRGB"
            nd.image = im
print("SHIELD", SHIELD_TRIS, SHIELD_MATS, SHIELD_UV_IN, SHIELD_V0.min(0), SHIELD_V0.max(0))
HAND_REST = None
_fv = REST_V[SHIELD_PART]
_h = _fv[_fv[:, 2] < HAND_Z_MAX]
HAND_REST = (_h.min(0) + _h.max(0)) / 2
print("HAND_REST", HAND_REST, "hand verts", len(_h))
SHIELD_REST = None       # filled after the guard solve (rest space)
SHIELD_MAT0 = None


# ---------------------------------------------------------------- pose machinery (copied from meshy_bake_anim.py)
def apply_pose(pose):
    for n in PARTS:
        o = OB[n]
        p = pose.get(n, {})
        r = p.get("rot", (0, 0, 0))
        d = p.get("loc", (0, 0, 0))
        o.rotation_euler = Euler([math.radians(a) for a in r], "XYZ")
        o.location = REST_LOC[n] + Vector(d)
    bpy.context.view_layer.update()


def world_state():
    out = {}
    for n in PARTS:
        m = OB[n].matrix_world
        out[n] = (np.array(m.to_3x3())[:], np.array(m.translation[:], float))
    return out


def part_world_verts(n):
    m = np.array(OB[n].matrix_world)
    loc = np.array([v.co[:] for v in OB[n].data.vertices], float)
    return loc @ m[:3, :3].T + m[:3, 3]


def shield_world():
    """exact world vertices of the shield: forearm world matrix applied to (v_rest - pivot)"""
    m = np.array(OB[SHIELD_PART].matrix_world)
    return (SHIELD_REST - REST_P[SHIELD_PART]) @ m[:3, :3].T + m[:3, 3]


def world_verts(n):
    w = part_world_verts(n)
    if n == SHIELD_PART and SHIELD_REST is not None:
        w = np.concatenate([w, shield_world()])
    return w


FOOT = ["ShinFoot_L", "ShinFoot_R"]


def foot_min():
    return [float(part_world_verts(f)[:, 2].min()) for f in FOOT]


def fit_ground(pose):
    pose.setdefault("Pelvis", {})
    pose["Pelvis"]["loc"] = tuple(pose["Pelvis"].get("loc", (0, 0, 0)))[:2] + (0.0,)
    apply_pose(pose)
    dz = -min(foot_min())
    pose["Pelvis"]["loc"] = pose["Pelvis"]["loc"][:2] + (dz,)
    apply_pose(pose)
    return dz


def smooth(x):
    x = min(1.0, max(0.0, x))
    return x * x * (3 - 2 * x)


def blend(pa, pb, w):
    out = {}
    for n in set(pa) | set(pb):
        a, b = pa.get(n, {}), pb.get(n, {})
        ra, rb = np.array(a.get("rot", (0, 0, 0)), float), np.array(b.get("rot", (0, 0, 0)), float)
        la, lb = np.array(a.get("loc", (0, 0, 0)), float), np.array(b.get("loc", (0, 0, 0)), float)
        out[n] = dict(rot=tuple(ra + (rb - ra) * w), loc=tuple(la + (lb - la) * w))
    return out


# ---------------------------------------------------------------- left arm solve
def R_y(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def arm_state(pose_base, u):
    pose = copy.deepcopy(pose_base)
    pose["UpperArm_L"] = dict(rot=(u[0], u[1], u[2]))
    pose["ForearmHand_L"] = dict(rot=(0, u[3], u[4]))
    dz = fit_ground(pose)
    R, q = world_state()[SHIELD_PART]
    hand = R @ (HAND_REST - REST_P[SHIELD_PART]) + q
    return pose, R, hand


def solve_arm(pose_base, key, R0=None, Rs=None, u0=(-8.0, -20.0, 0.0, -70.0, 0.0)):
    """damped least squares on u = (upper arm rx, ry, rz, forearm ry, rz) degrees. residuals: hand position (cm), plate pitch (R0, Rs given) or forearm elevation (frame 0), regulariser."""
    tgt = np.array(KEYS[key]["target"], float)
    tilt = KEYS[key]["tilt"]
    prior = np.array([-8.0, -20.0, 0.0, -70.0, 0.0])

    def res(u):
        pose, R, hand = arm_state(pose_base, u)
        r = list(hand - tgt)
        if R0 is not None:
            n = R @ R0.T @ Rs @ np.array([1.0, 0, 0])
            r.append(30.0 * (math.asin(max(-1, min(1, n[2]))) - math.radians(tilt)))
            r.append(30.0 * math.atan2(n[1], n[0]))
        else:
            d = R @ (HAND_REST - REST_P[SHIELD_PART])
            d = d / np.linalg.norm(d)
            r.append(30.0 * d[2])
            r.append(0.0)
        r += list(0.05 * np.radians(np.array(u) - prior) * 30.0)
        return np.array(r)

    u = np.array(u0, float)
    lam = 1.0
    r = res(u)
    for it in range(120):
        J = np.zeros((len(r), 5))
        for k in range(5):
            du = np.zeros(5)
            du[k] = 0.25
            J[:, k] = (res(u + du) - r) / 0.25
        A = J.T @ J + lam * np.eye(5)
        step = np.linalg.solve(A, -J.T @ r)
        r2 = res(u + step)
        if r2 @ r2 < r @ r:
            u = u + step
            r = r2
            lam = max(lam * 0.5, 1e-4)
        else:
            lam *= 4
        if np.linalg.norm(step) < 1e-4 and lam < 1e-2:
            break
    pose, R, hand = arm_state(pose_base, u)
    return u, pose, R, hand


# ---------------------------------------------------------------- base poses
def run_base(phase):
    th = 2 * math.pi * phase
    s = math.sin(th)
    pose = {}
    pose["HeadTorso"] = dict(rot=(0, A_LEAN, A_TWIST * s))
    pose["Thigh_L"] = dict(rot=(0, -A_THIGH * s, 0))
    pose["Thigh_R"] = dict(rot=(0, A_THIGH * s, 0))
    pose["ShinFoot_L"] = dict(rot=(0, A_KNEE * max(0.0, math.cos(th)), 0))
    pose["ShinFoot_R"] = dict(rot=(0, A_KNEE * max(0.0, -math.cos(th)), 0))
    pose["UpperArm_R"] = dict(rot=(0, -A_ARM_R * s, 0))
    pose["ForearmHand_R"] = dict(rot=(0, -A_ELBOW_R, 0))
    return pose


# chaser attack keys for the body and the right arm (meshy_bake_anim.py WIND / STRIKE), plus the pelvis pull back in the wind-up
ATT_REST = {}
ATT_WIND = {"HeadTorso": dict(rot=(0, -10, 0)), "UpperArm_R": dict(rot=(0, 60, 0)), "ForearmHand_R": dict(rot=(0, -70, 0)), "Pelvis": dict(rot=(0, 0, 0), loc=(-4, 0, 0))}
ATT_STRIKE = {"HeadTorso": dict(rot=(0, 15, 0)), "UpperArm_R": dict(rot=(0, -70, 0)), "ForearmHand_R": dict(rot=(0, -30, 0)),
              "Pelvis": dict(rot=(0, 0, 0), loc=(8, 0, 0)), "Thigh_L": dict(rot=(0, -20, 0)), "Thigh_R": dict(rot=(0, 6, 0))}

# ---------------------------------------------------------------- solve the guard of clip 0 frame 0 and place the shield
apply_pose({})
U = {}
base0 = run_base(0.0)
u, pose0, R0, H0 = solve_arm(base0, "guard_run")
U["guard_run"] = u
Rs = R_y(math.radians(-SHIELD_TILT_DEG))
state0 = world_state()[SHIELD_PART]
T0 = state0[1] - state0[0] @ REST_P[SHIELD_PART]
g = np.array(SHIELD_GRIP_CM)
org = H0 - Rs @ g
V_PLACED = SHIELD_V0 @ Rs.T + org
SHIELD_REST = (V_PLACED - T0) @ R0       # R0^T (v - t0) for row vectors
print("guard_run u", np.round(u, 2), "hand", np.round(H0, 2), "origin", np.round(org, 2))
# the other keys (the plate normal follows the forearm through R R0^T Rs)
for key, base in (("guard_stand", {}), ("wind", copy.deepcopy(ATT_WIND)), ("strike", copy.deepcopy(ATT_STRIKE))):
    init = tuple(U["guard_run"])
    u, pose_k, Rk, Hk = solve_arm(base, key, R0=R0, Rs=Rs, u0=init)
    U[key] = u
    n_ = Rk @ R0.T @ Rs @ np.array([1.0, 0, 0])
    print(key, "u", np.round(u, 2), "hand", np.round(Hk, 2), "target", KEYS[key]["target"], "plate pitch deg %.2f yaw deg %.2f" % (math.degrees(math.asin(n_[2])), math.degrees(math.atan2(n_[1], n_[0]))))


def left_arm(pose, u, extra_y=0.0):
    pose["UpperArm_L"] = dict(rot=(u[0], u[1] + extra_y, u[2]))
    pose["ForearmHand_L"] = dict(rot=(0, u[3], u[4]))
    return pose


def shield_run_pose(phase):
    pose = run_base(phase)
    return left_arm(pose, U["guard_run"], BOB_DEG * math.sin(2 * 2 * math.pi * phase))


def key_pose(base, u):
    return left_arm(copy.deepcopy(base), u)


KP_REST = key_pose(ATT_REST, U["guard_stand"])
KP_WIND = key_pose(ATT_WIND, U["wind"])
KP_STRIKE = key_pose(ATT_STRIKE, U["strike"])


def bash_pose(i):
    if i <= 2:
        return blend(KP_REST, KP_WIND, smooth((i + 1) / 3.0))
    if i <= 6:
        return blend(KP_WIND, KP_STRIKE, smooth((i - 2) / 4.0))
    return blend(KP_STRIKE, KP_REST, smooth((i - 6) / 8.0))


# ---------------------------------------------------------------- evaluate frames
POSES, DZ, FOOTZ, WORLD, VERTS, SHW, PARTW = [], [], [], [], [], [], []
for f in range(NCL):
    pose = shield_run_pose(f / NRUN) if f < NRUN else bash_pose(f - NRUN)
    dz = fit_ground(pose)
    POSES.append(pose)
    DZ.append(dz)
    FOOTZ.append(foot_min())
    WORLD.append(world_state())
    VERTS.append({n: world_verts(n) for n in PARTS})
    SHW.append(shield_world())
    PARTW.append({n: part_world_verts(n) for n in PARTS})
fit_ground(shield_run_pose(1.0))
V_NEXT = {n: world_verts(n) for n in PARTS}
loop_err = max(float(np.linalg.norm(V_NEXT[n] - VERTS[0][n], axis=1).max()) for n in PARTS)
apply_pose({})
V_REST_CHECK = max(float(np.linalg.norm(world_verts(n)[:len(REST_V[n])] - REST_V[n], axis=1).max()) for n in PARTS)
REST_V_FIT = {n: (np.concatenate([REST_V[n], SHIELD_REST]) if n == SHIELD_PART else REST_V[n]) for n in PARTS}
SH0_ERR = float(np.linalg.norm(SHW[0] - V_PLACED, axis=1).max())     # shield at frame 0 must equal the placed shield
end_err = max(float(np.linalg.norm(VERTS[NCL - 1][n] - VERTS[NRUN + 14][n], axis=1).max()) for n in PARTS)


def quat_from(R):
    q = Matrix(R.tolist()).to_quaternion()
    return np.array([q.x, q.y, q.z, q.w], float)


def qmat(q):
    x, y, z, w = q / np.linalg.norm(q)
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def qmat_raw(q):
    x, y, z, w = q
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
                     [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])


def half_neighbours(x):
    h = np.float16(x)
    return [float(np.nextafter(h, np.float16(-np.inf))), float(h), float(np.nextafter(h, np.float16(np.inf)))]


Q = np.zeros((NCL, len(PARTS), 4))
T = np.zeros((NCL, len(PARTS), 3))
for f in range(NCL):
    for pi, n in enumerate(PARTS):
        R, q = WORLD[f][n]
        Q[f, pi] = quat_from(R)
        T[f, pi] = q - R @ REST_P[n]
for lo, hi in ((0, NRUN), (NRUN, NCL)):
    for pi in range(len(PARTS)):
        if Q[lo, pi, 3] < 0:
            Q[lo, pi] *= -1
        for f in range(lo + 1, hi):
            if np.dot(Q[f, pi], Q[f - 1, pi]) < 0:
                Q[f, pi] *= -1
loop_dot_min = min(float(np.dot(Q[NRUN - 1, pi], Q[0, pi])) for pi in range(len(PARTS)))

# float16 storage with the best quaternion / translation choice per frame and part (the forearm L fit includes the shield vertices)
Qh = np.zeros_like(Q)
Th = np.zeros_like(T)
for f in range(NCL):
    for pi, n in enumerate(PARTS):
        tgt = VERTS[f][n]
        rest = REST_V_FIT[n]
        best = None
        for c in itertools.product(*[half_neighbours(Q[f, pi, k]) for k in range(4)]):
            qc = np.array(c, float)
            Rr, Rn = qmat_raw(qc), qmat(qc)
            br, bn = rest @ Rr.T, rest @ Rn.T
            for tl in ((tgt - br).mean(0), (tgt - bn).mean(0), (tgt - 0.5 * (br + bn)).mean(0)):
                tq = np.array([float(np.float16(x)) for x in tl])
                err = max(float(np.linalg.norm(br + tq - tgt, axis=1).max()), float(np.linalg.norm(bn + tq - tgt, axis=1).max()))
                if best is None or err < best[0]:
                    best = (err, qc, tq)
        Qh[f, pi], Th[f, pi] = best[1], best[2]
tex_new = np.zeros((NCL, TEXW, 4), np.float32)
for pi in range(len(PARTS)):
    tex_new[:, 2 * pi, :] = Qh[:, pi, :]
    tex_new[:, 2 * pi + 1, :3] = Th[:, pi, :]

# ---------------------------------------------------------------- source rows (T_ChaserAnim) read raw, copied exactly
sc = bpy.context.scene
ims = sc.render.image_settings


def read_exr_raw(path):
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
    names = [c[0] for c in ch]
    cidx = {nm: i for i, nm in enumerate(names)}
    arr = np.stack([r[1] for r in sorted(rows, key=lambda r: r[0])])
    return names, [c[1] for c in ch], w, h, comp, arr[:, :, [cidx["R"], cidx["G"], cidx["B"], cidx["A"]]]


src_names, src_types, sw_, sh_, src_comp, SRC_RGBA = read_exr_raw(SRC_EXR)
assert (sw_, sh_) == (TEXW, 31), (sw_, sh_)
tex = np.zeros((NROWS, TEXW, 4), np.float32)
tex[:NCL] = tex_new
tex[NCL:] = SRC_RGBA
half_ref = tex.astype(np.float16).astype(np.float32)
assert np.abs(half_ref - tex).max() == 0.0

EXR_W = os.path.join(WORK, "T_ShieldChaserAnim.exr")
ims.file_format = "OPEN_EXR"
ims.color_depth = "16"
ims.color_mode = "RGBA"
ims.exr_codec = "NONE"
img = bpy.data.images.new("T_ShieldChaserAnim", TEXW, NROWS, alpha=True, float_buffer=True)
img.colorspace_settings.name = "Non-Color"
img.pixels.foreach_set(np.flipud(tex).astype(np.float32).ravel())
img.pixels.update()
img.save_render(EXR_W, scene=sc)
out_names, out_types, ow_, oh_, out_comp, OUT_RGBA = read_exr_raw(EXR_W)
assert (ow_, oh_) == (TEXW, NROWS)
exr_vs_arrays = float(np.abs(OUT_RGBA - half_ref).max())
copy_diff_run = float(np.abs(OUT_RGBA[31:47] - SRC_RGBA[0:16]).max())
copy_diff_att = float(np.abs(OUT_RGBA[47:62] - SRC_RGBA[16:31]).max())
SAVED = OUT_RGBA.copy()

# ---------------------------------------------------------------- bake verification (saved EXR values on the rest mesh, shield included)
SQ = np.zeros((NCL, len(PARTS), 4))
ST = np.zeros((NCL, len(PARTS), 3))
for pi in range(len(PARTS)):
    SQ[:, pi] = SAVED[:NCL, 2 * pi]
    ST[:, pi] = SAVED[:NCL, 2 * pi + 1, :3]


def bake_err(QQ, TT, raw=False, which=None):
    per = {}
    for f in range(NCL):
        for pi, n in enumerate(PARTS):
            if which and n != which:
                continue
            Rm = qmat_raw(QQ[f, pi]) if raw else qmat(QQ[f, pi])
            new = REST_V_FIT[n] @ Rm.T + TT[f, pi]
            per[f] = max(per.get(f, 0.0), float(np.linalg.norm(new - VERTS[f][n], axis=1).max()))
    return per


def shield_bake_err(QQ, TT, raw=False):
    pi = SHIELD_PART_INDEX
    out = []
    for f in range(NCL):
        Rm = qmat_raw(QQ[f, pi]) if raw else qmat(QQ[f, pi])
        out.append(float(np.linalg.norm(SHIELD_REST @ Rm.T + TT[f, pi] - SHW[f], axis=1).max()))
    return out


e32 = bake_err(Q, T)
e16 = bake_err(SQ, ST)
e16r = bake_err(SQ, ST, raw=True)
sh16 = shield_bake_err(SQ, ST)
sh16r = shield_bake_err(SQ, ST, raw=True)
bake = dict(float32=dict(clip0_max_cm=max(e32[f] for f in range(NRUN)), clip1_max_cm=max(e32[f] for f in range(NRUN, NCL))),
            saved_exr_float16=dict(clip0_max_cm=max(e16[f] for f in range(NRUN)), clip1_max_cm=max(e16[f] for f in range(NRUN, NCL))),
            saved_exr_float16_not_renormalised=dict(clip0_max_cm=max(e16r[f] for f in range(NRUN)), clip1_max_cm=max(e16r[f] for f in range(NRUN, NCL))),
            shield_only_saved_exr_float16_max_cm=max(sh16), shield_only_not_renormalised_max_cm=max(sh16r))

# ---------------------------------------------------------------- clearance (shield against body)
GROUPS = dict(torso=["HeadTorso", "Pelvis"], legs=["Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"], arms=["UpperArm_L", "UpperArm_R", "ForearmHand_R"], hand_L=["ForearmHand_L"])
TRI = {}
for n in PARTS:
    me = OB[n].data
    me.calc_loop_triangles()
    TRI[n] = np.array([t.vertices[:] for t in me.loop_triangles], int)
SMESH = SHIELD.data
SMESH.calc_loop_triangles()
S_TRI = [tuple(t.vertices[:]) for t in SMESH.loop_triangles]


def group_geom(f, names):
    vs, ts, off = [], [], 0
    for n in names:
        vs.append(PARTW[f][n])
        ts.append(TRI[n] + off)
        off += len(PARTW[f][n])
    return np.concatenate(vs), np.concatenate(ts)


def clearance(f):
    sv = SHW[f]
    sb = BVHTree.FromPolygons([tuple(x) for x in sv], S_TRI, epsilon=0.0)
    res = {}
    for gname, names in GROUPS.items():
        gv, gt = group_geom(f, names)
        gb = BVHTree.FromPolygons([tuple(x) for x in gv], [tuple(t) for t in gt], epsilon=0.0)
        d1 = min(gb.find_nearest(Vector(p))[3] for p in sv)
        d2 = min(sb.find_nearest(Vector(p))[3] for p in gv)
        ov = len(gb.overlap(sb))
        res[gname] = dict(min_cm=float(min(d1, d2)), overlap_pairs=int(ov))
    return res


CLR = [clearance(f) for f in range(NCL)]


def clip_min(lo, hi, g):
    ff = list(range(lo, hi))
    k = min(ff, key=lambda f: CLR[f][g]["min_cm"])
    return dict(min_cm=round(CLR[k][g]["min_cm"], 3), frame=k, overlap_pairs_any_frame=int(sum(CLR[f][g]["overlap_pairs"] for f in ff)))


CLEAR = {c: {g: clip_min(lo, hi, g) for g in GROUPS} for c, (lo, hi) in (("clip0_shield_run", (0, NRUN)), ("clip1_shield_bash", (NRUN, NCL)))}
print("CLEAR", json.dumps(CLEAR))
SH_Z = [float(SHW[f][:, 2].min()) for f in range(NCL)]
SH_X_MAX = [float(SHW[f][:, 0].max()) for f in range(NCL)]
plate_pitch, plate_yaw = [], []
for f in range(NCL):
    R = WORLD[f][SHIELD_PART][0]
    n = R @ R0.T @ Rs @ np.array([1.0, 0, 0])
    plate_pitch.append(round(math.degrees(math.asin(n[2])), 2))
    plate_yaw.append(round(math.degrees(math.atan2(n[1], n[0])), 2))
print("pitch", plate_pitch, "yaw", plate_yaw)
print("shield zmin", [round(x, 1) for x in SH_Z], "xmax", [round(x, 1) for x in SH_X_MAX])
print("hand x", [round(float((WORLD[f][SHIELD_PART][0] @ (HAND_REST - REST_P[SHIELD_PART]) + WORLD[f][SHIELD_PART][1])[0]), 1) for f in range(NCL)])

# ---------------------------------------------------------------- combined mesh: shield into rest space, PartId, PartIndex, slots
sme = SHIELD.data
for v, co in zip(sme.vertices, SHIELD_REST):
    v.co = co.tolist()
sme.update()
sme.shade_flat()
pid = sme.uv_layers.new(name="PartId")
pid.data.foreach_set("uv", np.full(len(sme.loops) * 2, (SHIELD_PART_INDEX + 0.5) / 10.0, np.float32))
sme.uv_layers.active_index = 0
ca = sme.color_attributes.new("PartIndex", "FLOAT_COLOR", "CORNER")
ca.data.foreach_set("color", np.tile(np.array([SHIELD_PART_INDEX / 9.0, 0.0, 0.0, 1.0], np.float32), (len(sme.loops), 1)).ravel())
SHIELD.matrix_world = Matrix.Identity(4)
SHIELD.parent = None
# slot order: chaser material first (parts), shield, emblem (shield object holds the last two)
assert [m.name for m in sme.materials] == ["M_ChaserShield", "M_ChaserShield_Emblem"], [m.name for m in sme.materials]
for o in OB.values():
    o.data.shade_flat()
apply_pose({})
bpy.ops.object.select_all(action="DESELECT")
for o in list(OB.values()) + [SHIELD]:
    o.select_set(True)
bpy.context.view_layer.objects.active = OB["Pelvis"]
FBX = os.path.join(WORK, "SM_ShieldChaser_Meshy_Anim.fbx")
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
bpy.ops.export_scene.fbx(filepath=FBX, **EXPORT)
b_ = open(FBX, "rb").read()
usf = None
j = b_.find(b"UnitScaleFactor")
if j >= 0:
    k = b_.find(b"D", j + 20)
    usf = struct.unpack("<d", b_[k + 1:k + 9])[0]
# order of Model and Material nodes in the file (Unreal builds the slots from it)
FILE_ORDER = {}
try:
    from io_scene_fbx import parse_fbx
    root_ = parse_fbx.parse(FBX)[0]
    for el in root_.elems:
        if el.id == b"Objects":
            models, mats = [], []
            for gg in el.elems:
                if gg.id == b"Model":
                    models.append(gg.props[1].split(bytes(1))[0].decode())
                if gg.id == b"Material":
                    mats.append(gg.props[1].split(bytes(1))[0].decode())
            FILE_ORDER = dict(models=models, materials=mats)
except Exception as ex_:
    FILE_ORDER = dict(error=str(ex_))
print("FILE_ORDER", FILE_ORDER)

# ---------------------------------------------------------------- read back
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=FBX, colors_type="LINEAR")
by = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
RB = dict(objects=sorted(by), parents={n: (o.parent.name if o.parent else None) for n, o in by.items()}, per_part={})
tot = 0
chaser_unchanged = True
max_pos_diff = 0.0
for pi, n in enumerate(PARTS):
    o = by[n]
    me = o.data
    tris = sum(len(p.vertices) - 2 for p in me.polygons)
    tot += tris
    W = np.array([(o.matrix_world @ v.co)[:] for v in me.vertices])
    pd = float(np.abs(W - SRC_PART[n]["v"]).max()) if len(W) == len(SRC_PART[n]["v"]) else 1e9
    max_pos_diff = max(max_pos_diff, pd)
    u0 = np.array([l.uv[:] for l in me.uv_layers[0].data])
    u1 = np.array([l.uv[:] for l in me.uv_layers["LightMap"].data])
    u2 = np.array([l.uv[:] for l in me.uv_layers["PartId"].data])
    col = np.array([c.color[:] for c in me.color_attributes["PartIndex"].data])
    ok = (tris == SRC_PART[n]["tris"] and pd < 1e-3 and np.abs(u0 - SRC_PART[n]["uv0"]).max() < 1e-6 and np.abs(u1 - SRC_PART[n]["uv1"]).max() < 1e-6
          and np.abs(u2 - SRC_PART[n]["uv2"]).max() < 1e-7 and np.abs(col - SRC_PART[n]["col"]).max() < 1e-6 and [u.name for u in me.uv_layers] == SRC_PART[n]["uv"]
          and [m.name for m in me.materials] == SRC_PART[n]["mats"])
    chaser_unchanged &= bool(ok)
    RB["per_part"][n] = dict(tris=tris, max_vertex_diff_cm=round(pd, 5), unchanged_vs_source=bool(ok), partid_unique=np.unique(np.round(u2, 6)).tolist())
sh = by["Shield"]
sme2 = sh.data
sh_tris = sum(len(p.vertices) - 2 for p in sme2.polygons)
sh_u0 = np.array([l.uv[:] for l in sme2.uv_layers[0].data])
sh_u1 = np.array([l.uv[:] for l in sme2.uv_layers["LightMap"].data])
sh_u2 = np.array([l.uv[:] for l in sme2.uv_layers["PartId"].data], np.float64)
sh_col = np.array([c.color[:] for c in sme2.color_attributes["PartIndex"].data])
SW = np.array([(sh.matrix_world @ v.co)[:] for v in sme2.vertices])
exp_u2 = float(np.float32((SHIELD_PART_INDEX + 0.5) / 10.0))
tri_mat = np.bincount([p.material_index for p in sme2.polygons], minlength=2).tolist()
RB["shield"] = dict(tris=sh_tris, uv_layers=[u.name for u in sme2.uv_layers], materials=[m.name for m in sme2.materials], tris_per_material=tri_mat,
                    rest_vertices_vs_expected_max_cm=round(float(np.abs(SW - SHIELD_REST).max()), 5), world_matrix_identity=bool(max(abs(sh.matrix_world[i][j] - (1.0 if i == j else 0.0)) for i in range(4) for j in range(4)) < 1e-6),
                    partid=dict(expected=exp_u2, corners=len(sh_u2), unique_u=np.unique(sh_u2[:, 0]).tolist(), unique_v=np.unique(sh_u2[:, 1]).tolist(), max_abs_error=float(np.abs(sh_u2 - exp_u2).max())),
                    partindex_red_unique=np.unique(np.round(sh_col[:, 0], 6)).tolist(), expected_red=SHIELD_PART_INDEX / 9.0,
                    lightmap=dict(min=[float(x) for x in sh_u1.min(0)], max=[float(x) for x in sh_u1.max(0)], finite=bool(np.isfinite(sh_u1).all())))
RB["total_tris"] = tot + sh_tris
lm_ok = True
LM_ALL = {}
for n, o in by.items():
    a = np.array([l.uv[:] for l in o.data.uv_layers["LightMap"].data]).reshape(-1, 3, 2)
    area = 0.5 * np.abs((a[:, 1, 0] - a[:, 0, 0]) * (a[:, 2, 1] - a[:, 0, 1]) - (a[:, 2, 0] - a[:, 0, 0]) * (a[:, 1, 1] - a[:, 0, 1]))
    LM_ALL[n] = dict(in_0_1=bool(a.min() >= 0 and a.max() <= 1), finite=bool(np.isfinite(a).all()), zero_area_tris=int((area < 1e-12).sum()))
    lm_ok &= LM_ALL[n]["in_0_1"] and LM_ALL[n]["finite"]
allv = np.concatenate([np.array([(o.matrix_world @ v.co)[:] for v in o.data.vertices]) for o in by.values()])
RB["bounds_min_cm"] = [round(float(x), 2) for x in allv.min(0)]
RB["bounds_max_cm"] = [round(float(x), 2) for x in allv.max(0)]

# ---------------------------------------------------------------- previews (scene rebuilt from the exported FBX: parts + shield at rest, posed by the baked values)
RENDERS = []
PREV = os.path.join(OUT, "preview")
os.makedirs(PREV, exist_ok=True)
if not NORENDER:
    sc = bpy.context.scene
    OBJ = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
    img_ch = bpy.data.images.load(CH_TEX, check_existing=False)
    img_ch.colorspace_settings.name = "sRGB"
    for m in OBJ["Pelvis"].data.materials:
        for nd in m.node_tree.nodes:
            if nd.type == "TEX_IMAGE":
                nd.image = img_ch
    for m in OBJ["Shield"].data.materials:
        for nd in m.node_tree.nodes:
            if nd.type == "TEX_IMAGE":
                emb = ("Emblem" in m.name)
                im = bpy.data.images.load(os.path.join(SH_WORK, "T_ChaserShield_Emblem.png" if emb else "T_ChaserShield_BaseColor.png"), check_existing=True)
                im.colorspace_settings.name = "sRGB"
                nd.image = im
    # parts: drop the FBX hierarchy matrices, pose each object from the baked texture values (exactly what the material does)
    MW = {n: OBJ[n].matrix_world.copy() for n in OBJ}
    for n, o in OBJ.items():
        o.parent = None
        o.data.transform(MW[n])
        o.matrix_world = Matrix.Identity(4)
    bpy.context.view_layer.update()
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            continue
    sc.render.image_settings.file_format = "PNG"
    sc.render.image_settings.color_mode = "RGBA"
    sc.render.image_settings.color_depth = "8"
    sc.render.film_transparent = True
    sc.view_settings.view_transform = "Standard"
    sc.view_settings.look = "None"
    w = bpy.data.worlds.new("W")
    w.use_nodes = True
    sc.world = w
    bg = w.node_tree.nodes["Background"]
    bg.inputs["Color"].default_value = (1, 1, 1, 1)
    bg.inputs["Strength"].default_value = 0.35
    cam_d = bpy.data.cameras.new("Cam")
    cam_d.clip_start, cam_d.clip_end, cam_d.type = 1, 3000, "ORTHO"
    cam = bpy.data.objects.new("Cam", cam_d)
    sc.collection.objects.link(cam)
    sc.camera = cam
    sun_d = bpy.data.lights.new("Sun", "SUN")
    sun_d.energy = 1.0
    sun = bpy.data.objects.new("Sun", sun_d)
    sc.collection.objects.link(sun)
    floor_me = bpy.data.meshes.new("floor")
    floor_me.from_pydata([(-90, -60, -0.5), (90, -60, -0.5), (90, 60, -0.5), (-90, 60, -0.5)], [], [(0, 1, 2, 3)])
    fm = bpy.data.materials.new("floor")
    fm.use_nodes = True
    fm.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.35, 0.36, 0.38, 1)
    floor_me.materials.append(fm)
    floor_ob = bpy.data.objects.new("floor", floor_me)
    sc.collection.objects.link(floor_ob)
    GREY = np.array([0.76, 0.76, 0.76], dtype=np.float32)
    REST_MESH = {n: np.array([v.co[:] for v in OBJ[n].data.vertices], float) for n in PARTS}
    REST_MESH["Shield"] = np.array([v.co[:] for v in OBJ["Shield"].data.vertices], float)

    def pose_from_texture(row):
        """pose every object from the saved EXR row (quaternion + translation), as the Unreal material does"""
        for pi, n in enumerate(PARTS):
            Rm = qmat(SAVED[row, 2 * pi])
            t = SAVED[row, 2 * pi + 1, :3].astype(float)
            M = Matrix.Identity(4)
            for a in range(3):
                for b in range(3):
                    M[a][b] = Rm[a][b]
                M[a][3] = t[a]
            OBJ[n].matrix_world = M
        OBJ["Shield"].matrix_world = OBJ[SHIELD_PART].matrix_world.copy()
        bpy.context.view_layer.update()

    def dirv(az, el):
        a, e = math.radians(az), math.radians(el)
        return Vector((math.cos(e) * math.cos(a), math.cos(e) * math.sin(a), math.sin(e)))

    def shot(az, el, tgt, osc, px, py, path):
        d = dirv(az, el)
        target = Vector(tgt)
        cam_d.ortho_scale = osc
        sc.render.resolution_x, sc.render.resolution_y = px, py
        cam.location = target + d * 400
        cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
        side = d.cross(Vector((0, 0, 1)))
        lv = (d + Vector((0, 0, 0.6)) + side * 0.35).normalized()
        sun.rotation_euler = (-lv).to_track_quat("-Z", "Y").to_euler()
        sc.render.filepath = path
        bpy.ops.render.render(write_still=True)
        r = bpy.data.images.load(path)
        a = np.array(r.pixels[:], dtype=np.float32).reshape(py, px, 4)
        al = a[..., 3:4]
        out = np.clip(a[..., :3], 0, 1) * al + GREY * (1 - al)
        bpy.data.images.remove(r)
        return out

    def save_rgb(path, a):
        h, wd = a.shape[:2]
        im = bpy.data.images.new("o", wd, h, alpha=False)
        im.colorspace_settings.name = "sRGB"
        z = np.ones((h, wd, 4), np.float32)
        z[..., :3] = a
        im.pixels.foreach_set(z.ravel())
        im.filepath_raw = path
        im.file_format = "PNG"
        im.save()
        bpy.data.images.remove(im)

    VIEWS = dict(tq=(-40, 20, (14, 0, 66), 150, 750, 1000), side=(-90, 0, (14, 0, 66), 150, 750, 1000))
    FR_RUN = [0, 4, 8, 12]
    FR_BASH = [16, 18, 22, 26]
    tiles = {v: [] for v in VIEWS}
    for fr in FR_RUN + FR_BASH:
        pose_from_texture(fr)
        for vn, (az, el, tgt, osc, px, py) in VIEWS.items():
            a = shot(az, el, tgt, osc, px, py, os.path.join(WORK, "tmp.png"))
            tiles[vn].append(a)
            if vn == "tq":
                fn = "shield_chaser_%s_f%02d_tq.png" % ("run" if fr < 16 else "bash", fr)
                save_rgb(os.path.join(PREV, fn), a)
                RENDERS.append(fn)
    for vn in VIEWS:
        row1 = np.concatenate(tiles[vn][:4], axis=1)
        row2 = np.concatenate(tiles[vn][4:], axis=1)
        fn = "shield_chaser_contact_%s.png" % ("sheet" if vn == "tq" else "side")
        save_rgb(os.path.join(PREV, fn), np.concatenate([row2, row1], axis=0))
        RENDERS.append(fn)

# ---------------------------------------------------------------- report
if any(x > 0.05 for x in (bake["float32"]["clip0_max_cm"], bake["float32"]["clip1_max_cm"], bake["saved_exr_float16"]["clip0_max_cm"], bake["saved_exr_float16"]["clip1_max_cm"], loop_err)):
    WARN.append("a bake or loop error is above 0.05 cm")
bad_clear = [(c, g) for c in CLEAR for g in ("torso", "legs") if CLEAR[c][g]["overlap_pairs_any_frame"] or CLEAR[c][g]["min_cm"] < 1.0]
if bad_clear:
    WARN.append("shield to torso/legs distance below 1 cm or overlapping: %s" % bad_clear)
if not chaser_unchanged:
    WARN.append("a chaser part changed in the export read back")
rep = dict(
    task="task-219-01-shield-clips-bake", script="Tools/art/blender/shield_chaser_bake.py", blender=bpy.app.version_string,
    files=dict(texture="T_ShieldChaserAnim.exr", fbx="SM_ShieldChaser_Meshy_Anim.fbx", previews=RENDERS),
    tris=dict(chaser_parts=CHASER_TRIS, shield=SHIELD_TRIS, total_read_back=RB["total_tris"], expected_total=CHASER_TRIS + 979),
    texture=dict(width=TEXW, height=NROWS, format="OpenEXR RGBA half float, compression NONE, Non-Color", rows_from_top=True,
                 clips=CLIPS, texel_layout="same as T_ChaserAnim: x = 2p quaternion xyzw, x = 2p+1 translation cm, new = R v + t; shield follows part 4 (ForearmHand_L)",
                 clip2_rows_31_46_equal_T_ChaserAnim_rows_0_15_max_abs_diff=copy_diff_run, clip3_rows_47_61_equal_T_ChaserAnim_rows_16_30_max_abs_diff=copy_diff_att,
                 exr_reload_vs_arrays_max_abs=exr_vs_arrays, source_exr_compression_code=src_comp, written_compression_code=out_comp),
    clip0_loop_error_cm=loop_err, clip0_loop_min_quaternion_dot_frame15_to_0=loop_dot_min,
    shield_frame0_vs_placed_max_cm=SH0_ERR,
    bake_verification=dict(limit_cm=0.05, **bake, rest_pose_check_max_cm=V_REST_CHECK),
    shield_to_body_min_distance_cm=CLEAR,
    shield_to_body_per_frame=[dict(frame=f, torso=round(CLR[f]["torso"]["min_cm"], 2), legs=round(CLR[f]["legs"]["min_cm"], 2), arms=round(CLR[f]["arms"]["min_cm"], 2), hand_L=round(CLR[f]["hand_L"]["min_cm"], 2)) for f in range(NCL)],
    shield_pose=dict(plate_pitch_deg_per_frame=plate_pitch, plate_yaw_deg_per_frame=plate_yaw, lowest_vertex_z_cm_per_frame=[round(x, 1) for x in SH_Z], max_x_cm_per_frame=[round(x, 1) for x in SH_X_MAX]),
    shield_placement=dict(rule="v_rest = R0^T (v_placed - t0) with (R0, t0) = ForearmHand_L transform of clip 0 frame 0; placed = Ry(-%.1f deg) v + origin, origin = hand centre - Ry g" % SHIELD_TILT_DEG,
                          tilt_back_deg=SHIELD_TILT_DEG, grip_centre_cm_shield_space=list(SHIELD_GRIP_CM), hand_centre_rest_cm=[round(float(x), 3) for x in HAND_REST],
                          placed_origin_cm=[round(float(x), 3) for x in org], hand_centre_frame0_cm=[round(float(x), 3) for x in H0], placed_bounds_min=[round(float(x), 2) for x in V_PLACED.min(0)], placed_bounds_max=[round(float(x), 2) for x in V_PLACED.max(0)]),
    arm_solution_deg={k: [round(float(x), 3) for x in v] for k, v in U.items()}, keys=KEYS,
    uv2_check=dict(shield=RB["shield"]["partid"], shield_partindex_red=RB["shield"]["partindex_red_unique"], shield_expected_red=RB["shield"]["expected_red"],
                   chaser_parts_unchanged_vs_chaser_meshy_parts_fbx=chaser_unchanged, chaser_max_vertex_diff_cm=max_pos_diff, per_part=RB["per_part"]),
    slots=dict(file_order=FILE_ORDER, shield_materials=RB["shield"]["materials"], chaser_material=SRC_PART["Pelvis"]["mats"], shield_tris_per_material=RB["shield"]["tris_per_material"], expected="meshy_chaser, M_ChaserShield, M_ChaserShield_Emblem"),
    lightmap_uv1=dict(per_object=LM_ALL, all_in_0_1_and_finite=bool(lm_ok), note="every part keeps its own 0..1 packing like the chaser parts of chaser_meshy_parts.fbx (parts overlap each other in UV1; unchanged)"),
    export=dict(**{k: (sorted(v) if isinstance(v, set) else v) for k, v in EXPORT.items()}, fbx_unit_scale_factor_in_file=usf),
    readback=dict(objects=RB["objects"], parents=RB["parents"], bounds_min_cm=RB["bounds_min_cm"], bounds_max_cm=RB["bounds_max_cm"], shield=RB["shield"]),
    preview=dict(camera="orthographic 150 cm, 750 x 1000 px per tile, tq = azimuth -40 deg from +X toward +Y (camera front right of the robot), elevation 20, target (14, 0, 66); side = azimuth -90; light = sun 1.0 following the camera as in the ChaserShield previews, white world 0.35; poses taken from the saved EXR rows; frames are rows of the texture", frames=dict(clip0=FR_RUN if not NORENDER else [], clip1=FR_BASH if not NORENDER else [])),
    warnings=WARN)
with open(os.path.join(WORK, "SM_ShieldChaser_Meshy_Anim.report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
for fn in ("T_ShieldChaserAnim.exr", "SM_ShieldChaser_Meshy_Anim.fbx", "SM_ShieldChaser_Meshy_Anim.report.json"):
    shutil.copyfile(os.path.join(WORK, fn), os.path.join(OUT, fn))
print("DONE", json.dumps(dict(tris=RB["total_tris"], loop_err=loop_err, bake=bake, clear=CLEAR, warnings=WARN)))
