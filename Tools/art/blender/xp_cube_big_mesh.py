"""Big XP block mesh: data cube (Blender route, task-217-01), simplified from the owner's Meshy cube. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_cube_big_mesh.py -- --out Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx
Units: 1 Blender unit = 1 cm. Pivot: geometric centre of the body bbox (--pivot center|bottom). 30 x 30 x 27.5 cm (bolt tips set the extent), like the chip footprint.
Body = chamfered cuboid; per face (all six identical): recessed square window (dark frame, lighter centre) and four hex bolts; eight truncated corner caps.
Flat shading, flat per-material colours sampled from the Meshy base colour texture, emissive magenta window. glow_disc built as in xp_chip_big_mesh.py."""
import bpy, bmesh, math, json, os, sys
import numpy as np
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/xp-cube-big-mesh/SM_XPCubeBig.fbx"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
os.makedirs(os.path.dirname(OUT), exist_ok=True)
BUDGET = 844
FOOT = 30.0
HEIGHT = 27.5          # Meshy proportion 0.95 / 0.87 -> width/height 1.09


def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


# sampled from Meshy_AI_Neon_Power_Cube_1017063448_texture.png (dominant clusters: dark body (45,44,48), light grey (134,134,135), saturated magenta (250,17,248), pink centre (251,129,253))
HEX = {"body": "#2D2C30", "caps": "#868687", "core_dark": "#FA11F8", "core_bright": "#FB81FD"}
bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, h in HEX.items():
    m = bpy.data.materials.new("M_XPCubeBig_" + n)
    c = srgb(h)
    m.diffuse_color = c
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = c
    b.inputs["Roughness"].default_value = 0.8
    if n.startswith("core"):
        b.inputs["Emission Color"].default_value = c
        b.inputs["Emission Strength"].default_value = 4.0
    mats[n] = m
mesh = bpy.data.meshes.new("SM_XPCubeBig")
obj = bpy.data.objects.new("SM_XPCubeBig", mesh)
bpy.context.scene.collection.objects.link(obj)
slot = {}


def mslot(n):
    if n not in slot:
        mesh.materials.append(mats[n])
        slot[n] = len(mesh.materials) - 1
    return slot[n]


for n in HEX:
    mslot(n)
bm = bmesh.new()

# ---- dimensions (cm)
BX = BY = 13.5                     # body half extents in X, Y
BZ = HEIGHT / 2 - 1.5              # 12.25
HALF = {0: BX, 1: BY, 2: BZ}
CH = 1.2                           # body edge chamfer
BOLT_H = 1.5                       # bolt height: tips reach 15.0 / 13.75
BOLT_R0, BOLT_R1, BOLT_POS = 1.2, 1.0, 7.0
HOLE, WALL_D, FR_IN, CEN_TOP, CEN_UP = 5.4, 1.0, 3.2, 2.5, 0.5
CAP_S, CAP_T, CAP_OUT = 5.8, 3.2, 0.5


def flip_outward(faces, ref):
    for f in faces:
        f.normal_update()
        if (f.calc_center_median() - ref).dot(f.normal) < 0:
            f.normal_flip()


# ---- chamfered cuboid
bmesh.ops.create_cube(bm, size=2.0)
for v in bm.verts:
    v.co = Vector((v.co.x * BX, v.co.y * BY, v.co.z * BZ))
bmesh.ops.bevel(bm, geom=list(bm.edges), offset=CH, offset_type="OFFSET", segments=1, affect="EDGES")
bm.faces.ensure_lookup_table()
big = [f for f in bm.faces if len(f.verts) == 4 and max(abs(c) for c in f.normal) > 0.999]
assert len(big) == 6, len(big)
for f in bm.faces:
    f.material_index = mslot("body")


def frame_of(n):
    u = Vector((1, 0, 0)) if abs(n.x) < 0.9 else Vector((0, 1, 0))
    return u, n.cross(u)


def ring_pts(c, u, v, n, h, w):
    return [bm.verts.new(c + n * w + u * su * h + v * sv * h) for su, sv in ((1, -1), (1, 1), (-1, 1), (-1, -1))]


def band(a, b, mat):
    for k in range(4):
        f = bm.faces.new((a[k], a[(k + 1) % 4], b[(k + 1) % 4], b[k]))
        f.material_index = mslot(mat)


