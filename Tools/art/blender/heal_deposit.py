"""Heal deposit formations (cracked mound, flat inlaid slab, half-buried cluster), Blender route, task-234-07.
Faceted grey rocks + red crystals/veins, no stone pad, base sunk 5 cm under z 0, pivot at footprint centre,
two material slots `rock` (0) and `crystal` (1), flat shading, UVMap only. 1 Blender unit = 1 cm.
Run: blender -b --factory-startup -P heal_deposit.py -- --outdir Art/Meshes/ore-deposit"""
import bpy, bmesh, math, json, os, sys, random
from mathutils import Matrix, Vector

SINK = 5.0
FOOT = 134.0
ROCK, CRYS = 0, 1

class B:
    def __init__(s): s.v, s.f, s.m = [], [], []
    def add(s, verts, faces, mat):
        o = len(s.v); s.v += [tuple(p) for p in verts]
        s.f += [tuple(i + o for i in fc) for fc in faces]; s.m += [mat] * len(faces)

def rot2(x, y, a):
    c, sn = math.cos(a), math.sin(a); return x * c - y * sn, x * sn + y * c

def boulder(b, rng, cx, cy, rx, ry, h, rot=0.0, sub=2):
    """faceted rounded rock, top at h, flat bottom at -SINK"""
    bm = bmesh.new(); bmesh.ops.create_icosphere(bm, subdivisions=sub, radius=1.0)
    zc, sz = 0.35 * h, 0.65 * h
    vs = []
    for v in bm.verts:
        j = rng.uniform(0.86, 1.14); p = v.co * j
        x, y = rot2(p.x * rx, p.y * ry, rot)
        vs.append((cx + x, cy + y, max(-SINK, zc + p.z * sz)))
    idx = {v: i for i, v in enumerate(bm.verts)}
    fs = [tuple(idx[v] for v in f.verts) for f in bm.faces]
    bm.free(); b.add(vs, fs, ROCK)

def plate(b, rng, cx, cy, rx, ry, h, rot=0.0, n=8):
    """low flat rock plate: bottom ring, edge ring, smaller jittered top ring, top centre"""
    ph = rng.uniform(0, 1)
    def ring(k, z):
        pts = []
        for i in range(n):
            a = 2 * math.pi * i / n + ph; r = k * rng.uniform(0.88, 1.12)
            x, y = rot2(rx * r * math.cos(a), ry * r * math.sin(a), rot)
            pts.append((cx + x, cy + y, z))
        return pts
    v = ring(1.0, -SINK) + ring(1.0, h * 0.55) + ring(0.8, h)
    v.append((cx + rng.uniform(-2, 2), cy + rng.uniform(-2, 2), h + rng.uniform(0, 1.5)))
    f = []
    for i in range(n):
        j = (i + 1) % n
        f.append((i, j, n + j, n + i)); f.append((n + i, n + j, 2 * n + j, 2 * n + i)); f.append((2 * n + i, 2 * n + j, 3 * n))
    f.append(tuple(reversed(range(n))))
    b.add(v, f, ROCK)

def crystal(b, rng, bx, by, h, r, lean=0, ld=0, mat=CRYS):
    rotp = rng.uniform(0, math.pi / 3); L = h + SINK
    def ring(rad, z):
        return [(rad * rng.uniform(.9, 1.1) * math.cos(rotp + k * math.pi / 3), rad * rng.uniform(.9, 1.1) * math.sin(rotp + k * math.pi / 3), z) for k in range(6)]
    v = ring(r, 0) + ring(r * .86, L * .78) + [(rng.uniform(-.15, .15) * r, rng.uniform(-.15, .15) * r, L), (0, 0, 0)]
    f = []
    for k in range(6):
        n = (k + 1) % 6
        f += [(k, n, 6 + n, 6 + k), (6 + k, 6 + n, 12), (n, k, 13)]
    m = Matrix.Rotation(math.radians(ld), 4, "Z") @ Matrix.Rotation(math.radians(lean), 4, "Y")
    pts = [m @ Vector(p) for p in v]
    lift = -SINK - min(pts[i].z for i in (0, 1, 2, 3, 4, 5, 13))
    b.add([Vector((bx, by, lift)) + q for q in pts], f, mat)

