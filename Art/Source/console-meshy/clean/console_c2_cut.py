"""task-301-02: cut the Meshy console C2 mesh into a base and a mobile head, and split the material slots. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Art/Source/console-meshy/clean/console_c2_cut.py -- [--cut-z 108] [--out-dir Art/Source/console-meshy/clean]
Source: the untouched Meshy FBX (Blender unit = 1 m, 1.9 m tall); everything below is worked in cm (x100), stage frame z = 0 at the feet.
1. Screen: the planar slanted panel on the console front is cut along the screen rectangle (4 bisects limited to the panel faces) so the Screen slot is a clean rectangle.
2. Orange: faces are classified from the base colour texture (hue/saturation/value at 10 sample points per face). Mixed faces longer than 0.5 cm are subdivided (up to 3 rounds, UV interpolated) so
   the slot edge follows the painted lamp; a face is orange when at least half of its samples are.
3. Cut: one horizontal plane z = CUT_Z through the mast neck (single closed section there), cap faces on both parts (UV0 of the nearest face), parts by connectivity.
4. Slots: ConsoleBody (base colour texture), OtherOrange (orange faces of the base), AntennaOrange (orange faces of the head), Screen (screen rectangle).
5. Pivots: base = bottom centre of its footprint (x, y = middle of the bounding box, z = lowest point); head = the mast axis (area centroid of the head cap) at the cut height.
6. Export (FBX_SCALE_UNITS, metres in Blender = cm in Unreal, flat shading): console_c2_base.fbx, console_c2_head.fbx, report console_c2_cut.report.json. Blender only, nothing imported in Unreal."""
import bpy, bmesh, sys, os, json, time, shutil, struct
import numpy as np
from mathutils import Vector
from collections import defaultdict

T0 = time.time()
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--cut-z": "108.0", "--out-dir": "Art/Source/console-meshy/clean",
       "--screen": "2.4,26.4,57.4,68.5"}      # x0,x1,z0,z1 of the screen rectangle (cm, stage frame, from the front view)
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out-dir"])
CUT_Z = float(opt["--cut-z"])
SX0, SX1, SZ0, SZ1 = [float(x) for x in opt["--screen"].split(",")]
SRC_DIR = os.path.abspath("Art/Source/console-meshy/meshy")
SRC_FBX = os.path.join(SRC_DIR, "Meshy_AI_Satellite_Receiver_St_1023222045_texture.fbx")
SRC_TEX = os.path.join(SRC_DIR, "Meshy_AI_Satellite_Receiver_St_1023222045_texture.png")
TEX_NAME = "console_c2_basecolor.png"
os.makedirs(OUT, exist_ok=True)
if not os.path.exists(os.path.join(OUT, TEX_NAME)):
    shutil.copyfile(SRC_TEX, os.path.join(OUT, TEX_NAME))

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=SRC_FBX)
src = [o for o in bpy.data.objects if o.type == "MESH"][0]
bm = bmesh.new()
bm.from_mesh(src.data)
for v in bm.verts:
    v.co = src.matrix_world @ v.co
zmin = min(v.co.z for v in bm.verts)
for v in bm.verts:
    v.co = Vector((v.co.x * 100, v.co.y * 100, (v.co.z - zmin) * 100))
bmesh.ops.triangulate(bm, faces=bm.faces[:])
uv0 = bm.loops.layers.uv[0]
TRIS_IN = len(bm.faces)
SRC_OPEN = sum(1 for e in bm.edges if len(e.link_faces) == 1)
STAGE_MIN = np.array([min(v.co[i] for v in bm.verts) for i in range(3)])
STAGE_MAX = np.array([max(v.co[i] for v in bm.verts) for i in range(3)])

img = bpy.data.images.load(SRC_TEX)
TW, TH = img.size
TEXPX = np.array(img.pixels[:], dtype=np.float32).reshape(TH, TW, 4)[:, :, :3]


def sample_tex(u, v):
    return TEXPX[np.clip((v * TH).astype(int), 0, TH - 1), np.clip((u * TW).astype(int) % TW, 0, TW - 1)]


def is_orange(c):
    mx, mn = c.max(-1), c.min(-1)
    d = mx - mn + 1e-9
    r, g, b = c[..., 0], c[..., 1], c[..., 2]
    h = np.where(mx == r, ((g - b) / d) % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) / 6.0
    s = d / (mx + 1e-9)
    return (h > 0.07) & (h < 0.15) & (s > 0.6) & (mx > 0.6)


