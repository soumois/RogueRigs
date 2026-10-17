"""task-225-01: shield colour test, PREVIEW ONLY (nothing in Art/Meshes or Unreal is written). Headless:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/shield_colour_test.py -- [--out Art/Source/shield-colour-test] [--tmp <short dir>]
Variants A (charcoal #33363B) and B (light steel grey #A9AFB5): the plate texels of T_ChaserShield_BaseColor.png are recoloured, the yellow and black
stripe zone (same zone function as shield_v2.py), the grip and the bolts are kept (grip and bolts: stray yellow pushed to gunmetal). Emblem: yellow parts only
(KV and facets) on transparent. Renders: shield close-up, robot with the shield (clip 0 frame 0) from a steep camera on an in-game coloured floor, sheet."""
import bpy, json, math, os, sys
import numpy as np
from mathutils import Vector, Matrix

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
opt = {"--out": "Art/Source/shield-colour-test", "--tmp": "C:/Users/soumo/AppData/Local/Temp/rr-sct"}
for k, v in zip(argv[::2], argv[1::2]):
    opt[k] = v
ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
OUT = os.path.abspath(os.path.join(ROOT, opt["--out"]))
TMP = os.path.abspath(opt["--tmp"])
os.makedirs(OUT, exist_ok=True); os.makedirs(TMP, exist_ok=True)
SH = os.path.join(ROOT, "Art", "Meshes", "ChaserShield")
SC_DIR = os.path.join(ROOT, "Art", "Meshes", "shield-chaser")
CH_DIR = os.path.join(ROOT, "Art", "Meshes", "chaser-meshy")
assert os.path.abspath(OUT).startswith(os.path.join(ROOT, "Art", "Source", "shield-colour-test"))

VARIANTS = {"A": "#33363B", "B": "#A9AFB5"}
def hex_srgb(h): return np.array([int(h[i:i + 2], 16) for i in (1, 3, 5)], np.float32) / 255.0
rep = json.load(open(os.path.join(SH, "SM_ChaserShield.report.json"), encoding="utf-8"))
YEL = np.array(rep["colours_srgb"]["yellow"], np.float32); BLK = np.array(rep["colours_srgb"]["black"], np.float32); GUN = np.array(rep["colours_srgb"]["gunmetal"], np.float32)
DX = rep["decal"]["front_face_x_cm_from_pivot"] - 4.90            # pivot shift in x (raw frame -> exported frame)
X_BACK, X_FRONT, HEIGHT = 2.45, 4.90, 82.0

def load_np(path):
    im = bpy.data.images.load(path, check_existing=False); im.colorspace_settings.name = "Non-Color"
    w, h = im.size
    a = np.array(im.pixels[:], np.float32).reshape(h, w, 4)
    bpy.data.images.remove(im); return a            # rows bottom to top
def save_np(path, a):
    h, w = a.shape[:2]
    im = bpy.data.images.new("o", w, h, alpha=True, float_buffer=False)
    im.colorspace_settings.name = "sRGB"; im.alpha_mode = "STRAIGHT"
    im.pixels.foreach_set(np.clip(a, 0, 1).astype(np.float32).ravel())
    im.filepath_raw = path; im.file_format = "PNG"; im.save(); bpy.data.images.remove(im)

# ================================================================ 1) textures
def dilate(img, known, iters=12):
    """uncovered texels near a covered one take the mean of the covered neighbours (margin of the recoloured result: no stale yellow on island borders)"""
    img = img.copy(); kn = known.copy()
    for _ in range(iters):
        acc = np.zeros_like(img); cnt = np.zeros(kn.shape, np.float32)
        for dy, dx in ((1, 0), (-1, 0), (0, 1), (0, -1), (1, 1), (1, -1), (-1, 1), (-1, -1)):
            k2 = np.roll(kn, (dy, dx), (0, 1)); i2 = np.roll(img, (dy, dx), (0, 1))
            acc += i2 * k2[..., None]; cnt += k2
        upd = ~kn & (cnt > 0)
        img[upd] = acc[upd] / cnt[upd][:, None]; kn = kn | upd
    return img

