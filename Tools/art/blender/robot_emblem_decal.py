"""task-499: Kessler-Voss emblem decal on the top of an animated Meshy robot, without touching its parts or textures.
Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/robot_emblem_decal.py -- --robot heavy|kamikaze|chaser_v2|shield_chaser_v2 [--no-render] [--no-debris]
What it does (config per robot in ROBOTS below; centre and size in rest-pose Blender cm, front +X, robot left +Y, Z up):
  1. imports the animated parts FBX (`in_fbx`, never modified) and ray casts straight down over the emblem square (side S, centre C):
     every face hit first by a ray belongs to the decal; the report counts rays that hit another part (must be 0).
  2. copies those faces of the target part (`part`) in rest space (world, identity transform, no parent: as the shield of the
     shield chaser bake), clips them to the square with four vertical planes, pushes every vertex `offset_cm` along its normal,
     and writes ONE planar UV (UVMap): emblem up = robot back (-X), emblem right = robot left (+Y), so the emblem reads upright
     when the robot walks toward the bottom of the game screen (chaser v1 hood rule). LightMap, PartId (channel 2, the VAT part
     index) and the PartIndex colour are kept from the copied faces, so the decal follows its part in MF_RobotAnimWPO.
     One new material slot `robot_emblem`.
  3. exports parts + `Z_Emblem` to `out_fbx` (new file next to the source; Blender default axes -Z / Y, FBX_SCALE_UNITS).
  4. debris (unless --no-debris or no `debris` key): adds the same decal, moved by minus the piece centre of
     `debris_pieces.json` (index 0, the torso, Unreal offset (x, -y, z) -> Blender (x, -y, z)), UV0 only, to the torso
     debris FBX and writes it IN PLACE (generated file of chaser_debris_pieces.py; rerunning that script drops the decal:
     rerun this one after it). The original torso FBX is kept once as `<name>.pre_emblem.fbx` (not committed).
  5. report `<out_fbx>.report.json`, renders (top and three-quarter, base colour + emblem) to Saved/Renders/task-499/.
"""
import bpy, bmesh, sys, os, json, math, shutil
import numpy as np
from mathutils import Vector, Matrix
from mathutils.bvhtree import BVHTree

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
A = lambda p: os.path.join(REPO, p)
YELLOW = "Art/Meshes/ChaserShield/T_ChaserShield_Emblem.png"
DARK = "Art/Meshes/robot-emblem/T_RobotEmblem_Dark.png"
ROBOTS = {
    # size = about a third of the head-torso top width (Q3, owner 2026-11-10): heavy top 90 cm, kamikaze head top 66 cm, chaser v2 head-torso 41 cm
    "heavy": dict(in_fbx="Art/Meshes/heavy-meshy/heavy_meshy_parts.fbx", out_fbx="Art/Meshes/heavy-meshy/heavy_meshy_parts_emblem.fbx",
                  tex="Art/Meshes/heavy-meshy/heavy_meshy_clean_basecolor.png", emblem=YELLOW, part="HeadTorso",
                  centre=(16.0, 1.5), size=30.0, offset_cm=0.4, debris="Art/Source/heavy-meshy/debris"),
    "kamikaze": dict(in_fbx="Art/Meshes/kamikaze-meshy/kamikaze_meshy_parts.fbx", out_fbx="Art/Meshes/kamikaze-meshy/kamikaze_meshy_parts_emblem.fbx",
                     tex="Art/Meshes/kamikaze-meshy/kamikaze_meshy_clean_basecolor.png", emblem=YELLOW, part="Body",
                     centre=(-8.0, -3.0), size=22.0, offset_cm=0.3, debris="Art/Source/kamikaze-meshy/debris"),
    "chaser_v2": dict(in_fbx="Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts.fbx", out_fbx="Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_parts_emblem.fbx",
                      tex="Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_clean_basecolor.png", emblem=DARK, part="HeadTorso",
                      centre=(-4.0, 1.5), size=14.0, offset_cm=0.25, debris="Art/Source/chaser-v2-meshy/debris"),
    # same body as chaser v2: same part, centre and size; the shield bake FBX holds the chaser v2 parts unchanged plus the shield
    "shield_chaser_v2": dict(in_fbx="Art/Meshes/shield-chaser-v2/SM_ShieldChaserV2_Meshy_Anim.fbx", out_fbx="Art/Meshes/shield-chaser-v2/SM_ShieldChaserV2_Meshy_Anim_emblem.fbx",
                             tex="Art/Meshes/chaser-v2-meshy/chaser_v2_meshy_clean_basecolor.png", emblem=DARK, part="HeadTorso",
                             centre=(-4.0, 1.5), size=14.0, offset_cm=0.25),
}
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
NAME = argv[argv.index("--robot") + 1]
for k in ("--centre-x", "--centre-y", "--size"):
    if k in argv:
        v = float(argv[argv.index(k) + 1])
        if k == "--size":
            ROBOTS[NAME]["size"] = v
        else:
            c = list(ROBOTS[NAME]["centre"]); c[0 if k.endswith("x") else 1] = v; ROBOTS[NAME]["centre"] = tuple(c)
