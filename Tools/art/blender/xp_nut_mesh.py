"""XP nut mesh (Blender route, task-128-01). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/xp_nut_mesh.py -- [--out Art/Meshes/xp-nut-mesh/SM_XPNut.fbx]
Units: 1 Blender unit = 1 cm. Pivot: geometric centre of the bbox by default (--pivot center|bottom; task-135-02). Never imports into Unreal.
Hex nut from vault/assets/xp-nut.png: hexagonal prism (corner on +X), chamfered, smooth 12-sided THROUGH hole (run 5), 6 bevelled top corners (task-130-01 fidelity rebuild), flat shading.
Footprint: the placeholder XP cube is /Engine/BasicShapes/Cube (100 cm) at scale 0.3 = 30 cm (bp-pickup-xp); the hex is 30 cm across corners (X) so it fits the 30 x 30 footprint.
Materials: M_XPNut_body (mid grey), M_XPNut_hole (hole wall, slightly darker grey). No emissive, no blue (run 5)."""
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
# Through hole (run 5): mouth chamfer, then ONE smooth straight 12-sided wall down to the bottom face.
HOLE = [(H, 8.3), (10.6, 7.7), (0.0, 7.7)]
BUDGET = 300

def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#70757B"), "hole": srgb("#5E6368")}

bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, c in MATS.items():
    m = bpy.data.materials.new("M_XPNut_" + n); m.diffuse_color = c; m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]; b.inputs["Base Color"].default_value = c; b.inputs["Roughness"].default_value = 0.8
    mats[n] = m
mesh = bpy.data.meshes.new("SM_XPNut"); obj = bpy.data.objects.new("SM_XPNut", mesh)
bpy.context.scene.collection.objects.link(obj)
mesh.materials.append(mats["body"]); mesh.materials.append(mats["hole"])
MB, MG = 0, 1

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
hole = [ring(z, r, 12) for z, r in HOLE]
for k in range(6):   # bottom: hexagon with the through hole (pentagon per hex edge)
    b = hole[-1]
    bm.faces.new((hex_b[k], b[2 * k], b[2 * k + 1], b[(2 * k + 2) % 12], hex_b[(k + 1) % 6])).material_index = MB
inn = hole[0]
for k in range(12):   # top face: bevelled corners + flat mid-edges, to the chamfer mouth
    quad(top[k], top[(k + 1) % 12], inn[(k + 1) % 12], inn[k], MB)
for j in range(len(hole) - 1):
    for k in range(12):
        quad(hole[j][k], hole[j + 1][k], hole[j + 1][(k + 1) % 12], hole[j][(k + 1) % 12], MB if j == 0 else MG)
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

