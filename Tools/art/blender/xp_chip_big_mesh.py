"""Big XP block mesh: data chip (Blender route, task-131-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_chip_big_mesh.py -- --out Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx
Units: 1 Blender unit = 1 cm. Pivot bottom centre. Footprint 30 x 30 cm = the placeholder XP cube (/Engine/BasicShapes/Cube 100 cm x PickupMesh scale 0.3);
the 1.8 scale of the big block is applied in game (BP_PickupXPBig root scale 0.54), not baked. Flat shading, flat per-material colours, emissive magenta core."""
import bpy, bmesh, math, json, os, sys
import numpy as np
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/xp-chip-big-mesh/SM_XPChipBig.fbx"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
os.makedirs(os.path.dirname(OUT), exist_ok=True)
BUDGET = 1000
FOOT = 30.0


def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


HEX = {"board": "#2B2E34", "casing": "#3C4048", "legs": "#8A9099", "core": "#FF26BF"}
bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, h in HEX.items():
    m = bpy.data.materials.new("M_XPChipBig_" + n)
    c = srgb(h)
    m.diffuse_color = c
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = c
    b.inputs["Roughness"].default_value = 0.8
    if n == "core":
        b.inputs["Emission Color"].default_value = c
        b.inputs["Emission Strength"].default_value = 4.0
    mats[n] = m
mesh = bpy.data.meshes.new("SM_XPChipBig")
obj = bpy.data.objects.new("SM_XPChipBig", mesh)
bpy.context.scene.collection.objects.link(obj)
slot = {}


def mslot(n):
    if n not in slot:
        mesh.materials.append(mats[n])
        slot[n] = len(mesh.materials) - 1
    return slot[n]


bm = bmesh.new()


def quad(vs, mat):
    f = bm.faces.new(vs)
    f.material_index = mslot(mat)
    return f


def sq(z, hx, hy, cx=0.0, cy=0.0):
    return [bm.verts.new((cx - hx, cy - hy, z)), bm.verts.new((cx + hx, cy - hy, z)),
            bm.verts.new((cx + hx, cy + hy, z)), bm.verts.new((cx - hx, cy + hy, z))]


def loft(rings, mat, bottom=True, top=True):
    for a, b in zip(rings[:-1], rings[1:]):
        for k in range(len(a)):
            quad((a[k], a[(k + 1) % len(a)], b[(k + 1) % len(a)], b[k]), mat)
    if bottom:
        quad(rings[0][::-1], mat)
    if top:
        quad(rings[-1], mat)


def box(x0, x1, y0, y1, z0, z1, mat, bottom=True):
    hx, hy, cx, cy = (x1 - x0) / 2, (y1 - y0) / 2, (x0 + x1) / 2, (y0 + y1) / 2
    loft([sq(z0, hx, hy, cx, cy), sq(z1, hx, hy, cx, cy)], mat, bottom=bottom)


def hexring(z, r, cx, cy):
    return [bm.verts.new((cx + r * math.cos(math.radians(60 * k)), cy + r * math.sin(math.radians(60 * k)), z)) for k in range(6)]


H = FOOT / 2
ZB = 3.4                                                                    # board top (tiles), 11.3 % of width
box(-H, H, -H, H, 0.0, 2.6, "board")                                       # base slab, seams show as 0.8 cm steps between tiles
G = 0.3
for x0, x1 in ((-H, -G), (G, H)):
    for y0, y1 in ((-H, -G), (G, H)):
        box(x0, x1, y0, y1, 2.6, ZB, "board", bottom=False)                # four tiles
loft([sq(ZB, 10.2, 10.2), sq(ZB + 1.0, 10.2, 10.2)], "casing")             # flange 20.4
loft([sq(ZB + 1.0, 9.5, 9.5), sq(6.0, 9.5, 9.5), sq(9.5, 6.9, 6.9)], "casing", top=False)     # body 18 wide, slope to 14 at z 9.5 (rise 6.1 = 20 %)
loft([sq(9.5, 6.2, 6.2), sq(7.8, 6.2, 6.2)], "casing", bottom=False, top=False)   # recess wall 12.4
loft([sq(9.5, 6.9, 6.9), sq(9.5, 6.2, 6.2)], "casing", bottom=False, top=False)   # rim
# stepped faceted core pyramid 11.6 wide (39 % of 30), magenta
loft([sq(7.8, 5.8, 5.8), sq(8.3, 5.8, 5.8), sq(8.9, 4.2, 4.2), sq(9.0, 4.2, 4.2), sq(9.7, 2.4, 2.4)], "core", bottom=False)
# corner notches: small dark pits on the four top corners of the slope
for sx in (-1, 1):
    for sy in (-1, 1):
        cx, cy = sx * 7.6, sy * 7.6
        box(cx - 0.8, cx + 0.8, cy - 0.8, cy + 0.8, 8.2, 9.6, "board", bottom=False)