CFG = ROBOTS[NAME]
NORENDER, NODEBRIS = "--no-render" in argv, "--no-debris" in argv
C = Vector((CFG["centre"][0], CFG["centre"][1], 0.0))
S = CFG["size"]
RIGHT, UP = Vector((0, 1, 0)), Vector((-1, 0, 0))
REP = dict(robot=NAME, config={k: v for k, v in CFG.items()}, uv_rule="emblem up = -X (robot back), right = +Y (robot left), planar from the top")

# ---------------------------------------------------------------- 1. import and ray cast
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=A(CFG["in_fbx"]), colors_type="LINEAR")
OBS = [o for o in bpy.data.objects if o.type == "MESH"]
NAMES_IN = sorted(o.name for o in OBS)
T = [o for o in OBS if o.name == CFG["part"]][0]
verts, polys, owner = [], [], []
for o in OBS:
    off = len(verts)
    mw = o.matrix_world
    verts += [(mw @ v.co)[:] for v in o.data.vertices]
    for p in o.data.polygons:
        polys.append([off + i for i in p.vertices]); owner.append((o.name, p.index))
bvh = BVHTree.FromPolygons(verts, polys)
ztop = max(v[2] for v in verts) + 50
hit_faces, other, miss, zs, nz = set(), {}, 0, [], []
n = int(S / 0.5)
for i in range(n + 1):
    for j in range(n + 1):
        p = C + RIGHT * (-S / 2 + S * i / n) + UP * (-S / 2 + S * j / n)
        loc, nrm, idx, d = bvh.ray_cast(Vector((p.x, p.y, ztop)), Vector((0, 0, -1)))
        if loc is None:
            miss += 1
            continue
        on, pi = owner[idx]
        if on != T.name:
            other[on] = other.get(on, 0) + 1
            continue
        hit_faces.add(pi); zs.append(loc.z); nz.append(nrm.z)
REP["rays"] = dict(total=(n + 1) ** 2, on_part=len(zs), on_other_parts=other, missed=miss, faces=len(hit_faces),
                   z_min=round(min(zs), 2), z_max=round(max(zs), 2), normal_z_min=round(min(nz), 3), normal_z_mean=round(float(np.mean(nz)), 3))
print("RAYS", json.dumps(REP["rays"]))

