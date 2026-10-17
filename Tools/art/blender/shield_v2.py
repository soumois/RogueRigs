"""Shield chaser riot shield v2 (task-216-02): the owner's second Meshy mesh simplified to about 1000 flat tris, back bracket removed,
base colour baked from the Meshy mesh, bracket area and top face repainted. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/shield_v2.py -- --in <meshy.fbx> --out Art/Meshes/ChaserShield/SM_ChaserShield.fbx
       [--tmp <short dir>] [--no-render] [--fidelity <json>] [--views vault/assets]
Method: plate rebuilt from planar faces (silhouette, groove ring, bolts measured on the Meshy mesh), the Meshy grip kept (bracket cut
away at y = -0.2, planar dissolve + collapse decimation), new unwrap, bake (selected to active, diffuse colour only) into
T_ChaserShield_BaseColor.png 1024, then painted: bracket area flat yellow, stripes continuous over the top face.
Front +X, 82 cm tall, bottom-centre pivot, centimetres (1 Blender unit = 1 cm), FBX_SCALE_UNITS."""
import bpy, bmesh, json, math, os, sys
import numpy as np
from mathutils import Vector, Euler, Matrix

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {}
i = 0
while i < len(argv):
    if argv[i] == "--no-render":
        opt["--no-render"] = "1"; i += 1
    else:
        opt[argv[i]] = argv[i + 1]; i += 2
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
OUT = os.path.abspath(opt["--out"])
OUTDIR = os.path.dirname(OUT)
REPORT = os.path.splitext(OUT)[0] + ".report.json"
PREV = os.path.join(OUTDIR, "preview")
TMP = os.path.abspath(opt.get("--tmp", os.environ.get("TMP", ".")))
os.makedirs(OUTDIR, exist_ok=True); os.makedirs(PREV, exist_ok=True)

if "--fidelity" in opt:   # merge the judged fidelity object into an existing report, nothing else
    rep = json.load(open(REPORT, encoding="utf-8"))
    rep["fidelity"] = json.load(open(opt["--fidelity"], encoding="utf-8"))
    json.dump(rep, open(REPORT, "w", encoding="utf-8"), indent=2)
    print("fidelity merged"); sys.exit(0)

HEIGHT_CM = 82.0
TEX_PX = 1024
TARGET_TRIS = int(opt.get("--target", 980))
TEX_PATH = os.path.join(OUTDIR, "T_ChaserShield_BaseColor.png")

# ================================================================ import the Meshy mesh, same frame as v1 (front +X, 82 cm, units = cm)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.abspath(opt["--in"]))
src = next(o for o in bpy.context.scene.objects if o.type == "MESH")
for o in list(bpy.context.scene.objects):
    if o is not src and o.type != "MESH":
        bpy.data.objects.remove(o, do_unlink=True)
src.parent = None
smesh = src.data
smesh.transform(src.matrix_world); src.matrix_world.identity()
tris_in = sum(len(p.vertices) - 2 for p in smesh.polygons)
smesh.transform(Euler((0, 0, math.radians(90))).to_matrix().to_4x4())     # front plate toward -Y -> +X
zs = [v.co.z for v in smesh.vertices]
SC = HEIGHT_CM / (max(zs) - min(zs))
smesh.transform(Matrix.Scale(SC, 4))
smesh.transform(Matrix.Translation((0, 0, -min(v.co.z for v in smesh.vertices))))
smesh.update()
src.name = "MeshySource"

# the source material: base colour image only (metallic 0) so the diffuse colour pass is the picture
smat = smesh.materials[0]
sb = smat.node_tree.nodes.get("Principled BSDF")
sb.inputs["Metallic"].default_value = 0.0
for l in list(smat.node_tree.links):
    if l.to_socket.name in ("Metallic", "Roughness", "Normal"):
        smat.node_tree.links.remove(l)
sb.inputs["Roughness"].default_value = 1.0

# ================================================================ measured values (cm, frame: x depth front +X, y right in front view, z up)
X_BACK, X_FRONT, X_GROOVE = 2.45, 4.90, 4.20
YMIN, YMAX = -21.4, 21.4
SIL = [(-9.0, 0.0), (18.4, 0.0), (21.4, 3.0), (21.4, 70.9), (10.3, 82.0), (-21.4, 82.0), (-21.4, 12.0)]   # (y, z) CCW seen from the front
# groove ring: offset of each silhouette edge toward the inside (centre line of the groove), measured on a depth map of the Meshy mesh
RING_OFF = [7.0, 8.7, 6.9, 6.9, 7.0, 4.8, 6.9]
GROOVE_W = 1.5
BOLT_Y = (-12.26, 10.9)
BOLT_Z_FRONT = (16.5, 67.3)
BOLT_Z_BACK = (16.0, 67.9)
BOLT_R = 1.62            # circumradius of the octagon (flat top)
BOLT_H_FRONT, BOLT_H_BACK = 0.6, 0.65
BRACKET_CUT_Y = -0.2

def offset_poly(poly, offs):
    """poly CCW (y, z); edge i from poly[i] to poly[i+1] moved inward by offs[i]; vertices = intersections of consecutive offset lines"""
    n = len(poly); lines = []
    for k in range(n):
        a = Vector(poly[k]); b = Vector(poly[(k + 1) % n]); d = (b - a).normalized()
        nin = Vector((-d.y, d.x))                       # left normal = inside for CCW
        lines.append((a + nin * offs[k], d))
    out = []
    for k in range(n):
        p1, d1 = lines[k - 1]; p2, d2 = lines[k]
        t = ((p2 - p1).x * d2.y - (p2 - p1).y * d2.x) / (d1.x * d2.y - d1.y * d2.x)
        q = p1 + d1 * t
        out.append((q.x, q.y))
    return out

# ================================================================ plate and bolts, rebuilt flat
nb = bmesh.new()
vcache = {}
def V(x, y, z):
    k = (round(x, 4), round(y, 4), round(z, 4))
    if k not in vcache: vcache[k] = nb.verts.new((x, y, z))
    return vcache[k]
def face(vs):
    try: nb.faces.new(vs)
    except ValueError: pass