BARY = np.array([[1 / 3, 1 / 3, 1 / 3], [.8, .1, .1], [.1, .8, .1], [.1, .1, .8], [.5, .5, 0], [0, .5, .5], [.5, 0, .5],
                 [.6, .2, .2], [.2, .6, .2], [.2, .2, .6]])


def face_orange_fraction(faces):
    U = np.array([[l[uv0].uv[:] for l in f.loops] for f in faces])      # (n, 3, 2)
    P = np.einsum("sk,nkc->nsc", BARY, U)
    return is_orange(sample_tex(P[..., 0], P[..., 1])).mean(1)


def longest_edge(f):
    return max(e.calc_length() for e in f.edges)


# ------------------------------------------------------------ 1. screen rectangle
cand = [f for f in bm.faces if abs(f.normal.x) < 0.05 and f.normal.y < -0.7 and f.normal.z > 0.4
        and SX0 - 1 < f.calc_center_median().x < SX1 + 1 and SZ0 - 1 < f.calc_center_median().z < SZ1 + 1
        and f.calc_area() > 0.3]
n_pl = sum((f.normal for f in cand), Vector()).normalized()
d_pl = float(np.median([f.calc_center_median().dot(n_pl) for f in cand]))
SCREEN_PLANE = dict(normal=[round(x, 4) for x in n_pl], offset_cm=round(d_pl, 3))


def patch():
    out = []
    for f in bm.faces:
        c = f.calc_center_median()
        if f.normal.dot(n_pl) > 0.995 and abs(c.dot(n_pl) - d_pl) < 0.6 and SX0 - 3 < c.x < SX1 + 3 and SZ0 - 3 < c.z < SZ1 + 3 \
                and all(SX0 - 6 < v.co.x < SX1 + 6 and SZ0 - 6 < v.co.z < SZ1 + 6 for v in f.verts):
            out.append(f)
    return out


for co, no in (((SX0, 0, 0), (1, 0, 0)), ((SX1, 0, 0), (1, 0, 0)), ((0, 0, SZ0), (0, 0, 1)), ((0, 0, SZ1), (0, 0, 1))):
    fs = patch()
    es = list({e for f in fs for e in f.edges})
    vs = list({v for f in fs for v in f.verts})
    bmesh.ops.bisect_plane(bm, geom=fs + es + vs, dist=1e-5, plane_co=Vector(co), plane_no=Vector(no))
    bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 3])
bm.faces.ensure_lookup_table()
SCREEN_PATCH_FACES = len(patch())


def in_screen(f):
    c = f.calc_center_median()
    return f.normal.dot(n_pl) > 0.995 and abs(c.dot(n_pl) - d_pl) < 0.6 and SX0 < c.x < SX1 and SZ0 < c.z < SZ1


# ------------------------------------------------------------ 2. orange refinement
ROUNDS = []
for rnd in range(3):
    fl = bm.faces[:]
    fr = face_orange_fraction(fl)
    mixed = [f for f, q in zip(fl, fr) if 0.05 < q < 0.95 and longest_edge(f) > 0.5 and not in_screen(f)]
    ROUNDS.append(len(mixed))
    if not mixed:
        break
    es = list({e for f in mixed for e in f.edges})
    bmesh.ops.subdivide_edges(bm, edges=es, cuts=1, use_grid_fill=True)
    bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 3])
    bm.faces.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.normal_update()
TRIS_REFINED = len(bm.faces)

# ------------------------------------------------------------ 3. horizontal cut at the mast neck
lay_cut = bm.edges.layers.int.new("cutid")
lay_cap = bm.faces.layers.int.new("iscap")
r = bmesh.ops.bisect_plane(bm, geom=bm.verts[:] + bm.edges[:] + bm.faces[:], dist=1e-5, plane_co=Vector((0, 0, CUT_Z)), plane_no=Vector((0, 0, 1)))
cut_edges = [g for g in r["geom_cut"] if isinstance(g, bmesh.types.BMEdge)]
for e in cut_edges:
    e[lay_cut] = 1
sec = np.array([v.co[:] for e in cut_edges for v in e.verts])
SEC_MIN, SEC_MAX = sec.min(0), sec.max(0)
bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 3])
bmesh.ops.split_edges(bm, edges=[e for e in bm.edges if e[lay_cut] > 0])
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bnd = [e for e in bm.edges if e[lay_cut] == 1 and len(e.link_faces) == 1]
fill = list(bmesh.ops.holes_fill(bm, edges=bnd, sides=2000)["faces"])
if [e for e in bm.edges if e[lay_cut] == 1 and len(e.link_faces) == 1]:
    raise SystemExit("cap loops not closed")