# ---------------------------------------------------------------- 2. decal mesh in rest space
def build_decal(src, faces, shift=Vector((0, 0, 0)), keep_layers=True):
    me = src.data.copy()
    me.transform(src.matrix_world)
    bm = bmesh.new(); bm.from_mesh(me)
    bm.faces.ensure_lookup_table()
    bmesh.ops.delete(bm, geom=[f for f in bm.faces if f.index not in faces], context="FACES")
    for co, no in ((C + RIGHT * S / 2, RIGHT), (C - RIGHT * S / 2, -RIGHT), (C + UP * S / 2, UP), (C - UP * S / 2, -UP)):
        geom = bm.verts[:] + bm.edges[:] + bm.faces[:]
        bmesh.ops.bisect_plane(bm, geom=geom, plane_co=co, plane_no=no, clear_outer=True)
    bmesh.ops.delete(bm, geom=[v for v in bm.verts if not v.link_faces], context="VERTS")
    bm.normal_update()
    for v in bm.verts:
        v.co += v.normal * CFG["offset_cm"]
    uvl = bm.loops.layers.uv["UVMap"] if "UVMap" in bm.loops.layers.uv else bm.loops.layers.uv[0]
    for f in bm.faces:
        for l in f.loops:
            d = l.vert.co - C
            l[uvl].uv = (d.dot(RIGHT) / S + 0.5, d.dot(UP) / S + 0.5)
    for f in bm.faces:
        f.material_index = 0
    for v in bm.verts:
        v.co += shift
    bm.to_mesh(me); bm.free()
    if not keep_layers:
        for u in [u.name for u in me.uv_layers if u.name != "UVMap"]:
            me.uv_layers.remove(me.uv_layers[u])
        for c in [c.name for c in me.color_attributes]:
            me.color_attributes.remove(me.color_attributes[c])
    me.materials.clear()
    me.materials.append(MAT)
    ob = bpy.data.objects.new("Z_Emblem", me)      # sorts after the parts: Unreal builds the slots in object name order, so robot_emblem comes last
    bpy.context.scene.collection.objects.link(ob)
    return ob


img_e = bpy.data.images.load(A(CFG["emblem"]))
MAT = bpy.data.materials.new("robot_emblem")
MAT.use_nodes = True
nt = MAT.node_tree
bsdf = [x for x in nt.nodes if x.type == "BSDF_PRINCIPLED"][0]
te = nt.nodes.new("ShaderNodeTexImage"); te.image = img_e
nt.links.new(te.outputs["Color"], bsdf.inputs["Base Color"]); nt.links.new(te.outputs["Alpha"], bsdf.inputs["Alpha"])
EM = build_decal(T, hit_faces)
me = EM.data
uv = np.array([l.uv[:] for l in me.uv_layers["UVMap"].data])
pid = np.array([l.uv[:] for l in me.uv_layers["PartId"].data]) if "PartId" in me.uv_layers else None
src_pid = np.unique(np.round(np.array([l.uv[:] for l in T.data.uv_layers["PartId"].data]), 6), axis=0).tolist() if "PartId" in T.data.uv_layers else None
EV = np.array([v.co[:] for v in me.vertices])
REP["decal"] = dict(tris=sum(len(p.vertices) - 2 for p in me.polygons), verts=len(EV), uv_min=uv.min(0).round(4).tolist(), uv_max=uv.max(0).round(4).tolist(),
                    bounds_min=EV.min(0).round(2).tolist(), bounds_max=EV.max(0).round(2).tolist(), uv_layers=[u.name for u in me.uv_layers],
                    colour_attributes=[c.name for c in me.color_attributes],
                    partid_unique=np.unique(np.round(pid, 6), axis=0).tolist() if pid is not None else None, part_partid_unique=src_pid)
print("DECAL", json.dumps(REP["decal"]))

# ---------------------------------------------------------------- 3. export parts + Emblem
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")
bpy.ops.object.select_all(action="DESELECT")
for o in OBS + [EM]:
    o.select_set(True)
bpy.context.view_layer.objects.active = OBS[0]
OUTF = A(CFG["out_fbx"])
bpy.ops.export_scene.fbx(filepath=OUTF, **EXPORT)


def snapshot(objs):
    out = {}
    for o in objs:
        m = o.data
        out[o.name] = dict(v=np.array([(o.matrix_world @ v.co)[:] for v in m.vertices]), uv={u.name: np.array([l.uv[:] for l in u.data]) for u in m.uv_layers},
                           mats=[x.name for x in m.materials], tris=sum(len(p.vertices) - 2 for p in m.polygons))
    return out