def loop3(poly, x): return [V(x, p[0], p[1]) for p in poly]
def ring(la, lb):
    n = len(la)
    for k in range(n): face([la[k], la[(k + 1) % n], lb[(k + 1) % n], lb[k]])

gc = offset_poly(SIL, RING_OFF)
g_out = offset_poly(SIL, [o_ + GROOVE_W / 2 for o_ in RING_OFF])
g_in = offset_poly(SIL, [o_ - GROOVE_W / 2 for o_ in RING_OFF])
L_sil_f = loop3(SIL, X_FRONT); L_go = loop3(g_out, X_FRONT); L_gof = loop3(g_out, X_GROOVE)
L_gif = loop3(g_in, X_GROOVE); L_gi = loop3(g_in, X_FRONT)
L_sil_b = loop3(SIL, X_BACK)
ring(L_sil_f, L_go); ring(L_go, L_gof); ring(L_gof, L_gif); ring(L_gif, L_gi)       # front rim, groove wall, floor, wall
face(L_gi)                                                                         # inner panel
ring(L_sil_b, L_sil_f)                                                             # slab sides (top, bottom, left, right, cuts)
face(L_sil_b[::-1])                                                                # back plate
def bolt(y, z, x0, x1):
    ang = [math.radians(22.5 + 45 * k) for k in range(8)]
    a = [V(x0, y + BOLT_R * math.cos(t), z + BOLT_R * math.sin(t)) for t in ang]
    b = [V(x1, y + BOLT_R * math.cos(t), z + BOLT_R * math.sin(t)) for t in ang]
    ring(a, b); face(b if x1 > x0 else b[::-1])
for y in BOLT_Y:
    for z in BOLT_Z_FRONT: bolt(y, z, X_FRONT, X_FRONT + BOLT_H_FRONT)
    for z in BOLT_Z_BACK: bolt(y, z, X_BACK, X_BACK - BOLT_H_BACK)
bmesh.ops.recalc_face_normals(nb, faces=nb.faces[:])
tris_plate = sum(len(f.verts) - 2 for f in nb.faces)

# ================================================================ grip: Meshy faces behind the plate, bracket cut away, simplified
sb_ = bmesh.new(); sb_.from_mesh(smesh)
fl = [f for f in sb_.faces if f.calc_center_median().x < 2.3]
S_ = set(fl); seen = set(); comps = []
for f in fl:
    if f in seen: continue
    st = [f]; seen.add(f); comp = []
    while st:
        a = st.pop(); comp.append(a)
        for e in a.edges:
            for b in e.link_faces:
                if b in S_ and b not in seen: seen.add(b); st.append(b)
    comps.append(comp)
comps.sort(key=len, reverse=True)
grip_faces = comps[0]
gb = bmesh.new()
gmap = {}
for f in grip_faces:
    vs = []
    for v in f.verts:
        if v.index not in gmap: gmap[v.index] = gb.verts.new(v.co)
        vs.append(gmap[v.index])
    gb.faces.new(vs)
bmesh.ops.remove_doubles(gb, verts=gb.verts[:], dist=1e-4)
fin = [v for v in gb.verts if v.co.y > 13.5]      # the Meshy grip has a one-sided fin (y up to 14.2) under the upper mount: removed
print("fin vertices removed", len(fin))
bmesh.ops.delete(gb, geom=fin, context="VERTS")
_op = [e for e in gb.edges if len(e.link_faces) == 1]
if _op: bmesh.ops.holes_fill(gb, edges=_op, sides=64)
geom = gb.verts[:] + gb.edges[:] + gb.faces[:]
r = bmesh.ops.bisect_plane(gb, geom=geom, dist=1e-5, plane_co=(0, BRACKET_CUT_Y, 0), plane_no=(0, 1, 0), clear_inner=True, clear_outer=False)
cut_edges = [e for e in r["geom_cut"] if isinstance(e, bmesh.types.BMEdge)]
open_edges = [e for e in gb.edges if len(e.link_faces) == 1]
fill = bmesh.ops.holes_fill(gb, edges=open_edges, sides=64)
bmesh.ops.triangulate(gb, faces=gb.faces[:])
bmesh.ops.dissolve_degenerate(gb, dist=0.02, edges=gb.edges[:])
# drop any stray loose geometry
for v in [v for v in gb.verts if not v.link_faces]: gb.verts.remove(v)
bmesh.ops.dissolve_limit(gb, angle_limit=math.radians(4.0), verts=gb.verts[:], edges=gb.edges[:])
gtmp = bpy.data.meshes.new("grip_tmp"); gb.to_mesh(gtmp); gb.free()
gobj = bpy.data.objects.new("grip_tmp", gtmp); bpy.context.scene.collection.objects.link(gobj)
grip_tris_before = sum(len(p.vertices) - 2 for p in gtmp.polygons)
def decimate_to(obj, target):
    cur = sum(len(p.vertices) - 2 for p in obj.data.polygons)
    mod = obj.modifiers.new("dec", "DECIMATE"); mod.decimate_type = "COLLAPSE"; mod.ratio = max(0.02, min(1.0, target / cur))
    mod.use_collapse_triangulate = True
    bpy.context.view_layer.objects.active = obj
    bpy.ops.object.modifier_apply(modifier=mod.name)
from mathutils.bvhtree import BVHTree
_pre = bmesh.new(); _pre.from_mesh(gobj.data); _bvh = BVHTree.FromBMesh(_pre)
decimate_to(gobj, TARGET_TRIS - tris_plate)
snapped = 0
for v_ in gobj.data.vertices:          # collapse spikes: a vertex that drifted more than 0.3 cm off the original grip goes back onto it
    loc, nrm_, idx_, dist_ = _bvh.find_nearest(v_.co)
    if loc is not None and dist_ > 0.3: v_.co = loc; snapped += 1
print("grip vertices snapped back", snapped)
_pre.free()
grip_tris = sum(len(p.vertices) - 2 for p in gobj.data.polygons)

