"""task-301-03: retouch the console C2 after the cut (dish back texture, foot lamp caps). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Art/Source/console-meshy/clean/console_c2_retouch.py -- [--out-dir Art/Source/console-meshy/clean] [--in-dir <dir of the subtask-02 FBX>]
Inputs : console_c2_base.fbx, console_c2_head.fbx (subtask 02, FBX_SCALE_UNITS), console_c2_basecolor_meshy.png (the untouched Meshy texture).
1. Dish back: the head faces on the outer side of the bowl (dish axis 45 deg, centre in head frame cm) that are not dark clamps are repainted per texel from their 3D position:
   off-white panels, radial seams every 30 deg, one ring seam, teal panel strip at the top centre, thin teal rim band, sparse rust chips. Everything else in the texture is untouched.
2. Foot lamps: every orange face of the four feet (smears included) goes back to ConsoleBody with a dark texture; one round amber cap (cylinder + dome, 20 sides) per lamp (4 big, 4 small)
   is added in the OtherOrange slot (flat emissive material).
3. Export both FBX again (FBX_SCALE_UNITS, same origins, flat shading) and write console_c2_retouch.report.json. Blender only, nothing imported in Unreal."""
import bpy, bmesh, sys, os, json, time, struct, math
import numpy as np
from mathutils import Vector
from collections import defaultdict

T0 = time.time()
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out-dir": "Art/Source/console-meshy/clean", "--in-dir": "Art/Source/console-meshy/clean"}
for i in range(0, len(argv) - 1, 2):
    opt[argv[i]] = argv[i + 1]
OUT, IND = os.path.abspath(opt["--out-dir"]), os.path.abspath(opt["--in-dir"])
os.makedirs(OUT, exist_ok=True)
TEX_ORIG = os.path.join(IND, "console_c2_basecolor_meshy.png")
TEX_NEW = "console_c2_basecolor.png"

# dish frame in the head frame (cm): axis a opens to the front (-Y) and up, centre of the rim circle c
A = np.array([0, -math.sqrt(.5), math.sqrt(.5)])
UPV = np.array([0, math.sqrt(.5), math.sqrt(.5)])
C = np.array([0.0, -21.0, 45.0])
# foot lamps (stage frame minus base pivot, cm), measured on the top view close-ups: (x, y, radius of the cap)
LAMPS = {"FL_big": (-54.25, -39.2, 2.8), "FL_small": (-49.0, -42.1, 1.9), "FR_big": (52.3, -39.2, 2.8), "FR_small": (46.8, -40.0, 1.9),
         "BL_big": (-54.0, 39.7, 2.8), "BL_small": (-49.1, 42.1, 1.9), "BR_big": (52.3, 38.6, 2.8), "BR_small": (46.9, 40.9, 1.9)}
FOOT_ZONE = lambda p: abs(p[0]) > 40 and abs(p[1]) > 33 and p[2] < 30


def load_tex(path):
    im = bpy.data.images.load(path, check_existing=False)
    im.colorspace_settings.name = "Non-Color"
    W, H = im.size
    px = np.array(im.pixels[:], dtype=np.float32).reshape(H, W, 4)
    return px, W, H


TEXPX, TW, TH = load_tex(TEX_ORIG)
NEW = TEXPX.copy()


bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.join(IND, "console_c2_base.fbx"))
base_o = [x for x in bpy.data.objects if x.type == "MESH"][0]
before = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=os.path.join(IND, "console_c2_head.fbx"))
head_o = [x for x in set(bpy.data.objects) - before if x.type == "MESH"][0]
for o in (base_o, head_o):
    assert np.allclose(np.array(o.matrix_world), np.eye(4), atol=1e-4), (o.name, o.matrix_world)


def tex_face_rgb(uvs, n=3):
    """mean texture colour of a triangle (centre + corners pulled in)"""
    B = np.array([[1 / 3, 1 / 3, 1 / 3], [.7, .15, .15], [.15, .7, .15], [.15, .15, .7]])
    P = B @ uvs
    r = np.clip((P[:, 1] * TH).astype(int), 0, TH - 1)
    c = np.clip((P[:, 0] * TW).astype(int) % TW, 0, TW - 1)
    return TEXPX[r, c, :3].mean(0)