def make_textures():
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=os.path.join(SH, "SM_ChaserShield.fbx"))
    ob = next(o for o in bpy.data.objects if o.type == "MESH"); me = ob.data
    assert max(abs(ob.matrix_world[i][j] - (1.0 if i == j else 0.0)) for i in range(4) for j in range(4)) < 1e-4
    base = load_np(os.path.join(SH, "T_ChaserShield_BaseColor.png")); H, W = base.shape[:2]
    px = base[..., :3].copy()
    uvd = me.uv_layers["UVMap"].data
    tri = -np.ones((H, W), np.int32); pos = np.zeros((H, W, 3), np.float32); nrm = np.zeros((H, W, 3), np.float32)
    EPS = 2.5 / W; areas = []
    for p in me.polygons:
        if p.material_index != 0: continue                       # skip the emblem decal quad
        ls = list(p.loop_indices); assert len(ls) == 3
        uvs = np.array([uvd[l].uv[:] for l in ls], np.float64)
        ps = np.array([me.vertices[me.loops[l].vertex_index].co[:] for l in ls], np.float64)
        a2 = abs(np.cross(uvs[1] - uvs[0], uvs[2] - uvs[0])) / 2
        if a2 > 1e-9: areas.append(math.sqrt(p.area / a2) / W)
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
        inside = (l1 >= -EPS) & (l2 >= -EPS) & (l3 >= -EPS); strict = (l1 >= 0) & (l2 >= 0) & (l3 >= 0)
        upd = inside & ((tri[gy, gx] < 0) | strict)
        P = l1[..., None] * ps[0] + l2[..., None] * ps[1] + l3[..., None] * ps[2]
        tri[gy[upd], gx[upd]] = p.index; pos[gy[upd], gx[upd]] = P[upd]; nrm[gy[upd], gx[upd]] = np.array(p.normal[:], np.float32)
    cov = tri >= 0; PX_CM = float(np.median(areas)); print("coverage", float(cov.mean()), "px cm", PX_CM)
    X_ = pos[..., 0] - DX; Y_, Z_ = pos[..., 1], pos[..., 2]; nx, ny, nz = nrm[..., 0], nrm[..., 1], nrm[..., 2]
    slab = cov & (X_ > X_BACK - 0.03) & (X_ < X_FRONT + 0.03)
    is_face_fb = slab & (abs(nx) > 0.9) & ((X_ > X_FRONT - 0.03) | (X_ < X_BACK + 0.03))
    is_top = slab & (nz > 0.9) & (Z_ > HEIGHT - 0.05)
    is_cut = slab & (abs(nx) < 0.1) & (abs(abs(ny) - 0.7071) < 0.05) & (abs(abs(nz) - 0.7071) < 0.05) & ((ny * nz) > 0) & ((Y_ + Z_ > 60) | (Y_ + Z_ < 20))
    sd = np.maximum(np.maximum(Z_ - 75.75, (Y_ + Z_ - 83.56) / math.sqrt(2)), (-0.3 - (Y_ + Z_)) / math.sqrt(2))
    a_zone = np.where(is_face_fb, np.clip(0.5 + sd / PX_CM, 0, 1), np.where(is_top | is_cut, 1.0, 0.0)).astype(np.float32)[..., None]
    dyb = YEL - BLK
    t_y = np.clip(((px - BLK) @ dyb) / float(dyb @ dyb), 0, 1)
    alpha = np.minimum(1 - t_y, a_zone[..., 0])[..., None]                 # black share of the original texel
    yel_like = cov & ~slab & ((px[..., 0] - px[..., 2]) > 0.12)
    for name, hx in VARIANTS.items():
        V = hex_srgb(hx)
        new = px.copy()
        new[slab] = (V * (1 - a_zone) + a_zone * YEL + alpha * (BLK - YEL))[slab]
        new[yel_like] = GUN
        new = dilate(new, cov)
        out = np.concatenate([new, np.ones((H, W, 1), np.float32)], -1)
        save_np(os.path.join(OUT, "T_ChaserShield_BaseColor_%s.png" % name), out)
        print("variant", name, hx, "slab texels", int(slab.sum()), "zone", int((a_zone > 0.5).sum()), "grip yellow fixed", int(yel_like.sum()))
    # emblem: yellow parts only
    em = load_np(os.path.join(SH, "T_ChaserShield_Emblem.png")); rb = em[..., 0] - em[..., 2]
    solid = (rb > 0.9) & (em[..., 3] > 0.99); EY = np.median(em[..., :3][solid], axis=0); ry = float(np.median(rb[solid]))
    a = np.clip(rb / ry, 0, 1) * em[..., 3]
    a = np.where(a < 0.04, 0.0, a)
    outm = np.zeros_like(em); outm[..., :3] = EY; outm[..., 3] = a
    save_np(os.path.join(OUT, "T_ChaserShield_Emblem_YellowOnly.png"), outm)
    print("emblem yellow", EY, "opaque px", int((a > 0.5).sum()))

