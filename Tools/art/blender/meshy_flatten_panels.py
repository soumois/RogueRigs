"""Flatten bulged hard-surface panels of a Meshy mesh (task-281). Headless, no Meshy call:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/meshy_flatten_panels.py -- <in.fbx> <out.fbx> <report.json> [--ang 20] [--dist 1.8] [--sharp 30] [--min-area 80] [--max-move 2.0]
Finds planar regions (near-coplanar face groups bounded by sharp edges, grown from the biggest faces), fits a plane per region and
projects its vertices onto it (vertices shared by several regions go to the intersection of their planes). Curved surfaces are never
touched: a vertex that no region owns does not move. Topology, UVs, material and the object are unchanged. Lengths in cm in the report.
A region is dropped when a vertex would move more than --max-move cm or a face would flip; the report holds the final regions only."""
import bpy, bmesh, json, math, os, sys
import numpy as np

argv = sys.argv[sys.argv.index("--") + 1:]
pos = [a for a in argv if not a.startswith("--")]
opt = {"ang": 20.0, "dist": 1.8, "sharp": 30.0, "min-area": 80.0, "max-move": 2.0}
for i, a in enumerate(argv):
    if a.startswith("--") and a[2:] in opt:
        opt[a[2:]] = float(argv[i + 1]); pos.remove(argv[i + 1])
IN, OUT, REP = (os.path.abspath(p) for p in pos[:3])
M2CM = 100.0
MAXD = opt["max-move"] * 0.9   # prune distance before projection, margin for shared vertices

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=IN)
obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
me = obj.data
nv, nf = len(me.vertices), len(me.polygons)
tris_before = sum(len(p.vertices) - 2 for p in me.polygons)
V0 = np.empty(nv * 3); me.vertices.foreach_get("co", V0); V0 = V0.reshape(nv, 3) * M2CM   # cm
uv0 = np.array([l.uv[:] for l in me.uv_layers[0].data]) if me.uv_layers else np.zeros((0, 2))
faces = [list(p.vertices) for p in me.polygons]
fn0 = np.array([p.normal[:] for p in me.polygons])
fa = np.array([p.area for p in me.polygons]) * M2CM * M2CM
fc = np.array([V0[f].mean(axis=0) for f in faces])

# vertices at the same position are one site (normal or UV splits)
key = {}
site = np.empty(nv, dtype=int)
for i, c in enumerate(np.round(V0, 3)):
    site[i] = key.setdefault(tuple(c), len(key))
nsite = len(key)

# face adjacency over shared edges
edge_faces = {}
for fi, p in enumerate(me.polygons):
    for e in p.edge_keys:
        edge_faces.setdefault(tuple(sorted((site[e[0]], site[e[1]]))), []).append(fi)
adj = [[] for _ in range(nf)]
for fl in edge_faces.values():
    if len(fl) == 2:
        adj[fl[0]].append(fl[1]); adj[fl[1]].append(fl[0])

cos_ang = math.cos(math.radians(opt["ang"])); cos_sharp = math.cos(math.radians(opt["sharp"]))

def fit_plane(fidx):
    pts = np.unique(np.array([site[v] for f in fidx for v in faces[f]]))
    P = np.array([V0[np.nonzero(site == s)[0][0]] for s in pts])
    c = P.mean(axis=0)
    w, vec = np.linalg.eigh(np.cov((P - c).T))
    n = vec[:, 0]
    if np.dot(n, fn0[list(fidx)].T @ fa[list(fidx)]) < 0:
        n = -n
    return c, n, pts, P