# six L-shaped legs, 3 on +x side and 3 on -y side (facing the viewer): one extruded L profile each (arm out of the casing, chamfered elbow, drop, foot)
PROFILE = [(10.8, ZB), (13.4, ZB), (13.4, ZB + 0.9), (12.4, ZB + 0.9), (12.4, 5.9), (11.7, 6.5), (9.2, 6.5), (9.2, 5.0), (10.8, 5.0)]


def leg(side, c):
    vs = []
    for w in (-1.3, 1.3):
        ring = []
        for (u, z) in PROFILE:
            ring.append(bm.verts.new((u, c + w, z) if side == "x" else (c + w, -u, z)))
        vs.append(ring)
    fs = []
    n = len(PROFILE)
    for k in range(n):
        fs.append(quad((vs[0][k], vs[0][(k + 1) % n], vs[1][(k + 1) % n], vs[1][k]), "legs"))
    fs.append(quad(vs[0][::-1], "legs"))
    fs.append(quad(vs[1], "legs"))
    bmesh.ops.recalc_face_normals(bm, faces=fs)


for c in (-4.6, 0.0, 4.6):
    leg("x", c)
    leg("y", c)
# box components: two larger on opposite sides, one smaller (dark body, light end cap on the larger ones)
box(-14.2, -10.6, 0.5, 8.0, ZB, ZB + 2.2, "casing", bottom=False)
box(-14.3, -10.5, 0.5, 1.6, ZB, ZB + 2.3, "legs", bottom=False)
box(10.6, 14.2, 6.4, 11.0, ZB, ZB + 2.2, "casing", bottom=False)
box(10.5, 14.3, 6.4, 7.4, ZB, ZB + 2.3, "legs", bottom=False)
box(-9.0, -5.0, 11.2, 13.6, ZB, ZB + 1.4, "casing", bottom=False)
# light traces with right-angle bends, thin raised strips
def trace(pts, w=0.5, hgt=0.25):
    for (ax, ay), (bx, by) in zip(pts[:-1], pts[1:]):
        box(min(ax, bx) - w, max(ax, bx) + w, min(ay, by) - w, max(ay, by) + w, ZB, ZB + hgt, "legs", bottom=False)
trace([(-14.5, -2.0), (-11.5, -2.0), (-11.5, -8.5)])
trace([(1.0, 14.5), (1.0, 12.0), (8.0, 12.0)])
trace([(7.5, -14.5), (7.5, -10.8), (10.2, -10.8)])
trace([(14.5, -9.0), (11.5, -9.0), (11.5, -6.8)])
# four grey hex bolts at the corners, 3.4 cm across, 1.7 cm tall
for bx, by in ((-12.6, -12.6), (12.6, -12.6), (-12.6, 12.6), (12.6, 12.6)):
    n0 = len(bm.faces)
    loft([hexring(ZB, 1.75, bx, by), hexring(ZB + 1.7, 1.75, bx, by)], "legs", top=False)
    loft([hexring(ZB + 1.7, 1.75, bx, by), hexring(ZB + 1.9, 1.2, bx, by)], "legs", bottom=False)
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces)[n0:])
pass
bm.faces.ensure_lookup_table()
bm.faces.index_update()
bm.normal_update()

# ---- UV0 "UVMap": planar projection per face, 100 cm per UV unit. UV1 "LightMap": one island per face, packed at 256 px, margin >= 2 px.
uv0 = bm.loops.layers.uv.new("UVMap")
uv1 = bm.loops.layers.uv.new("LightMap")
TILE = 100.0


def basis(f):
    n = f.normal.normalized()
    if abs(n.z) > 0.99:
        return Vector((1, 0, 0)), Vector((0, 1, 0))
    t = Vector((-n.y, n.x, 0)).normalized()
    s = n.cross(t).normalized()
    return t, s


island = {}
for f in bm.faces:
    t, s = basis(f)
    loc = [(l.vert.co.dot(t), l.vert.co.dot(s)) for l in f.loops]
    mnx = min(p[0] for p in loc)
    mny = min(p[1] for p in loc)
    for l, (x, y) in zip(f.loops, loc):
        l[uv0].uv = (x / TILE, y / TILE)
    loc = [(x - mnx, y - mny) for x, y in loc]
    island[f.index] = (loc, max(p[0] for p in loc), max(p[1] for p in loc))
LM, PAD = 256, 3.0


def pack(scale):
    x = y = PAD
    rowh = 0.0
    out = {}
    for fi, (loc, w, h) in sorted(island.items(), key=lambda kv: -kv[1][2]):
        pw, ph = w * scale, h * scale
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
for f in bm.faces:
    x, y = placed[f.index]
    for l, (lx, ly) in zip(f.loops, island[f.index][0]):
        l[uv1].uv = ((x + lx * lo_s) / LM, (y + ly * lo_s) / LM)