# ================================================================ 2) renders
def mk_mat(name, tex, alpha=False):
    m = bpy.data.materials.new(name); m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]; b.inputs["Roughness"].default_value = 0.8
    t = m.node_tree.nodes.new("ShaderNodeTexImage"); im = bpy.data.images.load(tex, check_existing=False)
    im.colorspace_settings.name = "sRGB"
    if alpha: im.alpha_mode = "STRAIGHT"
    t.image = im; t.interpolation = "Linear"; t.extension = "CLIP" if alpha else "REPEAT"
    u = m.node_tree.nodes.new("ShaderNodeUVMap"); u.uv_map = "UVMap"
    m.node_tree.links.new(u.outputs["UV"], t.inputs["Vector"]); m.node_tree.links.new(t.outputs["Color"], b.inputs["Base Color"])
    if alpha:
        m.node_tree.links.new(t.outputs["Alpha"], b.inputs["Alpha"])
        for attr, val in (("surface_render_method", "BLENDED"), ("blend_method", "BLEND")):
            try: setattr(m, attr, val)
            except (TypeError, AttributeError): pass
    return m

VAR_TEX = {"cur": (os.path.join(SH, "T_ChaserShield_BaseColor.png"), os.path.join(SH, "T_ChaserShield_Emblem.png")),
           "A": (os.path.join(OUT, "T_ChaserShield_BaseColor_A.png"), os.path.join(OUT, "T_ChaserShield_Emblem_YellowOnly.png")),
           "B": (os.path.join(OUT, "T_ChaserShield_BaseColor_B.png"), os.path.join(OUT, "T_ChaserShield_Emblem_YellowOnly.png"))}

def srgb2lin(c): return np.where(c <= 0.04045, c / 12.92, ((c + 0.055) / 1.055) ** 2.4)
def lin2srgb(c): return np.where(c <= 0.0031308, c * 12.92, 1.055 * np.power(np.maximum(c, 1e-9), 1 / 2.4) - 0.055)

ig = load_np(os.path.join(ROOT, "vault", "assets", "shield-chaser-ingame-run-close.png"))[..., :3]
FLOOR_SRGB = np.median(ig.reshape(-1, 3), axis=0); print("in-game floor sRGB", FLOOR_SRGB, "hex", "#%02X%02X%02X" % tuple(int(round(x * 255)) for x in FLOOR_SRGB))
IGY = ig[(ig[..., 0] - ig[..., 2]) > 0.5]; print("in-game yellow sRGB median", np.median(IGY, axis=0), len(IGY))
GREY = np.array([0.76, 0.76, 0.76], np.float32)

def setup_render():
    sc = bpy.context.scene
    for n in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try: sc.render.engine = n; break
        except TypeError: continue
    sc.render.image_settings.file_format = "PNG"; sc.render.image_settings.color_mode = "RGBA"; sc.render.film_transparent = True
    sc.view_settings.view_transform = "Standard"; sc.view_settings.look = "None"
    w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
    bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (1, 1, 1, 1); bg.inputs["Strength"].default_value = 0.35
    cd = bpy.data.cameras.new("Cam"); cd.clip_start = 1; cd.clip_end = 6000; cd.type = "ORTHO"
    cam = bpy.data.objects.new("Cam", cd); sc.collection.objects.link(cam); sc.camera = cam
    sd = bpy.data.lights.new("Sun", "SUN"); sd.energy = float(os.environ.get("SCT_SUN", "1.7"))
    sun = bpy.data.objects.new("Sun", sd); sc.collection.objects.link(sun)
    return sc, cam, cd, sun

def dirv(az, el):
    a, e = math.radians(az), math.radians(el)
    return Vector((math.cos(e) * math.cos(a), math.cos(e) * math.sin(a), math.sin(e)))