BEFORE = snapshot(OBS)
# read back
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=OUTF, colors_type="LINEAR")
RB = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
AFTER = snapshot([o for o in RB.values() if o.name != "Z_Emblem"])
diff = {}
for k, b in BEFORE.items():
    a = AFTER.get(k)
    if a is None or len(a["v"]) != len(b["v"]):
        diff[k] = "missing or vertex count"
        continue
    dv = float(np.abs(a["v"] - b["v"]).max())
    du = max(float(np.abs(a["uv"][u] - b["uv"][u]).max()) for u in b["uv"])
    if dv > 1e-3 or du > 1e-6 or a["mats"] != b["mats"] or a["tris"] != b["tris"]:
        diff[k] = dict(dv=dv, du=du, mats=a["mats"], tris=a["tris"])
REP["export"] = dict(fbx=CFG["out_fbx"], objects=sorted(RB), parts_unchanged=not diff, part_diffs=diff,
                     emblem_tris=sum(len(p.vertices) - 2 for p in RB["Z_Emblem"].data.polygons), emblem_materials=[m.name for m in RB["Z_Emblem"].data.materials],
                     total_tris=sum(sum(len(p.vertices) - 2 for p in o.data.polygons) for o in RB.values()))
print("EXPORT", json.dumps(REP["export"]))
assert not diff, diff
assert sorted(set(RB) - {"Z_Emblem"}) == NAMES_IN

# ---------------------------------------------------------------- 4. debris torso
if CFG.get("debris") and not NODEBRIS:
    DJ = json.load(open(A(CFG["debris"] + "/debris_pieces.json"), encoding="utf-8"))
    p0 = DJ["pieces"][0]
    dfbx = A(CFG["debris"] + "/" + p0["file"])
    keep = dfbx[:-4] + ".pre_emblem.fbx"
    if not os.path.exists(keep):
        shutil.copyfile(dfbx, keep)
    ctr = Vector((p0["centre_offset_cm_unreal"][0], -p0["centre_offset_cm_unreal"][1], p0["centre_offset_cm_unreal"][2]))
    # rest-space emblem rebuilt from the parts file, then moved by -centre
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=A(CFG["in_fbx"]), colors_type="LINEAR")
    T2 = bpy.data.objects[CFG["part"]]
    src_parts = [bpy.data.objects[n] for n in p0["source_parts"]]
    rv = np.concatenate([np.array([(o.matrix_world @ v.co)[:] for v in o.data.vertices]) for o in src_parts])
    rest_ctr = (rv.min(0) + rv.max(0)) / 2
    img_e = bpy.data.images.load(A(CFG["emblem"]))
    MAT = bpy.data.materials.new("robot_emblem")
    D_EM = build_decal(T2, hit_faces, shift=-ctr, keep_layers=False)
    for o in [o for o in bpy.data.objects if o.type == "MESH" and o.name != D_EM.name]:
        bpy.data.objects.remove(o, do_unlink=True)
    for m in [m for m in bpy.data.materials if m != MAT]:
        bpy.data.materials.remove(m)        # so the piece keeps its slot name "body" (no "body.001")
    bpy.ops.import_scene.fbx(filepath=keep, colors_type="LINEAR")
    PIECE = [o for o in bpy.data.objects if o.type == "MESH" and o != D_EM]
    assert len(PIECE) == 1, [o.name for o in PIECE]
    pv = np.array([(PIECE[0].matrix_world @ v.co)[:] for v in PIECE[0].data.vertices])
    D_EM.name = "Z_Emblem"
    bpy.ops.object.select_all(action="DESELECT")
    PIECE[0].select_set(True); D_EM.select_set(True)
    bpy.context.view_layer.objects.active = PIECE[0]
    bpy.ops.export_scene.fbx(filepath=dfbx, **EXPORT)
    dv = np.array([v.co[:] for v in D_EM.data.vertices])
    REP["debris"] = dict(fbx=CFG["debris"] + "/" + p0["file"], kept_original=os.path.basename(keep), centre_blender=list(ctr), rest_bounds_centre=rest_ctr.round(2).tolist(),
                         piece_bounds_centre=((pv.min(0) + pv.max(0)) / 2).round(3).tolist(), piece_tris=sum(len(p.vertices) - 2 for p in PIECE[0].data.polygons),
                         emblem_bounds_min=dv.min(0).round(2).tolist(), emblem_bounds_max=dv.max(0).round(2).tolist(), emblem_uv=[u.name for u in D_EM.data.uv_layers],
                         piece_materials=[m.name for m in PIECE[0].data.materials])
    print("DEBRIS", json.dumps(REP["debris"]))

