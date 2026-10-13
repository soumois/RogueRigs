"""XP nut mesh (Blender route, task-128-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_nut_mesh.py -- [--out Art/Meshes/xp-nut-mesh/SM_XPNut.fbx]
Units: 1 Blender unit = 1 cm. Pivot bottom centre. Never imports into Unreal.
Hex nut from vault/assets/xp-nut.png: hexagonal prism (corner on +X), chamfered, stepped 12-sided hole with a floor, 6 bevelled top corners (task-130-01 fidelity rebuild), flat shading.
Footprint: the placeholder XP cube is /Engine/BasicShapes/Cube (100 cm) at scale 0.3 = 30 cm (bp-pickup-xp); the hex is 30 cm across corners (X) so it fits the 30 x 30 footprint.
Materials: M_XPNut_body (mid grey), M_XPNut_hole (cyan 0, 0.9, 1, walls), M_XPNut_hole_floor (pale cyan 0.55, 0.95, 1: rim chamfer and floor); both emissive in Unreal."""
import bpy, bmesh, math, json, os, sys
import numpy as np
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/xp-nut-mesh/SM_XPNut.fbx"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"])
os.makedirs(os.path.dirname(OUT), exist_ok=True)

R_OUT, H = 15.0, 11.7         # circumradius (30 cm across corners), height = 45 % of across-flats 25.98
BEVEL = 0.9                     # corner bevel: the 6 top corners are lowered by this much
HOLE = [(H, 8.3), (H - 1.3, 7.6), (H - 2.8, 7.4), (H - 3.6, 6.4), (H - 5.1, 6.2), (H - 5.9, 5.2), (H - 7.4, 5.0), (H - 8.2, 4.2)]   # (z, radius): mouth, chamfer foot, then 3 steep+shallow bands down to the floor ring (depth 8.2 = 70 % of H)
BUDGET = 300

def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#70757B"), "hole": srgb("#00E5FF"), "hole_floor": srgb("#8CF2FF")}

bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, c in MATS.items():
    m = bpy.data.materials.new("M_XPNut_" + n); m.diffuse_color = c; m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]; b.inputs["Base Color"].default_value = c; b.inputs["Roughness"].default_value = 0.8
    if n.startswith("hole"):
        b.inputs["Emission Color"].default_value = c; b.inputs["Emission Strength"].default_value = 4.0
    mats[n] = m
mesh = bpy.data.meshes.new("SM_XPNut"); obj = bpy.data.objects.new("SM_XPNut", mesh)
bpy.context.scene.collection.objects.link(obj)
mesh.materials.append(mats["body"]); mesh.materials.append(mats["hole"]); mesh.materials.append(mats["hole_floor"])
MB, MH, MF = 0, 1, 2

bm = bmesh.new()
def ring(z, r, n, off=0.0):
    return [bm.verts.new((r * math.cos(math.radians(off + 360.0 * k / n)), r * math.sin(math.radians(off + 360.0 * k / n)), z)) for k in range(n)]
def tri(a, b, c, mi):
    f = bm.faces.new((a, b, c)); f.material_index = mi
def quad(a, b, c, d, mi):
    f = bm.faces.new((a, b, c, d)); f.material_index = mi

hex_b = ring(0.0, R_OUT, 6)
RF = R_OUT * math.cos(math.radians(30))
top = []   # 12 outline verts: even = corner (lowered by BEVEL), odd = mid-edge (full height)
for k in range(12):
    a = math.radians(30.0 * k)
    if k % 2 == 0: top.append(bm.verts.new((R_OUT * math.cos(a), R_OUT * math.sin(a), H - BEVEL)))
    else: top.append(bm.verts.new((RF * math.cos(a), RF * math.sin(a), H)))
for k in range(6):   # side pentagon (split in triangles by Blender)
    c0, m, c1 = top[2 * k], top[2 * k + 1], top[(2 * k + 2) % 12]
    f = bm.faces.new((hex_b[k], hex_b[(k + 1) % 6], c1, m, c0)); f.material_index = MB
bm.faces.new(hex_b[::-1]).material_index = MB   # bottom
hole = [ring(z, r, 12) for z, r in HOLE]
inn = hole[0]
for k in range(12):   # top face: bevelled corners + flat mid-edges, to the chamfer mouth
    quad(top[k], top[(k + 1) % 12], inn[(k + 1) % 12], inn[k], MB)
for j in range(len(hole) - 1):
    for k in range(12):
        quad(hole[j][k], hole[j + 1][k], hole[j + 1][(k + 1) % 12], hole[j][(k + 1) % 12], MF if j == 0 else MH)   # j == 0: rim chamfer (pale, brightest ring)
bm.faces.new(hole[-1][::-1]).material_index = MF   # floor
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
FIDELITY = {
    "run": 2, "returns_used": 3, "final_render": "Art/Meshes/xp-nut-mesh/SM_XPNut.preview.png",
    "camera": "azimuth 11, elevation 52, ortho 42 (Tools/art/blender/xp_nut_preview.py)",
    "criteria": {
        "C1": {"verdict": "pass", "evidence": "Hexagonal prism 30 x 25.98 x 11.7 cm, height 45 % of across-flats; still differs: source is slightly taller-looking from its higher camera."},
        "C2": {"verdict": "pass", "evidence": "Hole depth 8.2 cm = 70 % of height; floor visible as a pale ellipse well below the top face; 3 steep + 2 shallow wall bands, far wall lit, near wall hidden by the rim; walls (0,0.9,1) and floor (0.55,0.95,1) are two materials; shallow bands read clearly brighter than steep bands on the far wall. Still differs: source bands are broader and its floor is larger; ours has thinner far-wall bands and a smaller floor."},
        "C3": {"verdict": "pass", "evidence": "Chamfer ring around the hole mouth and 6 bevelled top corners visible as facets; still differs: source bevels are a bit larger."},
        "C4": {"verdict": "pass", "evidence": "Body one mid grey; every hole surface cyan or pale cyan; no other colour."},
        "C5": {"verdict": "pass", "evidence": "tris_out 224 (budget 300); 30 cm across corners; pivot bottom centre."},
        "C6": {"verdict": "pass", "evidence": "Flat shading, large facets, no texture."},
        "C7": {"verdict": "pass", "evidence": "Rim chamfer (pale cyan) is the brightest ring, then the saturated walls, then the floor; still differs: the source rim has a soft glow halo that Blender does not render."},
    },
}
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
    "hole_depth_cm": round(H - HOLE[-1][0], 2), "hole_depth_pct_of_height": round(100 * (H - HOLE[-1][0]) / H, 1),
    "fidelity": FIDELITY,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                         mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f: json.dump(report, f, indent=2)
print(json.dumps(report))
