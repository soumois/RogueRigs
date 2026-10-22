"""task-288-01: compare the L/R centre-offset difference of the debris pieces with the asymmetry of the source parts mesh and the game mesh. Writes JSON to --out. Headless Blender."""
import bpy, sys, os, json
from mathutils import Vector
a = sys.argv[sys.argv.index("--") + 1:]
o = dict(zip(a[::2], a[1::2]))
M = "Art/Meshes/heavy-meshy/"


def bbox(vs):
    lo = Vector((min(v.x for v in vs), min(v.y for v in vs), min(v.z for v in vs)))
    hi = Vector((max(v.x for v in vs), max(v.y for v in vs), max(v.z for v in vs)))
    return (lo + hi) / 2, hi - lo


def tris(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.abspath(M + "heavy_meshy_parts.fbx"), colors_type="LINEAR")
parts = {x.name: x for x in bpy.data.objects if x.type == "MESH"}
bpy.context.view_layer.update()
pv, ptri = {}, {}
for n, ob in parts.items():
    pv[n] = [ob.matrix_world @ v.co for v in ob.data.vertices]; ptri[n] = tris(ob.data)
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=os.path.abspath(M + "heavy_meshy_game.fbx"), colors_type="LINEAR")
bpy.context.view_layer.update()
gv = [x.matrix_world @ v.co for x in bpy.data.objects if x.type == "MESH" for v in x.data.vertices]
deb = json.load(open(os.path.abspath(o["--debris"])))
rp = json.load(open(os.path.abspath(M + "heavy_meshy_parts.report.json")))["triangles"]["per_part"]
srcmap = {p["name"].replace("SM_HeavyDebris_", ""): p for p in deb["pieces"]}


def unreal(c):  # same convention as debris_pieces.json: (x, -y, z)
    return [c.x, -c.y, c.z]


def game_zone(piece):
    # game-mesh vertices inside the piece bbox (from the debris json, Blender world = (x, -uy, z)), grown 1 cm: only a sample of the source, not a cut
    p = srcmap[piece]; c = p["centre_offset_cm_unreal"]; s = p["size_cm"]
    cb = Vector((c[0], -c[1], c[2]))
    inside = [v for v in gv if all(abs(v[i] - cb[i]) <= s[i] / 2 + 1.0 for i in range(3))]
    return bbox(inside) if inside else (Vector((0, 0, 0)), Vector((0, 0, 0)))


def d(l, r):  # mirror difference of two Unreal-axis centres: x, -y mirrored, z
    return [round(abs(l[0] - r[0]), 2), round(abs(l[1] + r[1]), 2), round(abs(l[2] - r[2]), 2)]


out = dict(note=("debris_diff = |L - mirror(R)| of centre_offset_cm_unreal (x, y mirrored, z). parts_diff = same on the bbox centre of the source parts (rest pose, heavy_meshy_parts.fbx, the same source parts joined per piece). "
                 "game_zone_diff = same on the bbox centre of the game-mesh vertices inside each piece's bbox grown 1 cm (heavy_meshy_game.fbx is not split into parts, so this is a sample of the source, not an exact cut). "
                 "No geometry or offset was altered."), pairs={}, face_check={})
groups = {"UpperArm": ["UpperArm"], "Forearm": ["ForearmHand"], "Arm": ["UpperArm", "ForearmHand"], "Thigh": ["Thigh"], "LowerLeg": ["ShinFoot"], "Leg": ["Thigh", "ShinFoot"]}


def src_centre(side, names):
    vs = []
    for n in names:
        vs += pv[n + "_" + side]
    c, s = bbox(vs)
    return unreal(c), [round(t, 2) for t in s]


for g, names in groups.items():
    L, R = g + "_L", g + "_R"
    sl, ssl = src_centre("L", names); sr, ssr = src_centre("R", names)
    zl, zr = game_zone(L), game_zone(R)
    out["pairs"][g] = dict(
        debris_diff=d(srcmap[L]["centre_offset_cm_unreal"], srcmap[R]["centre_offset_cm_unreal"]),
        parts_diff=d(sl, sr),
        game_zone_diff=d(unreal(zl[0]), unreal(zr[0])),
        size_cm_L=srcmap[L]["size_cm"], size_cm_R=srcmap[R]["size_cm"], parts_size_cm_L=ssl, parts_size_cm_R=ssr)
for k, p in srcmap.items():
    names = p["source_parts"]
    exp_import = sum(ptri[n] for n in names)
    exp_report = sum(rp[n]["tris"] for n in names)
    out["face_check"][k] = dict(piece_tris=p["triangles"], sum_parts_tris_reimport=exp_import, sum_parts_tris_report=exp_report, delta_vs_report=p["triangles"] - exp_report)
json.dump(out, open(os.path.abspath(o["--out"]), "w"), indent=1)