def raster(tris_uv, tris_pos, painter, mask_out):
    """paint the texels covered by each UV triangle; painter(P (n,3), tri index) -> colours (n,3)"""
    for ti, (uv, pos) in enumerate(zip(tris_uv, tris_pos)):
        px = uv * np.array([TW, TH])
        x0, x1 = int(np.floor(px[:, 0].min())), int(np.ceil(px[:, 0].max()))
        y0, y1 = int(np.floor(px[:, 1].min())), int(np.ceil(px[:, 1].max()))
        xs, ys = np.meshgrid(np.arange(x0, x1 + 1), np.arange(y0, y1 + 1))
        q = np.stack([xs.ravel() + 0.5, ys.ravel() + 0.5], 1)
        d = (px[1, 1] - px[2, 1]) * (px[0, 0] - px[2, 0]) + (px[2, 0] - px[1, 0]) * (px[0, 1] - px[2, 1])
        if abs(d) < 1e-9:
            continue
        l0 = ((px[1, 1] - px[2, 1]) * (q[:, 0] - px[2, 0]) + (px[2, 0] - px[1, 0]) * (q[:, 1] - px[2, 1])) / d
        l1 = ((px[2, 1] - px[0, 1]) * (q[:, 0] - px[2, 0]) + (px[0, 0] - px[2, 0]) * (q[:, 1] - px[2, 1])) / d
        l2 = 1 - l0 - l1
        ins = (l0 >= -0.02) & (l1 >= -0.02) & (l2 >= -0.02)
        if not ins.any():
            continue
        L = np.stack([l0, l1, l2], 1)[ins]
        col = 0
        for ox, oy in ((-.25, -.25), (.25, -.25), (-.25, .25), (.25, .25)):
            qq = q[ins] + np.array([ox, oy])
            m0 = ((px[1, 1] - px[2, 1]) * (qq[:, 0] - px[2, 0]) + (px[2, 0] - px[1, 0]) * (qq[:, 1] - px[2, 1])) / d
            m1 = ((px[2, 1] - px[0, 1]) * (qq[:, 0] - px[2, 0]) + (px[0, 0] - px[2, 0]) * (qq[:, 1] - px[2, 1])) / d
            col = col + painter(np.stack([m0, m1, 1 - m0 - m1], 1) @ pos, ti) / 4
        cx = (q[ins, 0] - 0.5).astype(int)
        cy = (q[ins, 1] - 0.5).astype(int)
        ok = (cy >= 0) & (cy < TH)
        cx = cx % TW
        NEW[cy[ok], cx[ok], :3] = col[ok]
        mask_out[cy[ok], cx[ok]] = True


def coverage(objs):
    cov = np.zeros((TH, TW), bool)
    for o in objs:
        me = o.data
        uvl = me.uv_layers[0].data
        for p in me.polygons:
            uv = np.array([uvl[i].uv[:] for i in p.loop_indices])
            px = uv * np.array([TW, TH])
            x0, x1 = int(np.floor(px[:, 0].min())), int(np.ceil(px[:, 0].max()))
            y0, y1 = int(np.floor(px[:, 1].min())), int(np.ceil(px[:, 1].max()))
            xs, ys = np.meshgrid(np.arange(x0, x1 + 1), np.arange(y0, y1 + 1))
            q = np.stack([xs.ravel() + 0.5, ys.ravel() + 0.5], 1)
            d = (px[1, 1] - px[2, 1]) * (px[0, 0] - px[2, 0]) + (px[2, 0] - px[1, 0]) * (px[0, 1] - px[2, 1])
            if abs(d) < 1e-9:
                continue
            l0 = ((px[1, 1] - px[2, 1]) * (q[:, 0] - px[2, 0]) + (px[2, 0] - px[1, 0]) * (q[:, 1] - px[2, 1])) / d
            l1 = ((px[2, 1] - px[0, 1]) * (q[:, 0] - px[2, 0]) + (px[0, 0] - px[2, 0]) * (q[:, 1] - px[2, 1])) / d
            ins = (l0 >= -0.02) & (l1 >= -0.02) & (1 - l0 - l1 >= -0.02)
            cx = (q[ins, 0] - 0.5).astype(int) % TW
            cy = np.clip((q[ins, 1] - 0.5).astype(int), 0, TH - 1)
            cov[cy, cx] = True
    return cov


# ================================================================== 1. dish back (head)
hme = head_o.data
hV = np.array([v.co[:] for v in hme.vertices]) * 100
huv = hme.uv_layers[0].data
sel_uv, sel_pos, sel_info = [], [], []
n_dark = 0
for p in hme.polygons:
    vi = list(p.vertices)
    P3 = hV[vi]
    cen = P3.mean(0)
    nrm = np.array(p.normal[:])
    rel = cen - C
    h = rel @ A
    rho = math.hypot(rel @ np.array([1, 0, 0]), rel @ UPV)
    if -32 < h < 3.0 and nrm @ A < 0.6 and rho > 6:
        uv = np.array([huv[i].uv[:] for i in p.loop_indices])
        rgb = tex_face_rgb(uv)
        if rgb.max() < 0.30 and rgb.max() - rgb.min() < 0.08:
            n_dark += 1
            continue
        sel_uv.append(uv)
        sel_pos.append(P3)
        sel_info.append(p.index)