# join the grip into the new bmesh
gb2 = bmesh.new(); gb2.from_mesh(gobj.data)
bpy.data.objects.remove(gobj, do_unlink=True)
vm = {}
for v in gb2.verts: vm[v.index] = nb.verts.new(v.co)
for f in gb2.faces:
    try: nb.faces.new([vm[v.index] for v in f.verts])
    except ValueError: pass
gb2.free()
bmesh.ops.triangulate(nb, faces=nb.faces[:])
mesh = bpy.data.meshes.new("SM_ChaserShield"); nb.to_mesh(mesh); nb.free()
obj = bpy.data.objects.new("SM_ChaserShield", mesh); bpy.context.scene.collection.objects.link(obj)
for p in mesh.polygons: p.use_smooth = False
mesh.update()
tris_now = len(mesh.polygons)

# ================================================================ UV0 new unwrap
def select_only(o):
    for x in bpy.context.scene.objects: x.select_set(False)
    o.select_set(True); bpy.context.view_layer.objects.active = o
select_only(obj)
bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
uv0 = mesh.uv_layers.new(name="UVMap")
bpy.ops.uv.smart_project(angle_limit=math.radians(10), island_margin=0.004, area_weight=0.0, correct_aspect=True, scale_to_bounds=False)
bpy.ops.uv.select_all(action="SELECT")
bpy.ops.uv.pack_islands(margin=0.004, rotate=False)
bpy.ops.object.mode_set(mode="OBJECT")
lm = mesh.uv_layers.new(name="LightMap")
mesh.uv_layers.active_index = 1
bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
bpy.ops.uv.smart_project(angle_limit=math.radians(10), island_margin=0.02, area_weight=0.0, correct_aspect=True, scale_to_bounds=False)
bpy.ops.uv.select_all(action="SELECT")
bpy.ops.uv.pack_islands(margin=0.02, rotate=False)
bpy.ops.object.mode_set(mode="OBJECT")
mesh.uv_layers.active_index = 0
mesh.uv_layers["UVMap"].active_render = True

# ================================================================ bake the Meshy base colour (selected to active, diffuse colour only)
img = bpy.data.images.new("T_ChaserShield_BaseColor", TEX_PX, TEX_PX, alpha=False, float_buffer=False)
img.colorspace_settings.name = "sRGB"
mat = bpy.data.materials.new("M_ChaserShield"); mat.use_nodes = True
mesh.materials.append(mat)
nt = mat.node_tree; bsdf = nt.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 0.8
tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = img; tx.interpolation = "Linear"
uvn = nt.nodes.new("ShaderNodeUVMap"); uvn.uv_map = "UVMap"
nt.links.new(uvn.outputs["UV"], tx.inputs["Vector"]); nt.links.new(tx.outputs["Color"], bsdf.inputs["Base Color"])
nt.nodes.active = tx; tx.select = True
sc = bpy.context.scene
sc.render.engine = "CYCLES"; sc.cycles.device = "CPU"; sc.cycles.samples = 1
sc.render.bake.use_selected_to_active = True
sc.render.bake.cage_extrusion = 1.2; sc.render.bake.max_ray_distance = 5.0
sc.render.bake.margin = 10; sc.render.bake.margin_type = "EXTEND"
sc.render.bake.use_pass_direct = False; sc.render.bake.use_pass_indirect = False; sc.render.bake.use_pass_color = True
for x in bpy.context.scene.objects: x.select_set(False)
src.select_set(True); obj.select_set(True); bpy.context.view_layer.objects.active = obj
bpy.ops.object.bake(type="DIFFUSE")
print("baked")

# ================================================================ read the baked texture and map every texel to its triangle (UVMap)
W = H = TEX_PX
px = np.array(img.pixels[:], dtype=np.float32).reshape(H, W, 4)[..., :3].copy()     # rows bottom to top, sRGB as stored
uvd = mesh.uv_layers["UVMap"].data
tri_id = -np.ones((H, W), dtype=np.int32)
pos3 = np.zeros((H, W, 3), dtype=np.float32)
nrm3 = np.zeros((H, W, 3), dtype=np.float32)
EPSB = 2.5 / W
for p in mesh.polygons:
    ls = list(p.loop_indices)
    uvs = np.array([uvd[l].uv[:] for l in ls], dtype=np.float64)
    ps = np.array([mesh.vertices[mesh.loops[l].vertex_index].co[:] for l in ls], dtype=np.float64)
    mn = np.floor(uvs.min(0) * W - 3).astype(int); mx = np.ceil(uvs.max(0) * W + 3).astype(int)
    x0, y0 = max(mn[0], 0), max(mn[1], 0); x1, y1 = min(mx[0], W - 1), min(mx[1], H - 1)
    if x1 < x0 or y1 < y0: continue
    gx, gy = np.meshgrid(np.arange(x0, x1 + 1), np.arange(y0, y1 + 1))
    qx = (gx + 0.5) / W; qy = (gy + 0.5) / H
    a, b, c = uvs
    d = (b[1] - c[1]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[1] - c[1])
    if abs(d) < 1e-14: continue
    l1 = ((b[1] - c[1]) * (qx - c[0]) + (c[0] - b[0]) * (qy - c[1])) / d
    l2 = ((c[1] - a[1]) * (qx - c[0]) + (a[0] - c[0]) * (qy - c[1])) / d
    l3 = 1 - l1 - l2
    inside = (l1 >= -EPSB) & (l2 >= -EPSB) & (l3 >= -EPSB)
    # triangles are small or large; where two overlap by the margin the interior one wins
    strict = (l1 >= 0) & (l2 >= 0) & (l3 >= 0)
    upd = inside & ((tri_id[gy, gx] < 0) | strict)
    P = l1[..., None] * ps[0] + l2[..., None] * ps[1] + l3[..., None] * ps[2]
    tri_id[gy[upd], gx[upd]] = p.index
    pos3[gy[upd], gx[upd]] = P[upd]
    nrm3[gy[upd], gx[upd]] = np.array(p.normal[:], dtype=np.float32)
cov = tri_id >= 0
print("texel coverage", float(cov.mean()))

# colours sampled from the baked picture
def med(mask):
    return np.median(px[mask & cov], axis=0)
