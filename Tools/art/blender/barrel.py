"""Barrel mesh (Blender route, task-101-04, thin edge bands task-101-07). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/barrel.py -- [--out Art/Meshes/barrel/SM_Barrel.fbx] [--image vault/assets/barrel.png]
Units: 1 Blender unit = 1 cm (same convention as cleanup.py). Pivot bottom centre. Never imports into Unreal.
Body flat-shaded, flat per-material colours; thin lighter edge bands (own flat materials, seeded gaps), stepped cap with 4 hex bolts; two small textured decals (sticker, pictogram) cropped from the image."""
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

SIDES = 8
FACETS = 8
SEED = 1011
R_BODY, R_RING = 42.5, 45.0   # circumradius (corner to axis); width across corners = 90
# Profile (z, r, material of the band ABOVE this ring up to the next one)
P = [
    (0.0, 34.0, "body"), (12.0, 42.5, "body"),
    (33.0, 42.5, "ring"), (36.0, 45.0, "ring"), (48.0, 45.0, "ring"), (51.0, 42.5, "body"),
    (80.0, 42.5, "ring"), (83.0, 45.0, "ring"), (97.0, 45.0, "ring"), (100.0, 42.5, "body"),
    (126.0, 42.5, "body"), (130.0, 38.0, "lid"), (132.0, 34.0, "lid"),
]
P_LID_BAND = 11   # band 130 -> 132 is lid coloured (the chamfer 126 -> 130 keeps the body colour)
# materials: name -> RGBA (linear). Colours read on the image: orange, darker orange, dark greys.
def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#FF6A00"), "ring": srgb("#D9480F"), "lid": srgb("#3C4048"), "cap": srgb("#2B2E34"),
        # lighter tones of the surface colours for the thin edge bands (wear as in the image: lighter band, not bare metal)
        "lid_edge": srgb("#5C626D"), "cap_edge": srgb("#4A4F58"), "ring_edge": srgb("#EE6A2E")}
EDGE_HEX = {"lid": "#5C626D", "cap": "#4A4F58"}   # lighter tone used by the faint scratch lerp (lid, cap)

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
def band(a, b, mat):
    n = len(a)
    for k in range(n):
        f = bm.faces.new((a[k], a[(k + 1) % n], b[(k + 1) % n], b[k])); f.material_index = mslot(mat)
def disc(a, mat, up=True):
    f = bm.faces.new(a if up else a[::-1]); f.material_index = mslot(mat)

rings = [ring(z, r, SIDES) for z, r, _ in P]
for i in range(len(P) - 1):
    band(rings[i], rings[i + 1], "lid" if i == P_LID_BAND else P[i][2])
disc(rings[0], "body", up=False)   # bottom
disc(rings[-1], "lid", up=True)    # lid top at z=132
# cap: 10-sided stepped base (two stacked rings, the lower wider) + chamfer + top; 4 hex bolts bring the top to 134
cx, cy = 14.0, 12.0
CAPR = [(132.0, 9.5), (132.6, 9.5), (132.6, 8.5), (133.0, 8.5), (133.3, 7.5)]
cr = [ring(z, r, 10, cx, cy) for z, r in CAPR]
for i in range(len(cr) - 1):
    band(cr[i], cr[i + 1], "cap")
disc(cr[-1], "cap")
for a_ in (45.0, 135.0, 225.0, 315.0):
    bx = cx + 4.2 * math.cos(math.radians(a_)); by = cy + 4.2 * math.sin(math.radians(a_))
    hb = ring(133.3, 1.3, 6, bx, by, 0.0); ht = ring(134.0, 1.3, 6, bx, by, 0.0)
    band(hb, ht, "cap_edge"); disc(ht, "cap_edge")