for f in fill:
    f[lay_cap] = 1
bmesh.ops.triangulate(bm, faces=fill)
bm.verts.ensure_lookup_table()
bm.edges.ensure_lookup_table()
bm.faces.ensure_lookup_table()
bm.faces.index_update()
bm.normal_update()
NF = len(bm.faces)
comp = -np.ones(NF, int)
nc = 0
for f0 in bm.faces:
    if comp[f0.index] >= 0:
        continue
    st = [f0]
    comp[f0.index] = nc
    while st:
        f = st.pop()
        for e in f.edges:
            for g in e.link_faces:
                if comp[g.index] < 0:
                    comp[g.index] = nc
                    st.append(g)
    nc += 1
fc = np.array([f.calc_center_median()[:] for f in bm.faces])
fa = np.array([f.calc_area() for f in bm.faces])
capm = np.array([f[lay_cap] for f in bm.faces]).astype(bool)
comp_z = np.array([(fc[(comp == k) & ~capm][:, 2] * fa[(comp == k) & ~capm]).sum() / fa[(comp == k) & ~capm].sum() for k in range(nc)])
comp_area = np.bincount(comp, weights=fa, minlength=nc)
FRAG = []
partof = np.zeros(NF, int)         # 0 base, 1 head
for k in range(nc):
    partof[comp == k] = 1 if comp_z[k] > CUT_Z else 0
    if comp_area[k] < 0.01 * comp_area.sum():
        FRAG.append(dict(component=k, faces=int((comp == k).sum()), area_cm2=round(float(comp_area[k]), 2), part="head" if comp_z[k] > CUT_Z else "base"))
for f in bm.faces:
    if f[lay_cap]:
        want = 1.0 if partof[f.index] == 0 else -1.0      # base cap faces up, head cap faces down
        if f.normal.z * want < 0:
            f.normal_flip()
        cen = f.calc_center_median()
        cands = {g for v in f.verts for g in v.link_faces if not g[lay_cap] and partof[g.index] == partof[f.index]}
        s_ = min(cands, key=lambda g: (g.calc_center_median() - cen).length)
        u = sum((l[uv0].uv for l in s_.loops), Vector((0, 0))) / 3
        for l in f.loops:
            l[uv0].uv = u
bm.normal_update()

# ------------------------------------------------------------ 4. slots
SLOT_NAMES = ["ConsoleBody", "OtherOrange", "AntennaOrange", "Screen"]
fl = bm.faces[:]
orf = face_orange_fraction(fl)
slot = np.zeros(NF, int)
for f, q in zip(fl, orf):
    if f[lay_cap]:
        continue
    if in_screen(f):
        slot[f.index] = 3
    elif q >= 0.5:
        slot[f.index] = 2 if partof[f.index] == 1 else 1

head_caps = [f for f in bm.faces if f[lay_cap] and partof[f.index] == 1]
ca = np.array([f.calc_area() for f in head_caps])
cc = np.array([f.calc_center_median()[:] for f in head_caps])
AXIS = (cc * ca[:, None]).sum(0) / ca.sum()
AXIS = np.array([AXIS[0], AXIS[1], CUT_Z])
base_v = np.array([v.co[:] for f in bm.faces if partof[f.index] == 0 for v in f.verts])
BASE_PIV = np.array([(base_v[:, 0].min() + base_v[:, 0].max()) / 2, (base_v[:, 1].min() + base_v[:, 1].max()) / 2, base_v[:, 2].min()])
mast_sections = {}
for zz in (98, 102, 106):
    b2 = bmesh.new()
    b2.from_mesh(src.data)
    for v in b2.verts:
        v.co = Vector((v.co.x * 100, v.co.y * 100, (v.co.z - zmin) * 100))
    rr = bmesh.ops.bisect_plane(b2, geom=b2.verts[:] + b2.edges[:] + b2.faces[:], dist=1e-5, plane_co=Vector((0, 0, zz)), plane_no=Vector((0, 0, 1)))
    pts = np.array([v.co[:] for g in rr["geom_cut"] if isinstance(g, bmesh.types.BMEdge) for v in g.verts])
    mast_sections[str(zz)] = [round(float((pts[:, 0].min() + pts[:, 0].max()) / 2), 2), round(float((pts[:, 1].min() + pts[:, 1].max()) / 2), 2)]
    b2.free()