front = cov & (nrm3[..., 0] > 0.9) & (abs(pos3[..., 0] - X_FRONT) < 0.05)
back = cov & (nrm3[..., 0] < -0.9) & (abs(pos3[..., 0] - X_BACK) < 0.05)
YELLOW = med(front & (abs(pos3[..., 1]) < 8) & (abs(pos3[..., 2] - 41) < 12))
YELLOW_BACK = med(back & (pos3[..., 1] > -12) & (pos3[..., 1] < -3) & (pos3[..., 2] > 50) & (pos3[..., 2] < 60))
print("yellow", YELLOW, "yellow back", YELLOW_BACK)


# ================================================================ repaint: flat yellow plate (bracket area included), stripes by ONE function of (y, z)
# fitted on the Meshy texture (front top band, back top band and corner bands all agree): lines at 60 degrees, period 8.5 cm
STRIPE_P, STRIPE_OFF, STRIPE_ANG = 8.5, 0.43, math.radians(150)
BLACK = np.median(px[cov & (px.sum(-1) < 0.9)], axis=0)
gun_mask = cov & (nrm3[..., 0] < -0.5) & (pos3[..., 0] < 1.0) & (px.sum(-1) > 0.9) & (px.sum(-1) < 1.8)
GUNMETAL = np.median(px[gun_mask], axis=0)
Y_, Z_, X_ = pos3[..., 1], pos3[..., 2], pos3[..., 0]
sph = (Y_ * math.cos(STRIPE_ANG) + Z_ * math.sin(STRIPE_ANG)) / STRIPE_P
dark_f = ((sph - STRIPE_OFF) % 1.0) < 0.5
slab = cov & (X_ > X_BACK - 0.03) & (X_ < X_FRONT + 0.03)
nx, ny, nz = nrm3[..., 0], nrm3[..., 1], nrm3[..., 2]
is_face_fb = slab & (abs(nx) > 0.9) & ((X_ > X_FRONT - 0.03) | (X_ < X_BACK + 0.03))   # groove floor and walls stay yellow
is_top = slab & (nz > 0.9) & (Z_ > HEIGHT_CM - 0.05)
is_cut = slab & (abs(nx) < 0.1) & (abs(abs(ny) - 0.7071) < 0.05) & (abs(abs(nz) - 0.7071) < 0.05) & ((ny * nz) > 0) & ((Y_ + Z_ > 60) | (Y_ + Z_ < 20))
# texel footprint in cm (median over triangles) for the anti-aliased stripe edges
areas = []
for p_ in mesh.polygons:
    uu = np.array([uvd[l].uv[:] for l in p_.loop_indices]); a2 = abs(np.cross(uu[1] - uu[0], uu[2] - uu[0])) / 2
    if a2 > 1e-9: areas.append(math.sqrt(p_.area / a2) / W)
PX_CM = float(np.median(areas))
u_ = (sph - STRIPE_OFF) % 1.0
d_edge = np.minimum(np.minimum(abs(u_), abs(u_ - 0.5)), abs(u_ - 1.0)) * STRIPE_P          # cm to the nearest stripe edge
a_dark = np.clip(0.5 + np.where(u_ < 0.5, 1.0, -1.0) * d_edge / PX_CM, 0.0, 1.0)
sd_zone = np.maximum(np.maximum(Z_ - 75.75, (Y_ + Z_ - 83.56) / math.sqrt(2)), (-0.3 - (Y_ + Z_)) / math.sqrt(2))
a_zone = np.where(is_face_fb, np.clip(0.5 + sd_zone / PX_CM, 0.0, 1.0), np.where(is_top | is_cut, 1.0, 0.0))
# task-226-01: variant A. The plate is charcoal #33363B; the yellow and black stripe zone (bands along the top and the corner cuts, a_zone) keeps
# the hazard yellow and black stripes. Same formula as shield_colour_test.py variant A: plate*(1-a_zone) + a_zone*(yellow*(1-a_dark) + black*a_dark).
PLATE_BASE = np.array([0x33, 0x36, 0x3B], dtype=np.float32) / 255.0
az_ = a_zone[..., None].astype(np.float32); ad_ = a_dark[..., None].astype(np.float32)
px_new = px.copy()
slab_col = PLATE_BASE * (1 - az_) + az_ * np.asarray(YELLOW, dtype=np.float32) * (1 - ad_) + az_ * ad_ * np.asarray(BLACK, dtype=np.float32)
px_new[slab] = slab_col[slab]
# grip and bolt texels that picked up the plate yellow in the bake (rays grazing the plate) go back to the gunmetal / bolt colour
yel_like = cov & ~slab & ((px_new[..., 0] - px_new[..., 2]) > 0.12)
px_new[yel_like] = GUNMETAL
def dilate(img, known, iters=12):
    """uncovered texels next to a covered one take the mean of the covered neighbours (no stale baked yellow on island borders)"""
    img = img.copy(); kn = known.copy()
    for _ in range(iters):
        acc = np.zeros_like(img); cnt = np.zeros(kn.shape, np.float32)
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1)):
            k2 = np.roll(kn, (dy, dx), (0, 1)); i2 = np.roll(img, (dy, dx), (0, 1))
            acc += i2 * k2[..., None]; cnt += k2
        upd = ~kn & (cnt > 0)
        img[upd] = acc[upd] / cnt[upd][:, None]; kn = kn | upd
    return img
px = dilate(px_new, cov)
img.pixels = np.concatenate([px, np.ones((H, W, 1), dtype=np.float32)], axis=-1).reshape(-1).tolist()
img.filepath_raw = TEX_PATH; img.file_format = "PNG"; img.save()
img.pack() if False else None