# ---- thin edge bands (about 1 cm) on the lid chamfer, the cap top chamfer and the top edge of the two rings.
# Each loop is covered 72..80 % with 2 or 3 seeded gaps; pieces are cut at facet corners, offset 0.15 cm outward (radially).
rng = random.Random(SEED)
band_stats = {}
def lerp3(a, b, t): return a + (b - a) * t
def strip(name, A, B, s0, s1, mat, n, ctr):
    cov = rng.uniform(0.72, 0.80); g = rng.choice((2, 3))
    gap_total = n * (1.0 - cov); w = [rng.uniform(0.7, 1.3) for _ in range(g)]; sw = sum(w)
    gaps = []
    for i in range(g):
        ln = gap_total * w[i] / sw; sec = n / g
        st_ = i * sec + rng.uniform(0.0, sec - ln); gaps.append((st_, st_ + ln))
    ivs = []   # covered intervals [a, b] in edge units 0..n
    for i in range(g):
        a_ = gaps[i][1]; b_ = gaps[(i + 1) % g][0]
        if b_ > a_: ivs.append((a_, b_))
        else: ivs += [(a_, float(n)), (0.0, b_)]
    covered = 0.0; perim = 0.0
    for k in range(n):
        perim += (A[k].co - A[(k + 1) % n].co).length
    for a_, b_ in ivs:
        k = int(a_)
        while k < n and k < b_:
            ua = max(a_, k) - k; ub = min(b_, k + 1) - k
            if ub - ua > 1e-4:
                def pt(u, s):
                    pa = A[k].co.lerp(A[(k + 1) % n].co, u); pb = B[k].co.lerp(B[(k + 1) % n].co, u)
                    p = pa.lerp(pb, s); d = Vector((p.x - ctr[0], p.y - ctr[1], 0.0)).normalized() * 0.15
                    return bm.verts.new(p + d)
                f = bm.faces.new((pt(ua, s0), pt(ub, s0), pt(ub, s1), pt(ua, s1))); f.material_index = mslot(mat)
                covered += (A[k].co - A[(k + 1) % n].co).length * (ub - ua)
            k += 1
    band_stats[name] = {"gaps": g, "coverage": round(covered / perim, 3), "material": "M_Barrel_" + mat}
def width_s(Aring, Bring):
    return 1.0 / max(1.0, (Aring[0].co - Bring[0].co).length)   # about 1 cm along the slope
O = (0.0, 0.0)
# lid chamfer: top edge side of band 130 -> 132
i = P_LID_BAND; ws = width_s(rings[i], rings[i + 1]); strip("lid_chamfer", rings[i], rings[i + 1], 1.0 - ws, 1.0, "lid_edge", SIDES, O)
# ring top edges: bottom of the slope that follows each ring (48 -> 51, 97 -> 100)
for nm, i in (("ring_top_lower", 4), ("ring_top_upper", 8)):
    ws = width_s(rings[i], rings[i + 1]); strip(nm, rings[i], rings[i + 1], 0.1, 0.1 + ws, "ring_edge", SIDES, O)   # starts 0.1 down the slope so the 0.15 offset stays inside 90 cm
# cap top chamfer (133.0 -> 133.3)
strip("cap_top", cr[3], cr[4], 0.0, 1.0, "cap_edge", 10, (cx, cy))

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
        "scratch_applies_to": ["M_Barrel_lid", "M_Barrel_cap"],
        "scratch_not_on": ["M_Barrel_body", "M_Barrel_ring", "M_Barrel_lid_edge", "M_Barrel_cap_edge", "M_Barrel_ring_edge", "M_Barrel_sticker", "M_Barrel_pictogram"],
        "formula": "M_Barrel_lid colour = lerp(lid colour #3C4048, lighter lid tone #5C626D, (1 - T_WearMask) * 0.6); M_Barrel_cap colour = lerp(cap colour #2B2E34, lighter cap tone #4A4F58, (1 - T_WearMask) * 0.6)",
        "paint_colours_srgb": {"M_Barrel_body": "#FF6A00", "M_Barrel_ring": "#D9480F", "M_Barrel_lid": "#3C4048", "M_Barrel_cap": "#2B2E34",
                               "M_Barrel_lid_edge": "#5C626D", "M_Barrel_cap_edge": "#4A4F58", "M_Barrel_ring_edge": "#EE6A2E"},
        "unmasked_materials": ["M_Barrel_body", "M_Barrel_ring", "M_Barrel_lid_edge", "M_Barrel_cap_edge", "M_Barrel_ring_edge", "M_Barrel_sticker", "M_Barrel_pictogram"],
        "no_vertex_colour": True, "no_bare_metal": True,
        "note": "FBX cannot hold the node graph: the Unreal master material applies the lerp on lid and cap only; every other material is the flat colour exported in the FBX (edge bands are real geometry with their own flat materials).",
    },
    "edge_bands": {"seed": SEED, "width_cm": 1.0, "offset_cm": 0.15, "loops": band_stats,
                   "cap": "stepped base (r 9.5 then r 8.5), chamfer to r 7.5, 4 hex bolts (r 1.3, height 0.7, material M_Barrel_cap_edge) at radius 4.2, top z = 134"},
    "removed": ["EdgeWear vertex colour attribute", "hard-threshold edge chips", "bare metal #8A9099 mix", "16-column rings and the intermediate edge loops added for EdgeWear (back to 8 columns, 12 bands)"],
    "decal_faces": 2, "shading": "flat", "warnings": warnings,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"},
                         apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
report["vertex_colour_attribute"] = None
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print(json.dumps(report))
