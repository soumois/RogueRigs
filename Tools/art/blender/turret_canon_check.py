"""Reimport Art/Meshes/turret/SM_Turret.fbx in a clean Blender scene and print/append the checks.
Usage: blender -b --factory-startup -P Tools/art/blender/turret_canon_check.py -- <fbx> <report.json>"""
import bpy, sys, json, numpy as np
fbx, rep = sys.argv[sys.argv.index("--") + 1:][:2]
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=fbx)
o = [o for o in bpy.context.scene.objects if o.type == "MESH"][0]
me = o.data
n = len(me.vertices)
a = np.empty(n * 3); me.vertices.foreach_get("co", a); a = a.reshape(-1, 3) * 100 * np.array(o.scale)  # cm
mn, mx = a.min(0), a.max(0)
mi = np.empty(len(me.polygons), int); me.polygons.foreach_get("material_index", mi)
me.calc_loop_triangles()
tri_mat = np.array([mi[t.polygon_index] for t in me.loop_triangles])
body_vs = set(); canon_vs = set()
for p in me.polygons:
    (body_vs if mi[p.index] == 0 else canon_vs).update(p.vertices)
res = {
    "object_scale": [round(v, 4) for v in o.scale],
    "dimensions_cm": [round(float(v), 2) for v in (mx - mn)],
    "bbox_min_cm": [round(float(v), 2) for v in mn], "bbox_max_cm": [round(float(v), 2) for v in mx],
    "pivot_bottom_centre": bool(abs((mn[0] + mx[0]) / 2) < 0.05 and abs((mn[1] + mx[1]) / 2) < 0.05 and abs(mn[2]) < 0.05),
    "materials": [m.name for m in me.materials], "uv_layers": [u.name for u in me.uv_layers],
    "tris_total": int(len(me.loop_triangles)), "tris_body": int((tri_mat == 0).sum()), "tris_canon": int((tri_mat != 0).sum()),
    "vertices_total": n, "vertices_body": len(body_vs), "vertices_canon_only": len(canon_vs - body_vs),
    "faces_flat_canon": int(sum(1 for p in me.polygons if mi[p.index] != 0 and not p.use_smooth)),
    "faces_canon": int((mi != 0).sum()),
}
print("CHECK", json.dumps(res))
r = json.load(open(rep, encoding="utf-8"))
r["reimport_check"] = res
json.dump(r, open(rep, "w", encoding="utf-8"), indent=2)