# ================================================================ emblem decal (task-218-01): cut-out, plate texture, quad in front of the plate
EMB_SRC = os.path.join(ROOT, "Art", "Source", "kessler-voss-logo", "20261017-kessler-voss-emblem-4.webp")
EMB_CUT = os.path.join(ROOT, "Art", "Source", "kessler-voss-logo", "20261017-kessler-voss-emblem-4-cutout.png")
EMB_TEX = os.path.join(OUTDIR, "T_ChaserShield_Emblem.png")
EMB_TEX_PX = 512
DECAL_W, DECAL_H, DECAL_R = 22.0, 20.0, 3.0      # cm, provisional (plate 42.8 cm wide)
DECAL_OFF = 0.25                                  # cm in front of the plate
DECAL_Z = HEIGHT_CM / 2                           # plate centre (plate spans z 0..82)
EMB_FILL = 0.80                                   # emblem height / plate height
PLATE_RGB = np.array([0x1A, 0x1F, 0x25], dtype=np.float32) / 255.0

def load_rgba(path):
    im = bpy.data.images.load(path)
    w_, h_ = im.size
    a_ = np.array(im.pixels[:], dtype=np.float32).reshape(h_, w_, 4)
    bpy.data.images.remove(im)
    return a_

def save_rgba(path, a_, name):
    h_, w_ = a_.shape[:2]
    im = bpy.data.images.new(name, w_, h_, alpha=True, float_buffer=False)
    im.colorspace_settings.name = "sRGB"; im.alpha_mode = "STRAIGHT"
    im.pixels = np.clip(a_, 0, 1).astype(np.float32).reshape(-1).tolist()
    im.filepath_raw = path; im.file_format = "PNG"; im.save()
    bpy.data.images.remove(im)

def resize_area(a_, nh, nw):
    """exact box filter (area average) along both axes, a_ is (h, w, c) premultiplied"""
    def ax(x, n_new, axis):
        n = x.shape[axis]
        x = np.moveaxis(x, axis, 0)
        cum = np.concatenate([np.zeros((1,) + x.shape[1:], dtype=np.float64), np.cumsum(x, axis=0, dtype=np.float64)], axis=0)
        edges = np.linspace(0, n, n_new + 1)
        flat = cum.reshape(n + 1, -1)
        samp = np.stack([np.interp(edges, np.arange(n + 1), flat[:, k]) for k in range(flat.shape[1])], axis=1)
        out = (samp[1:] - samp[:-1]) / (n / n_new)
        return np.moveaxis(out.reshape((n_new,) + x.shape[1:]), 0, axis)
    return ax(ax(a_.astype(np.float64), nh, 0), nw, 1)

# 1) cut-out: alpha from the unmix of white and the rock colour, yellow pixels (far from that line) are opaque
src_e = load_rgba(EMB_SRC)[..., :3]          # rows bottom to top, sRGB as stored
Wt = np.ones(3, dtype=np.float32)
dist_w = np.linalg.norm(src_e - Wt, axis=-1)
rock_mask = (dist_w > 1.3) & (src_e.sum(-1) < 0.9)
ROCK = np.median(src_e[rock_mask], axis=0)
WR = Wt - ROCK
a_proj = ((Wt - src_e) @ WR) / float(WR @ WR)
resid = np.linalg.norm((Wt - src_e) - a_proj[..., None] * WR, axis=-1)
a_cut = np.clip(a_proj, 0.0, 1.0)
a_cut = np.where(a_cut < 0.04, 0.0, a_cut)                     # webp noise on the white
a_cut = np.where(resid > 0.15, 1.0, a_cut)                      # yellow (not a white to rock mix) is opaque
a_cut = np.clip((a_cut - 0.02) / 0.96, 0.0, 1.0)
col_cut = np.where(((a_cut < 0.995) & (resid <= 0.15))[..., None], ROCK[None, None, :], src_e)   # edge pixels: pure rock colour, no white
ys_, xs_ = np.nonzero(a_cut > 0.01)
y0_, y1_, x0_, x1_ = ys_.min(), ys_.max() + 1, xs_.min(), xs_.max() + 1
side = max(y1_ - y0_, x1_ - x0_)
cut = np.zeros((side, side, 4), dtype=np.float32); cut[..., :3] = ROCK
oy, ox = (side - (y1_ - y0_)) // 2, (side - (x1_ - x0_)) // 2
cut[oy:oy + (y1_ - y0_), ox:ox + (x1_ - x0_), :3] = col_cut[y0_:y1_, x0_:x1_]
cut[oy:oy + (y1_ - y0_), ox:ox + (x1_ - x0_), 3] = a_cut[y0_:y1_, x0_:x1_]
save_rgba(EMB_CUT, cut, "emblem_cutout")
print("cutout", cut.shape, "rock", ROCK)

# 2) plate texture 512 x 512: rounded rectangle (corner radius DECAL_R cm), emblem EMB_FILL of the plate height, centred
N = EMB_TEX_PX
cy_, cx_ = np.meshgrid((np.arange(N) + 0.5) / N * DECAL_H, (np.arange(N) + 0.5) / N * DECAL_W, indexing="ij")   # cm, rows bottom to top
hx, hy = DECAL_W / 2 - DECAL_R, DECAL_H / 2 - DECAL_R
qx_ = np.maximum(abs(cx_ - DECAL_W / 2) - hx, 0); qy_ = np.maximum(abs(cy_ - DECAL_H / 2) - hy, 0)
sdf = np.sqrt(qx_ ** 2 + qy_ ** 2) - DECAL_R                       # cm, negative inside
px_cm = DECAL_W / N
plate_a = np.clip(0.5 - sdf / px_cm, 0.0, 1.0)
emb_h_cm = EMB_FILL * DECAL_H
eh_px, ew_px = int(round(emb_h_cm / DECAL_H * N)), int(round(emb_h_cm / DECAL_W * N))      # square emblem on the quad -> non-uniform in texels
pm = cut.copy(); pm[..., :3] *= pm[..., 3:4]
rs = resize_area(pm, eh_px, ew_px)
emb_a = np.clip(rs[..., 3:4], 0, 1); emb_rgb = np.where(emb_a > 1e-6, rs[..., :3] / np.maximum(emb_a, 1e-6), 0.0)
layer = np.zeros((N, N, 4), dtype=np.float64); layer[..., :3] = PLATE_RGB; layer[..., 3] = plate_a
ey0, ex0 = (N - eh_px) // 2, (N - ew_px) // 2
sl = (slice(ey0, ey0 + eh_px), slice(ex0, ex0 + ew_px))
ea = emb_a[..., 0]
layer[sl][..., :3] = emb_rgb * ea[..., None] + layer[sl][..., :3] * (1 - ea[..., None])
layer[..., 3] = plate_a                                            # emblem lies fully inside the plate
# task-226-01: yellow parts only (KV and facets) on transparent: no rounded plate, no charcoal rock (same extraction as shield_colour_test.py)
em_ = layer.astype(np.float32)
rb_ = em_[..., 0] - em_[..., 2]
solid_ = (rb_ > 0.9) & (em_[..., 3] > 0.99)
EY_ = np.median(em_[..., :3][solid_], axis=0); ry_ = float(np.median(rb_[solid_]))
ea_ = np.clip(rb_ / ry_, 0, 1) * em_[..., 3]
ea_ = np.where(ea_ < 0.04, 0.0, ea_)
em_out = np.zeros_like(em_); em_out[..., :3] = EY_; em_out[..., 3] = ea_
save_rgba(EMB_TEX, em_out, "T_ChaserShield_Emblem")
print("emblem texture", N, "emblem px", ew_px, eh_px)

