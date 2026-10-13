"""XP nut mesh (Blender route, task-128-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_nut_mesh.py -- [--out Art/Meshes/xp-nut-mesh/SM_XPNut.fbx]
Units: 1 Blender unit = 1 cm. Pivot bottom centre. Never imports into Unreal.
Hex nut from vault/assets/xp-nut.png: hexagonal prism (corner on +X), tapered 12-sided hole with a floor, flat shading.
Footprint: the placeholder XP cube is /Engine/BasicShapes/Cube (100 cm) at scale 0.3 = 30 cm (bp-pickup-xp); the hex is 30 cm across corners (X) so it fits the 30 x 30 footprint.
Materials: M_XPNut_body (dark grey), M_XPNut_hole (cyan 0, 0.9, 1, emissive in Unreal)."""
import bpy, bmesh, math, json, os, sys
import numpy as np
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/xp-nut-mesh/SM_XPNut.fbx"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
os.makedirs(os.path.dirname(OUT), exist_ok=True)

R_OUT, H = 15.0, 17.0           # circumradius (30 cm across corners), height
HOLE = [(H, 8.5), (H * 0.55, 7.2), (H * 0.25, 6.4)]   # (z, radius) of the hole rings, then the floor
BUDGET = 300

def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#4A4F58"), "hole": srgb("#00E5FF")}

bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, c in MATS.items():
    m = bpy.data.materials.new("M_XPNut_" + n); m.diffuse_color = c; m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]; b.inputs["Base Color"].default_value = c; b.inputs["Roughness"].default_value = 0.8
    if n == "hole":
        b.inputs["Emission Color"].default_value = c; b.inputs["Emission Strength"].default_value = 4.0
    mats[n] = m
mesh = bpy.data.meshes.new("SM_XPNut"); obj = bpy.data.objects.new("SM_XPNut", mesh)
bpy.context.scene.collection.objects.link(obj)
mesh.materials.append(mats["body"]); mesh.materials.append(mats["hole"])
MB, MH = 0, 1

bm = bmesh.new()
def ring(z, r, n, off=0.0):
    return [bm.verts.new((r * math.cos(math.radians(off + 360.0 * k / n)), r * math.sin(math.radians(off + 360.0 * k / n)), z)) for k in range(n)]
def tri(a, b, c, mi):
    f = bm.faces.new((a, b, c)); f.material_index = mi
def quad(a, b, c, d, mi):
    f = bm.faces.new((a, b, c, d)); f.material_index = mi

hex_b = ring(0.0, R_OUT, 6); hex_t = ring(H, R_OUT, 6)
for k in range(6):
    quad(hex_b[k], hex_b[(k + 1) % 6], hex_t[(k + 1) % 6], hex_t[k], MB)
bm.faces.new(hex_b[::-1]).material_index = MB   # bottom
hole = [ring(z, r, 12) for z, r in HOLE]
inn = hole[0]
for k in range(6):   # top annulus, hex corner k aligned with hole vertex 2k
    o0, o1 = hex_t[k], hex_t[(k + 1) % 6]
    i0, i1, i2 = inn[2 * k], inn[2 * k + 1], inn[(2 * k + 2) % 12]
    tri(o0, o1, i1, MB); tri(o0, i1, i0, MB); tri(o1, i2, i1, MB)
for j in range(len(hole) - 1):
    for k in range(12):
        quad(hole[j][k], hole[j + 1][k], hole[j + 1][(k + 1) % 12], hole[j][(k + 1) % 12], MH)
bm.faces.new(hole[-1][::-1]).material_index = MH   # floor
bmesh.ops.recalc_face_normals(bm, faces=bm.faces[:])
bm.faces.ensure_lookup_table(); bm.faces.index_update()

# UV0 "UVMap": planar on each face plane, 100 cm per unit. UV1 "LightMap": one island per face, shelf packed.
uv0 = bm.loops.layers.uv.new("UVMap"); uv1 = bm.loops.layers.uv.new("LightMap")
def basis(f):
    n = f.normal.normalized()
    if abs(n.z) > 0.99: return Vector((1, 0, 0)), Vector((0, 1, 0))
    t = Vector((-n.y, n.x, 0)).normalized(); s = n.cross(t).normalized()
    return t, s * (1 if s.z > 0 else -1)
