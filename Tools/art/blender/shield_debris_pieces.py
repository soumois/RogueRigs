"""task-222-01: cut the 1.2-scaled shield (guard pose of the shield chaser, clip 0 frame 0) into five debris pieces for the Niagara shield-break burst. Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/shield_debris_pieces.py -- [--in-fbx Art/Meshes/ChaserShield/SM_ChaserShield.fbx] [--in-dir Art/Meshes/ChaserShield] [--out-dir Art/Source/shield-chaser/debris] [--work <short dir>] [--no-render]
Pieces (fixed order = Niagara mesh index): Shard_A (centre plate with the KV emblem decal), Shard_B (top left), Shard_C (top right), Shard_D (bottom right corner), Grip (dark handle and mounts).
The four shards are the plate and its bolts cut by straight planes (perpendicular to the plate, convex regions in the plate plane, see SHARDS) and capped with a flat yellow texel of the base colour UV (material M_ChaserShield).
Shard_A has two slots (M_ChaserShield, M_ChaserShield_Emblem), the other pieces one. UV0 only (LightMap dropped). Flat shading, caps kept.
Guard pose = the placement of shield_chaser_bake.py: v_actor = Rs (1.2 v) + origin, Rs = rotation about Y of -3 degrees, origin = (30.65, 5.63, 4.34) cm (Blender axes, actor origin on the floor between the feet; printed by the bake as origin).
Each piece is baked into that frame, then its vertices are moved so the pivot is the bounds centre. centre_offset_cm_unreal = (x, -y_blender, z) of the bounds centre, as in chaser_debris_pieces.py.
Export: cm, FBX_SCALE_UNITS, axis -Z/Y (Blender default = Unreal import: X kept, Y mirrored)."""
import bpy, bmesh, sys, os, json, shutil, math
from mathutils import Vector, Matrix

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
RENDER = "--no-render" not in argv
argv = [a for a in argv if a != "--no-render"]
opt = {"--in-fbx": "Art/Meshes/ChaserShield/SM_ChaserShield.fbx", "--in-dir": "Art/Meshes/ChaserShield", "--out-dir": "Art/Source/shield-chaser/debris",
       "--work": "C:/Users/soumo/AppData/Local/Temp/rr-sd/render"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT = os.path.abspath(opt["--out-dir"])
INDIR = os.path.abspath(opt["--in-dir"])
os.makedirs(OUT, exist_ok=True)
for f in os.listdir(OUT):
    if f.startswith("SM_ShieldDebris_") or f == "shield_debris_pieces.json":
        os.remove(os.path.join(OUT, f))
TEXS = ("T_ChaserShield_BaseColor.png", "T_ChaserShield_Emblem.png")
for t in TEXS:
    shutil.copyfile(os.path.join(INDIR, t), os.path.join(OUT, t))

SCALE = 1.2
TILT = math.radians(-3.0)
ORG = Vector((30.65, 5.63, 4.34))
RS = Matrix(((math.cos(TILT), 0, math.sin(TILT)), (0, 1, 0), (-math.sin(TILT), 0, math.cos(TILT))))
# convex cut regions in the plate plane (y, z) cm of the unscaled shield, counter-clockwise (y = Blender +Y)
# plate y -21.4..21.4, z 0..82; the emblem decal (y -11..11, z 31..51) is inside Shard_A; the four bolts stay whole
SHARDS = [
    ("Shard_A", [(-21.4, 0), (12.5, 0), (21.4, 40), (21.4, 64), (2, 62), (-21.4, 56)]),
    ("Shard_B", [(-21.4, 56), (2, 62), (-2, 82), (-21.4, 82)]),
    ("Shard_C", [(2, 62), (21.4, 64), (21.4, 82), (-2, 82)]),
    ("Shard_D", [(12.5, 0), (21.4, 0), (21.4, 40)]),
]
EPS = 1e-4

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.abspath(opt["--in-fbx"]), colors_type="LINEAR")
src = [o for o in bpy.data.objects if o.type == "MESH"][0]
sme = src.data
assert len(sme.materials) == 2 and abs(src.matrix_world[0][0] - 1) < 1e-6
SLOT_NAMES = [m.name for m in sme.materials]
SRC_TRIS = sum(len(p.vertices) - 2 for p in sme.polygons)
uv_name = sme.uv_layers[0].name


