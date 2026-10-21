"""task-278-02: move every eye face of the chaser FBX into a new LAST material slot `robot_eye`.
Run: blender -b --factory-startup -P eye_slot_split.py -- --fbx <fbx> --tex <basecolor png> [--body-mat meshy_chaser] [--write]
Nothing else changes (positions, UV0/1/2, vertex colours, tris, existing material order). With --write the FBX is overwritten
with the original export settings, only when every check passes. Without --write only the report is computed.
Exit code 1 on a failed check."""
import bpy, sys, os, json
import numpy as np

a = sys.argv[sys.argv.index("--") + 1:]


def arg(n, d=None):
    return a[a.index(n) + 1] if n in a else d


FBX = os.path.abspath(arg("--fbx"))
TEX = os.path.abspath(arg("--tex"))
BODY = arg("--body-mat", "meshy_chaser")
WRITE = "--write" in a
ZMIN, ZMAX, XMIN = float(arg("--zmin", 92)), float(arg("--zmax", 108)), float(arg("--xmin", 2))
NECK = (85.0, 91.6)
THR = 0.1
EXPORT = dict(use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS", global_scale=1.0, axis_forward="-Z", axis_up="Y",
              mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False, colors_type="LINEAR")


def load(path):
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path, colors_type="LINEAR")
    return sorted([o for o in bpy.data.objects if o.type == "MESH"], key=lambda o: o.name)


# ---- mask from the PNG (raw bytes / 255, sRGB decoded by hand to linear)
img = bpy.data.images.load(TEX, check_existing=False)
img.colorspace_settings.name = "Non-Color"
W, H = img.size
px = np.empty(W * H * 4, np.float32)
img.pixels.foreach_get(px)
px = px.reshape(H, W, 4)[:, :, :3]  # row 0 = bottom (v = 0)
lin = np.where(px <= 0.04045, px / 12.92, ((px + 0.055) / 1.055) ** 2.4)
mask = np.clip((lin[:, :, 0] - 2 * np.maximum(lin[:, :, 1], lin[:, :, 2])) * 3, 0, 1)
RED = (mask > THR).reshape(-1)
print("texels mask>0.1", int(RED.sum()))


def cover(uv3):
    """texels covered by a UV triangle: texel centres inside, plus the texels holding the vertices and the centroid."""
    p = uv3 * np.array([W, H])
    out = set()
    for q in list(p) + [p.mean(0)]:
        x, y = int(np.floor(q[0])), int(np.floor(q[1]))
        if 0 <= x < W and 0 <= y < H:
            out.add(y * W + x)
    x0, x1 = int(np.floor(p[:, 0].min())), int(np.ceil(p[:, 0].max()))
    y0, y1 = int(np.floor(p[:, 1].min())), int(np.ceil(p[:, 1].max()))
    x0, y0 = max(x0, 0), max(y0, 0)
    x1, y1 = min(x1, W - 1), min(y1, H - 1)
    if x1 >= x0 and y1 >= y0:
        xs, ys = np.meshgrid(np.arange(x0, x1 + 1) + 0.5, np.arange(y0, y1 + 1) + 0.5)
        A, B, C = p
        d = (B[1] - C[1]) * (A[0] - C[0]) + (C[0] - B[0]) * (A[1] - C[1])
        if abs(d) > 1e-12:
            l1 = ((B[1] - C[1]) * (xs - C[0]) + (C[0] - B[0]) * (ys - C[1])) / d
            l2 = ((C[1] - A[1]) * (xs - C[0]) + (A[0] - C[0]) * (ys - C[1])) / d
            l3 = 1 - l1 - l2
            ins = (l1 >= 0) & (l2 >= 0) & (l3 >= 0)
            for yy, xx in zip(ys[ins], xs[ins]):
                out.add(int(yy) * W + int(xx))
    return out


def snapshot(o):
    me = o.data
    mw = np.array(o.matrix_world)
    co = np.empty(len(me.vertices) * 3, np.float64)
    me.vertices.foreach_get("co", co)
    co = co.reshape(-1, 3)
    wco = co @ mw[:3, :3].T + mw[:3, 3]
    vi = np.empty(len(me.loops), np.int64)
    me.loops.foreach_get("vertex_index", vi)
    s = dict(pos=wco[vi])
    for k, n in (("uv0", me.uv_layers[0].name), ("uv1", "LightMap"), ("uv2", "PartId")):
        u = np.empty(len(me.loops) * 2, np.float64)
        me.uv_layers[n].data.foreach_get("uv", u)
        s[k] = u.reshape(-1, 2)
    ca = me.color_attributes["PartIndex"]
    c = np.empty(len(ca.data) * 4, np.float64)
    ca.data.foreach_get("color", c)
    c = c.reshape(-1, 4)
    s["col"] = c[vi] if ca.domain == "POINT" else c
    s["tris"] = sum(len(p.vertices) - 2 for p in me.polygons)
    s["mats"] = [m.name for m in me.materials]
    mi = np.empty(len(me.polygons), np.int64)
    me.polygons.foreach_get("material_index", mi)
    s["mi"] = mi
    return s