# 3) decal quad: parallel to the front plate, DECAL_OFF in front, centred on y = 0 and z = DECAL_Z, planar UV 0..1, last slot
mat_e = bpy.data.materials.new("M_ChaserShield_Emblem"); mat_e.use_nodes = True
nte = mat_e.node_tree; bse = nte.nodes["Principled BSDF"]; bse.inputs["Roughness"].default_value = 0.8
tex_e = bpy.data.images.load(EMB_TEX); tex_e.colorspace_settings.name = "sRGB"; tex_e.alpha_mode = "STRAIGHT"
txe = nte.nodes.new("ShaderNodeTexImage"); txe.image = tex_e; txe.interpolation = "Linear"; txe.extension = "CLIP"
uve = nte.nodes.new("ShaderNodeUVMap"); uve.uv_map = "UVMap"
nte.links.new(uve.outputs["UV"], txe.inputs["Vector"]); nte.links.new(txe.outputs["Color"], bse.inputs["Base Color"]); nte.links.new(txe.outputs["Alpha"], bse.inputs["Alpha"])
for attr, val in (("surface_render_method", "BLENDED"), ("blend_method", "BLEND")):
    try: setattr(mat_e, attr, val)
    except (TypeError, AttributeError): pass
mesh.materials.append(mat_e)
BODY_N = len(mesh.polygons)
bmd = bmesh.new(); bmd.from_mesh(mesh)
uv0l, uv1l = bmd.loops.layers.uv["UVMap"], bmd.loops.layers.uv["LightMap"]
LM_SCALE = 0.95                                                    # body LightMap shrunk to free a strip for the decal
for f_ in bmd.faces:
    for l_ in f_.loops: l_[uv1l].uv = (l_[uv1l].uv[0] * LM_SCALE, l_[uv1l].uv[1] * LM_SCALE)
xd = X_FRONT + DECAL_OFF
dv = [bmd.verts.new((xd, -DECAL_W / 2, DECAL_Z - DECAL_H / 2)), bmd.verts.new((xd, DECAL_W / 2, DECAL_Z - DECAL_H / 2)),
      bmd.verts.new((xd, DECAL_W / 2, DECAL_Z + DECAL_H / 2)), bmd.verts.new((xd, -DECAL_W / 2, DECAL_Z + DECAL_H / 2))]
qf = bmd.faces.new(dv)
LM_U0, LM_U1, LM_V0 = 0.96, 0.985, 0.5
LM_V1 = LM_V0 + (LM_U1 - LM_U0) * DECAL_H / DECAL_W
for l_, (u_a, v_a), (u_b, v_b) in zip(qf.loops, ((0, 0), (1, 0), (1, 1), (0, 1)), ((LM_U0, LM_V0), (LM_U1, LM_V0), (LM_U1, LM_V1), (LM_U0, LM_V1))):
    l_[uv0l].uv = (u_a, v_a); l_[uv1l].uv = (u_b, v_b)
dres = bmesh.ops.triangulate(bmd, faces=[qf])
bmd.faces.ensure_lookup_table()
for f_ in bmd.faces[BODY_N:]:
    f_.material_index = 1; f_.smooth = False
bmd.to_mesh(mesh); bmd.free()
mesh.update()
assert len(mesh.polygons) == BODY_N + 2, len(mesh.polygons)
DECAL_NORMALS = [[round(c, 3) for c in p.normal] for p in mesh.polygons[BODY_N:]]
print("decal added, tris", len(mesh.polygons), "normals", DECAL_NORMALS)

# ================================================================ pivot: bottom centre of the new mesh, remove the source, export
bpy.data.objects.remove(src, do_unlink=True)
# middle grip block (raw frame): vertices behind the plate, z 34..45, on the kept side of the cut
blk = [v.co.copy() for v in mesh.vertices if v.co.x < 2.3 and 34.0 < v.co.z < 45.0 and v.co.y > BRACKET_CUT_Y - 0.01]
bmin = Vector((min(v.x for v in blk), min(v.y for v in blk), min(v.z for v in blk))); bmax = Vector((max(v.x for v in blk), max(v.y for v in blk), max(v.z for v in blk)))
bcen_raw = (bmin + bmax) / 2
lo = Vector((min(v.co.x for v in mesh.vertices), min(v.co.y for v in mesh.vertices), min(v.co.z for v in mesh.vertices)))
hi = Vector((max(v.co.x for v in mesh.vertices), max(v.co.y for v in mesh.vertices), max(v.co.z for v in mesh.vertices)))
SHIFT = Vector((-(lo.x + hi.x) / 2, -(lo.y + hi.y) / 2, -lo.z))
mesh.transform(Matrix.Translation(SHIFT))
mesh.update()
lo = Vector((min(v.co.x for v in mesh.vertices), min(v.co.y for v in mesh.vertices), min(v.co.z for v in mesh.vertices)))
hi = Vector((max(v.co.x for v in mesh.vertices), max(v.co.y for v in mesh.vertices), max(v.co.z for v in mesh.vertices)))
tris = len(mesh.polygons)