island = {}
for f in bm.faces:
    t, s_ = basis(f)
    loc = [(l.vert.co.dot(t), l.vert.co.dot(s_)) for l in f.loops]
    for l, p in zip(f.loops, loc): l[uv0].uv = (p[0] / 100.0, p[1] / 100.0)
    mx = min(p[0] for p in loc); my = min(p[1] for p in loc); loc = [(x - mx, y - my) for x, y in loc]
    island[f.index] = (loc, max(p[0] for p in loc), max(p[1] for p in loc))
LM, PAD = 256, 3.0
def pack(sc):
    x = y = PAD; rh = 0.0; out = {}
    for fi, (loc, w, h) in sorted(island.items(), key=lambda kv: -kv[1][2]):
        pw, ph = w * sc, h * sc
        if x + pw + PAD > LM: x = PAD; y += rh + PAD; rh = 0.0
        if x + pw + PAD > LM or y + ph + PAD > LM: return None
        out[fi] = (x, y); x += pw + PAD; rh = max(rh, ph)
    return out
lo_s, hi_s = 0.01, 40.0
for _ in range(40):
    mid = (lo_s + hi_s) / 2
    if pack(mid): lo_s = mid
    else: hi_s = mid
placed = pack(lo_s)
for f in bm.faces:
    x, y = placed[f.index]
    for l, (lx, ly) in zip(f.loops, island[f.index][0]): l[uv1].uv = ((x + lx * lo_s) / LM, (y + ly * lo_s) / LM)
for f in bm.faces: f.smooth = False
bm.to_mesh(mesh); bm.free()
for p in mesh.polygons: p.use_smooth = False

# checks
bb = [Vector(c) for c in obj.bound_box]
lo = Vector([min(v[i] for v in bb) for i in range(3)]); hi = Vector([max(v[i] for v in bb) for i in range(3)])
tris = sum(len(p.vertices) - 2 for p in mesh.polygons)
u1a = np.array([l.uv[:] for l in mesh.uv_layers["LightMap"].data])
boxes = []
for p in mesh.polygons:
    q = u1a[p.loop_start:p.loop_start + p.loop_total] * LM
    boxes.append((q[:, 0].min(), q[:, 1].min(), q[:, 0].max(), q[:, 1].max()))
gaps = [max(max(B[0] - A[2], A[0] - B[2]), max(B[1] - A[3], A[1] - B[3])) for i, A in enumerate(boxes) for B in boxes[i + 1:]]
min_gap = min(gaps); overlaps = sum(1 for g in gaps if g < 0)
border = min(min(b[0], b[1], LM - b[2], LM - b[3]) for b in boxes)
lm_ok = overlaps == 0 and min_gap >= 2.0 and border >= 2.0 and u1a.min() >= 0 and u1a.max() <= 1
warnings = []
if not lm_ok: warnings.append("LightMap check FAILED")
if tris > BUDGET: warnings.append("tris above 300 budget")
if abs(lo.z) > 1e-4 or abs((lo.x + hi.x) / 2) > 1e-3 or abs((lo.y + hi.y) / 2) > 1e-3: warnings.append("pivot not bottom centre")
if max(hi.x - lo.x, hi.y - lo.y) > 30.01: warnings.append("footprint above 30 cm")
warnings.append("hole emissive is not carried by the FBX: the Unreal material must set the emissive colour (0, 0.9, 1) on M_XPNut_hole")
report = {
    "asset": "SM_XPNut", "route": "blender", "script": "Tools/art/blender/xp_nut_mesh.py", "source_image": "vault/assets/xp-nut.png",
    "tris_out": tris, "budget_tris": BUDGET,
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "footprint_reference": "placeholder XP cube /Engine/BasicShapes/Cube (100 cm) x scale 0.3 = 30 cm (BP_PickupXP, bp-pickup-xp); hex 30 cm across corners (X), 25.98 across flats (Y)",
    "pivot": "bottom", "bbox_min": [round(c, 3) for c in lo], "bbox_max": [round(c, 3) for c in hi],
    "materials": [m.name for m in mesh.materials], "uv_layers": [u.name for u in mesh.uv_layers],
    "uv_checks": {"UVMap": {"tile_cm": 100.0}, "LightMap": {"faces": len(boxes), "overlapping_face_pairs": overlaps,
        "bounds_min": [round(float(u1a.min()), 5)], "bounds_max": [round(float(u1a.max()), 5)], "min_gap_px_at_256": round(float(min_gap), 3),
        "min_border_px_at_256": round(float(border), 3), "required_margin_px": 2, "pass": bool(lm_ok)}},
    "shading": "flat", "warnings": warnings,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                         mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f: json.dump(report, f, indent=2)
print(json.dumps(report))