def tris(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


def islands(bm):
    seen, res = set(), []
    for f in bm.faces:
        if f.index in seen:
            continue
        st, fs = [f], []
        seen.add(f.index)
        while st:
            c = st.pop()
            fs.append(c)
            for e in c.edges:
                for g in e.link_faces:
                    if g.index not in seen:
                        seen.add(g.index)
                        st.append(g)
        res.append(fs)
    return res


# ---- split the source: decal (slot 1), grip (largest island), plate and bolts (the rest)
bm0 = bmesh.new()
bm0.from_mesh(sme)
bm0.faces.ensure_lookup_table()
isl = islands(bm0)
decal_faces = [f for i in isl for f in i if f.material_index == 1]
body = [i for i in isl if i[0].material_index == 0]
grip_isl = max(body, key=len)
grip_idx = {f.index for f in grip_isl}
decal_idx = {f.index for f in decal_faces}
plate_idx = {f.index for f in bm0.faces} - grip_idx - decal_idx
bpy.data.objects.remove(src)     # the imported source stays out of the renders
print("source tris", SRC_TRIS, "grip", len(grip_idx), "decal", len(decal_idx), "plate+bolts", len(plate_idx))


def sub_bm(idx):
    bm = bm0.copy()
    bm.faces.ensure_lookup_table()
    drop = [f for f in bm.faces if f.index not in idx]
    bmesh.ops.delete(bm, geom=drop, context="FACES")
    for v in [v for v in bm.verts if not v.link_faces]:
        bm.verts.remove(v)
    return bm


# ---- a flat yellow texel of the base colour UV: centroid of a +X face of the plate whose 5x5 neighbourhood is uniform and yellow
img = bpy.data.images.load(os.path.join(OUT, TEXS[0]), check_existing=False)
img.colorspace_settings.name = "sRGB"
W, H = img.size
px = list(img.pixels)


def texel(u, v):
    x, y = min(W - 1, max(0, int(u * W))), min(H - 1, max(0, int(v * H)))
    k = (y * W + x) * 4
    return tuple(px[k:k + 3])


bmp = sub_bm(plate_idx)
uvl = bmp.loops.layers.uv[uv_name]
YEL = None
for f in sorted(bmp.faces, key=lambda f: -f.calc_area()):
    if f.normal.x < 0.99:
        continue
    cu = sum((l[uvl].uv for l in f.loops), Vector((0, 0))) / len(f.loops)
    blk = [texel(cu.x + dx / W, cu.y + dy / H) for dx in range(-2, 3) for dy in range(-2, 3)]
    c = blk[0]
    if all(max(abs(a - b) for a, b in zip(c, t)) < 1e-4 for t in blk) and c[0] > 0.5 and c[1] > 0.3 and c[2] < 0.25:
        YEL = (cu.x, cu.y, c)
        break
assert YEL, "no flat yellow texel"
print("cap texel uv", YEL[:2], "colour", YEL[2])
bmp.free()


def cut_shard(region):
    bm = sub_bm(plate_idx)
    n = len(region)
    for i in range(n):
        (ay, az), (by, bz) = region[i], region[(i + 1) % n]
        if (abs(ay - by) < EPS and abs(abs(ay) - 21.4) < EPS) or (abs(az - bz) < EPS and (abs(az) < EPS or abs(az - 82) < EPS)):
            continue   # edge on the plate outline
        dy, dz = by - ay, bz - az
        no = Vector((0, dz, -dy)).normalized()    # outward (left of a counter-clockwise edge is inside)
        geom = list(bm.verts) + list(bm.edges) + list(bm.faces)
        bmesh.ops.bisect_plane(bm, geom=geom, dist=1e-6, plane_co=Vector((0, ay, az)), plane_no=no, clear_outer=True, clear_inner=False)
    for v in [v for v in bm.verts if not v.link_faces]:
        bm.verts.remove(v)
    old = set(bm.faces)      # ops clear the tag flags: keep the cut faces by identity
    bnd = [e for e in bm.edges if e.is_boundary]
    r = bmesh.ops.holes_fill(bm, edges=bnd, sides=0)
    bmesh.ops.triangulate(bm, faces=r["faces"])
    uvl = bm.loops.layers.uv[uv_name]
    for f in [f for f in bm.faces if f not in old]:
        f.material_index = 0
        f.smooth = False
        for l in f.loops:
            l[uvl].uv = Vector(YEL[:2])
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    return bm


pieces = []
for name, region in SHARDS:
    bm = cut_shard(region)
    if name == "Shard_A":
        dbm = sub_bm(decal_idx)
        tmp = bpy.data.meshes.new("tmp")
        dbm.to_mesh(tmp)
        bm.from_mesh(tmp)
        bpy.data.meshes.remove(tmp)
        dbm.free()
    pieces.append((name, bm))
pieces.append(("Grip", sub_bm(grip_idx)))

# ---- bake into the guard pose frame, pivot at the bounds centre, export
report = dict(
    source=opt["--in-fbx"], textures=list(TEXS), script="Tools/art/blender/shield_debris_pieces.py", units="cm",
    shield_scale=SCALE, guard_pose=dict(tilt_deg=-3.0, origin_cm_blender=[ORG.x, ORG.y, ORG.z], note="same placement as shield_chaser_bake.py (clip 0 frame 0), actor origin on the floor between the feet"),
    offset_convention=("centre_offset_cm_unreal = (x, -y, z) of the piece bounds centre in the guard-pose Blender world (cm, actor origin on the floor, front +X, Z up), the shield as placed on the shield chaser at clip 0 frame 0. "
                       "The FBX uses the Blender default axes (forward -Z, up Y), which Unreal imports keeping X and Z and mirroring Y, so Y is negated. "
                       "The mesh pivot is the bounds centre and the vertices are already in the guard orientation (3 degrees back tilt): a piece spawned at actor origin + offset, no rotation, sits exactly where it was."),
    source_triangles=SRC_TRIS, pieces=[])
final = []
for idx, (name, bm) in enumerate(pieces):
    me = bpy.data.meshes.new("SM_ShieldDebris_" + name)
    bm.to_mesh(me)
    bm.free()
    for u in [u.name for u in me.uv_layers][1:]:
        me.uv_layers.remove(me.uv_layers[u])
    for c in [c.name for c in me.color_attributes]:
        me.color_attributes.remove(me.color_attributes[c])
    uses_emblem = any(p.material_index == 1 for p in me.polygons)
    assert len(me.materials) == 0
    for n_ in (SLOT_NAMES if uses_emblem else SLOT_NAMES[:1]):
        me.materials.append(bpy.data.materials.get(n_) or bpy.data.materials.new(n_))
    M = Matrix.Translation(ORG) @ RS.to_4x4() @ Matrix.Scale(SCALE, 4)
    me.transform(M)
    vs = [v.co.copy() for v in me.vertices]
    lo = Vector((min(v.x for v in vs), min(v.y for v in vs), min(v.z for v in vs)))
    hi = Vector((max(v.x for v in vs), max(v.y for v in vs), max(v.z for v in vs)))
    ctr = (lo + hi) / 2
    me.transform(Matrix.Translation(-ctr))
    me.update()
    for p in me.polygons:
        p.use_smooth = False
    ob = bpy.data.objects.new(me.name, me)
    bpy.context.scene.collection.objects.link(ob)
    bpy.ops.object.select_all(action="DESELECT")
    ob.select_set(True)
    bpy.context.view_layer.objects.active = ob
    bpy.ops.export_scene.fbx(filepath=os.path.join(OUT, ob.name + ".fbx"), use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0,
                             axis_forward="-Z", axis_up="Y", mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
    bmc = bmesh.new()
    bmc.from_mesh(me)
    open_edges = sum(1 for e in bmc.edges if e.is_boundary)
    nonman = sum(1 for e in bmc.edges if len(e.link_faces) > 2)
    bmc.free()
    report["pieces"].append(dict(index=idx, name=ob.name, file=ob.name + ".fbx", triangles=tris(me),
                                 centre_offset_cm_unreal=[round(ctr.x, 2), round(-ctr.y, 2), round(ctr.z, 2)],
                                 size_cm=[round(hi.x - lo.x, 2), round(hi.y - lo.y, 2), round(hi.z - lo.z, 2)],
                                 bounds_guard_pose_blender_cm=dict(min=[round(x, 2) for x in lo], max=[round(x, 2) for x in hi]),
                                 uv_layers=[u.name for u in me.uv_layers], slots=[m.name for m in me.materials], open_edges=open_edges, non_manifold_edges=nonman))
    final.append((ob, ctr))
report["sum_triangles"] = sum(p["triangles"] for p in report["pieces"])
allmin = [min(p["bounds_guard_pose_blender_cm"]["min"][i] for p in report["pieces"]) for i in range(3)]
allmax = [max(p["bounds_guard_pose_blender_cm"]["max"][i] for p in report["pieces"]) for i in range(3)]
report["union_bounds_guard_pose_blender_cm"] = dict(min=allmin, max=allmax)
with open(os.path.join(OUT, "shield_debris_pieces.json"), "w") as f:
    json.dump(report, f, indent=2)
print("pieces", [(p["name"], p["triangles"], p["open_edges"]) for p in report["pieces"]], "sum", report["sum_triangles"], "union", allmin, allmax)

# ---- renders: exploded and reassembled, three-quarter front and back (Cycles CPU, flat colours)
if RENDER:
    PREV = os.path.join(OUT, "preview")
    os.makedirs(PREV, exist_ok=True)
    sc = bpy.context.scene
    sc.render.engine = "CYCLES"
    sc.cycles.device = "CPU"
    sc.cycles.samples = 48
    sc.cycles.use_denoising = False
    sc.render.resolution_x, sc.render.resolution_y = 1100, 900
    sc.view_settings.view_transform = "Standard"
    w = bpy.data.worlds.new("w")
    w.use_nodes = True
    w.node_tree.nodes["Background"].inputs[0].default_value = (0.16, 0.17, 0.19, 1)
    w.node_tree.nodes["Background"].inputs[1].default_value = 1.0
    sc.world = w

    def tex_mat(name, tex, emblem):
        m = bpy.data.materials.new("render_" + name)
        m.use_nodes = True
        nt = m.node_tree
        for n in list(nt.nodes):
            nt.nodes.remove(n)
        t = nt.nodes.new("ShaderNodeTexImage")
        t.image = bpy.data.images.load(os.path.join(OUT, tex), check_existing=False)
        t.interpolation = "Closest"
        d = nt.nodes.new("ShaderNodeBsdfDiffuse")
        o = nt.nodes.new("ShaderNodeOutputMaterial")
        nt.links.new(t.outputs["Color"], d.inputs["Color"])
        if emblem:
            tr = nt.nodes.new("ShaderNodeBsdfTransparent")
            mx = nt.nodes.new("ShaderNodeMixShader")
            nt.links.new(t.outputs["Alpha"], mx.inputs[0])
            nt.links.new(tr.outputs[0], mx.inputs[1])
            nt.links.new(d.outputs[0], mx.inputs[2])
            nt.links.new(mx.outputs[0], o.inputs[0])
        else:
            nt.links.new(d.outputs[0], o.inputs[0])
        return m
    RM = [tex_mat("base", TEXS[0], False), tex_mat("emblem", TEXS[1], True)]
    for ob, ctr in final:
        for i in range(len(ob.data.materials)):
            ob.data.materials[i] = RM[i]
    bpy.ops.mesh.primitive_plane_add(size=400, location=(30, 0, 0))
    fl = bpy.context.object
    fm = bpy.data.materials.new("floor")
    fm.use_nodes = True
    fm.node_tree.nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.05, 0.05, 0.06, 1)
    fl.data.materials.append(fm)
    sun = bpy.data.objects.new("sun", bpy.data.lights.new("sun", "SUN"))
    sun.data.energy = 3.5
    sun.rotation_euler = (math.radians(50), math.radians(10), math.radians(55))
    sc.collection.objects.link(sun)
    fill = bpy.data.objects.new("fill", bpy.data.lights.new("fill", "SUN"))
    fill.data.energy = 1.2
    fill.rotation_euler = (math.radians(60), 0, math.radians(-120))
    sc.collection.objects.link(fill)
    cam = bpy.data.objects.new("cam", bpy.data.cameras.new("cam"))
    cam.data.lens = 50
    sc.collection.objects.link(cam)
    sc.camera = cam

    def look(frm, to):
        cam.location = frm
        cam.rotation_euler = (Vector(to) - Vector(frm)).to_track_quat("-Z", "Y").to_euler()

    centre = Vector(((allmin[0] + allmax[0]) / 2, (allmin[1] + allmax[1]) / 2, (allmin[2] + allmax[2]) / 2))
    PUSH_X = {"Shard_A": 0, "Shard_B": 3, "Shard_C": 3, "Shard_D": 3, "Grip": -14}

    def place(explode):
        for ob, ctr in final:
            d = ctr - centre
            d.x = 0
            ob.location = ctr + d * explode
            if explode:
                ob.location.x += PUSH_X[ob.name.replace("SM_ShieldDebris_", "")]

    def shot(name, explode, frm, to):
        place(explode)
        look(frm, to)
        sc.render.filepath = os.path.join(PREV, name + ".png")
        bpy.ops.render.render(write_still=True)

    d = 200
    tq_front = centre + Vector((d * 0.80, -d * 0.55, 25))
    tq_back = centre + Vector((-d * 0.75, -d * 0.60, 25))
    shot("shield_debris_exploded_tq_front", 0.45, tq_front, centre)
    shot("shield_debris_reassembled_tq_front", 0.0, tq_front, centre)
    high = centre + Vector((d * 0.70, -d * 0.45, 140))
    shot("shield_debris_exploded_tq_front_high", 0.45, high, centre + Vector((0, 0, 8)))
    shot("shield_debris_reassembled_tq_front_high", 0.0, high, centre + Vector((0, 0, 8)))
    shot("shield_debris_exploded_tq_back", 0.45, tq_back, centre)
    shot("shield_debris_reassembled_tq_back", 0.0, tq_back, centre)
print("done")
