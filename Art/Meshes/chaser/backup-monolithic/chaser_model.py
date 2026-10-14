"""Chaser robot, ten rigid parts (Blender route, task-151-03, refined in task-151-04). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser_model.py -- --out Art/Meshes/chaser/SM_Chaser_parts.fbx [--render-dir Art/Meshes/chaser]
Units: 1 Blender unit = 1 cm. Height H = 124 cm = 2 x 62 (ARRChaser capsule half height 62, BP_Chaser BodyMesh 124 uu): sole z = 0 to the top of the cap.
Forward = +X, robot left = +Y. Ten objects, pivots at their joints, hierarchy Pelvis > HeadTorso > UpperArm > ForearmHand, Pelvis > Thigh > ShinFoot.
Flat shading, four flat materials (yellow_body, gunmetal, eye_red emissive, logo_plate textured decal). Dimensions are read from the validated sheet crops (fractions of H).
UV0: logo decal only (everything else collapsed to 0,0); UV1 LightMap: one island per face, no overlap, per part."""
import bpy, bmesh, math, json, os, sys
import numpy as np
from mathutils import Vector, Matrix

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/chaser/SM_Chaser_parts.fbx", "--render-dir": ""}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
OUTDIR = os.path.dirname(OUT)
os.makedirs(OUTDIR, exist_ok=True)
BUDGET = 3000
H = 124.0


def h(v):
    return v * H