SEL_FACES = set(sel_info)
# reference colours taken from the Meshy texture of the selected faces
whites, tealz = [], []
for uv in sel_uv:
    c = tex_face_rgb(uv)
    if c.min() > 0.6 and c.max() - c.min() < 0.15:
        whites.append(c)
    elif c[2] > c[0] + 0.12 and c[1] > c[0] + 0.12:
        tealz.append(c)
WHITE = np.median(np.array(whites), 0) if whites else np.array([.80, .77, .72])
TEAL = np.median(np.array(tealz), 0) if tealz else np.array([.12, .37, .38])
SEAM = np.array([.36, .37, .38])
RUST = np.array([.34, .19, .12])
MOUNT = np.array([.20, .21, .22])
pts = np.array([q for P3 in sel_pos for q in P3])
rel_all = pts - C
RHO_MAX = float(np.hypot(rel_all @ np.array([1, 0, 0]), rel_all @ UPV).max())
rs = np.random.RandomState(11)
CHIPS = []
for k in range(22):
    rr = rs.uniform(0.40, 0.99) * RHO_MAX
    ph = rs.uniform(-180, 180)
    CHIPS.append((rr, ph, rs.uniform(0.7, 1.6), rs.uniform(0, 6.28)))
# chips on the teal strip corners and along the rim edge
for ph in (-14.5, 14.5):
    CHIPS.append((0.97 * RHO_MAX, ph, 0.9, 1.0))
    CHIPS.append((0.55 * RHO_MAX, ph, 0.8, 2.0))


def hash01(n):
    return (np.sin(n * 12.9898 + 4.1414) * 43758.5453) % 1.0


def paint_dish(P, ti):
    u = P - C
    x = u @ np.array([1.0, 0, 0])
    w = u @ UPV
    rho = np.hypot(x, w)
    phi = np.degrees(np.arctan2(x, w))
    col = np.tile(WHITE, (len(P), 1))
    mount = rho < 19
    sector = np.floor((phi + 180) / 30 - 0.5).astype(int)      # seams at 15 + 30k deg (sector centred on 0, 30, ...)
    seam_ang = ((phi - 15 + 180) % 30) - 15
    d_rad = np.abs(seam_ang) * np.pi / 180 * np.maximum(rho, 1)
    ring_split = rho > 0.58 * RHO_MAX
    pid = sector * 2 + ring_split
    col = col * (1 + (hash01(pid.astype(float)) - 0.5) * 0.07)[:, None]
    # teal strip, top centre (sector |phi| < 15) from the ring seam to the rim
    teal = (np.abs(phi) < 15) & ring_split
    col[teal] = TEAL
    edge = teal & ((np.abs(np.abs(phi) - 15) * np.pi / 180 * rho < 1.2) | (rho > 0.97 * RHO_MAX - 1.2) & (rho < 0.97 * RHO_MAX) | (np.abs(rho - 0.58 * RHO_MAX) < 1.0))
    col[edge] = TEAL * 0.62
    # thin teal rim band all around
    band = rho > RHO_MAX - 2.4
    col[band & ~teal] = TEAL * 0.9
    col[band & teal] = TEAL * 0.62
    # seams
    dd = np.minimum(d_rad, np.abs(rho - 0.58 * RHO_MAX))
    al = np.clip((0.75 - dd) / 0.35, 0, 1) * (rho > 0.18 * RHO_MAX)
    tgt = np.where(teal[:, None], TEAL * 0.45, SEAM)
    col = col * (1 - al[:, None]) + tgt * al[:, None]
    # rust chips
    for rr, ph, rad, sd in CHIPS:
        dph = ((phi - ph + 180) % 360) - 180
        dx = (rho - rr)
        dy = dph * np.pi / 180 * rr
        ang = np.arctan2(dy, dx)
        dist = np.hypot(dx, dy) / (rad * (1 + 0.22 * np.sin(3 * ang + sd) + 0.12 * np.sin(5 * ang + 2 * sd)))
        m = dist < 1
        col[m] = RUST * (0.85 + 0.3 * dist[m])[:, None]
    col[mount] = MOUNT
    if "--debug" in sys.argv:
        col[:] = (1, 0, 1)
    return np.clip(col, 0, 1)