for F in big:
    n = F.normal.copy().freeze()
    axis = [abs(c) > 0.5 for c in n].index(True)
    c = n * HALF[axis]
    u, v = frame_of(n)
    ang = lambda p: math.atan2((p.co - c).dot(v), (p.co - c).dot(u))
    outer = sorted(F.verts, key=ang)
    bm.faces.remove(F)

    def ring(h, w):
        return sorted(ring_pts(c, u, v, n, h, w), key=ang)
    r0, r1, r2, r3 = ring(HOLE, 0.0), ring(HOLE, -WALL_D), ring(FR_IN, -WALL_D), ring(CEN_TOP, -WALL_D + CEN_UP)
    band(outer, r0, "body")          # face with a square hole
    band(r0, r1, "body")             # recess wall
    band(r1, r2, "core_dark")        # window frame (flat, recessed)
    band(r2, r3, "core_bright")      # centre bevel
    top = bm.faces.new(r3)
    top.material_index = mslot("core_bright")
bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))     # closed manifold body

# ---- hex bolts: 4 per face, frustum + flat top (no bottom, sits on the face)
for ax in range(3):
    for sgn in (1, -1):
        n = Vector((0, 0, 0))
        n[ax] = sgn
        u, v = frame_of(n)
        c = n * HALF[ax]
        for su in (-1, 1):
            for sv in (-1, 1):
                b0 = c + u * su * BOLT_POS + v * sv * BOLT_POS
                ring0 = [bm.verts.new(b0 + u * BOLT_R0 * math.cos(math.radians(60 * k)) + v * BOLT_R0 * math.sin(math.radians(60 * k))) for k in range(6)]
                ring1 = [bm.verts.new(b0 + n * BOLT_H + u * BOLT_R1 * math.cos(math.radians(60 * k)) + v * BOLT_R1 * math.sin(math.radians(60 * k))) for k in range(6)]
                fs = [bm.faces.new((ring0[k], ring0[(k + 1) % 6], ring1[(k + 1) % 6], ring1[k])) for k in range(6)]
                fs.append(bm.faces.new(ring1))
                for f in fs:
                    f.material_index = mslot("caps")
                flip_outward(fs, b0 - n * 3.0)

# ---- corner caps: cube corner block, outer vertex truncated, 0.5 cm proud of the body faces
for sx in (-1, 1):
    for sy in (-1, 1):
        for sz in (-1, 1):
            ox, oy, oz = sx * (BX + CAP_OUT), sy * (BY + CAP_OUT), sz * (BZ + CAP_OUT)
            pts = []
            for dx in (0, 1):
                for dy in (0, 1):
                    for dz in (0, 1):
                        if dx == 0 and dy == 0 and dz == 0:
                            continue
                        pts.append((ox - sx * CAP_S * dx, oy - sy * CAP_S * dy, oz - sz * CAP_S * dz))
            pts += [(ox - sx * CAP_T, oy, oz), (ox, oy - sy * CAP_T, oz), (ox, oy, oz - sz * CAP_T)]
            vs = [bm.verts.new(p) for p in pts]
            res = bmesh.ops.convex_hull(bm, input=vs, use_existing_faces=False)
            fs = [g for g in res["geom"] if isinstance(g, bmesh.types.BMFace)]
            for f in fs:
                f.material_index = mslot("caps")
            flip_outward(fs, Vector((ox - sx * CAP_S / 2, oy - sy * CAP_S / 2, oz - sz * CAP_S / 2)))
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

PIVOT = opt.get("--pivot", "center")
if PIVOT == "center":
    zs = [v.co.z for v in mesh.vertices]
    mesh.transform(__import__("mathutils").Matrix.Translation((0.0, 0.0, -(min(zs) + max(zs)) / 2)))
    mesh.update()

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
if abs(lo.z if PIVOT == "bottom" else (lo.z + hi.z) / 2) > 1e-4 or abs((lo.x + hi.x) / 2) > 1e-3 or abs((lo.y + hi.y) / 2) > 1e-3:
    warnings.append("pivot not " + PIVOT + " centre")