# ---------------------------------------------------------------- 5. renders
if not NORENDER:
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=OUTF, colors_type="LINEAR")
    img_b = bpy.data.images.load(A(CFG["tex"]))
    img_e = bpy.data.images.load(A(CFG["emblem"]))
    for o in bpy.data.objects:
        if o.type != "MESH":
            continue
        for m in o.data.materials:
            if not m or not m.node_tree:
                continue
            nt = m.node_tree
            bs = [x for x in nt.nodes if x.type == "BSDF_PRINCIPLED"]
            tn = [x for x in nt.nodes if x.type == "TEX_IMAGE"]
            if not bs:
                continue
            if not tn:
                tn = [nt.nodes.new("ShaderNodeTexImage")]
                nt.links.new(tn[0].outputs["Color"], bs[0].inputs["Base Color"])
            tn[0].image = img_e if m.name.startswith("robot_emblem") else img_b
            if m.name.startswith("robot_emblem"):
                nt.links.new(tn[0].outputs["Alpha"], bs[0].inputs["Alpha"])
                m.blend_method = "CLIP" if hasattr(m, "blend_method") else None
    sc = bpy.context.scene
    sc.render.engine = "BLENDER_EEVEE"
    sc.view_settings.view_transform = "Standard"
    sc.render.resolution_x = sc.render.resolution_y = 900
    w = bpy.data.worlds.new("w"); sc.world = w; w.use_nodes = True
    w.node_tree.nodes["Background"].inputs[0].default_value = (0.55, 0.5, 0.6, 1); w.node_tree.nodes["Background"].inputs[1].default_value = 1.0
    sd = bpy.data.lights.new("s", "SUN"); sd.energy = 2.5
    so = bpy.data.objects.new("s", sd); sc.collection.objects.link(so); so.rotation_euler = (math.radians(25), math.radians(10), 0)
    cd = bpy.data.cameras.new("c"); cd.type = "ORTHO"; cd.clip_end = 10000
    cam = bpy.data.objects.new("c", cd); sc.collection.objects.link(cam); sc.camera = cam
    allv = np.concatenate([np.array([(o.matrix_world @ v.co)[:] for v in o.data.vertices]) for o in bpy.data.objects if o.type == "MESH"])
    H = float(allv[:, 2].max())
    out_dir = A("Saved/Renders/task-499"); os.makedirs(out_dir, exist_ok=True)
    R = []
    # top close-up (robot front toward the bottom of the image: emblem must read upright), and game-like view: camera south of the robot, pitch -60,
    # robot facing the camera (front -> screen bottom)
    for tag, loc, rot, scale in (("top", (C.x, C.y, H + 300), (0, 0, math.radians(90)), S * 2.6),
                                 ("game", (600 * math.cos(math.radians(60)) + C.x, C.y, H / 2 + 600 * math.sin(math.radians(60))), (math.radians(30), 0, math.radians(90)), H * 1.6)):
        cam.location = loc; cam.rotation_euler = rot; cd.ortho_scale = scale
        sc.render.filepath = os.path.join(out_dir, "%s_emblem_%s.png" % (NAME, tag))
        bpy.ops.render.render(write_still=True)
        R.append(os.path.relpath(sc.render.filepath, REPO).replace("\\", "/"))
    REP["renders"] = R
json.dump(REP, open(OUTF + ".report.json", "w"), indent=1, default=str)
print("DONE", OUTF)
