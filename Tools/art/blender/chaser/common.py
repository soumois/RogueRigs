"""Shared code of the chaser model (task-151-05): palette, materials, logo texture, part builder, finish (normals, UVs, object), lightmap check, part report.
Units: 1 Blender unit = 1 cm. H = 124 cm (sole z = 0 to the top of the cap). Forward = +X, robot left = +Y.
THE CONTRACT (joints, parents, slots) lives here: part modules import it and must not change it."""
import bpy, bmesh, math, json, os
import numpy as np
from mathutils import Vector

H = 124.0
BUDGET = 3000


def h(v):
    return v * H


def srgb(hx):
    c = [int(hx[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    return tuple(((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92 for x in c) + (1.0,)


HEX = {"yellow_body": "#F5BA20", "gunmetal": "#3A3A3A", "eye_red": "#FF1010", "logo_plate": "#FFD84A"}   # logo plate: a lighter yellow than the body, so the plate reads on the block
ORDER = ["yellow_body", "gunmetal", "eye_red", "logo_plate"]          # material slot order, the same on every part
MI = {n: i for i, n in enumerate(ORDER)}

# ---------------------------------------------------------------- joint contract (cm for x, fractions of H for y and z), do not change from a part module
PIV_PELVIS = (1.5, 0.0, h(0.585))
PIV_WAIST = (1.5, 0.0, h(0.655))
SH = (-2.5, 0.185, 0.86)       # shoulder, left side
EL = (-4.0, 0.252, 0.755)      # elbow
HIP = (4.5, 0.105, 0.585)
KNEE = (1.5, 0.125, 0.383)
SIGN = {"L": 1.0, "R": -1.0, None: 1.0}
# hierarchy by base name; the parent of a sided part is the part of the same side
PARENT = {"Pelvis": None, "HeadTorso": "Pelvis", "UpperArm": "HeadTorso", "ForearmHand": "UpperArm", "Thigh": "Pelvis", "ShinFoot": "Thigh"}
SIDED = {"UpperArm", "ForearmHand", "Thigh", "ShinFoot"}
# part groups: group id -> (module, object base name, sided)
GROUPS = {"headtorso": ("part_headtorso", "HeadTorso", False), "pelvis": ("part_pelvis", "Pelvis", False), "upperarm": ("part_upperarm", "UpperArm", True),
          "forearmhand": ("part_forearmhand", "ForearmHand", True), "thigh": ("part_thigh", "Thigh", True), "shinfoot": ("part_shinfoot", "ShinFoot", True)}
BUILD_ORDER = ["Pelvis", "HeadTorso", "UpperArm_L", "ForearmHand_L", "UpperArm_R", "ForearmHand_R", "Thigh_L", "ShinFoot_L", "Thigh_R", "ShinFoot_R"]
HIERARCHY = {"Pelvis": ["HeadTorso", "Thigh_L", "Thigh_R"], "HeadTorso": ["UpperArm_L", "UpperArm_R"], "UpperArm_L": ["ForearmHand_L"], "UpperArm_R": ["ForearmHand_R"],
             "Thigh_L": ["ShinFoot_L"], "Thigh_R": ["ShinFoot_R"]}

MATS = {}
PIVOT_W = {}        # object name -> world pivot (Vector), filled by finish
PARENT_NAME = {}    # object name -> parent object name


def parent_name(base, side):
    p = PARENT[base]
    if p is None:
        return None
    return p + "_" + side if p in SIDED else p


def pivot_arm(side):
    return Vector((SH[0], SH[1] * H * SIGN[side], h(SH[2])))


def pivot_elbow(side):
    return Vector((EL[0], EL[1] * H * SIGN[side], h(EL[2])))


def pivot_hip(side):
    return Vector((HIP[0], HIP[1] * H * SIGN[side], h(HIP[2])))


def pivot_knee(side):
    return Vector((KNEE[0], KNEE[1] * H * SIGN[side], h(KNEE[2])))


# ---------------------------------------------------------------- scene setup: logo texture + materials
def setup(outdir):
    """Empty scene, logo texture file in outdir, the four materials."""
    os.makedirs(outdir, exist_ok=True)
    logo = os.path.join(outdir, "T_ChaserLogo.png")
    N = 128
    yy, xx = np.mgrid[0:N, 0:N]
    px = (xx + 0.5) / N * 2 - 1
    py = (yy + 0.5) / N * 2 - 1

    def raw(hx):
        return np.array([int(hx[i:i + 2], 16) / 255 for i in (1, 3, 5)])
    plate = raw(HEX["logo_plate"])
    dark = raw(HEX["gunmetal"])
    img = np.ones((N, N, 3)) * plate
    border = (np.abs(px) > 0.90) | (np.abs(py) > 0.90)                                    # dark frame around the plate
    img[border] = dark
    hexm = (np.abs(py) < 0.80) & (np.abs(px) * 0.866 + np.abs(py) * 0.5 < 0.76)           # large dark shield (hexagon)
    img[hexm] = dark
    star = (np.abs(px) ** 0.65 + np.abs(py) ** 0.65) < 0.46 ** 0.65                       # four-point star glyph, plate yellow
    img[star] = plate
    rgba = np.ones((N, N, 4))
    rgba[:, :, :3] = img
    rgba = rgba[::-1]                                                                      # image row 0 is the bottom in Blender
    timg = bpy.data.images.new("T_ChaserLogo", N, N, alpha=False)
    timg.pixels = rgba.flatten().tolist()
    timg.filepath_raw = logo
    timg.file_format = "PNG"
    timg.save()
    bpy.ops.wm.read_factory_settings(use_empty=True)
    timg = bpy.data.images.load(logo)
    timg.colorspace_settings.name = "sRGB"
    MATS.clear()
    for n in ORDER:
        m = bpy.data.materials.new(n)
        c = srgb(HEX[n])
        m.diffuse_color = c
        m.use_nodes = True
        b = m.node_tree.nodes["Principled BSDF"]
        b.inputs["Base Color"].default_value = c
        b.inputs["Roughness"].default_value = 0.8
        if n == "eye_red":
            b.inputs["Emission Color"].default_value = c
            b.inputs["Emission Strength"].default_value = 4.0
        if n == "logo_plate":
            tx = m.node_tree.nodes.new("ShaderNodeTexImage")
            tx.image = timg
            tx.interpolation = "Linear"
            m.node_tree.links.new(tx.outputs["Color"], b.inputs["Base Color"])
        MATS[n] = m


# ---------------------------------------------------------------- part builder
class Part:
    def __init__(s, name, pivot, parent=None, sy=1.0):
        s.name, s.pivot, s.parent, s.sy = name, Vector(pivot), parent, sy
        s.bm = bmesh.new()
        s.post = None        # optional hook post(bm, uv0) run in finish(); returns the decal face or None

    def P(s, x, y, z):                      # y is the lateral distance from the centre line, mirrored by sy
        return s.bm.verts.new((x, y * s.sy, z))

    def quad(s, vs, mat):
        f = s.bm.faces.new(vs)
        f.material_index = MI[mat]
        return f

    def loft(s, rings, mat, caps=True):
        for a, b in zip(rings[:-1], rings[1:]):
            n = len(a)
            for k in range(n):
                s.quad((a[k], a[(k + 1) % n], b[(k + 1) % n], b[k]), mat)
        if caps:
            s.quad(rings[0][::-1], mat)
            s.quad(rings[-1], mat)

    def abox(s, x0, x1, y0, y1, z0, z1, mat):
        def ring(z):
            return [s.P(x0, y0, z), s.P(x1, y0, z), s.P(x1, y1, z), s.P(x0, y1, z)]
        s.loft([ring(z0), ring(z1)], mat)

    def prism(s, p0, p1, w0, d0, w1, d1, mat, ch=0.28, bulge=0.07, at=0.4, simple=False):
        """Faceted box between two points (x, y, z with y lateral): octagonal sections w x d (full sizes, cm) at p0, near p0 (bulge), p1."""
        a0 = Vector((p0[0], p0[1] * s.sy, p0[2]))
        a1 = Vector((p1[0], p1[1] * s.sy, p1[2]))
        ax = (a1 - a0).normalized()
        lat = (Vector((0, 1, 0)) - ax * ax.y).normalized()
        dep = ax.cross(lat).normalized()
        wm, dm = max(w0, w1) * (1 + bulge), max(d0, d1) * (1 + bulge)
        rings = []
        for t, w, d in (((0.0, w0, d0), (1.0, w1, d1)) if simple else ((0.0, w0, d0), (at, wm, dm), (1.0, w1, d1))):
            c = a0 + (a1 - a0) * t
            hw, hd = w / 2, d / 2
            cc = ch * min(hw, hd)
            pts = [(hw, -hd + cc), (hw, hd - cc), (hw - cc, hd), (-hw + cc, hd), (-hw, hd - cc), (-hw, -hd + cc), (-hw + cc, -hd), (hw - cc, -hd)]
            rings.append([s.bm.verts.new(c + lat * u + dep * v) for u, v in pts])
        s.loft(rings, mat)

    def secs(s, rows, mat, ch=0.25):
        """Faceted solid from horizontal octagonal sections (z, x centre, y centre (lateral, outward), width, depth); the shin, the arms, the palm."""
        rings = []
        for z, xc, yc, w, d in rows:
            hw, hd = w / 2, d / 2
            cc = ch * min(hw, hd)
            pts = [(hw, -hd + cc), (hw, hd - cc), (hw - cc, hd), (-hw + cc, hd), (-hw, hd - cc), (-hw, -hd + cc), (-hw + cc, -hd), (hw - cc, -hd)]
            rings.append([s.P(xc + v, yc + u, z) for u, v in pts])
        s.loft(rings, mat)

    def oct_y(s, cx, cy, cz, r, length, mat):
        """Octagonal prism along the lateral axis (the knee prism)."""
        rings = []
        for yy_ in (cy - length / 2, cy + length / 2):
            rings.append([s.P(cx + r * math.cos(math.radians(22.5 + 45 * k)), yy_, cz + r * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)])
        s.loft(rings, mat)

    def profile(s, pts, y0, y1, mat):
        """Plain extrusion of an (x, z) profile between two lateral values."""
        s.loft([[s.P(x, y0, z) for x, z in pts], [s.P(x, y1, z) for x, z in pts]], mat)


def finish(p):
    """Normals, UV0 (decal only), UV1 LightMap (one island per face), flat shading, local coordinates (pivot subtracted), the Blender object at its world pivot."""
    bm = p.bm
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    uv0 = bm.loops.layers.uv.new("UVMap")
    uv1 = bm.loops.layers.uv.new("LightMap")
    decal_face = p.post(bm, uv0) if p.post else None
    bm.faces.ensure_lookup_table()
    bm.faces.index_update()
    bm.normal_update()
    island = {}
    for f in bm.faces:
        nn = f.normal.normalized()
        if abs(nn.z) > 0.99:
            t, s_ = Vector((1, 0, 0)), Vector((0, 1, 0))
        else:
            t = Vector((-nn.y, nn.x, 0)).normalized()
            s_ = nn.cross(t).normalized()
        loc = [(l.vert.co.dot(t), l.vert.co.dot(s_)) for l in f.loops]
        mnx, mny = min(a for a, b in loc), min(b for a, b in loc)
        loc = [(a - mnx, b - mny) for a, b in loc]
        island[f.index] = (loc, max(a for a, b in loc), max(b for a, b in loc))
    LM, PAD = 256, 3.0

    def pack(scale):
        x = y = PAD
        rowh = 0.0
        out = {}
        for fi, (loc, w, hh) in sorted(island.items(), key=lambda kv: -kv[1][2]):
            pw, ph = w * scale, hh * scale
            if x + pw + PAD > LM:
                x = PAD
                y += rowh + PAD
                rowh = 0.0
            if x + pw + PAD > LM or y + ph + PAD > LM:
                return None
            out[fi] = (x, y)
            x += pw + PAD
            rowh = max(rowh, ph)
        return out
    lo_s, hi_s = 0.01, 20.0
    for _ in range(40):
        mid = (lo_s + hi_s) / 2
        if pack(mid):
            lo_s = mid
        else:
            hi_s = mid
    placed = pack(lo_s)
    decal_idx = decal_face.index if decal_face is not None else -1
    for f in bm.faces:
        x, y = placed[f.index]
        for l, (lx, ly) in zip(f.loops, island[f.index][0]):
            l[uv1].uv = ((x + lx * lo_s) / LM, (y + ly * lo_s) / LM)
            if f.index != decal_idx:
                l[uv0].uv = (0.0, 0.0)
    for f in bm.faces:
        f.smooth = False
    bmesh.ops.translate(bm, vec=-p.pivot, verts=list(bm.verts))
    mesh = bpy.data.meshes.new(p.name)
    bm.to_mesh(mesh)
    bm.free()
    for m in ORDER:
        mesh.materials.append(MATS[m])
    for q in mesh.polygons:
        q.use_smooth = False
    ob = bpy.data.objects.new(p.name, mesh)
    bpy.context.scene.collection.objects.link(ob)
    ob.location = p.pivot            # world pivot; assemble.py parents the object and turns it into a local offset
    PIVOT_W[p.name] = p.pivot.copy()
    PARENT_NAME[p.name] = p.parent
    return ob


# ---------------------------------------------------------------- checks and part report
def lm_check(mesh, LM=256):
    u1 = np.array([l.uv[:] for l in mesh.uv_layers["LightMap"].data])
    boxes = []
    for q in mesh.polygons:
        a = u1[q.loop_start:q.loop_start + q.loop_total] * LM
        boxes.append((a[:, 0].min(), a[:, 1].min(), a[:, 0].max(), a[:, 1].max()))
    min_gap, ov = 1e9, 0
    for i in range(len(boxes)):
        for j in range(i + 1, len(boxes)):
            A, B = boxes[i], boxes[j]
            g = max(max(B[0] - A[2], A[0] - B[2]), max(B[1] - A[3], A[1] - B[3]))
            min_gap = min(min_gap, g)
            if g < 0:
                ov += 1
    border = min(min(b[0], b[1], LM - b[2], LM - b[3]) for b in boxes)
    ok = ov == 0 and min_gap >= 2.0 and border >= 2.0 and u1.min() >= 0 and u1.max() <= 1
    return {"faces": len(boxes), "overlapping_face_pairs": ov, "bounds_min": round(float(u1.min()), 5), "bounds_max": round(float(u1.max()), 5),
            "min_gap_px_at_256": round(float(min_gap), 3), "min_border_px_at_256": round(float(border), 3), "required_margin_px": 2, "pass": bool(ok)}


def object_info(ob):
    """Per-object numbers of the report (world values; the object has no rotation or scale)."""
    me = ob.data
    tris = sum(len(q.vertices) - 2 for q in me.polygons)
    bb = [ob.matrix_world @ Vector(c) for c in ob.bound_box]
    lo = Vector((min(v.x for v in bb), min(v.y for v in bb), min(v.z for v in bb)))
    hi = Vector((max(v.x for v in bb), max(v.y for v in bb), max(v.z for v in bb)))
    pv = ob.matrix_world.translation
    sect = [ob.matrix_world @ v.co for v in me.vertices if abs((ob.matrix_world @ v.co).z - pv.z) <= 1.0]
    att = None
    if sect:
        att = {"plane_z_cm": round(pv.z, 2), "slab_cm": 1.0, "min_xy_cm": [round(min(v.x for v in sect), 2), round(min(v.y for v in sect), 2)],
               "max_xy_cm": [round(max(v.x for v in sect), 2), round(max(v.y for v in sect), 2)]}
    return {"tris": tris, "bounds_min_cm": [round(c, 2) for c in lo], "bounds_max_cm": [round(c, 2) for c in hi],
            "pivot_world_cm": [round(c, 2) for c in pv], "parent": PARENT_NAME.get(ob.name),
            "uv_layers": [u.name for u in me.uv_layers], "lightmap": lm_check(me), "attachment_section": att,
            "material_slots": [m.name if m else None for m in me.materials]}


def write_part_report(group, objs, parts_dir):
    """Write <parts_dir>/<group>.report.json for the objects of a part group (keeps an existing fidelity object)."""
    os.makedirs(parts_dir, exist_ok=True)
    path = os.path.join(parts_dir, group + ".report.json")
    rep = {"part": group, "route": "blender", "script": "Tools/art/blender/chaser/part_%s.py" % group, "units": "cm",
           "objects": {o.name: object_info(o) for o in objs}}
    rep["tris"] = sum(v["tris"] for v in rep["objects"].values())
    if os.path.exists(path):
        try:
            old = json.load(open(path, encoding="utf-8"))
            if "fidelity" in old:
                rep["fidelity"] = old["fidelity"]
        except Exception:
            pass
    with open(path, "w", encoding="utf-8") as f:
        json.dump(rep, f, indent=2)
    return rep