DISH_MASK = np.zeros((TH, TW), bool)
raster(sel_uv, sel_pos, paint_dish, DISH_MASK)

# ================================================================== 2. foot lamps (base)
bme = base_o.data
names = [m.name.split(".")[0] for m in bme.materials]
bm = bmesh.new()
bm.from_mesh(bme)
for v in bm.verts:
    v.co *= 100
uvl = bm.loops.layers.uv[0]
oi = names.index("OtherOrange")
bi = names.index("ConsoleBody")
bm.faces.ensure_lookup_table()
conv = [f for f in bm.faces if f.material_index == oi and FOOT_ZONE(f.calc_center_median())]
conv_uv = [np.array([l[uvl].uv[:] for l in f.loops]) for f in conv]
conv_pos = [np.array([v.co[:] for v in f.verts]) for f in conv]
DARK = np.array([.14, .145, .15])
FOOT_MASK = np.zeros((TH, TW), bool)
raster(conv_uv, conv_pos, lambda P, ti: np.tile(DARK, (len(P), 1)), FOOT_MASK)
for f in conv:
    f.material_index = bi
N_CONV = len(conv)

# caps
CAPS = {}
N = 20
PROFILE = [(1.0, -0.6), (1.0, 0.75), (0.9, 1.15), (0.66, 1.5), (0.34, 1.73), (0.0, 1.8)]
allv = np.array([v.co[:] for v in bm.verts])
cap_faces = 0
for nm, (cx, cy, r) in LAMPS.items():
    d = np.hypot(allv[:, 0] - cx, allv[:, 1] - cy)
    near = allv[(d < r * 0.9) & (allv[:, 2] > 20) & (allv[:, 2] < 30)]
    ztop = float(np.percentile(near[:, 2], 85)) if len(near) else 25.0
    CAPS[nm] = dict(centre_xy=[cx, cy], radius=r, housing_top_z=round(ztop, 2))
    rings = []
    for fr, dz in PROFILE:
        if fr == 0.0:
            rings.append([bm.verts.new((cx, cy, ztop + dz))])
        else:
            rings.append([bm.verts.new((cx + math.cos(2 * math.pi * k / N) * r * fr, cy + math.sin(2 * math.pi * k / N) * r * fr, ztop + dz)) for k in range(N)])
    fl = []
    for i in range(len(rings) - 1):
        a, b = rings[i], rings[i + 1]
        for k in range(N):
            k2 = (k + 1) % N
            if len(b) == 1:
                fl.append(bm.faces.new((a[k], a[k2], b[0])))
            else:
                fl.append(bm.faces.new((a[k], a[k2], b[k2], b[k])))
    bot = bm.verts.new((cx, cy, ztop + PROFILE[0][1]))
    for k in range(N):
        fl.append(bm.faces.new((bot, rings[0][(k + 1) % N], rings[0][k])))
    bmesh.ops.recalc_face_normals(bm, faces=fl)
    for f in fl:
        f.material_index = oi
        for l in f.loops:
            l[uvl].uv = (0.0, 0.0)
    cap_faces += len(fl)
bmesh.ops.triangulate(bm, faces=[f for f in bm.faces if len(f.verts) > 3])
for v in bm.verts:
    v.co *= 0.01
bm.to_mesh(bme)
bm.free()
bme.shade_flat()

# ================================================================== 3. texture write, bleed, materials
COV = coverage([base_o, head_o])
# the covered mask is computed on the new mesh too; caps map to (0,0), ignore them: dilate only into texels no face covers
for MASK in (DISH_MASK | FOOT_MASK,):
    cur = MASK.copy()
    for it in range(3):
        grow = np.zeros_like(cur)
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            grow |= np.roll(cur, (dy, dx), (0, 1))
        add = grow & ~cur & ~COV
        # colour from the nearest painted neighbour
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            src = np.roll(cur, (dy, dx), (0, 1))
            tgt = add & src
            NEW[tgt, :3] = np.roll(NEW, (dy, dx), (0, 1))[tgt, :3]
            add &= ~tgt
        cur = cur | grow & ~COV
img = bpy.data.images.new("retouch", TW, TH, alpha=False)
img.colorspace_settings.name = "Non-Color"
NEW[..., 3] = 1.0
img.pixels.foreach_set(NEW.ravel())
img.filepath_raw = os.path.join(OUT, TEX_NEW)
img.file_format = "PNG"
img.save()


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
    t.image = bpy.data.images.load(os.path.join(OUT, TEX_NEW), check_existing=False)
    nt.links.new(t.outputs["Color"], p.inputs["Base Color"])
    p.inputs["Roughness"].default_value = 0.8
    return m


