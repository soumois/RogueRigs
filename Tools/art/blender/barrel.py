"""Barrel mesh (Blender route, task-101-04, edge wear task-101-06). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/barrel.py -- [--out Art/Meshes/barrel/SM_Barrel.fbx] [--image vault/assets/barrel.png]
Units: 1 Blender unit = 1 cm (same convention as cleanup.py). Pivot bottom centre. Never imports into Unreal.
Body flat-shaded, flat per-material colours; two small textured decals (sticker, pictogram) cropped from the image."""
import bpy, bmesh, math, json, os, sys, random
import numpy as np
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Meshes/barrel/SM_Barrel.fbx", "--image": "vault/assets/barrel.png"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out"]); IMG = os.path.abspath(opt["--image"])
OUTDIR = os.path.dirname(OUT)
os.makedirs(OUTDIR, exist_ok=True)

SIDES = 16     # 16 columns: even vertices = the 8 facet corners, odd = facet mid-edge (collinear) so wear chips can sit mid-edge
FACETS = 8
SEED = 1011
R_BODY, R_RING = 42.5, 45.0   # circumradius (corner to axis); width across corners = 90
# Profile (z, r, material of the band ABOVE this ring up to the next one, wear tag)
# wear tag: "corner" = vertical body corners (even columns only), "ring"/"lid" = convex horizontal edges (all columns), None = no wear
P = [
    (0.0, 34.0, "body", None), (12.0, 42.5, "body", "corner"), (22.5, 42.5, "body", "corner"),
    (33.0, 42.5, "ring", None), (36.0, 45.0, "ring", "ring"), (48.0, 45.0, "ring", "ring"), (51.0, 42.5, "body", None),
    (65.5, 42.5, "body", "corner"),
    (80.0, 42.5, "ring", None), (83.0, 45.0, "ring", "ring"), (97.0, 45.0, "ring", "ring"), (100.0, 42.5, "body", None),
    (113.0, 42.5, "body", "corner"),
    (126.0, 42.5, "body", "lid"), (130.0, 38.0, "lid", "lid"), (132.0, 34.0, "lid", "lid"),
]
P_LID_BAND = 14   # band 130 -> 132 is lid coloured (the chamfer 126 -> 130 keeps the body colour)
P_PROB = {"corner": 0.17, "ring": 0.13, "lid": 0.20, "cap": 0.20}
# materials: name -> RGBA (linear). Colours read on the image: orange, darker orange, dark greys.
def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#FF6A00"), "ring": srgb("#D9480F"), "lid": srgb("#3C4048"), "cap": srgb("#2B2E34")}
BARE = "#8A9099"   # bare metal, blended in by the wear mask (see report mask_contract)

bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, c in MATS.items():
    m = bpy.data.materials.new("M_Barrel_" + n)
    m.diffuse_color = c
    m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = c
    b.inputs["Roughness"].default_value = 0.8
    mats[n] = m

mesh = bpy.data.meshes.new("SM_Barrel")
obj = bpy.data.objects.new("SM_Barrel", mesh)
bpy.context.scene.collection.objects.link(obj)
slot = {}
def mslot(n):
    if n not in slot:
        mesh.materials.append(mats[n]); slot[n] = len(mesh.materials) - 1
    return slot[n]

bm = bmesh.new()
def ring(z, r, sides, cx=0.0, cy=0.0, off=-90.0):
    return [bm.verts.new((cx + r * math.cos(math.radians(off + 360.0 * k / sides)),
                          cy + r * math.sin(math.radians(off + 360.0 * k / sides)), z)) for k in range(sides)]
def ring16(z, r):
    out = []
    for k in range(SIDES):
        rr = r if k % 2 == 0 else r * math.cos(math.radians(180.0 / FACETS))
        a = math.radians(-90.0 + 360.0 * k / SIDES)
        out.append(bm.verts.new((rr * math.cos(a), rr * math.sin(a), z)))
    return out
def band(a, b, mat):
    n = len(a)
    for k in range(n):
        f = bm.faces.new((a[k], a[(k + 1) % n], b[(k + 1) % n], b[k])); f.material_index = mslot(mat)