# region growing from the biggest faces
region = -np.ones(nf, dtype=int)
regions = []
for seed in np.argsort(-fa):
    if region[seed] >= 0:
        continue
    rid = len(regions)
    members = [seed]; region[seed] = rid
    sn = fn0[seed] * fa[seed]; sc = fc[seed] * fa[seed]; sa = fa[seed]
    queue = [seed]
    while queue:
        f = queue.pop()
        for g in adj[f]:
            if region[g] >= 0:
                continue
            n = sn / np.linalg.norm(sn); c = sc / sa
            if np.dot(fn0[g], n) < cos_ang or np.dot(fn0[g], fn0[f]) < cos_sharp:
                continue
            if max(abs(np.dot(V0[v] - c, n)) for v in faces[g]) > opt["dist"]:
                continue
            region[g] = rid; members.append(g); queue.append(g)
            sn += fn0[g] * fa[g]; sc += fc[g] * fa[g]; sa += fa[g]
    regions.append(members)

def prune(members):
    """least-squares plane, drop faces with a vertex too far, keep the biggest connected piece"""
    members = set(members)
    for _ in range(4):
        if len(members) < 4:
            return set(), None
        c, n, _, _ = fit_plane(members)
        keep = {f for f in members if max(abs(np.dot(V0[v] - c, n)) for v in faces[f]) <= MAXD}
        # biggest connected piece
        left, best = set(keep), set()
        while left:
            s = left.pop(); comp = {s}; st = [s]
            while st:
                f = st.pop()
                for g in adj[f]:
                    if g in left:
                        left.discard(g); comp.add(g); st.append(g)
            if sum(fa[list(comp)]) > sum(fa[list(best)] if best else [0]):
                best = comp
        if best == members:
            break
        members = best
    return members, fit_plane(members)

regs = []
for m in regions:
    if sum(fa[m]) < opt["min-area"]:
        continue
    mm, pl = prune(m)
    if mm and sum(fa[list(mm)]) >= opt["min-area"]:
        regs.append(mm)

def solve(regs):
    """project vertex sites; return new positions per vertex and the plane list"""
    planes = [fit_plane(r) for r in regs]
    owners = {}
    for ri, r in enumerate(regs):
        for f in r:
            for v in faces[f]:
                owners.setdefault(int(site[v]), set()).add(ri)
    site_pos = {}
    first = {}
    for i in range(nv):
        first.setdefault(int(site[i]), i)
    newpos = {}
    for s, rs in owners.items():
        p = V0[first[s]].copy()
        rs = sorted(rs)
        for _ in range(60):
            for ri in rs:
                c, n, _, _ = planes[ri]
                p = p - np.dot(p - c, n) * n
        newpos[s] = p
    return planes, owners, newpos

# drop regions whose vertices would move too far or flip faces, iterate until stable
for _ in range(40):
    planes, owners, newpos = solve(regs)
    first = {}
    for i in range(nv):
        first.setdefault(int(site[i]), i)
    bad_sites = {s for s, p in newpos.items() if np.linalg.norm(p - V0[first[s]]) > opt["max-move"]}
    # flipped faces: after-normal against before-normal
    V1 = V0.copy()
    for i in range(nv):
        if int(site[i]) in newpos:
            V1[i] = newpos[int(site[i])]
    flipped = set()
    for fi, f in enumerate(faces):
        a, b, c = V1[f[0]], V1[f[1]], V1[f[2]]
        n1 = np.cross(b - a, c - a)
        if np.dot(n1, fn0[fi]) <= 0:
            flipped.add(fi)
    for fi in flipped:
        bad_sites.update(int(site[v]) for v in faces[fi])
    # vertices that cannot sit on all their planes (3+ planes, near-parallel planes): the smallest owner region gives them up
    drop = {}   # region index -> sites
    for s_, rs in owners.items():
        p = newpos[s_]
        if max(abs(np.dot(p - planes[ri][0], planes[ri][1])) for ri in rs) > 0.03:
            small = min(rs, key=lambda ri: sum(fa[list(regs[ri])]))
            drop.setdefault(small, set()).add(s_)
    if not bad_sites and not drop:
        break
    newregs = []
    for ri_, r in enumerate(regs):
        gone = bad_sites | drop.get(ri_, set())
        r2 = {f for f in r if not any(int(site[v]) in gone for v in faces[f])}
        if r2:
            r2, pl = prune(r2)
        if r2 and sum(fa[list(r2)]) >= opt["min-area"]:
            newregs.append(r2)
    regs = newregs
