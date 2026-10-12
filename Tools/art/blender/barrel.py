"""Barrel mesh (Blender route, task-101-04). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/barrel.py -- [--out Art/Meshes/barrel/SM_Barrel.fbx] [--image vault/assets/barrel.png]
Units: 1 Blender unit = 1 cm (same convention as cleanup.py). Pivot bottom centre. Never imports into Unreal.
Body flat-shaded, flat per-material colours; two small textured decals (sticker, pictogram) cropped from the image."""
import bpy, bmesh, math, json, os, sys
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
R_BODY, R_RING = 42.5, 45.0   # circumradius (corner to axis); width across corners = 90
# Profile (z, r, material of the band ABOVE this ring up to the next one)
P = [
    (0.0, 34.0, "body"), (12.0, 42.5, "body"),
    (33.0, 42.5, "ring"), (36.0, 45.0, "ring"), (48.0, 45.0, "ring"), (51.0, 42.5, "body"),
    (80.0, 42.5, "ring"), (83.0, 45.0, "ring"), (97.0, 45.0, "ring"), (100.0, 42.5, "body"),
    (126.0, 42.5, "body"), (130.0, 38.0, "edge"), (132.0, 34.0, "lid"),
]
# materials: name -> RGBA (linear). Colours read on the image: orange, darker orange, dark greys.
def srgb(h):
    c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)
MATS = {"body": srgb("#FF6A00"), "ring": srgb("#D9480F"), "lid": srgb("#3C4048"), "cap": srgb("#2B2E34"), "edge": srgb("#8A9099")}

bpy.ops.wm.read_factory_settings(use_empty=True)
mats = {}
for n, c in MATS.items():
    m = bpy.data.materials.new("M_Barrel_" + ("edge_wear" if n == "edge" else n))
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
    band(rings[i], rings[i + 1], P[i][2])
disc(rings[0], "body", up=False)   # bottom
disc(rings[-1], "lid", up=True)    # lid top at z=132
# cap: 10-sided short cylinder, back right of the lid, grey
cx, cy = 14.0, 12.0
c0 = ring(132.0, 9.0, 10, cx, cy); c1 = ring(132.5, 9.0, 10, cx, cy); c2 = ring(134.0, 7.0, 10, cx, cy)
band(c0, c1, "cap"); band(c1, c2, "edge"); disc(c2, "cap")

# decals: quads on two adjacent facets (left and right of the front corner at -90 deg)
def facet_quad(angle_deg, zc, w, h, off=0.25):
    a = math.radians(angle_deg)
    n = Vector((math.cos(a), math.sin(a), 0)); t = Vector((-math.sin(a), math.cos(a), 0))
    d = R_BODY * math.cos(math.radians(180.0 / SIDES)) + off
    c = n * d + Vector((0, 0, zc))
    pts = [c - t * w / 2 - Vector((0, 0, h / 2)), c + t * w / 2 - Vector((0, 0, h / 2)),
           c + t * w / 2 + Vector((0, 0, h / 2)), c - t * w / 2 + Vector((0, 0, h / 2))]
    return [bm.verts.new(p) for p in pts]

for dn, ang, zc, w, h in (("sticker", -112.5, 66.0, 28.0, 11.5), ("pictogram", -67.5, 64.0, 19.0, 22.0)):
    mats[dn] = bpy.data.materials.new("M_Barrel_" + dn)
    vs = facet_quad(ang, zc, w, h)
    f = bm.faces.new(vs); f.material_index = mslot(dn)
    uvl = bm.loops.layers.uv.verify()
    for l, uv in zip(f.loops, ((0, 0), (1, 0), (1, 1), (0, 1))):
        l[uvl].uv = uv
bm.faces.ensure_lookup_table()
# UVs for the non-decal faces: planar top projection normalised to 0..1 (flat colours, no texture)
uvl = bm.loops.layers.uv.verify()
for f in bm.faces:
    if mesh.materials[f.material_index].name[9:] in ("sticker", "pictogram"):
        continue
    for l in f.loops:
        l[uvl].uv = (l.vert.co.x / 100.0 + 0.5, l.vert.co.y / 100.0 + 0.5)
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
uvs = [l.uv for l in mesh.uv_layers.active.data]
warnings = []
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
    "uv_bounds": {"min": [min(u[0] for u in uvs), min(u[1] for u in uvs)], "max": [max(u[0] for u in uvs), max(u[1] for u in uvs)]},
    "decal_faces": 2, "shading": "flat", "warnings": warnings,
}
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"},
                         apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
with open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
print(json.dumps(report))