def vein(b, pts, w=5.0, top=2.5, bot=-SINK):
    """thin raised strip along a polyline: one closed box per segment"""
    for (x0, y0), (x1, y1) in zip(pts[:-1], pts[1:]):
        d = Vector((x1 - x0, y1 - y0)); d.normalize(); nx, ny = -d.y * w / 2, d.x * w / 2
        ex, ey = d.x * w / 2, d.y * w / 2
        a = [(x0 - ex + nx, y0 - ey + ny), (x1 + ex + nx, y1 + ey + ny), (x1 + ex - nx, y1 + ey - ny), (x0 - ex - nx, y0 - ey - ny)]
        v = [(x, y, bot) for x, y in a] + [(x, y, top) for x, y in a]
        b.add(v, [(0, 1, 2, 3), (4, 7, 6, 5), (0, 4, 5, 1), (1, 5, 6, 2), (2, 6, 7, 3), (3, 7, 4, 0)], CRYS)

def pocket(b, rng, cx, cy, R, ncry, hmax):
    """rock rim ring around a crystal-filled hollow"""
    for i in range(7):
        a = 2 * math.pi * i / 7 + rng.uniform(-.1, .1)
        boulder(b, rng, cx + R * math.cos(a), cy + R * math.sin(a), R * .5, R * .42, rng.uniform(6, 9), a, 1)
    for i in range(ncry):
        a = rng.uniform(0, 6.28); d = rng.uniform(0, R * .5)
        crystal(b, rng, cx + d * math.cos(a), cy + d * math.sin(a), rng.uniform(hmax * .4, hmax), R * rng.uniform(.14, .2), rng.uniform(0, 28), math.degrees(a))

def mound(b, rng):
    boulder(b, rng, -4, 2, 34, 26, 56, 0.5, 3)
    boulder(b, rng, 36, -10, 20, 17, 40, 0.3)
    boulder(b, rng, -34, -22, 17, 14, 28, -0.4)
    boulder(b, rng, 8, -42, 24, 15, 24, 0.1)
    boulder(b, rng, -40, 34, 18, 14, 22, 0.8)
    boulder(b, rng, 24, 44, 16, 12, 18, 0.2)
    boulder(b, rng, -52, -4, 8, 8, 12, 0, 1)
    vein(b, [(10, -14), (14, -26), (22, -34)], 6); vein(b, [(14, -26), (4, -30), (-16, -24)], 8)
    vein(b, [(-20, 20), (-22, 30), (-12, 44), (6, 52)], 6); vein(b, [(22, 28), (30, 14), (24, 4)], 8)
    crystal(b, rng, 22, 26, 70, 14, 22, 20)
    crystal(b, rng, 4, 30, 50, 10, 18, 80)
    crystal(b, rng, -40, 8, 40, 11, 22, 170)
    crystal(b, rng, -16, -30, 44, 10, 10, -100)
    crystal(b, rng, 44, 24, 30, 7, 25, 10)
    crystal(b, rng, 32, -34, 18, 5, 25, -30)
    crystal(b, rng, -6, -46, 16, 5, 20, -90)

def flat(b, rng):
    plate(b, rng, -30, 34, 28, 20, 9, 0.3)
    plate(b, rng, 30, 36, 26, 17, 8, -0.3)
    plate(b, rng, 38, -4, 19, 15, 7, 0.2)
    plate(b, rng, 6, -40, 26, 17, 8, 0.1)
    plate(b, rng, -54, -4, 8, 22, 6, 0.1, 7)
    plate(b, rng, 54, -40, 8, 10, 6, 0.4, 7)
    pocket(b, rng, -34, -20, 21, 14, 28)
    pocket(b, rng, 46, -22, 10, 7, 14)
    vein(b, [(2, 56), (2, 40), (2, 12), (-4, -4)], 8)
    vein(b, [(2, 12), (22, 4), (36, 10)], 7)
    vein(b, [(-4, -4), (-14, -18)], 7)
    vein(b, [(-4, -4), (2, -18), (-8, -32)], 7)
    vein(b, [(2, 40), (-12, 50)], 6); vein(b, [(2, 12), (-8, 18)], 6)
    vein(b, [(2, 12), (6, 22), (14, 20)], 6)
    crystal(b, rng, 2, 52, 28, 7, 8, 90); crystal(b, rng, -6, 50, 15, 4.5, 14, 160)
    crystal(b, rng, -14, 56, 12, 4, 20, 140)
    crystal(b, rng, -10, -42, 12, 4, 25, 200); crystal(b, rng, 14, -4, 7, 2.5, 20, 0)
    crystal(b, rng, -2, -22, 8, 2.8, 25, 50)