def disc(a, mat, up=True):
    f = bm.faces.new(a if up else a[::-1]); f.material_index = mslot(mat)

rings = [ring16(z, r) for z, r, _, _ in P]
for i in range(len(P) - 1):
    band(rings[i], rings[i + 1], "lid" if i == P_LID_BAND else P[i][2])
disc(rings[0], "body", up=False)   # bottom
disc(rings[-1], "lid", up=True)    # lid top at z=132
# cap: 10-sided short cylinder, back right of the lid, grey
cx, cy = 14.0, 12.0
c0 = ring(132.0, 9.0, 10, cx, cy); c1 = ring(132.5, 9.0, 10, cx, cy); c2 = ring(134.0, 7.0, 10, cx, cy)
band(c0, c1, "cap"); band(c1, c2, "cap"); disc(c2, "cap")
# ---- EdgeWear: per-vertex value (0..1), high only on convex outer edges, deterministic. Hit vertex = 0.62..1.0, else 0.
rng = random.Random(SEED)
wear = {}
for ri, (rv, (z, r, _m, tag)) in enumerate(zip(rings, P)):
    if tag is None: continue
    for k, v in enumerate(rv):
        if tag == "corner" and k % 2: continue
        wear[v] = (0.62 + 0.38 * rng.random()) if rng.random() < P_PROB[tag] else 0.0
for v in c2:
    wear[v] = (0.62 + 0.38 * rng.random()) if rng.random() < P_PROB["cap"] else 0.0

# decals: quads on two adjacent facets (left and right of the front corner at -90 deg)
def facet_quad(angle_deg, zc, w, h, off=0.25):
    a = math.radians(angle_deg)
    n = Vector((math.cos(a), math.sin(a), 0)); t = Vector((-math.sin(a), math.cos(a), 0))
    d = R_BODY * math.cos(math.radians(180.0 / FACETS)) + off
    c = n * d + Vector((0, 0, zc))
    pts = [c - t * w / 2 - Vector((0, 0, h / 2)), c + t * w / 2 - Vector((0, 0, h / 2)),
           c + t * w / 2 + Vector((0, 0, h / 2)), c - t * w / 2 + Vector((0, 0, h / 2))]
    return [bm.verts.new(p) for p in pts]

for dn, ang, zc, w, h in (("sticker", -112.5, 66.0, 28.0, 11.5), ("pictogram", -67.5, 64.0, 19.0, 22.0)):
    mats[dn] = bpy.data.materials.new("M_Barrel_" + dn)
    vs = facet_quad(ang, zc, w, h)
    f = bm.faces.new(vs); f.material_index = mslot(dn)
    uvl = bm.loops.layers.uv.get("UVMap") or bm.loops.layers.uv.new("UVMap")
    for l, uv in zip(f.loops, ((0, 0), (1, 0), (1, 1), (0, 1))):
        l[uvl].uv = uv
bm.faces.ensure_lookup_table()
bm.faces.index_update(); bm.normal_update()
# ---- UV channel 0 "UVMap": planar projection on each face's own plane, 1 tile of the wear mask = 100 cm
# (constant texel density). Decal quads keep their own 0..1 UV. Channel 1 "LightMap": one island per face, packed.
DECALS = ("sticker", "pictogram")
uv0 = bm.loops.layers.uv.get("UVMap") or bm.loops.layers.uv.new("UVMap")
uv1 = bm.loops.layers.uv.new("LightMap")
TILE_CM = 100.0
def plane_basis(f):
    n = f.normal.normalized()
    if abs(n.z) > 0.99:
        return Vector((1, 0, 0)), Vector((0, 1, 0))
    t = Vector((-n.y, n.x, 0)).normalized()
    return t, n.cross(t).normalized() * (1 if n.cross(t).z > 0 else -1)   # t horizontal, s points up the slope