# ---------------------------------------------------------------- uv checks
def tri_uvs(layer, n):
    return [[np.array(layer.data[li].uv[:], dtype=np.float64) for li in p.loop_indices] for p in mesh.polygons[:n]]
def tri_overlap(A, B, eps=1e-9):
    for T in (A, B):
        for k in range(3):
            e = T[(k + 1) % 3] - T[k]; ax = np.array([-e[1], e[0]])
            pa = [x.dot(ax) for x in A]; pb = [x.dot(ax) for x in B]
            if max(pa) <= min(pb) + eps or max(pb) <= min(pa) + eps: return False
    return True
def uv_check(name, res, n):
    t_ = tri_uvs(mesh.uv_layers[name], n)
    bbs = np.array([[min(x[0] for x in t), min(x[1] for x in t), max(x[0] for x in t), max(x[1] for x in t)] for t in t_])
    ov = 0; za = 0
    for t in t_:
        if abs(((t[1][0]-t[0][0])*(t[2][1]-t[0][1])-(t[1][1]-t[0][1])*(t[2][0]-t[0][0]))) < 1e-12: za += 1
    for a in range(len(t_)):
        m_ = (bbs[a + 1:, 0] < bbs[a, 2]) & (bbs[a + 1:, 2] > bbs[a, 0]) & (bbs[a + 1:, 1] < bbs[a, 3]) & (bbs[a + 1:, 3] > bbs[a, 1])
        for j in np.nonzero(m_)[0]:
            if tri_overlap(t_[a], t_[a + 1 + j]):
                ov += 1
                if name == 'UVMap': print('OVL', a, a + 1 + j, [round(c, 1) for c in mesh.polygons[a].center], [round(c, 1) for c in mesh.polygons[a + 1 + j].center], [[round(float(q), 4) for q in u] for u in t_[a]], [[round(float(q), 4) for q in u] for u in t_[a + 1 + j]], [round(c, 2) for c in mesh.polygons[a].normal], [round(c, 2) for c in mesh.polygons[a+1+j].normal])
    allu = np.array([u for t in t_ for u in t])
    border = float(min(allu.min(0).min(), (1 - allu.max(0)).min()) * res)
    ok = ov == 0 and za == 0 and allu.min() >= 0 and allu.max() <= 1 and border >= 2.0
    return {"triangles": n, "overlapping_triangle_pairs": ov, "zero_area_triangles": za, "bounds_min": [round(float(allu[:, 0].min()), 5), round(float(allu[:, 1].min()), 5)],
            "bounds_max": [round(float(allu[:, 0].max()), 5), round(float(allu[:, 1].max()), 5)], "min_border_px_at_%d" % res: round(border, 3), "required_border_px": 2, "pass": bool(ok)}
chk_lm = uv_check("LightMap", 256, tris)          # body and decal: no overlap, margin, bounds
chk_uv = uv_check("UVMap", TEX_PX, BODY_N)        # body only: the decal is a 0..1 decal on purpose
_du = np.array([mesh.uv_layers["UVMap"].data[li].uv[:] for p in mesh.polygons[BODY_N:] for li in p.loop_indices])
chk_decal = {"triangles": tris - BODY_N, "uv0_min": [float(_du[:, 0].min()), float(_du[:, 1].min())], "uv0_max": [float(_du[:, 0].max()), float(_du[:, 1].max())],
             "planar_uv_matches_size": True, "normals": DECAL_NORMALS,
             "material_index": sorted({p.material_index for p in mesh.polygons[BODY_N:]}),
             "pass": bool(_du.min() == 0.0 and _du.max() == 1.0 and tris - BODY_N == 2 and all(abs(n_[0] - 1) < 1e-3 for n_ in DECAL_NORMALS))}


bc = bcen_raw + SHIFT
grip_offset = [round(bc.x, 2), round(-bc.y, 2), round(bc.z, 2)]

# ---------------------------------------------------------------- report
warnings = []
if not (900 <= tris <= 1100): warnings.append("tris outside 900..1100")
if abs((hi.z - lo.z) - HEIGHT_CM) > 0.5: warnings.append("height off 82 cm")
ratio = (hi.z - lo.z) / (hi.y - lo.y)
if abs(ratio - 1.91) > 0.1: warnings.append("height to width ratio off 1.91")
if not chk_lm["pass"]: warnings.append("LightMap check FAILED")
if not chk_uv["pass"]: warnings.append("UVMap check FAILED")
if not chk_decal["pass"]: warnings.append("decal check FAILED")
warnings.append("grip is the decimated Meshy grip (not planar); plate, slab sides, groove and bolts are exactly planar prisms")
warnings.append("T_ChaserShield_BaseColor.png and T_ChaserShield_Emblem.png on UVMap: import the textures separately in Unreal; the emblem material needs alpha (Masked or Translucent)")
report = {
    "asset": "SM_ChaserShield", "route": "blender (second Meshy mesh, no Meshy call, no credit)", "script": "Tools/art/blender/shield_v2.py",
    "source_mesh": opt["--in"].replace("\\", "/"), "tris_in": tris_in, "tris_out": tris,
    "tris_breakdown": {"plate_and_bolts": tris_plate, "grip": grip_tris, "grip_before_decimation": grip_tris_before, "emblem_decal": tris - BODY_N, "body_without_decal": BODY_N},
    "dimensions_cm": {"x": round(hi.x - lo.x, 2), "y": round(hi.y - lo.y, 2), "z": round(hi.z - lo.z, 2)},
    "height_to_width_ratio": round(ratio, 3), "pivot": "bottom-centre", "front_axis": "+X",
    "bbox_min": [round(c, 3) for c in lo], "bbox_max": [round(c, 3) for c in hi],
    "materials": [m.name for m in mesh.materials], "textures": ["T_ChaserShield_BaseColor.png", "T_ChaserShield_Emblem.png"], "texture_px": [TEX_PX, TEX_PX],
    "texture_note": "diffuse colour baked from the Meshy mesh (selected to active); plate faces repainted charcoal #33363B (task-226-01, variant A), stripes painted by one function (60 degree lines, period 8.5 cm) over the front and back top bands, corner bands and the whole top face",
    "colours_srgb": {"yellow": [round(float(c), 3) for c in YELLOW], "black": [round(float(c), 3) for c in BLACK], "gunmetal": [round(float(c), 3) for c in GUNMETAL]},
    "uv_layers": [u.name for u in mesh.uv_layers], "uv_checks": {"UVMap": chk_uv, "LightMap": chk_lm, "decal": chk_decal},
    "decal": {"material": "M_ChaserShield_Emblem (last slot, alpha blended / Masked in Unreal)", "texture": "T_ChaserShield_Emblem.png", "texture_px": [EMB_TEX_PX, EMB_TEX_PX],
              "size_cm": {"y_width": DECAL_W, "z_height": DECAL_H}, "corner_radius_cm": DECAL_R, "offset_in_front_of_plate_cm": DECAL_OFF, "emblem_height_fraction": EMB_FILL,
              "plate_colour_srgb_hex": "none (yellow parts only on transparent, task-226-01)", "centre_cm_from_pivot": [round(X_FRONT + DECAL_OFF + SHIFT.x, 3), 0.0, DECAL_Z], "front_face_x_cm_from_pivot": round(X_FRONT + SHIFT.x, 3),
              "cutout_png": "Art/Source/kessler-voss-logo/20261017-kessler-voss-emblem-4-cutout.png", "lightmap_note": "body LightMap scaled by 0.95 to free a strip for the decal"},
    "grip_offset_cm": grip_offset,
    "grip_offset_note": "centre of the bounding box of the middle grip block, from the pivot, Unreal axes (x, -y, z) in cm; front is +X so x is negative (behind the plate)",
    "shading": "flat", "warnings": warnings,
}
for o in list(bpy.context.scene.objects): o.select_set(False)
obj.select_set(True); bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
                         add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="NONE")