# Pivot (task-135-02): "center" = geometric centre of the bounding box (pickup actor root placed by SetActorLocation); "bottom" = bottom centre.
PIVOT = opt.get("--pivot", "center")
if PIVOT == "center":
    zs = [v.co.z for v in mesh.vertices]
    mesh.transform(__import__("mathutils").Matrix.Translation((0.0, 0.0, -(min(zs) + max(zs)) / 2)))
    mesh.update()

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
if abs(lo.z if PIVOT == "bottom" else (lo.z + hi.z) / 2) > 1e-4 or abs((lo.x + hi.x) / 2) > 1e-3 or abs((lo.y + hi.y) / 2) > 1e-3: warnings.append("pivot not " + PIVOT + " centre")
if max(hi.x - lo.x, hi.y - lo.y) > 30.01: warnings.append("footprint above 30 cm")
warnings.append("M_XPNut_hole is a separate slot so Unreal can later set an emissive on it for bloom; the mesh itself has no emissive and no blue")
FIDELITY = {
    "run": 5, "returns_used": 0, "final_render": "Art/Meshes/xp-nut-mesh/SM_XPNut.preview.png",
    "camera": "azimuth 11, elevation 52, ortho 42 (Tools/art/blender/xp_nut_preview.py)",
    "history": "run 1-3: stepped hole with cyan ring (run 3 base); run 4 flat cyan disc abandoned by owner; run 5 = run 3 body with smooth hole, no blue.",
    "criteria": {
        "C1": {"verdict": "pass", "evidence": "Hexagonal prism 30 x 25.98 x 11.7 cm, height 45 % of across-flats; still differs: the source looks slightly taller from its higher camera."},
        "C2": {"verdict": "pass", "evidence": "Ground shows through a 12-sided opening at the bottom, no floor; top diameter 63.9 % of across-flats; mouth chamfer kept; below it one straight 12-sided wall (radius 7.7 cm from z 10.6 to 0) with no bands, threads, groove or ring in the render. Still differs: the source has a floor and stepped rings, removed by owner decision."},
        "C3": {"verdict": "pass", "evidence": "Chamfer facets at the hole mouth and 6 bevelled top corners visible; still differs: source bevels are a bit larger."},
        "C4": {"verdict": "pass", "evidence": "Exactly two plain grey slots: M_XPNut_body (#70757B, body and chamfer) and M_XPNut_hole (#5E6368, slightly darker, wall); no emissive."},
        "C5": {"verdict": "pass", "evidence": "tris_out 108 (budget 300); 30 cm across corners; pivot bottom centre."},
        "C6": {"verdict": "pass", "evidence": "Flat shading, large facets, no texture."},
        "C7": {"verdict": "pass", "evidence": "No cyan or blue in the mesh or the render; the render is grey only. Still differs: the source's cyan glow is absent by owner decision; Unreal can set an emissive on M_XPNut_hole."},
    },
}
# ---- glow disc (task-145-01): flat 16-sided disc, LAST slot "glow_disc", added AFTER the pivot centring and the body checks (the pivot and every body check use the body only).
DISC_R, DISC_Z, DISC_N = 50, -29.8, 16
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
        "placement": "BP_PickupXP root scale 1: world radius 50 cm, 29.8 cm below the actor origin (HoverHeight 30, 0.2 cm above the floor)"}
report = {
    "asset": "SM_XPNut", "route": "blender", "script": "Tools/art/blender/xp_nut_mesh.py", "source_image": "vault/assets/xp-nut.png",
    "tris_out": tris, "budget_tris": BUDGET,
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "footprint_reference": "placeholder XP cube /Engine/BasicShapes/Cube (100 cm) x scale 0.3 = 30 cm (BP_PickupXP, bp-pickup-xp); hex 30 cm across corners (X), 25.98 across flats (Y)",
    "pivot": PIVOT, "pivot_computed_on": "body only (disc excluded)", "bbox_min": [round(c, 3) for c in body_lo], "bbox_max": [round(c, 3) for c in body_hi],
    "total_bbox_min": tot_lo, "total_bbox_max": tot_hi, "total_tris": total_tris, "body_tris": tris, "glow_disc": GLOW,
    "materials": [m.name for m in mesh.materials], "slots": [m.name for m in mesh.materials], "uv_layers": [u.name for u in mesh.uv_layers],
    "uv_checks": {"UVMap": {"tile_cm": 100.0}, "LightMap": {"faces": len(boxes), "overlapping_face_pairs": overlaps,
        "bounds_min": [round(float(u1a.min()), 5)], "bounds_max": [round(float(u1a.max()), 5)], "min_gap_px_at_256": round(float(min_gap), 3),
        "min_border_px_at_256": round(float(border), 3), "required_margin_px": 2, "pass": bool(lm_ok)}},
    "shading": "flat", "warnings": warnings,
    "hole": "through, no floor", "hole_top_diameter_pct_of_across_flats": round(100 * 2 * HOLE[0][1] / (2 * RF), 1),
    "fidelity": FIDELITY,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                         mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f: json.dump(report, f, indent=2)
print(json.dumps(report))