# ------------------------------------------------------------ 5. materials, parts, export
def flat_mat(name, rgb, emit=0.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    p = m.node_tree.nodes["Principled BSDF"]
    p.inputs["Base Color"].default_value = (*rgb, 1)
    p.inputs["Roughness"].default_value = 0.7
    if emit:
        p.inputs["Emission Color"].default_value = (*rgb, 1)
        p.inputs["Emission Strength"].default_value = emit
    return m


def body_mat():
    m = bpy.data.materials.new("ConsoleBody")
    m.use_nodes = True
    nt = m.node_tree
    p = nt.nodes["Principled BSDF"]
    t = nt.nodes.new("ShaderNodeTexImage")
    t.image = bpy.data.images.load(os.path.join(OUT, TEX_NAME), check_existing=False)
    nt.links.new(t.outputs["Color"], p.inputs["Base Color"])
    p.inputs["Roughness"].default_value = 0.8
    return m


MATS = {"ConsoleBody": body_mat(), "OtherOrange": flat_mat("OtherOrange", (1.0, 0.45, 0.02), 2.0),
        "AntennaOrange": flat_mat("AntennaOrange", (1.0, 0.45, 0.02), 2.0), "Screen": flat_mat("Screen", (0.04, 0.06, 0.08))}
PARTS = ["console_c2_base", "console_c2_head"]
PIV = [BASE_PIV, AXIS]
bpy.context.scene.unit_settings.scale_length = 1.0
objs, PART_REP, FACES_SLOT = {}, {}, {}
for pi, name in enumerate(PARTS):
    b = bm.copy()
    b.faces.ensure_lookup_table()
    bmesh.ops.delete(b, geom=[f for f in b.faces if partof[f.index] != pi], context="FACES")
    keep = [i for i in range(NF) if partof[i] == pi]
    b.faces.ensure_lookup_table()
    used = [s for s in range(4) if (slot[keep] == s).any()]
    order = {s: k for k, s in enumerate(used)}
    for f, i in zip(b.faces, keep):
        f.material_index = order[slot[i]]
    for v in b.verts:
        v.co = (v.co - Vector(PIV[pi])) * 0.01
    b.edges.layers.int.remove(b.edges.layers.int["cutid"])
    b.faces.layers.int.remove(b.faces.layers.int["iscap"])
    me = bpy.data.meshes.new(name)
    b.to_mesh(me)
    b.free()
    for s in used:
        me.materials.append(MATS[SLOT_NAMES[s]])
    me.shade_flat()
    o = bpy.data.objects.new(name, me)
    bpy.context.scene.collection.objects.link(o)
    objs[name] = o
    FACES_SLOT[name] = {SLOT_NAMES[s]: int((slot[keep] == s).sum()) for s in used}
    PART_REP[name] = dict(tris=len(me.polygons), vertices=len(me.vertices), pivot_stage_cm=[round(float(x), 3) for x in PIV[pi]])
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
for n in PARTS:
    bpy.ops.object.select_all(action="DESELECT")
    objs[n].select_set(True)
    bpy.context.view_layer.objects.active = objs[n]
    bpy.ops.export_scene.fbx(filepath=os.path.join(OUT, n + ".fbx"), **EXPORT)
usf = []
for n in PARTS:
    b_ = open(os.path.join(OUT, n + ".fbx"), "rb").read()
    j = b_.find(b"UnitScaleFactor")
    k = b_.find(b"D", j + 20)
    usf.append(struct.unpack("<d", b_[k + 1:k + 9])[0])

# ------------------------------------------------------------ read back
RB = {}
for n in PARTS:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=os.path.join(OUT, n + ".fbx"))
    o = [x for x in bpy.data.objects if x.type == "MESH"][0]
    me = o.data
    W = np.array([(o.matrix_world @ v.co)[:] for v in me.vertices]) * 100
    ec = defaultdict(int)
    for p in me.polygons:
        for k in p.edge_keys:
            ec[k] += 1
    mi = np.array([p.material_index for p in me.polygons])
    RB[n] = dict(tris=len(me.polygons), size_cm=[round(float(x), 2) for x in (W.max(0) - W.min(0))], bounds_min_cm=[round(float(x), 2) for x in W.min(0)],
                 bounds_max_cm=[round(float(x), 2) for x in W.max(0)], world_origin_cm=[round(float(x) * 100, 3) for x in o.matrix_world.translation],
                 slots=[m.name.split(".")[0] for m in me.materials], faces_per_slot={me.materials[k].name.split(".")[0]: int((mi == k).sum()) for k in range(len(me.materials))},
                 uv_layers=[u.name for u in me.uv_layers], open_edges=sum(1 for v in ec.values() if v == 1), nonmanifold_edges=sum(1 for v in ec.values() if v > 2))