island = {}    # face index -> (list of local 2D cm coords per loop, w, h)
for f in bm.faces:
    t, s_ = plane_basis(f)
    loc = [(l.vert.co.dot(t), l.vert.co.dot(s_)) for l in f.loops]
    mnx = min(p[0] for p in loc); mny = min(p[1] for p in loc)
    loc = [(x - mnx, y - mny) for x, y in loc]
    island[f.index] = (loc, max(p[0] for p in loc), max(p[1] for p in loc))
    if mesh.materials[f.material_index].name[9:] in DECALS:
        continue
    for l in f.loops:
        l[uv0].uv = (l.vert.co.dot(t) / TILE_CM, l.vert.co.dot(s_) / TILE_CM)

# LightMap packing: shelf packing in pixel units at 256 px, gap and border >= PAD px between island bounding boxes
LM_RES, PAD = 256, 3.0
def pack(scale):
    items = sorted(island.items(), key=lambda kv: -kv[1][2])
    x = y = PAD; rowh = 0.0; out = {}
    for fi, (loc, w, h) in items:
        pw, ph = w * scale, h * scale
        if x + pw + PAD > LM_RES:
            x = PAD; y += rowh + PAD; rowh = 0.0
        if x + pw + PAD > LM_RES or y + ph + PAD > LM_RES:
            return None
        out[fi] = (x, y, pw, ph); x += pw + PAD; rowh = max(rowh, ph)
    return out
lo_s, hi_s = 0.01, 20.0
for _ in range(40):
    mid = (lo_s + hi_s) / 2
    if pack(mid): lo_s = mid
    else: hi_s = mid
lm_scale = lo_s; placed = pack(lm_scale)
for f in bm.faces:
    x, y, pw, ph = placed[f.index]; loc = island[f.index][0]
    for l, (lx, ly) in zip(f.loops, loc):
        l[uv1].uv = ((x + lx * lm_scale) / LM_RES, (y + ly * lm_scale) / LM_RES)
for f in bm.faces:
    f.smooth = False
# vertex colour attribute EdgeWear (corner domain, float, red channel = wear); decal quads and flat faces stay 0
cl = bm.loops.layers.float_color.new("EdgeWear")
for f in bm.faces:
    for l in f.loops:
        l[cl] = (wear.get(l.vert, 0.0), 0.0, 0.0, 1.0)
# edge-length coverage where the linearly interpolated value exceeds the 0.5 threshold, per family of convex edges
cover = {"rings": [0.0, 0.0], "lid_rim": [0.0, 0.0], "cap_top": [0.0, 0.0], "body_corners": [0.0, 0.0]}
def frac_above(a, b, t=0.5):
    if a > t and b > t: return 1.0
    if a <= t and b <= t: return 0.0
    hi_, lo_ = max(a, b), min(a, b)
    return (hi_ - t) / (hi_ - lo_)
ring_of = {v: ri for ri, rv in enumerate(rings) for v in rv}
c2set = set(c2)
for e in bm.edges:
    a, b = e.verts
    key = None
    if a in c2set and b in c2set: key = "cap_top"
    elif a in ring_of and b in ring_of:
        ra, rb = ring_of[a], ring_of[b]
        if ra == rb:
            key = {"ring": "rings", "lid": "lid_rim"}.get(P[ra][3])
        elif abs(P[ra][1] - P[rb][1]) < 1e-6 and rings[ra].index(a) == rings[rb].index(b) and rings[ra].index(a) % 2 == 0:
            key = "rings" if P[ra][3] == "ring" and P[rb][3] == "ring" else ("body_corners" if "body" in (P[ra][2], P[rb][2]) else None)
    if key:
        L = (a.co - b.co).length; cover[key][0] += L * frac_above(wear.get(a, 0.0), wear.get(b, 0.0)); cover[key][1] += L
bm.to_mesh(mesh); bm.free()
for p in mesh.polygons:
    p.use_smooth = False

# ---- decal textures: crop + flatten from the validated image (numpy inside Blender)
src = bpy.data.images.load(IMG)
W, H = src.size
px = np.array(src.pixels[:], dtype=np.float32).reshape(H, W, 4)[::-1]   # row 0 = top of image