planes, owners, newpos = solve(regs)

first = {}
for i in range(nv):
    first.setdefault(int(site[i]), i)
V1 = V0.copy()
for i in range(nv):
    if int(site[i]) in newpos:
        V1[i] = newpos[int(site[i])]
me.vertices.foreach_set("co", (V1 / M2CM).reshape(-1)); me.update()

# report
out_regions = []
for ri, r in enumerate(regs):
    c, n, pts, P = planes[ri]
    vs = sorted({v for f in r for v in faces[f]})
    before = max(abs(np.dot(V0[v] - c, n)) for v in vs)
    after = max(abs(np.dot(V1[v] - c, n)) for v in vs)
    move = max(np.linalg.norm(V1[v] - V0[v]) for v in vs)
    out_regions.append({"id": ri, "faces": len(r), "verts": len(vs), "area_cm2": round(float(sum(fa[list(r)])), 1),
                        "normal": [round(float(x), 3) for x in n], "centroid_cm": [round(float(x), 1) for x in c],
                        "deviation_before_cm": round(float(before), 3), "deviation_after_cm": round(float(after), 4),
                        "max_move_cm": round(float(move), 3)})
uv1 = np.array([l.uv[:] for l in me.uv_layers[0].data]) if me.uv_layers else np.zeros((0, 2))
fn1 = np.array([p.normal[:] for p in me.polygons])
flips = int(sum(1 for i in range(nf) if np.dot(fn1[i], fn0[i]) <= 0))
moved = np.linalg.norm(V1 - V0, axis=1)
sm = [bool(p.use_smooth) for p in me.polygons]
report = {
    "input": IN.replace("\\", "/"), "output": OUT.replace("\\", "/"),
    "params": opt, "object_name": obj.name,
    "regions_found": len(out_regions), "regions": out_regions,
    "faces_in_regions": int(sum(len(r) for r in regs)), "faces_total": nf,
    "max_deviation_before_cm": max([r["deviation_before_cm"] for r in out_regions] or [0]),
    "max_deviation_after_cm": max([r["deviation_after_cm"] for r in out_regions] or [0]),
    "max_move_cm": round(float(moved.max()), 3), "verts_moved": int((moved > 1e-6).sum()),
    "tris_before": tris_before, "tris_after": sum(len(p.vertices) - 2 for p in me.polygons),
    "verts_before": nv, "verts_after": len(me.vertices),
    "uv_layers": [u.name for u in me.uv_layers], "materials": [m.name for m in me.materials if m],
    "uv0_max_delta": float(np.abs(uv1 - uv0).max()) if len(uv0) else 0.0,
    "flipped_normals": flips, "smooth_faces": int(sum(sm)),
    "dimensions_cm_before": [round(float(x), 2) for x in (V0.max(axis=0) - V0.min(axis=0))],
    "dimensions_cm_after": [round(float(x), 2) for x in (V1.max(axis=0) - V1.min(axis=0))],
    "warnings": [],
}
if report["max_deviation_after_cm"] >= 0.1: report["warnings"].append("deviation after >= 0.1 cm")
if report["max_move_cm"] > opt["max-move"]: report["warnings"].append("max move above limit")
if flips: report["warnings"].append("flipped normals")
if not out_regions: report["warnings"].append("no planar region found")

os.makedirs(os.path.dirname(OUT), exist_ok=True)
bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
bpy.context.view_layer.objects.active = obj
bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                         mesh_smooth_type="OFF", add_leaf_bones=False, path_mode="AUTO", embed_textures=False)
with open(REP, "w", encoding="utf-8") as fh:
    json.dump(report, fh, indent=2)
print(json.dumps({k: v for k, v in report.items() if k != "regions"}))