def shot(sc, cam, cd, sun, az, el, tgt, osc, px, py, light=None, floor=False, roll_up=None):
    d = dirv(az, el); target = Vector(tgt)
    cd.ortho_scale = osc; sc.render.resolution_x, sc.render.resolution_y = px, py
    cam.location = target + d * 1500
    cam.rotation_euler = (target - cam.location).to_track_quat("-Z", "Y").to_euler()
    if light is None:
        side = d.cross(Vector((0, 0, 1))); lv = (d + Vector((0, 0, 0.6)) + side * 0.35).normalized()
    else:
        lv = Vector(light).normalized()
    sun.rotation_euler = (-lv).to_track_quat("-Z", "Y").to_euler()
    path = os.path.join(TMP, "r.png"); sc.render.filepath = path
    sc.render.film_transparent = not floor
    bpy.ops.render.render(write_still=True)
    r = bpy.data.images.load(path, check_existing=False); r.colorspace_settings.name = "Non-Color"
    a = np.array(r.pixels[:], np.float32).reshape(py, px, 4); bpy.data.images.remove(r)
    al = a[..., 3:4]
    return np.clip(a[..., :3], 0, 1) * al + GREY * (1 - al)

def save_rgb(path, a):
    save_np(path, np.concatenate([a, np.ones(a.shape[:2] + (1,), np.float32)], -1))

GAME_LIGHT = (0.55, 0.35, 1.0)          # sun toward the front plate, from above (the plate faces +X)
GAME_CAM = dict(az=90, el=72)
FRONT_CAM = dict(az=30, el=70)           # steep, near vertical, robot (facing +X) towards screen left like the in-game shot
STAGE = {}

def shield_close(var):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=os.path.join(SH, "SM_ChaserShield.fbx"))
    ob = next(o for o in bpy.data.objects if o.type == "MESH")
    t0, t1 = VAR_TEX[var]
    ob.data.materials[0] = mk_mat("M_" + var, t0); ob.data.materials[1] = mk_mat("E_" + var, t1, True)
    sc, cam, cd, sun = setup_render()
    a = shot(sc, cam, cd, sun, -40, 35, (0, 0, 41), 100, 900, 1200)
    save_rgb(os.path.join(OUT, "shield_%s_close.png" % var), a)
    return a

def load_pose_rows():
    im = bpy.data.images.load(os.path.join(SC_DIR, "T_ShieldChaserAnim.exr"), check_existing=False); im.colorspace_settings.name = "Non-Color"
    w, h = im.size; a = np.array(im.pixels[:], np.float32).reshape(h, w, 4); bpy.data.images.remove(im)
    return a[::-1]            # row 0 = top of the texture

def qmat(q):
    x, y, z, w = q / np.linalg.norm(q)
    return np.array([[1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)], [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
                     [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)]])

PARTS = ["Pelvis", "HeadTorso", "UpperArm_L", "UpperArm_R", "ForearmHand_L", "ForearmHand_R", "Thigh_L", "Thigh_R", "ShinFoot_L", "ShinFoot_R"]

def robot_scene(var):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=os.path.join(SC_DIR, "SM_ShieldChaser_Meshy_Anim.fbx"), colors_type="LINEAR")
    OBJ = {o.name: o for o in bpy.data.objects if o.type == "MESH"}
    bpy.context.view_layer.update()
    MW = {n: o.matrix_world.copy() for n, o in OBJ.items()}
    for n, o in OBJ.items():
        o.parent = None; o.data.transform(MW[n]); o.matrix_world = Matrix.Identity(4)
    img = bpy.data.images.load(os.path.join(CH_DIR, "chaser_meshy_fixed_basecolor.png"), check_existing=False); img.colorspace_settings.name = "sRGB"
    for nd in OBJ["Pelvis"].data.materials[0].node_tree.nodes:
        if nd.type == "TEX_IMAGE": nd.image = img
    t0, t1 = VAR_TEX[var]
    OBJ["Shield"].data.materials[0] = mk_mat("M_" + var, t0); OBJ["Shield"].data.materials[1] = mk_mat("E_" + var, t1, True)
    rows = load_pose_rows()
    for pi, n in enumerate(PARTS):
        Rm = qmat(rows[0, 2 * pi].astype(float)); t = rows[0, 2 * pi + 1, :3].astype(float)
        M = Matrix.Identity(4)
        for i in range(3):
            for j in range(3): M[i][j] = Rm[i][j]
            M[i][3] = t[i]
        OBJ[n].matrix_world = M
    OBJ["Shield"].matrix_world = OBJ["ForearmHand_L"].matrix_world.copy()
    bpy.context.view_layer.update()
    sc, cam, cd, sun = setup_render()
    fl = bpy.data.meshes.new("floor"); fl.from_pydata([(-3000, -3000, -0.3), (3000, -3000, -0.3), (3000, 3000, -0.3), (-3000, 3000, -0.3)], [], [(0, 1, 2, 3)])
    fm = bpy.data.materials.new("floor"); fm.use_nodes = True; fl.materials.append(fm)
    sc.collection.objects.link(bpy.data.objects.new("floor", fl))
    return sc, cam, cd, sun, fm