hs = np.array(RB["console_c2_head"]["bounds_min_cm"]) + AXIS - BASE_PIV
he = np.array(RB["console_c2_head"]["bounds_max_cm"]) + AXIS - BASE_PIV
asm_min = np.minimum(np.array(RB["console_c2_base"]["bounds_min_cm"]), hs)
asm_max = np.maximum(np.array(RB["console_c2_base"]["bounds_max_cm"]), he)
src_min, src_max = STAGE_MIN - BASE_PIV, STAGE_MAX - BASE_PIV
WARN = []
if nc != 2:
    WARN.append("%d connected pieces after the cut (expected 2)" % nc)
if float(np.abs(asm_min - src_min).max()) > 0.02 or float(np.abs(asm_max - src_max).max()) > 0.02:
    WARN.append("reassembled bounds differ from the source by more than 0.2 mm")
rep = dict(task="task-301-02-console-c2-cut", source=os.path.relpath(SRC_FBX), source_tris=TRIS_IN, source_open_edges=SRC_OPEN, tris_after_refinement=TRIS_REFINED,
           frame="cm, front of the console faces -Y, Z up; Blender unit = 1 m, FBX_SCALE_UNITS, axis_forward -Z, axis_up Y as the drone parts; UnitScaleFactor read in files: %s" % usf,
           source_size_cm=[round(float(x), 2) for x in (STAGE_MAX - STAGE_MIN)], scale_note="Meshy scale kept as is (1 unit = 1 m): 190 cm tall; the subtask gave no height",
           cut=dict(plane_z_cm=CUT_Z, section_bounds_cm=[[round(float(x), 2) for x in SEC_MIN[:2]], [round(float(x), 2) for x in SEC_MAX[:2]]], connected_pieces=int(nc), small_fragments=FRAG,
                    mast_section_centres_xy_cm={"z98": mast_sections["98"], "z102": mast_sections["102"], "z106": mast_sections["106"], "cap": [round(float(AXIS[0]), 2), round(float(AXIS[1]), 2)]}),
           pivots=dict(base_pivot_stage_cm=[round(float(x), 3) for x in BASE_PIV], mast_axis_xy_in_base_frame_cm=[round(float(AXIS[0] - BASE_PIV[0]), 3), round(float(AXIS[1] - BASE_PIV[1]), 3)],
                       head_origin_height_in_base_frame_cm=round(float(CUT_Z - BASE_PIV[2]), 3), head_offset_on_base_cm=[round(float(x), 3) for x in (AXIS - BASE_PIV)],
                       head_note="head origin on the mast axis at the cut height: rotate the head about Z through its origin; place it at head_offset_on_base_cm in the base frame"),
           screen=dict(rect_x_cm=[SX0, SX1], rect_z_cm=[SZ0, SZ1], plane=SCREEN_PLANE, patch_faces_after_cut=SCREEN_PATCH_FACES),
           orange_refinement_rounds_mixed_faces=ROUNDS, parts={n: dict(PART_REP[n], faces_per_slot=FACES_SLOT[n], readback=RB[n]) for n in PARTS},
           assembled=dict(size_cm=[round(float(x), 2) for x in (asm_max - asm_min)], bounds_min_cm=[round(float(x), 2) for x in asm_min], bounds_max_cm=[round(float(x), 2) for x in asm_max],
                          source_bounds_min_cm=[round(float(x), 2) for x in src_min], source_bounds_max_cm=[round(float(x), 2) for x in src_max]),
           texture=TEX_NAME, warnings=WARN, seconds=round(time.time() - T0, 1))
with open(os.path.join(OUT, "console_c2_cut.report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps(dict(base=rep["parts"]["console_c2_base"]["faces_per_slot"], head=rep["parts"]["console_c2_head"]["faces_per_slot"], pieces=int(nc), rounds=ROUNDS,
                              axis=[round(float(x), 2) for x in AXIS], base_piv=[round(float(x), 2) for x in BASE_PIV], warnings=WARN)))