def half(b, rng):
    boulder(b, rng, -6, -22, 31, 28, 46, 0.2, 3)
    boulder(b, rng, -40, 30, 20, 15, 28, 0.5)
    boulder(b, rng, -10, 44, 17, 12, 22, 0.1)
    boulder(b, rng, 30, 38, 17, 13, 26, -0.4)
    boulder(b, rng, 40, -42, 18, 11, 22, 0.5)
    boulder(b, rng, -54, -30, 7, 6, 11, 0)
    boulder(b, rng, 52, 8, 8, 10, 14, 0)
    vein(b, [(28, -4), (22, 14), (4, 26), (-14, 22)], 8)
    vein(b, [(22, 14), (36, -10), (30, -30)], 8)
    vein(b, [(-14, 22), (-30, 6), (-38, -10)], 8)
    crystal(b, rng, 14, 24, 68, 14, 20, 30)
    crystal(b, rng, -34, 8, 44, 12, 28, 180)
    crystal(b, rng, 36, 0, 46, 11, 24, -20)
    crystal(b, rng, 44, 20, 28, 7, 24, 20)
    crystal(b, rng, -22, 22, 22, 6, 22, 120)
    crystal(b, rng, 26, -28, 16, 5, 22, -60)

VARIANTS = {"01": ("mound", mound), "02": ("flat", flat), "03": ("half", half)}

def build(name, fn, seed):
    rng = random.Random(seed); b = B(); fn(b, rng)
    me = bpy.data.meshes.new(name); me.from_pydata(b.v, [], b.f); me.update()
    bm = bmesh.new(); bm.from_mesh(me)
    bm.faces.ensure_lookup_table()
    for fc, mi in zip(bm.faces, b.m): fc.material_index = mi
    bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
    bmesh.ops.triangulate(bm, faces=bm.faces)
    bm.to_mesh(me); bm.free()
    for p in me.polygons: p.use_smooth = False
    uv = me.uv_layers.new(name="UVMap")
    for l in me.loops:
        x, y, _ = me.vertices[l.vertex_index].co
        uv.data[l.index].uv = (x / FOOT + 0.5, y / FOOT + 0.5)
    for n, c in (("rock", (0.45, 0.45, 0.48, 1)), ("crystal", (0.9, 0.08, 0.08, 1))):
        mat = bpy.data.materials.get(n) or bpy.data.materials.new(n); mat.diffuse_color = c; me.materials.append(mat)
    ob = bpy.data.objects.new(name, me); bpy.context.collection.objects.link(ob)
    return ob

def main():
    argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
    outdir = argv[argv.index("--outdir") + 1] if "--outdir" in argv else "Art/Meshes/ore-deposit"
    os.makedirs(outdir, exist_ok=True)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    for key, (label, fn) in VARIANTS.items():
        name = "SM_HealDeposit_" + key
        ob = build(name, fn, int(key) * 6151)
        bpy.context.view_layer.objects.active = ob; ob.select_set(True)
        bb = [Vector(c) for c in ob.bound_box]
        mn = [min(c[i] for c in bb) for i in range(3)]; mx = [max(c[i] for c in bb) for i in range(3)]
        per = {}
        for p in ob.data.polygons: per[p.material_index] = per.get(p.material_index, 0) + 1
        rep = {"mesh": name, "variant": label, "tris": len(ob.data.polygons),
               "tris_per_slot": {ob.data.materials[k].name: v for k, v in per.items()},
               "bounds_min_cm": [round(x, 2) for x in mn], "bounds_max_cm": [round(x, 2) for x in mx],
               "dimensions_cm": [round(mx[i] - mn[i], 2) for i in range(3)], "materials": [m.name for m in ob.data.materials],
               "uv_layers": [u.name for u in ob.data.uv_layers], "warnings": []}
        if max(abs(mn[0]), abs(mx[0]), abs(mn[1]), abs(mx[1])) > FOOT / 2: rep["warnings"].append("outside 134 footprint")
        rep["inside_footprint"] = not rep["warnings"]
        bpy.ops.export_scene.fbx(filepath=os.path.join(outdir, name + ".fbx"), use_selection=True, object_types={"MESH"},
            apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="STRIP", embed_textures=False)
        with open(os.path.join(outdir, name + ".report.json"), "w") as fh: json.dump(rep, fh, indent=2)
        print(json.dumps(rep)); ob.select_set(False)
main()