def srgb(hx):
    c = [int(hx[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


HEX = {"yellow_body": "#F5BA20", "gunmetal": "#3A3A3A", "eye_red": "#FF1010", "logo_plate": "#FFD84A"}   # logo plate: a lighter yellow than the body, so the plate reads on the block
ORDER = ["yellow_body", "gunmetal", "eye_red", "logo_plate"]
MI = {n: i for i, n in enumerate(ORDER)}

# ---------------------------------------------------------------- logo decal texture (numpy, saved as a file)
LOGO = os.path.join(OUTDIR, "T_ChaserLogo.png")
N = 128
yy, xx = np.mgrid[0:N, 0:N]
px = (xx + 0.5) / N * 2 - 1
py = (yy + 0.5) / N * 2 - 1
def raw(hx):
    return np.array([int(hx[i:i + 2], 16) / 255 for i in (1, 3, 5)])


plate = raw(HEX["logo_plate"])
dark = raw(HEX["gunmetal"])
img = np.ones((N, N, 3)) * plate
border = (np.abs(px) > 0.90) | (np.abs(py) > 0.90)                                    # dark frame around the plate
img[border] = dark
hexm = (np.abs(py) < 0.80) & (np.abs(px) * 0.866 + np.abs(py) * 0.5 < 0.76)           # large dark shield (hexagon)
img[hexm] = dark
star = (np.abs(px) ** 0.65 + np.abs(py) ** 0.65) < 0.46 ** 0.65                       # four-point star glyph, plate yellow
img[star] = plate
rgba = np.ones((N, N, 4))
rgba[:, :, :3] = img
rgba = rgba[::-1]                                                                      # image row 0 is the bottom in Blender
timg = bpy.data.images.new("T_ChaserLogo", N, N, alpha=False)
timg.pixels = rgba.flatten().tolist()
timg.filepath_raw = LOGO
timg.file_format = "PNG"
timg.save()

# ---------------------------------------------------------------- materials
bpy.ops.wm.read_factory_settings(use_empty=True)
timg = bpy.data.images.load(LOGO)
timg.colorspace_settings.name = "sRGB"
MATS = {}
for n in ORDER:
    m = bpy.data.materials.new(n)
    c = srgb(HEX[n])
    m.diffuse_color = c
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = c
    b.inputs["Roughness"].default_value = 0.8
    if n == "eye_red":
        b.inputs["Emission Color"].default_value = c
        b.inputs["Emission Strength"].default_value = 4.0
    if n == "logo_plate":
        tx = m.node_tree.nodes.new("ShaderNodeTexImage")
        tx.image = timg
        tx.interpolation = "Linear"
        m.node_tree.links.new(tx.outputs["Color"], b.inputs["Base Color"])
    MATS[n] = m


# ---------------------------------------------------------------- part builder
class Part:
    def __init__(s, name, pivot, parent=None, sy=1.0):
        s.name, s.pivot, s.parent, s.sy = name, Vector(pivot), parent, sy
        s.bm = bmesh.new()
        s.decals = []        # (face-verts as Vector, uv list, mat) added after normals are fixed

    def P(s, x, y, z):                      # y is the lateral distance from the centre line, mirrored by sy
        return s.bm.verts.new((x, y * s.sy, z))

    def quad(s, vs, mat):
        f = s.bm.faces.new(vs)
        f.material_index = MI[mat]
        return f

    def loft(s, rings, mat, caps=True):
        for a, b in zip(rings[:-1], rings[1:]):
            n = len(a)
            for k in range(n):
                s.quad((a[k], a[(k + 1) % n], b[(k + 1) % n], b[k]), mat)
        if caps:
            s.quad(rings[0][::-1], mat)
            s.quad(rings[-1], mat)

    def abox(s, x0, x1, y0, y1, z0, z1, mat):
        def ring(z):
            return [s.P(x0, y0, z), s.P(x1, y0, z), s.P(x1, y1, z), s.P(x0, y1, z)]
        s.loft([ring(z0), ring(z1)], mat)

    def prism(s, p0, p1, w0, d0, w1, d1, mat, ch=0.28, bulge=0.07, at=0.4, simple=False):
        """Faceted box between two points (x, y, z with y lateral): octagonal sections w x d (full sizes, cm) at p0, near p0 (bulge), p1."""
        a0 = Vector((p0[0], p0[1] * s.sy, p0[2]))
        a1 = Vector((p1[0], p1[1] * s.sy, p1[2]))
        ax = (a1 - a0).normalized()
        lat = (Vector((0, 1, 0)) - ax * ax.y).normalized()
        dep = ax.cross(lat).normalized()
        wm, dm = max(w0, w1) * (1 + bulge), max(d0, d1) * (1 + bulge)
        rings = []
        for t, w, d in (((0.0, w0, d0), (1.0, w1, d1)) if simple else ((0.0, w0, d0), (at, wm, dm), (1.0, w1, d1))):
            c = a0 + (a1 - a0) * t
            hw, hd = w / 2, d / 2
            cc = ch * min(hw, hd)
            pts = [(hw, -hd + cc), (hw, hd - cc), (hw - cc, hd), (-hw + cc, hd), (-hw, hd - cc), (-hw, -hd + cc), (-hw + cc, -hd), (hw - cc, -hd)]
            rings.append([s.bm.verts.new(c + lat * u + dep * v) for u, v in pts])
        s.loft(rings, mat)

    def secs(s, rows, mat, ch=0.25):
        """Faceted solid from horizontal octagonal sections (z, x centre, y centre (lateral, outward), width, depth); the shin, the arms, the palm."""
        rings = []
        for z, xc, yc, w, d in rows:
            hw, hd = w / 2, d / 2
            cc = ch * min(hw, hd)
            pts = [(hw, -hd + cc), (hw, hd - cc), (hw - cc, hd), (-hw + cc, hd), (-hw, hd - cc), (-hw, -hd + cc), (-hw + cc, -hd), (hw - cc, -hd)]
            rings.append([s.P(xc + v, yc + u, z) for u, v in pts])
        s.loft(rings, mat)

    def oct_y(s, cx, cy, cz, r, length, mat):
        """Octagonal prism along the lateral axis (the knee prism)."""
        rings = []
        for yy_ in (cy - length / 2, cy + length / 2):
            rings.append([s.P(cx + r * math.cos(math.radians(22.5 + 45 * k)), yy_, cz + r * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)])
        s.loft(rings, mat)

    def profile(s, pts, y0, y1, mat):
        """Plain extrusion of an (x, z) profile between two lateral values."""
        s.loft([[s.P(x, y0, z) for x, z in pts], [s.P(x, y1, z) for x, z in pts]], mat)


parts = {}


def new_part(name, pivot, parent=None, sy=1.0):
    p = Part(name, pivot, parent, sy)
    parts[name] = p
    return p


# ---------------------------------------------------------------- Pelvis (root), pivot at the hip height on the centre line
PIV_PELVIS = (1.5, 0.0, h(0.585))
pel = new_part("Pelvis", PIV_PELVIS)
pel.prism((1.5, 0, h(0.662)), (1.5, 0, h(0.585)), h(0.135), h(0.165), h(0.085), h(0.150), "gunmetal", bulge=0.0)
pel.secs([(h(0.558), 2.25, 0.0, 11.5, 18.0), (h(0.572), 2.25, 0.0, 14.0, 21.5), (h(0.590), 2.25, 0.0, 13.0, 20.0)], "yellow_body", ch=0.3)        # yellow lower rim, chamfered

# ---------------------------------------------------------------- HeadTorso: chunky block, hood front, cap, recess + eye, logo decal
PIV_WAIST = (1.5, 0.0, h(0.655))
ht = new_part("HeadTorso", PIV_WAIST, parent="Pelvis")
W = h(0.120)            # block half width
CH = 3.0                # side chamfer
PROF = [(-8.4, h(0.655)), (22.5, h(0.672)), (32.2, h(0.840)), (27.4, h(0.903)), (20.5, h(0.925)), (-5.7, h(0.925)), (-8.9, h(0.840))]      # (27.4, .903) = chamfered top edge of the hood
cxz = (sum(p[0] for p in PROF) / len(PROF), sum(p[1] for p in PROF) / len(PROF))
LAY = [(-W, 0.80), (-W + 1.8, 0.93), (-W + 4.2, 1.0), (W - 4.2, 1.0), (W - 1.8, 0.93), (W, 0.80)]      # rounded shoulder line: three steps per side
L = []
for y, sc in LAY:
    L.append([ht.bm.verts.new((cxz[0] + (x - cxz[0]) * sc, y, cxz[1] + (z - cxz[1]) * sc)) for x, z in PROF])
n = len(PROF)
SPECIAL = (1, 2)        # edge 1 = lower front face (recess), edge 2 = upper front face (logo)
for li in range(len(LAY) - 1):
    for k in range(n):
        if li == 2 and k in SPECIAL:
            continue
        ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
ht.quad(L[0][::-1], "yellow_body")
ht.quad(L[-1], "yellow_body")


def bil(a, b, c, d, s, t):
    """bilinear on a quad a(s=0,t=0) b(s=1,t=0) c(s=1,t=1) d(s=0,t=1)"""
    return a.co * (1 - s) * (1 - t) + b.co * s * (1 - t) + c.co * s * t + d.co * (1 - s) * t


# lower front face: frame + recess + floor
fa, fb, fc, fd = L[2][1], L[3][1], L[3][2], L[2][2]            # bottom-left, bottom-right, top-right, top-left (y -, y +)
nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
if nrm.x < 0:
    nrm = -nrm
RS = (0.5 - 0.388, 0.5 + 0.388)    # recess across 0.776 of the centre face width = 62 % of the block width (the crop measures 69 %; criterion C2 text says 45 %)
RT = (0.12, 0.70)
DEPTH = 3.5
outer = [fa, fd, fc, fb]
st = [(RS[0], RT[0]), (RS[0], RT[1]), (RS[1], RT[1]), (RS[1], RT[0])]
inner = [ht.bm.verts.new(bil(fa, fb, fc, fd, s_, t_)) for s_, t_ in st]
floor = [ht.bm.verts.new(bil(fa, fb, fc, fd, s_, t_) - nrm * DEPTH) for s_, t_ in st]
for k in range(4):
    ht.quad((outer[k], outer[(k + 1) % 4], inner[(k + 1) % 4], inner[k]), "yellow_body")
    ht.quad((inner[k], inner[(k + 1) % 4], floor[(k + 1) % 4], floor[k]), "gunmetal")
ht.quad(floor[::-1], "gunmetal")
EYE_C = sum((v.co for v in floor), Vector()) / 4 + nrm * 0.2
EYE_R = 3.2
fa_, fb_ = L[2][1].co, L[3][1].co
ytan = (fb_ - fa_).normalized()
ztan = nrm.cross(ytan)
if ztan.z < 0:
    ztan = -ztan
# upper front face: plain yellow quad (the logo decal sits on top of it)
ua, ub, uc, ud = L[2][2], L[3][2], L[3][3], L[2][3]
ht.quad((ua, ub, uc, ud), "yellow_body")
unrm = (ub.co - ua.co).cross(ud.co - ua.co).normalized()
if unrm.x < 0:
    unrm = -unrm
# cap: small box on top of the block
cap_b = [ht.bm.verts.new(p) for p in ((-6.5, -h(0.086), h(0.925) - 0.2), (22.5, -h(0.086), h(0.925) - 0.2), (22.5, h(0.086), h(0.925) - 0.2), (-6.5, h(0.086), h(0.925) - 0.2))]
cap_t = [ht.bm.verts.new(p) for p in ((-1.0, -h(0.070), h(1.0)), (15.0, -h(0.070), h(1.0)), (15.0, h(0.070), h(1.0)), (-1.0, h(0.070), h(1.0)))]
ht.loft([cap_b, cap_t], "yellow_body")
# back: three large raised plates (centre plate and one on each side), profile (x, z) extruded across
CEN = [(-7.0, h(0.680)), (-11.0, h(0.690)), (-11.3, h(0.790)), (-8.2, h(0.885)), (-6.2, h(0.919)), (-5.0, h(0.919))]      # centre plate: vertical lower facet, leaning upper facet, chamfered top
SIDE = [(-7.0, h(0.680)), (-12.0, h(0.690)), (-7.4, h(0.900)), (-6.0, h(0.917)), (-5.0, h(0.917))]            # side plates: one long facet, deeper chamfer
for y0, y1, base, ia, ib in ((-h(0.054), h(0.054), CEN, 1.0, 1.0), (h(0.058), h(0.100), SIDE, 1.0, 0.4), (-h(0.100), -h(0.058), SIDE, 0.4, 1.0)):
    rg = []
    for y, k in ((y0, ia), (y1, ib)):          # side plates tilt outward: the outer edge sits closer to the block
        rg.append([ht.P(x - (1.0 - k) * 1.6 * (1 if 0 < i < len(base) - 1 else 0), y, z) for i, (x, z) in enumerate(base)])
    ht.loft(rg, "yellow_body")

# ---------------------------------------------------------------- arms (build left, mirror for right)
SH = (-2.5, 0.185, 0.86)
EL = (-4.0, 0.252, 0.755)
ARM_PARENT = {"L": 1.0, "R": -1.0}


def build_arm(side):
    sy = ARM_PARENT[side]
    pv = Vector((SH[0], SH[1] * H * sy, h(SH[2])))
    ua_ = new_part("UpperArm_" + side, pv, parent="HeadTorso", sy=sy)
    ua_.abox(-7.0, 3.0, h(0.113), h(0.170), h(0.815), h(0.905), "gunmetal")                      # dark shoulder connector
    # faceted upper arm: narrow top, wide shoulder cap just below, taper to the elbow
    ua_.secs([(h(0.930), -2.5, h(0.196), 10.0, 11.0), (h(0.903), -2.8, h(0.202), 14.0, 14.6), (h(0.848), -3.3, h(0.218), 13.4, 14.0), (h(0.775), -4.0, h(0.243), 10.2, 12.2)], "yellow_body", ch=0.3)
    ev = Vector((EL[0], EL[1] * H * sy, h(EL[2])))
    fa_ = new_part("ForearmHand_" + side, ev, parent="UpperArm_" + side, sy=sy)
    fa_.abox(-8.0, 0.0, h(0.210), h(0.294), h(0.728), h(0.768), "gunmetal")                     # dark elbow connector
    fa_.secs([(h(0.735), -4.5, h(0.264), 11.4, 12.0), (h(0.697), -3.2, h(0.275), 12.9, 13.4), (h(0.637), -1.2, h(0.286), 11.8, 12.2), (h(0.595), 0.3, h(0.294), 9.6, 11.2)], "yellow_body", ch=0.3)
    fa_.prism((2.0, h(0.272), h(0.715)), (5.4, h(0.290), h(0.623)), 6.8, 2.6, 6.0, 2.4, "yellow_body", ch=0.3, simple=True)    # guard plate on the front face of the forearm
    # curled claw: wrist ring, narrow palm, three fingers (two segments) side by side in depth hooking inward and forward, thumb on the inner side
    fa_.secs([(h(0.608), 0.8, 37.3, 6.2, 7.6), (h(0.578), 1.2, 37.8, 6.6, 8.2)], "gunmetal", ch=0.3)
    fa_.secs([(h(0.578), 1.8, 39.0, 5.8, 10.2), (h(0.563), 2.0, 39.4, 6.4, 11.0), (63.2, 2.4, 39.6, 5.4, 9.6)], "gunmetal", ch=0.3)
    for xi in (5.6, 2.3, -1.0):
        fa_.prism((xi, 39.2, 63.8), (xi + 0.9, 38.0, 59.4), 4.4, 3.4, 4.0, 3.2, "gunmetal", ch=0.25, simple=True)
        fa_.prism((xi + 0.9, 38.0, 59.8), (xi + 3.0, 36.4, 55.4), 4.0, 3.2, 3.4, 2.8, "gunmetal", ch=0.25, simple=True)
    fa_.prism((6.0, 36.6, 70.0), (8.0, 34.6, 65.6), 4.2, 3.6, 3.8, 3.2, "gunmetal", ch=0.25, simple=True)                  # thumb, two segments
    fa_.prism((8.0, 34.6, 65.9), (9.6, 32.4, 61.4), 3.8, 3.2, 3.2, 2.8, "gunmetal", ch=0.25, simple=True)


build_arm("L")
build_arm("R")

# ---------------------------------------------------------------- legs
HIP = (4.5, 0.105, 0.585)
KNEE = (1.5, 0.125, 0.383)


def build_leg(side):
    sy = 1.0 if side == "L" else -1.0
    pv = Vector((HIP[0], HIP[1] * H * sy, h(HIP[2])))
    th = new_part("Thigh_" + side, pv, parent="Pelvis", sy=sy)
    th.abox(-2.5, 9.0, h(0.074), h(0.115), h(0.435), h(0.590), "gunmetal")                         # upper leg bar
    th.secs([(h(0.431), 7.3, h(0.104), 9.5, 6.0), (h(0.379), 6.8, h(0.104), 11.8, 9.8), (h(0.335), 7.2, h(0.104), 10.3, 7.0), (h(0.308), 7.8, h(0.104), 8.0, 4.6)], "yellow_body", ch=0.3)   # faceted knee guard
    for y0, y1 in ((0.058, 0.079), (0.112, 0.138)):                                                # hip guard plates, chamfered
        th.profile([(4.0, h(0.580)), (14.0, h(0.585)), (15.5, h(0.615)), (12.0, h(0.640)), (4.0, h(0.628))], h(y0), h(y1), "yellow_body")
    th.oct_y(1.5, h(0.108), h(0.383), h(0.034), h(0.124), "gunmetal")                              # knee prism
    kv = Vector((KNEE[0], KNEE[1] * H * sy, h(KNEE[2])))
    sf = new_part("ShinFoot_" + side, kv, parent="Thigh_" + side, sy=sy)
    FY = h(0.1455)
    # one continuous tapering shin guard from the knee to the ankle
    sf.secs([(44.1, -0.2, 16.4, 8.0, 5.6), (40.2, -0.2, 16.6, 10.2, 9.6), (37.4, 0.0, 16.7, 10.6, 12.3), (30.0, -0.7, 17.2, 9.0, 8.8), (23.4, -0.8, 17.6, 7.0, 5.4)], "yellow_body", ch=0.3)
    sf.prism((7.0, h(0.130), h(0.300)), (2.6, h(0.142), 12.5), h(0.05), h(0.05), h(0.045), h(0.045), "gunmetal")     # dark leg bar in front of the shin
    sf.secs([(24.0, -2.6, 17.9, 5.0, 4.6), (14.0, -3.0, 18.0, 5.0, 5.0)], "gunmetal", ch=0.3)                        # dark neck behind the ankle
    sf.oct_y(1.3, FY, 13.3, 3.6, 15.6, "gunmetal")                                                                   # dark octagonal ankle joint
    sf.profile([(2.4, 22.6), (-1.0, 17.5), (3.5, 10.0), (8.3, 13.3)], FY - 3.2, FY + 3.2, "yellow_body")             # slanted instep shield
    for sgn in (-1, 1):                                                                                             # ankle cheek plates
        y0, y1 = sorted((FY + sgn * 3.3, FY + sgn * 5.9))
        sf.profile([(-3.0, 10.0), (-3.0, 19.5), (1.0, 21.0), (4.2, 18.5), (4.2, 10.0)], y0, y1, "yellow_body")
    sf.profile([(-0.5, 14.5), (-4.0, 18.6), (-6.8, 17.6), (-8.0, 8.0), (-0.5, 8.0)], FY - h(0.058), FY + h(0.058), "yellow_body")      # heel
    sf.profile([(17.0, h(0.030)), (17.0, 7.4), (8.5, 13.2), (-0.5, 13.2), (-0.5, h(0.030))], FY - 7.4, FY + 7.4, "yellow_body")        # foot upper
    sf.abox(-8.6, 21.5, FY - 8.3, FY + 8.3, 0.0, h(0.030), "gunmetal")                                              # sole
    for sgn in (-1, 1):                                                                                             # two angled toe blocks
        sf.profile([(15.0, 0.0), (21.5, 0.0), (19.8, 7.4), (15.0, 7.4)], FY + sgn * 5.4 - 2.6, FY + sgn * 5.4 + 2.6, "gunmetal")
    sf.abox(-8.6, -2.7, FY - 7.0, FY + 7.0, 0.0, 7.4, "gunmetal")                                                    # heel block


build_leg("L")
build_leg("R")

# ---------------------------------------------------------------- finish parts: normals, decals, UVs, objects
bpy.context.view_layer.update()
PIVOT_W = {k: p.pivot for k, p in parts.items()}


def finish(p):
    bm = p.bm
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    uv0 = bm.loops.layers.uv.new("UVMap")
    uv1 = bm.loops.layers.uv.new("LightMap")
    if p.name == "HeadTorso":
        # eye octagon (emissive), floor of the recess
        ev_ = [bm.verts.new(EYE_C + ytan * EYE_R * math.cos(math.radians(22.5 + 45 * k)) + ztan * EYE_R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
        f = bm.faces.new(ev_)
        if f.normal.dot(nrm) < 0:
            f.normal_flip()
        f.material_index = MI["eye_red"]
        # logo decal: copy of the covered region of the upper face, offset 0.25 cm, ONE planar UV
        s0, s1, t0, t1 = 0.5 - 0.17, 0.5 + 0.17, 0.46 - 0.40, 0.46 + 0.40
        cs = [(s0, t0), (s1, t0), (s1, t1), (s0, t1)]
        dv = [bm.verts.new(bil(ua, ub, uc, ud, s_, t_) + unrm * 0.25) for s_, t_ in cs]
        f = bm.faces.new(dv)
        if f.normal.dot(unrm) < 0:
            f.normal_flip()
        f.material_index = MI["logo_plate"]
        uvs = [(0, 0), (1, 0), (1, 1), (0, 1)]
        order = [v for v in dv]
        for l in f.loops:
            l[uv0].uv = uvs[order.index(l.vert)]
        decal_face = f
    bm.faces.ensure_lookup_table()
    bm.faces.index_update()
    bm.normal_update()
    # UV1 islands
    island = {}
    for f in bm.faces:
        nn = f.normal.normalized()
        if abs(nn.z) > 0.99:
            t, s_ = Vector((1, 0, 0)), Vector((0, 1, 0))
        else:
            t = Vector((-nn.y, nn.x, 0)).normalized()
            s_ = nn.cross(t).normalized()
        loc = [(l.vert.co.dot(t), l.vert.co.dot(s_)) for l in f.loops]
        mnx, mny = min(a for a, b in loc), min(b for a, b in loc)
        loc = [(a - mnx, b - mny) for a, b in loc]
        island[f.index] = (loc, max(a for a, b in loc), max(b for a, b in loc))
    LM, PAD = 256, 3.0

    def pack(scale):
        x = y = PAD
        rowh = 0.0
        out = {}
        for fi, (loc, w, hh) in sorted(island.items(), key=lambda kv: -kv[1][2]):
            pw, ph = w * scale, hh * scale
            if x + pw + PAD > LM:
                x = PAD
                y += rowh + PAD
                rowh = 0.0
            if x + pw + PAD > LM or y + ph + PAD > LM:
                return None
            out[fi] = (x, y)
            x += pw + PAD
            rowh = max(rowh, ph)
        return out
    lo_s, hi_s = 0.01, 20.0
    for _ in range(40):
        mid = (lo_s + hi_s) / 2
        if pack(mid):
            lo_s = mid
        else:
            hi_s = mid
    placed = pack(lo_s)
    decal_idx = decal_face.index if p.name == "HeadTorso" else -1
    for f in bm.faces:
        x, y = placed[f.index]
        for l, (lx, ly) in zip(f.loops, island[f.index][0]):
            l[uv1].uv = ((x + lx * lo_s) / LM, (y + ly * lo_s) / LM)
            if f.index != decal_idx:
                l[uv0].uv = (0.0, 0.0)
    for f in bm.faces:
        f.smooth = False
    # local coordinates: subtract the pivot
    bmesh.ops.translate(bm, vec=-p.pivot, verts=list(bm.verts))
    mesh = bpy.data.meshes.new(p.name)
    bm.to_mesh(mesh)
    bm.free()
    for m in ORDER:
        mesh.materials.append(MATS[m])
    for q in mesh.polygons:
        q.use_smooth = False
    ob = bpy.data.objects.new(p.name, mesh)
    bpy.context.scene.collection.objects.link(ob)
    return ob


objs = {k: finish(p) for k, p in parts.items()}
for k, p in parts.items():
    if p.parent:
        objs[k].parent = objs[p.parent]
        objs[k].location = p.pivot - parts[p.parent].pivot
    else:
        objs[k].location = p.pivot
bpy.context.view_layer.update()

# ---------------------------------------------------------------- checks
LM = 256


def lm_check(mesh):
    u1 = np.array([l.uv[:] for l in mesh.uv_layers["LightMap"].data])
    boxes = []
    for q in mesh.polygons:
        a = u1[q.loop_start:q.loop_start + q.loop_total] * LM
        boxes.append((a[:, 0].min(), a[:, 1].min(), a[:, 0].max(), a[:, 1].max()))
    min_gap, ov = 1e9, 0
    for i in range(len(boxes)):
        for j in range(i + 1, len(boxes)):
            A, B = boxes[i], boxes[j]
            g = max(max(B[0] - A[2], A[0] - B[2]), max(B[1] - A[3], A[1] - B[3]))
            min_gap = min(min_gap, g)
            if g < 0:
                ov += 1
    border = min(min(b[0], b[1], LM - b[2], LM - b[3]) for b in boxes)
    ok = ov == 0 and min_gap >= 2.0 and border >= 2.0 and u1.min() >= 0 and u1.max() <= 1
    return {"faces": len(boxes), "overlapping_face_pairs": ov, "bounds_min": round(float(u1.min()), 5), "bounds_max": round(float(u1.max()), 5),
            "min_gap_px_at_256": round(float(min_gap), 3), "min_border_px_at_256": round(float(border), 3), "required_margin_px": 2, "pass": bool(ok)}


warnings = []
per = {}
tot = 0
glo, ghi = Vector((1e9,) * 3), Vector((-1e9,) * 3)
for k, ob in objs.items():
    me = ob.data
    tris = sum(len(q.vertices) - 2 for q in me.polygons)
    tot += tris
    bb = [ob.matrix_world @ Vector(c) for c in ob.bound_box]
    lo = Vector((min(v.x for v in bb), min(v.y for v in bb), min(v.z for v in bb)))
    hi = Vector((max(v.x for v in bb), max(v.y for v in bb), max(v.z for v in bb)))
    glo = Vector((min(glo[i], lo[i]) for i in range(3)))
    ghi = Vector((max(ghi[i], hi[i]) for i in range(3)))
    lmr = lm_check(me)
    if not lmr["pass"]:
        warnings.append("LightMap check FAILED on " + k)
    per[k] = {"tris": tris, "bounds_min_cm": [round(c, 2) for c in lo], "bounds_max_cm": [round(c, 2) for c in hi],
              "pivot_world_cm": [round(c, 2) for c in ob.matrix_world.translation], "parent": ob.parent.name if ob.parent else None,
              "uv_layers": [u.name for u in me.uv_layers], "lightmap": lmr}
if tot > BUDGET:
    warnings.append("tris above %d budget" % BUDGET)
dim = [round(ghi[i] - glo[i], 2) for i in range(3)]
if abs(ghi.z - H) > 0.3 or abs(glo.z) > 0.01:
    warnings.append("height off %.0f cm" % H)


# joint overlaps: sampled by bbox intersection of parent/child (cm)
def overlap(a, b):
    la, ha, lb, hb = per[a]["bounds_min_cm"], per[a]["bounds_max_cm"], per[b]["bounds_min_cm"], per[b]["bounds_max_cm"]
    return [round(max(0.0, min(ha[i], hb[i]) - max(la[i], lb[i])), 2) for i in range(3)]


bbox_overlap = {}
for k, p in parts.items():
    if p.parent:
        bbox_overlap[p.parent + " / " + k] = overlap(p.parent, k)

def inside_depth(child, parent):
    """largest depth (cm) of a child vertex inside the closed parent mesh (ray parity for inside, BVH nearest-surface distance for the depth)"""
    from mathutils.bvhtree import BVHTree
    bvh = BVHTree.FromObject(parent, bpy.context.evaluated_depsgraph_get())
    inv = parent.matrix_world.inverted()
    dirs = [Vector(t).normalized() for t in ((0.3713, 0.5113, 0.7713), (-0.6211, 0.2417, 0.7441), (0.1931, -0.8123, 0.5511))]
    deepest = 0.0
    for v in child.data.vertices:
        lp = inv @ (child.matrix_world @ v.co)
        votes = 0
        for d in dirs:                                    # majority vote of three ray parities (a ray through an edge can miscount)
            o, cnt = lp.copy(), 0
            while True:
                hit = bvh.ray_cast(o, d)
                if hit[0] is None:
                    break
                cnt += 1
                o = hit[0] + d * 1e-4
            votes += cnt % 2
        if votes >= 2:
            nr = bvh.find_nearest(lp)
            if nr[0] is not None:
                deepest = max(deepest, nr[3])
    return deepest


pen = {}
for k, p in parts.items():
    if p.parent:
        pen[p.parent + " / " + k] = round(max(inside_depth(objs[k], objs[p.parent]), inside_depth(objs[p.parent], objs[k])), 2)
report = {
    "asset": "SM_Chaser_parts", "route": "blender", "script": "Tools/art/blender/chaser_model.py", "source_image": "vault/assets/chaser-turnaround.png (crops chaser-front, chaser-side, chaser-back)",
    "height_cm": H, "height_source": "ARRChaser capsule 24 x 62 half height (Source/RogueRigs/Enemies/RRChaser.cpp InitCapsuleSize(24, 62)) = 124 cm; bp-chaser BodyMesh 124 uu centred on the capsule; model from the sole to the top of the cap",
    "axes": "forward +X, robot left +Y, up +Z; _L parts at +Y", "units": "cm",
    "total_tris": tot, "budget_tris": BUDGET, "parts": per, "dimensions_cm": {"x": dim[0], "y": dim[1], "z": dim[2]},
    "bbox_min": [round(c, 2) for c in glo], "bbox_max": [round(c, 2) for c in ghi],
    "hierarchy": {"Pelvis": ["HeadTorso", "Thigh_L", "Thigh_R"], "HeadTorso": ["UpperArm_L", "UpperArm_R"], "UpperArm_L": ["ForearmHand_L"], "UpperArm_R": ["ForearmHand_R"],
                  "Thigh_L": ["ShinFoot_L"], "Thigh_R": ["ShinFoot_R"]},
    "joints_cm": {"Pelvis_root_hip_height": [round(c, 2) for c in PIVOT_W["Pelvis"]], "waist_HeadTorso": [round(c, 2) for c in PIVOT_W["HeadTorso"]],
                  "shoulder_L": [round(c, 2) for c in PIVOT_W["UpperArm_L"]], "shoulder_R": [round(c, 2) for c in PIVOT_W["UpperArm_R"]],
                  "elbow_L": [round(c, 2) for c in PIVOT_W["ForearmHand_L"]], "elbow_R": [round(c, 2) for c in PIVOT_W["ForearmHand_R"]],
                  "hip_L": [round(c, 2) for c in PIVOT_W["Thigh_L"]], "hip_R": [round(c, 2) for c in PIVOT_W["Thigh_R"]],
                  "knee_L": [round(c, 2) for c in PIVOT_W["ShinFoot_L"]], "knee_R": [round(c, 2) for c in PIVOT_W["ShinFoot_R"]]},
    "joint_bbox_overlap_cm_xyz": bbox_overlap, "joint_penetration_cm": pen,
    "materials": ORDER, "slots": ORDER, "paint_colours_srgb": HEX, "emissive_material": ["eye_red"],
    "decal": {"texture": "Art/Meshes/chaser/T_ChaserLogo.png", "uv": "UVMap (UV0) 0..1 on the logo quad only, other faces collapsed to (0,0)", "offset_cm": 0.25, "material": "logo_plate (Masked not needed: no alpha)"},
    "uv_layers": ["UVMap", "LightMap"], "shading": "flat", "warnings": warnings,
}
warnings.append("eye_red is emissive in Blender (strength 4, #FF1010): the FBX does not carry emission, the Unreal material sets the glow")

# ---------------------------------------------------------------- optional renders (fidelity loop)
RD = opt["--render-dir"]
if RD:
    RD = os.path.abspath(RD)
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            pass
    sc.view_settings.view_transform = "Standard"
    sc.render.film_transparent = False
    w = bpy.data.worlds.new("W")
    sc.world = w
    w.use_nodes = True
    bgn = w.node_tree.nodes["Background"]
    BG = 0.45
    bgn.inputs["Color"].default_value = (BG, BG, BG, 1)
    bgn.inputs["Strength"].default_value = 1.0
    suns = []
    for e in (3.0, 0.8):
        sun = bpy.data.lights.new("sun", "SUN")
        sun.energy = e
        so = bpy.data.objects.new("sun", sun)
        sc.collection.objects.link(so)
        suns.append(so)


    def light_from(cam_obj):
        """key sun comes from the camera side, raised; fill from above-behind"""
        fwd = (cam_obj.rotation_euler.to_matrix() @ Vector((0, 0, -1)))
        k = (fwd + Vector((0, 0, -0.9))).normalized()
        suns[0].rotation_euler = k.to_track_quat("-Z", "Y").to_euler()
        f = (-fwd * 0.5 + Vector((0, 0, -1))).normalized()
        suns[1].rotation_euler = f.to_track_quat("-Z", "Y").to_euler()
    cam = bpy.data.cameras.new("cam")
    cam.type = "ORTHO"
    co = bpy.data.objects.new("cam", cam)
    sc.collection.objects.link(co)
    sc.camera = co
    # crop measurements: (res w, h, bottom row of sole, px per cm, camera lateral coordinate, rotation, axis)
    VIEWS = {
        "front": ((550, 790), 757, 713 / H, (300.0, -0.28, None), (90, 0, 90)),
        "side": ((320, 790), 757, 736 / H, (None, 300.0, None), (90, 0, 180)),
        "back": ((560, 790), 757, 715 / H, (-300.0, -1.44, None), (90, 0, -90)),
    }
    for vn, (res, bot, ppc, loc, rot) in VIEWS.items():
        cz = (bot - res[1] / 2) / ppc
        if vn == "side":
            cx = (221 - res[0] / 2) / 5.935
            location = (cx, 300.0, cz)
        else:
            location = (loc[0], loc[1], cz)
        co.location = location
        co.rotation_euler = tuple(math.radians(a) for a in rot)
        cam.ortho_scale = res[1] / ppc
        bpy.context.view_layer.update()
        light_from(co)
        sc.render.resolution_x, sc.render.resolution_y = res
        sc.render.resolution_percentage = 100
        sc.render.filepath = os.path.join(RD, "render_%s.png" % vn)
        bpy.ops.render.render(write_still=True)
    # close-ups, ortho: hand and calf, front and side (right limbs in front view as on the crop; left limbs in side view)
    CLOSE = {"hand_front": ((528, 528), (300.0, -36.0, 65.0), (90, 0, 90), 22.0), "hand_side": ((528, 528), (3.5, 300.0, 65.0), (90, 0, 180), 22.0),
             "calf_front": ((528, 528), (300.0, -17.0, 24.0), (90, 0, 90), 44.0), "calf_side": ((528, 528), (0.5, 300.0, 24.0), (90, 0, 180), 44.0)}
    for vn, (res, loc, rot, osz) in CLOSE.items():
        co.location = loc
        co.rotation_euler = tuple(math.radians(a) for a in rot)
        cam.ortho_scale = osz
        bpy.context.view_layer.update()
        light_from(co)
        sc.render.resolution_x, sc.render.resolution_y = res
        sc.render.filepath = os.path.join(RD, "closeup_%s.png" % vn)
        bpy.ops.render.render(write_still=True)
    # three-quarter
    tgt = Vector((0, 0, 62))
    d = Vector((math.cos(math.radians(-35)) * math.cos(math.radians(25)), math.sin(math.radians(-35)) * math.cos(math.radians(25)), math.sin(math.radians(25)))) * 300
    co.location = tgt + d
    co.rotation_euler = (tgt - co.location).to_track_quat("-Z", "Y").to_euler()
    cam.ortho_scale = 150
    bpy.context.view_layer.update()
    light_from(co)
    sc.render.resolution_x, sc.render.resolution_y = 800, 900
    sc.render.filepath = os.path.join(RD, "render_threequarter.png")
    bpy.ops.render.render(write_still=True)
    for o in [co] + suns:
        bpy.data.objects.remove(o)

# ---------------------------------------------------------------- export
bpy.ops.object.select_all(action="DESELECT")
for o in objs.values():
    o.select_set(True)
bpy.context.view_layer.objects.active = objs["Pelvis"]
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
RP = os.path.splitext(OUT)[0] + ".report.json"
if os.path.exists(RP):
    try:
        old = json.load(open(RP, encoding="utf-8"))
        for key in ("fidelity", "refinement"):
            if key in old:
                report[key] = old[key]
    except Exception:
        pass
with open(RP, "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print("TOTAL_TRIS", tot, "DIM", dim)
print(json.dumps({k: v["tris"] for k, v in per.items()}))
print("WARNINGS", warnings)