FLOOR_ALB = None
def calibrate(sc, cam, cd, sun, fm):
    """set the floor albedo so the rendered floor equals the in-game floor sRGB"""
    global FLOOR_ALB
    bsdf = fm.node_tree.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 1.0
    alb = srgb2lin(FLOOR_SRGB).copy()
    for _ in range(3):
        bsdf.inputs["Base Color"].default_value = (*alb, 1)
        a = shot(sc, cam, cd, sun, GAME_CAM["az"], GAME_CAM["el"], (1500, 1500, 0), 20, 40, 40, GAME_LIGHT, True)
        got = srgb2lin(np.median(a.reshape(-1, 3), axis=0))
        alb = alb * (srgb2lin(FLOOR_SRGB) / np.maximum(got, 1e-4))
    FLOOR_ALB = alb; print("floor albedo (lin)", alb, "rendered", lin2srgb(got))
    return alb

def robot_shots(var):
    sc, cam, cd, sun, fm = robot_scene(var)
    bsdf = fm.node_tree.nodes["Principled BSDF"]; bsdf.inputs["Roughness"].default_value = 1.0
    global FLOOR_ALB
    if FLOOR_ALB is None: calibrate(sc, cam, cd, sun, fm)
    bsdf.inputs["Base Color"].default_value = (*FLOOR_ALB, 1)
    out = {}
    for cn, cc in (("side", GAME_CAM), ("front", FRONT_CAM)):
        mid = shot(sc, cam, cd, sun, cc["az"], cc["el"], (14, 0, 66), 200, 900, 900, GAME_LIGHT, True)
        save_rgb(os.path.join(OUT, "robot_%s_mid_%s.png" % (var, cn)), mid)
        # game-scale: 600 px frame height ~ 560 cm like the in-game shot (robot about 130 px tall), floor everywhere
        game = shot(sc, cam, cd, sun, cc["az"], cc["el"], (14, 0, 0), 1120, 1200, 600, GAME_LIGHT, True)
        save_rgb(os.path.join(OUT, "robot_%s_game_%s.png" % (var, cn)), game)
        out[cn] = (mid, game)
    return out

def fit_h(a, h):
    ys = (np.arange(h) * a.shape[0] / h).astype(int); xs = (np.arange(int(a.shape[1] * h / a.shape[0])) * a.shape[0] / h).astype(int)
    return a[ys][:, xs]

if __name__ == "__main__":
    make_textures()
    V3 = ("cur", "A", "B")
    close, rob = {}, {}
    for var in V3:
        close[var] = shield_close(var); rob[var] = robot_shots(var)
    # sheet (top to bottom): shield close-up; robot from the front camera; robot from the side camera; game-scale crops (x2) front and side. Columns: current yellow, A, B.
    crop = lambda g: np.repeat(np.repeat(g[150:450, 450:750], 2, 0), 2, 1)
    rows_ = [[fit_h(close[v], 600) for v in V3], [fit_h(rob[v]["front"][0], 600) for v in V3], [fit_h(rob[v]["side"][0], 600) for v in V3],
             [crop(rob[v]["front"][1]) for v in V3], [crop(rob[v]["side"][1]) for v in V3]]
    SEP = np.float32(0.1)
    w = max(sum(a.shape[1] + 8 for a in r) for r in rows_)
    def rowcat(r):
        a = np.concatenate([np.concatenate([x, np.full((x.shape[0], 8, 3), SEP, np.float32)], 1) for x in r], 1)
        return np.concatenate([a, np.full((a.shape[0], w - a.shape[1], 3), SEP, np.float32)], 1)
    parts = []
    for r in rows_:
        parts += [rowcat(r), np.full((8, w, 3), SEP, np.float32)]
    sheet = np.concatenate(parts[::-1], 0)           # image rows are stored bottom to top: first part ends up on top
    save_rgb(os.path.join(OUT, "sheet_shield_colours.png"), sheet)
    print("sheet", sheet.shape)