def key_sort(s):
    k = np.round(np.hstack([s["pos"], s["uv0"]]), 4)
    return np.lexsort(k.T[::-1])


def compare(sa, sb):
    if any(len(sa[k]) != len(sb[k]) for k in ("pos", "uv0", "uv1", "uv2", "col")):
        return None
    ia, ib = key_sort(sa), key_sort(sb)
    out = {}
    for k in ("pos", "uv0", "uv1", "uv2", "col"):
        out[k] = dict(direct=float(np.abs(sa[k] - sb[k]).max()), matched_by_pos_uv=float(np.abs(sa[k][ia] - sb[k][ib]).max()))
    return out


# ---------------------------------------------------------------- classify
objs = load(FBX)
before = {o.name: snapshot(o) for o in objs}
tris_before = sum(s["tris"] for s in before.values())
mat_order_before = {o.name: [m.name for m in o.data.materials] for o in objs}
eye_faces, per_obj = {}, {}
body_zone_red, neck_red, eye_cov = set(), set(), set()
zmax_all = max(float(s["pos"][:, 2].max()) for s in before.values())
print("max z", zmax_all)
for o in objs:
    me = o.data
    names = [m.name for m in me.materials]
    if BODY not in names:
        continue
    bi = names.index(BODY)
    mw = o.matrix_world
    uv = me.uv_layers[0].data
    me.calc_loop_triangles()
    tri_of = {}
    for t in me.loop_triangles:
        tri_of.setdefault(t.polygon_index, []).append(t)
    sel = []
    for p in me.polygons:
        if p.material_index != bi:
            continue
        c = mw @ p.center
        cov = set()
        for t in tri_of[p.index]:
            cov |= cover(np.array([uv[l].uv[:] for l in t.loops], np.float64))
        ids = np.fromiter(cov, np.int64, len(cov))
        red = ids[RED[ids]] if len(ids) else ids
        zone = ZMIN <= c.z <= ZMAX and c.x >= XMIN
        if len(red) and zone:
            sel.append(p.index)
            eye_cov |= set(red.tolist())
        else:
            if zone:
                body_zone_red |= set(red.tolist())
            if NECK[0] <= c.z <= NECK[1]:
                neck_red |= set(red.tolist())
    eye_faces[o.name] = sel
    per_obj[o.name] = len(sel)
n_eye = sum(per_obj.values())
left_in_body = len(body_zone_red)
neck_in_eye = len(neck_red & eye_cov)
print("eye faces", per_obj, "eye texels", len(eye_cov), "left in body", left_in_body, "neck in eye", neck_in_eye)

# ---------------------------------------------------------------- apply and export to a temp file
mat = bpy.data.materials.new("robot_eye")
mat.use_nodes = True
for o in objs:
    if eye_faces.get(o.name):
        me = o.data
        me.materials.append(mat)
        ei = len(me.materials) - 1
        for pi in eye_faces[o.name]:
            me.polygons[pi].material_index = ei
# Blender writes the Material nodes in order of first use over the objects (alphabetical): robot_eye would land before the shield
# materials. Reorder scene_data.data_materials after the scene scan so robot_eye is written last (model order untouched).
import io_scene_fbx.export_fbx_bin as _efb
_orig_scan = _efb.fbx_data_from_scene
def _scan(scene, depsgraph, settings):
    sd = _orig_scan(scene, depsgraph, settings)
    items = list(sd.data_materials.items())
    sd.data_materials.clear()
    for k, v in [i for i in items if getattr(i[0], 'name', '') != 'robot_eye'] + [i for i in items if getattr(i[0], 'name', '') == 'robot_eye']:
        sd.data_materials[k] = v
    return sd
_efb.fbx_data_from_scene = _scan
tmp = FBX[:-4] + ".tmp.fbx"
bpy.ops.object.select_all(action="DESELECT")
for o in objs:
    o.select_set(True)