def unwarp(tl, tr, br, bl, ow, oh):
    """bilinear sampling of the source quad (image pixel coords, y down) into an ow x oh rectangle"""
    u = (np.arange(ow) + 0.5) / ow; v = (np.arange(oh) + 0.5) / oh
    uu, vv = np.meshgrid(u, v)
    tl, tr, br, bl = [np.array(p, dtype=np.float32) for p in (tl, tr, br, bl)]
    P_ = (tl * (1 - uu)[..., None] * (1 - vv)[..., None] + tr * uu[..., None] * (1 - vv)[..., None]
          + br * uu[..., None] * vv[..., None] + bl * (1 - uu)[..., None] * vv[..., None])
    x = np.clip(P_[..., 0] - 0.5, 0, W - 1.001); y = np.clip(P_[..., 1] - 0.5, 0, H - 1.001)
    x0 = x.astype(int); y0 = y.astype(int); fx = (x - x0)[..., None]; fy = (y - y0)[..., None]
    return (px[y0, x0] * (1 - fx) * (1 - fy) + px[y0, x0 + 1] * fx * (1 - fy)
            + px[y0 + 1, x0] * (1 - fx) * fy + px[y0 + 1, x0 + 1] * fx * fy)

def save_tex(name, arr):
    arr = np.ascontiguousarray(arr[::-1])  # back to bottom-up for Blender
    im = bpy.data.images.new(name, arr.shape[1], arr.shape[0], alpha=True)
    im.pixels = arr.reshape(-1).tolist()
    im.filepath_raw = os.path.join(OUTDIR, name + ".png"); im.file_format = "PNG"; im.save()
    return im

# sticker: yellow plate, quad corners read on the image (TL, TR, BR, BL)
st = unwarp((410, 673), (615, 708), (613, 797), (410, 757), 512, 210)
st[..., 3] = 1.0
# pictogram: dark flame on orange; shear of the right facet (rises 0.15 px/px to the right); alpha from the red channel
pg = unwarp((668, 699), (822, 676), (822, 831), (668, 855), 256, 300)
alpha = np.clip((200.0 / 255.0 - pg[..., 0]) / (60.0 / 255.0), 0, 1)
pg[..., :3] = np.array([0.17, 0.18, 0.21]); pg[..., 3] = alpha
for name, arr, dn in (("T_Barrel_Sticker", st, "sticker"), ("T_Barrel_Pictogram", pg, "pictogram")):
    im = save_tex(name, arr)
    m = mats[dn]; m.use_nodes = True
    nt = m.node_tree; bsdf = nt.nodes["Principled BSDF"]
    tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = im
    nt.links.new(tx.outputs["Color"], bsdf.inputs["Base Color"])
    bsdf.inputs["Roughness"].default_value = 0.8
    if dn == "pictogram":
        nt.links.new(tx.outputs["Alpha"], bsdf.inputs["Alpha"])
        m.blend_method = "BLEND" if hasattr(m, "blend_method") else None

# ---- checks, report, export
dg = bpy.context.evaluated_depsgraph_get()
bb = [obj.matrix_world @ Vector(c) for c in obj.bound_box]
lo = Vector((min(v.x for v in bb), min(v.y for v in bb), min(v.z for v in bb)))
hi = Vector((max(v.x for v in bb), max(v.y for v in bb), max(v.z for v in bb)))
tris = sum(len(p.vertices) - 2 for p in mesh.polygons)
def layer_arr(name): return np.array([l.uv[:] for l in mesh.uv_layers[name].data], dtype=np.float64)
u0a, u1a = layer_arr("UVMap"), layer_arr("LightMap")
# LightMap checks on the real mesh data: per-face bbox in px at 256, pairwise gap (separated boxes => no triangle overlap)
boxes = []
for p in mesh.polygons:
    q = u1a[p.loop_start:p.loop_start + p.loop_total] * LM_RES
    boxes.append((q[:, 0].min(), q[:, 1].min(), q[:, 0].max(), q[:, 1].max()))
min_gap = 1e9
for i in range(len(boxes)):
    for j in range(i + 1, len(boxes)):
        A, B = boxes[i], boxes[j]
        gx = max(B[0] - A[2], A[0] - B[2]); gy = max(B[1] - A[3], A[1] - B[3])
        min_gap = min(min_gap, max(gx, gy))