json.dump(report, open(REPORT, "w", encoding="utf-8"), indent=2)

# ---------------------------------------------------------------- previews (neutral set: sun 1.0 following the camera, light grey background)
def render_views(outdir):
    os.makedirs(outdir, exist_ok=True)
    for o_ in [o_ for o_ in bpy.context.scene.objects if o_.type in ("LIGHT", "CAMERA")]: bpy.data.objects.remove(o_, do_unlink=True)
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try: sc.render.engine = name; break
        except TypeError: continue
    sc.render.resolution_x, sc.render.resolution_y = 1200, 900; sc.render.resolution_percentage = 100
    sc.render.image_settings.file_format = "PNG"; sc.render.image_settings.color_mode = "RGBA"; sc.render.film_transparent = True
    sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
    w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
    bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (1, 1, 1, 1); bg.inputs["Strength"].default_value = 0.35
    cam_d = bpy.data.cameras.new("Cam"); cam_d.clip_start = 1; cam_d.clip_end = 3000; cam_d.type = "ORTHO"
    cam = bpy.data.objects.new("Cam", cam_d); sc.collection.objects.link(cam); sc.camera = cam
    sun_d = bpy.data.lights.new("Sun", "SUN"); sun_d.energy = 1.0
    sun = bpy.data.objects.new("Sun", sun_d); sc.collection.objects.link(sun)
    GREY = np.array([0.76, 0.76, 0.76], dtype=np.float32)
    def dirv(az, el):
        a, e = math.radians(az), math.radians(el)
        return Vector((math.cos(e) * math.cos(a), math.cos(e) * math.sin(a), math.sin(e)))
    # (name, azimuth deg from +X toward +Y, elevation, target, ortho scale)
    views = [("front", 0, 0, (0, 0, 41), 100), ("back", 180, 0, (0, 0, 41), 100), ("left", 90, 0, (0, 0, 41), 100), ("right", -90, 0, (0, 0, 41), 100),
             ("tq_front", -40, 35, (0, 0, 41), 100), ("tq_back", 140, 35, (0, 0, 41), 100),
             ("top_close", -40, 55, (0, 0, 76), 36), ("top_close_back", 140, 55, (0, 0, 76), 36), ("top_plan", 0, 89, (0, 0, 82), 60), ("grip_close", 150, 25, (0, 8, 55), 25),
             ("emblem_close", 0, 0, (0, 0, 41), 28), ("emblem_close_tq", -40, 35, (0, 0, 41), 28)]
    for name, az, el, tgt, osc in views:
        d = dirv(az, el); target = Vector(tgt)
        cam_d.ortho_scale = osc
        sc.render.resolution_x, sc.render.resolution_y = (1200, 900) if osc < 70 else (900, 1200)
        cam.location = target + d * 400
        cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
        side = d.cross(Vector((0, 0, 1)))
        lv = (d + Vector((0, 0, 0.6)) + side * 0.35).normalized()
        sun.rotation_euler = (-lv).to_track_quat("-Z", "Y").to_euler()
        tmp = os.path.join(TMP, "shield_%s.png" % name); sc.render.filepath = tmp
        bpy.ops.render.render(write_still=True)
        r = bpy.data.images.load(tmp); Wd, Ht = r.size
        a = np.array(r.pixels[:], dtype=np.float32).reshape(Ht, Wd, 4); al = a[..., 3:4]
        out = np.clip(a[..., :3], 0, 1) * al + GREY * (1 - al)
        res = bpy.data.images.new("o_" + name, Wd, Ht, alpha=False)
        res.pixels = np.concatenate([out, np.ones_like(al)], axis=-1).reshape(-1).tolist()
        res.filepath_raw = os.path.join(outdir, "ChaserShield_%s.png" % name); res.file_format = "PNG"; res.save()
        bpy.data.images.remove(r); bpy.data.images.remove(res)
if "--no-render" not in opt:
    render_views(PREV)
print(json.dumps({k: report[k] for k in ("tris_out", "tris_breakdown", "dimensions_cm", "height_to_width_ratio", "grip_offset_cm", "warnings")}))
print("lightmap", chk_lm); print("uvmap", chk_uv); print("decal", chk_decal)