MATS = {"ConsoleBody": body_mat(), "OtherOrange": flat_mat("OtherOrange", (1.0, 0.45, 0.02), 2.0),
        "AntennaOrange": flat_mat("AntennaOrange", (1.0, 0.45, 0.02), 2.0), "Screen": flat_mat("Screen", (0.04, 0.06, 0.08))}
for o in (base_o, head_o):
    for k, s in enumerate(o.material_slots):
        s.material = MATS[s.material.name.split(".")[0]]
base_o.name, head_o.name = "console_c2_base", "console_c2_head"
base_o.data.name, head_o.data.name = "console_c2_base", "console_c2_head"

EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
for o in (base_o, head_o):
    bpy.ops.object.select_all(action="DESELECT")
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.export_scene.fbx(filepath=os.path.join(OUT, o.name + ".fbx"), **EXPORT)
usf = []
for n in ("console_c2_base", "console_c2_head"):
    b_ = open(os.path.join(OUT, n + ".fbx"), "rb").read()
    j = b_.find(b"UnitScaleFactor")
    k = b_.find(b"D", j + 20)
    usf.append(struct.unpack("<d", b_[k + 1:k + 9])[0])

# ================================================================== read back + report
cut = json.load(open(os.path.join(IND, "console_c2_cut.report.json")))
OFF = np.array(cut["pivots"]["head_offset_on_base_cm"])
RB = {}
for n in ("console_c2_base", "console_c2_head"):
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
                 uv_layers=[u.name for u in me.uv_layers], open_edges=sum(1 for v in ec.values() if v == 1), nonmanifold_edges=sum(1 for v in ec.values() if v > 2),
                 texture=[m.node_tree.nodes["Image Texture"].image.filepath for m in me.materials if "Image Texture" in m.node_tree.nodes])
hs = np.array(RB["console_c2_head"]["bounds_min_cm"]) + OFF
he = np.array(RB["console_c2_head"]["bounds_max_cm"]) + OFF
asm_min = np.minimum(np.array(RB["console_c2_base"]["bounds_min_cm"]), hs)
asm_max = np.maximum(np.array(RB["console_c2_base"]["bounds_max_cm"]), he)
rep = dict(task="task-301-03-console-c2-retouch", inputs=dict(base="console_c2_base.fbx", head="console_c2_head.fbx", texture_before="console_c2_basecolor_meshy.png"),
           frame=cut["frame"], unit_scale_factor_in_files=usf,
           pivots=dict(base_pivot_stage_cm=cut["pivots"]["base_pivot_stage_cm"], head_offset_on_base_cm=cut["pivots"]["head_offset_on_base_cm"],
                       note="unchanged from console_c2_cut.report.json: base origin = bottom centre of the footprint, head origin = mast axis at z 108 cm"),
           dish_back=dict(frame_centre_head_cm=C.tolist(), axis=[round(float(x), 4) for x in A], faces_repainted=len(SEL_FACES), dark_faces_kept=n_dark, rho_max_cm=round(RHO_MAX, 1),
                          texels_painted=int(DISH_MASK.sum()), colours=dict(white=[round(float(x), 3) for x in WHITE], teal=[round(float(x), 3) for x in TEAL]), rust_chips=len(CHIPS)),
           foot_lamps=dict(orange_faces_returned_to_ConsoleBody=N_CONV, texels_darkened=int(FOOT_MASK.sum()), cap_faces_added=cap_faces, caps=CAPS,
                           cap_profile="cylinder + 4-ring dome, 20 sides, height about 2.4 cm (buried 0.6), OtherOrange slot, flat emissive material (1.0, 0.45, 0.02) strength 2.0"),
           parts={n: RB[n] for n in RB},
           faces_per_slot={n: RB[n]["faces_per_slot"] for n in RB},
           faces_per_slot_before={n: cut["parts"][n]["faces_per_slot"] for n in cut["parts"]},
           assembled=dict(size_cm=[round(float(x), 2) for x in (asm_max - asm_min)], bounds_min_cm=[round(float(x), 2) for x in asm_min], bounds_max_cm=[round(float(x), 2) for x in asm_max],
                          before_size_cm=cut["assembled"]["size_cm"]),
           texture=TEX_NEW, seconds=round(time.time() - T0, 1))
with open(os.path.join(OUT, "console_c2_retouch.report.json"), "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("DONE", json.dumps(dict(slots=rep["faces_per_slot"], dish=rep["dish_back"], lamps=CAPS, conv=N_CONV, asm=rep["assembled"]["size_cm"])))