border = min(min(b[0], b[1], LM_RES - b[2], LM_RES - b[3]) for b in boxes)
overlaps = sum(1 for i in range(len(boxes)) for j in range(i + 1, len(boxes))
               if max(max(boxes[j][0] - boxes[i][2], boxes[i][0] - boxes[j][2]), max(boxes[j][1] - boxes[i][3], boxes[i][1] - boxes[j][3])) < 0)
lm_ok = overlaps == 0 and min_gap >= 2.0 and border >= 2.0 and u1a.min() >= 0 and u1a.max() <= 1
# texel density check on UVMap (non-decal faces): cm per UV unit must be 100
dens = []
for p in mesh.polygons:
    if mesh.materials[p.material_index].name[9:] in DECALS: continue
    ls = list(range(p.loop_start, p.loop_start + p.loop_total))
    for k in range(len(ls)):
        a_, b_ = ls[k], ls[(k + 1) % len(ls)]
        va, vb = mesh.loops[a_].vertex_index, mesh.loops[b_].vertex_index
        d3 = (mesh.vertices[va].co - mesh.vertices[vb].co).length; d2 = np.linalg.norm(u0a[a_] - u0a[b_])
        if d3 > 1e-6:
            dens.append(d3 / d2 if d2 > 1e-9 else float("inf"))
            if d2 <= 1e-9: print("DEGEN", p.index, mesh.materials[p.material_index].name, p.normal[:], d3)
warnings = []
if not lm_ok: warnings.append("LightMap check FAILED")
if tris > 600: warnings.append("tris above 600 budget")
if abs((hi.z - lo.z) - 134) > 1.0: warnings.append("height off 134 cm")
if max(hi.x - lo.x, hi.y - lo.y) > 91.0: warnings.append("width above 90 cm")
if abs(lo.z) > 1e-4 or abs((lo.x + hi.x) / 2) > 1e-3: warnings.append("pivot not bottom centre")
warnings.append("pictogram material uses alpha blend: Unreal import must set it Masked/Translucent")
warnings.append("decal crop is an approximate bilinear unwarp of the 3/4 view, edges not pixel-perfect")
report = {
    "asset": "SM_Barrel", "route": "blender", "script": "Tools/art/blender/barrel.py", "source_image": "vault/assets/barrel.png",
    "tris_out": tris, "budget_tris": 600,
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "pivot": "bottom", "bbox_min": [round(c, 3) for c in lo], "bbox_max": [round(c, 3) for c in hi],
    "materials": [m.name for m in mesh.materials], "decal_textures": ["T_Barrel_Sticker.png", "T_Barrel_Pictogram.png"],
    "uv_layers": [u.name for u in mesh.uv_layers],
    "uv_checks": {
        "UVMap": {"purpose": "wear mask + decals", "tile_cm": 100.0, "cm_per_uv_unit_min": round(min(dens), 3), "cm_per_uv_unit_max": round(max(dens), 3),
                  "decal_quads_uv": "0..1 own", "note": "non-decal faces tile past 0..1 (mask is tileable, wrap/repeat)"},
        "LightMap": {"faces": len(boxes), "triangles": tris, "overlapping_face_pairs": overlaps, "bounds_min": [round(float(u1a.min()), 5)], "bounds_max": [round(float(u1a.max()), 5)],
                     "min_gap_px_at_256": round(float(min_gap), 3), "min_border_px_at_256": round(float(border), 3), "required_margin_px": 2, "pass": bool(lm_ok), "texel_scale_px_per_cm_at_256": round(lm_scale, 4)},
    },
    "mask_contract": {
        "scratch_mask_file": "Art/Textures/T_WearMask.png", "mask_colour_space": "linear/grayscale (sRGB off)", "uv_channel": 0, "uv_channel_name": "UVMap", "tile_cm": 100.0, "wrap": "repeat",
        "scratch_applies_to": ["M_Barrel_lid", "M_Barrel_cap"], "scratch_not_on": ["M_Barrel_body", "M_Barrel_ring", "M_Barrel_sticker", "M_Barrel_pictogram"],
        "edge_wear_vertex_colour": {"attribute": "EdgeWear", "channel": "R (linear, no sRGB conversion)", "threshold": 0.5,
                                    "applies_to": ["M_Barrel_body", "M_Barrel_ring", "M_Barrel_lid", "M_Barrel_cap"], "hard_edge": "step(0.5, EdgeWear.R)"},
        "formula": "worn = max(step(0.5, EdgeWear.R) on body, ring, lid, cap ; (1 - T_WearMask) on lid and cap only); colour = lerp(paint_colour, bare_metal, worn)",
        "bare_metal_srgb": BARE, "paint_colours_srgb": {"M_Barrel_body": "#FF6A00", "M_Barrel_ring": "#D9480F", "M_Barrel_lid": "#3C4048", "M_Barrel_cap": "#2B2E34"},
        "unmasked_materials": ["M_Barrel_sticker", "M_Barrel_pictogram"],
        "note": "FBX cannot hold the node graph: the Unreal master material applies it; paint colour = the per-material flat colour exported in the FBX. The importer must keep vertex colours (do not override) and a VertexColor node (R) feeds the threshold.",
    },
    "removed": ["scratch/blotch mask on body and ring (mask now lid and cap only)", "linear blend lerp(bare, paint, mask) as the only wear term (replaced by max(hard EdgeWear, 1 - mask on lid/cap))", "8-column rings (now 16 columns, 15 bands)"],
    "decal_faces": 2, "shading": "flat", "warnings": warnings,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"},
                         apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
# ---- EdgeWear stats + proof that the attribute survives the FBX export (re-import and read the red channel)
ca = mesh.color_attributes["EdgeWear"]
red0 = np.array([c.color[0] for c in ca.data], dtype=np.float64)
vred = {}
for p in mesh.polygons:
    for li in range(p.loop_start, p.loop_start + p.loop_total):
        vred[mesh.loops[li].vertex_index] = red0[li]
hv = sum(1 for x in vred.values() if x > 0.5)
elig = len(wear); elig_hi = sum(1 for x in wear.values() if x > 0.5)
before = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=OUT, colors_type="LINEAR")
imp = next(o for o in bpy.data.objects if o not in before and o.type == "MESH")
im_names = [c.name for c in imp.data.color_attributes]
fbx_ok = False; fbx_maxdiff = None
if "EdgeWear" in im_names:
    red1 = np.array([c.color[0] for c in imp.data.color_attributes["EdgeWear"].data], dtype=np.float64)
    if len(red1) == len(red0):
        fbx_maxdiff = float(np.abs(np.sort(red0) - np.sort(red1)).max()); fbx_ok = fbx_maxdiff < 0.02