for f in bm.faces:
    f.smooth = False
bm.to_mesh(mesh)
bm.free()
for p in mesh.polygons:
    p.use_smooth = False

# ---- checks, report, export
bb = [Vector(c) for c in obj.bound_box]
lo = Vector((min(v.x for v in bb), min(v.y for v in bb), min(v.z for v in bb)))
hi = Vector((max(v.x for v in bb), max(v.y for v in bb), max(v.z for v in bb)))
tris = sum(len(p.vertices) - 2 for p in mesh.polygons)
u0a = np.array([l.uv[:] for l in mesh.uv_layers["UVMap"].data])
u1a = np.array([l.uv[:] for l in mesh.uv_layers["LightMap"].data])
boxes = []
for p in mesh.polygons:
    q = u1a[p.loop_start:p.loop_start + p.loop_total] * LM
    boxes.append((q[:, 0].min(), q[:, 1].min(), q[:, 0].max(), q[:, 1].max()))
min_gap = 1e9
overlaps = 0
for i in range(len(boxes)):
    for j in range(i + 1, len(boxes)):
        A, B = boxes[i], boxes[j]
        g = max(max(B[0] - A[2], A[0] - B[2]), max(B[1] - A[3], A[1] - B[3]))
        min_gap = min(min_gap, g)
        if g < 0:
            overlaps += 1
border = min(min(b[0], b[1], LM - b[2], LM - b[3]) for b in boxes)
lm_ok = overlaps == 0 and min_gap >= 2.0 and border >= 2.0 and u1a.min() >= 0 and u1a.max() <= 1
dens = []
for p in mesh.polygons:
    ls = list(range(p.loop_start, p.loop_start + p.loop_total))
    for k in range(len(ls)):
        a_, b_ = ls[k], ls[(k + 1) % len(ls)]
        d3 = (mesh.vertices[mesh.loops[a_].vertex_index].co - mesh.vertices[mesh.loops[b_].vertex_index].co).length
        d2 = np.linalg.norm(u0a[a_] - u0a[b_])
        if d3 > 1e-6 and d2 > 1e-9:
            dens.append(d3 / d2)
warnings = []
if not lm_ok:
    warnings.append("LightMap check FAILED")
if tris > BUDGET:
    warnings.append("tris above %d budget" % BUDGET)
if abs((hi.x - lo.x) - FOOT) > 0.01 or abs((hi.y - lo.y) - FOOT) > 0.01:
    warnings.append("footprint off 30 cm")
if abs(lo.z) > 1e-4 or abs((lo.x + hi.x) / 2) > 1e-3 or abs((lo.y + hi.y) / 2) > 1e-3:
    warnings.append("pivot not bottom centre")
warnings.append("M_XPChipBig_core is emissive in Blender (strength 4, #FF26BF): the FBX does not carry emission, the Unreal material sets the glow (MI_PickupXPBig)")
report = {
    "asset": "SM_XPChipBig", "route": "blender", "script": "Tools/art/blender/xp_chip_big_mesh.py", "source_image": "vault/assets/xp-chip-big.png",
    "tris_out": tris, "budget_tris": BUDGET,
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "footprint_target_cm": {"x": FOOT, "y": FOOT, "source": "BP_PickupXP PickupMesh /Engine/BasicShapes/Cube (100 cm) x scale 0.3; big block scale 1.8 (root 0.54) applied in game, not baked"},
    "pivot": "bottom", "bbox_min": [round(c, 3) for c in lo], "bbox_max": [round(c, 3) for c in hi],
    "materials": [m.name for m in mesh.materials],
    "paint_colours_srgb": {"M_XPChipBig_" + k: v for k, v in HEX.items()}, "emissive_material": "M_XPChipBig_core",
    "uv_layers": [u.name for u in mesh.uv_layers],
    "uv_checks": {"UVMap": {"tile_cm": 100.0, "cm_per_uv_unit_min": round(min(dens), 3), "cm_per_uv_unit_max": round(max(dens), 3)},
                  "LightMap": {"faces": len(boxes), "triangles": tris, "overlapping_face_pairs": overlaps,
                               "bounds_min": round(float(u1a.min()), 5), "bounds_max": round(float(u1a.max()), 5),
                               "min_gap_px_at_256": round(float(min_gap), 3), "min_border_px_at_256": round(float(border), 3),
                               "required_margin_px": 2, "pass": bool(lm_ok)}},
    "shading": "flat", "warnings": warnings,
}
bpy.ops.object.select_all(action="DESELECT")
obj.select_set(True)
bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print(json.dumps(report))