bpy.context.view_layer.objects.active = objs[0]
bpy.ops.export_scene.fbx(filepath=tmp, **EXPORT)
FILE_ORDER = {}
from io_scene_fbx import parse_fbx
root_ = parse_fbx.parse(tmp)[0]
for el in root_.elems:
    if el.id == b"Objects":
        FILE_ORDER = dict(models=[g.props[1].split(bytes(1))[0].decode() for g in el.elems if g.id == b"Model"],
                          materials=[g.props[1].split(bytes(1))[0].decode() for g in el.elems if g.id == b"Material"])

# ---------------------------------------------------------------- read back and compare
objs2 = load(tmp)
after = {o.name: snapshot(o) for o in objs2}
cmp_ = {n: compare(before[n], after[n]) for n in before}
counts_ok = set(before) == set(after) and all(c is not None for c in cmp_.values())
if counts_ok:
    deltas = {k: dict(direct=max(c[k]["direct"] for c in cmp_.values()), matched_by_pos_uv=max(c[k]["matched_by_pos_uv"] for c in cmp_.values()))
              for k in ("pos", "uv0", "uv1", "uv2", "col")}
else:
    deltas = {k: dict(direct=1e9, matched_by_pos_uv=1e9) for k in ("pos", "uv0", "uv1", "uv2", "col")}
tris_after = sum(s["tris"] for s in after.values())
mats_after = {n: s["mats"] for n, s in after.items()}
eye_after = sum(int((s["mi"] == s["mats"].index("robot_eye")).sum()) for s in after.values() if "robot_eye" in s["mats"])
order_ok = all(mats_after[n][:len(mat_order_before[n])] == mat_order_before[n]
               and (mats_after[n][-1] == "robot_eye" if eye_faces.get(n) else mats_after[n] == mat_order_before[n]) for n in before)
slot_ok = True
for n in before:
    mb, ma = before[n]["mi"], after[n]["mi"]
    sel = np.zeros(len(mb), bool)
    sel[eye_faces.get(n, [])] = True
    slot_ok &= len(mb) == len(ma) and bool((ma[~sel] == mb[~sel]).all())
EPS = 1e-4
checks = dict(tris_unchanged=tris_before == tris_after, eye_faces_match=eye_after == n_eye and n_eye > 0,
              eye_texels_left_in_body_zero=left_in_body == 0, neck_texels_in_eye_zero=neck_in_eye == 0,
              material_order_ok=order_ok, other_faces_slot_ok=bool(slot_ok), corner_counts_ok=counts_ok,
              pos_ok=deltas["pos"]["direct"] < EPS, uv0_ok=deltas["uv0"]["direct"] < 1e-6, uv1_ok=deltas["uv1"]["direct"] < 1e-6,
              uv2_ok=deltas["uv2"]["direct"] < 1e-6, col_ok=deltas["col"]["direct"] < 1e-6)
ok = all(checks.values())
rep = dict(task="task-278-02-eye-slot-split", fbx=os.path.basename(FBX), script="Tools/art/blender/eye_slot_split.py",
           criterion=dict(texture=os.path.basename(TEX),
                          mask="saturate((R - 2*max(G,B))*3), linear, > 0.1, on a texel covered by the UV0 triangle (texel centres inside + texels of the vertices and centroid)",
                          head_zone_cm=dict(z=[ZMIN, ZMAX], x_min=XMIN), candidate_material=BODY, neck_ring_z_cm=list(NECK)),
           eye_faces=n_eye, eye_faces_per_object={k: v for k, v in per_obj.items() if v}, eye_texels_in_eye_faces=len(eye_cov),
           tris_before=tris_before, tris_after=tris_after,
           eye_texels_left_in_body_faces_head_zone=left_in_body, neck_ring_texels_in_eye_faces=neck_in_eye,
           max_deltas_before_vs_after=deltas, material_order_before_per_object=mat_order_before, material_order_after_per_object=mats_after,
           fbx_file_material_order=FILE_ORDER.get("materials"), fbx_file_object_order=FILE_ORDER.get("models"),
           checks=checks, passed=bool(ok), written=bool(WRITE and ok))
if WRITE and ok:
    os.replace(tmp, FBX)
else:
    os.remove(tmp)
with open(FBX[:-4] + ".eye_slot_split.report.json", "w", encoding="utf-8") as f:
    json.dump(rep, f, indent=1)
print("RESULT", json.dumps(dict(passed=bool(ok), eye_faces=n_eye, checks=checks, deltas=deltas, file_materials=FILE_ORDER.get("materials"))))
sys.exit(0 if ok else 1)