print("FBX reimport color attributes:", im_names, "maxdiff", fbx_maxdiff)
if not fbx_ok: report["warnings"].append("EdgeWear vertex colour did NOT survive FBX export")
report["vertex_colour_attribute"] = "EdgeWear"
report["edge_wear"] = {
    "attribute": "EdgeWear", "domain": "corner (same value on all corners of a vertex)", "channel": "R", "range": [0.0, 1.0],
    "fbx_export": "colors_type LINEAR (no sRGB curve: red channel exact)", "fbx_roundtrip_ok": fbx_ok, "fbx_roundtrip_attrs": im_names, "fbx_roundtrip_max_abs_diff": fbx_maxdiff,
    "seed": SEED, "hit_value_range": [0.62, 1.0], "threshold": 0.5,
    "vertices_total": len(mesh.vertices), "vertices_high_gt_0_5": hv, "share_high_of_all_vertices": round(hv / len(mesh.vertices), 4),
    "eligible_convex_edge_vertices": elig, "eligible_high": elig_hi, "share_high_of_eligible": round(elig_hi / elig, 4),
    "edge_length_above_threshold_share": {k: round(v[0] / v[1], 4) for k, v in cover.items() if v[1] > 0},
    "edge_length_above_threshold_share_all": round(sum(v[0] for v in cover.values()) / sum(v[1] for v in cover.values()), 4),
    "zero_on": "all vertices not on a convex outer edge, decal quads, bottom",
}
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print(json.dumps(report))