warnings.append("M_XPCubeBig_core_bright and _dark are emissive in Blender (strength 4, #FB81FD and #FA11F8): the FBX does not carry emission, the Unreal material sets the glow")
# ---- glow disc (as xp_chip_big_mesh.py): flat 16-sided disc, LAST slot "glow_disc", added AFTER the pivot centring and the body checks.
DISC_R, DISC_Z, DISC_N = 44.44, -16.56, 16
body_lo, body_hi = lo.copy(), hi.copy()
dm = bpy.data.materials.new("glow_disc"); dm.diffuse_color = (1.0, 1.0, 1.0, 1.0)
mesh.materials.append(dm); DISC_SLOT = len(mesh.materials) - 1
bmd = bmesh.new(); bmd.from_mesh(mesh)
d0 = bmd.loops.layers.uv["UVMap"]; d1 = bmd.loops.layers.uv["LightMap"]
dc = bmd.verts.new((0.0, 0.0, DISC_Z))
dv = [bmd.verts.new((DISC_R * math.cos(2 * math.pi * k / DISC_N), DISC_R * math.sin(2 * math.pi * k / DISC_N), DISC_Z)) for k in range(DISC_N)]
for k in range(DISC_N):
    f = bmd.faces.new((dc, dv[k], dv[(k + 1) % DISC_N])); f.material_index = DISC_SLOT; f.smooth = False
    for l in f.loops:
        l[d0].uv = (0.5 + l.vert.co.x / (2 * DISC_R), 0.5 + l.vert.co.y / (2 * DISC_R))
        l[d1].uv = (0.0, 0.0)   # disc is unlit and shadowless: it does not use the lightmap (all UV1 at one point, body islands untouched)
bmd.to_mesh(mesh); bmd.free()
for p in mesh.polygons: p.use_smooth = False
bbt = [Vector(c) for c in obj.bound_box]
tot_lo = [round(min(v[i] for v in bbt), 3) for i in range(3)]; tot_hi = [round(max(v[i] for v in bbt), 3) for i in range(3)]
disc_tris = sum(len(p.vertices) - 2 for p in mesh.polygons if p.material_index == DISC_SLOT)
total_tris = sum(len(p.vertices) - 2 for p in mesh.polygons)
GLOW = {"radius_cm": DISC_R, "z_cm": DISC_Z, "tris": disc_tris, "slot_index": DISC_SLOT, "sides": DISC_N,
        "normals": "+Z", "shading": "flat, one flat material, colour irrelevant (Unreal material set later)",
        "uv_note": "UV0 radial: u = 0.5 + x / (2 R), v = 0.5 + y / (2 R), distance to (0.5, 0.5) is the radius in 0..0.5. UV1 LightMap: the disc does not use the lightmap (unlit, shadowless), its UV1 is a single point (0, 0); body LightMap islands and uv_checks unchanged.",
        "placement": "same numbers as the chip (centred pivot): BP_PickupXPBig root scale 1.8: world radius 80 cm and z -29.8 become local radius 44.44 and z -16.56"}
report = {
    "asset": "SM_XPCubeBig", "route": "blender", "script": "Tools/art/blender/xp_cube_big_mesh.py",
    "source": "Art/Source/xp-core/meshy/Meshy_AI_Neon_Power_Cube_1017063448_texture_fbx (owner's Meshy cube, 5044 tris, 0.95 x 0.95 x 0.87; no Meshy call, no credit)",
    "tris_out": tris, "budget_tris": BUDGET,
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "footprint_target_cm": {"x": FOOT, "y": FOOT, "z": HEIGHT},
    "pivot": PIVOT, "pivot_computed_on": "body only (disc excluded)", "bbox_min": [round(c, 3) for c in body_lo], "bbox_max": [round(c, 3) for c in body_hi],
    "total_bbox_min": tot_lo, "total_bbox_max": tot_hi, "total_tris": total_tris, "body_tris": tris, "glow_disc": GLOW,
    "materials": [m.name for m in mesh.materials], "slots": [m.name for m in mesh.materials],
    "paint_colours_srgb": {"M_XPCubeBig_" + k: v for k, v in HEX.items()}, "emissive_material": ["M_XPCubeBig_core_bright", "M_XPCubeBig_core_dark"],
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
RP = os.path.splitext(OUT)[0] + ".report.json"
if os.path.exists(RP):                                   # keep the fidelity verdicts written after the visual judgement
    try:
        old = json.load(open(RP, encoding="utf-8"))
        if "fidelity" in old:
            report["fidelity"] = old["fidelity"]
    except Exception:
        pass
with open(RP, "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print(json.dumps(report))
