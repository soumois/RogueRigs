"""Bonus chest cleanup (task-306-01). Blender route, no Meshy call. Headless:
  flatten : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_flatten.py -- flatten [--out Art/Source/chest/clean/chest_flat.fbx] [--budget 2950] [--width 130]
  render  : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_flatten.py -- render <fbx> <out_prefix>   (front, side, back, q, top PNGs, 800 px)
  compare : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_flatten.py -- compare <dir>   (sheet | original | flat, per view, from <dir>/orig_* and <dir>/flat_*)
The Meshy mesh (9336 tris, smooth shaded, noisy triangles) is: 1 planar-region fit at full resolution (meshy_flatten_panels.py), 2 collapse decimation to the
budget, 3 planar-region fit again on the decimated mesh, 4 flat shading, uniform scale to --width cm along X, bottom-centre origin (1 Blender unit = 1 cm), FBX_SCALE_UNITS.
Front stays -Y (lock face toward -Y). Texture and UVs are the Meshy ones (UVs follow the decimation, no re-bake). Report: <out>.report.json (planarity per large face)."""
import bpy, bmesh, json, math, os, runpy, shutil, sys, tempfile
import numpy as np
from mathutils import Vector, Euler

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
SRC_DIR = os.path.join(ROOT, "Art", "Source", "chest", "meshy")
STEM = "Meshy_AI_Black_Tactical_Storag_1023224033_texture"
SRC_FBX = os.path.join(SRC_DIR, STEM + ".fbx")
SHEET = os.path.join(ROOT, "vault", "assets", "chest-turnaround.webp")
FLATTEN = os.path.join(os.path.dirname(__file__), "meshy_flatten_panels.py")
argv = sys.argv[sys.argv.index("--") + 1:]
mode = argv[0]


def opt(name, default):
    return type(default)(argv[argv.index(name) + 1]) if name in argv else default


def run_flatten(src, dst, rep, extra):
    """run the shared flatten tool in this Blender session; the scene then holds its (flattened) object"""
    saved = sys.argv
    sys.argv = ["x", "--", src, dst, rep] + extra
    try:
        runpy.run_path(FLATTEN)
    finally:
        sys.argv = saved
    return json.load(open(rep, encoding="utf-8"))


def fix_textures(obj):
    m = obj.data.materials[0]
    for n in m.node_tree.nodes:
        if n.type != "TEX_IMAGE":
            continue
        nm = n.image.name.lower()
        for suffix, cs in (("_normal", "Non-Color"), ("_roughness", "Non-Color"), ("_metallic", "Non-Color"), ("", "sRGB")):
            hit = ("normal" in nm) if suffix == "_normal" else ("roughness" in nm) if suffix == "_roughness" else \
                  ("metallic" in nm) if suffix == "_metallic" else True
            if hit:
                img = bpy.data.images.load(os.path.join(SRC_DIR, STEM + suffix + ".png"), check_existing=True)
                img.colorspace_settings.name = cs
                n.image = img
                break


def tri_count(me):
    return sum(len(p.vertices) - 2 for p in me.polygons)


def red_faces(obj):
    """faces whose UV triangle holds a red pixel of the base colour (the lock light, 5 faces on the Meshy mesh)"""
    me = obj.data
    img = bpy.data.images.load(os.path.join(SRC_DIR, STEM + ".png"), check_existing=True)
    w, h = img.size
    px = np.array(img.pixels[:]).reshape(h, w, 4)
    ys, xs = np.nonzero((px[:, :, 0] > 0.5) & (px[:, :, 1] < 0.15) & (px[:, :, 2] < 0.15))
    P = np.stack([(xs + .5) / w, (ys + .5) / h], 1)[::3]
    uv = me.uv_layers[0].data
    T = np.array([[uv[l].uv[:] for l in p.loop_indices[:3]] for p in me.polygons])
    A, B, C = T[:, 0], T[:, 1], T[:, 2]
    sg = lambda a, b: (P[:, None, 0] - b[None, :, 0]) * (a[None, :, 1] - b[None, :, 1]) - (a[None, :, 0] - b[None, :, 0]) * (P[:, None, 1] - b[None, :, 1])
    d1, d2, d3 = sg(A, B), sg(B, C), sg(C, A)
    inside = ~(((d1 < 0) | (d2 < 0) | (d3 < 0)) & ((d1 > 0) | (d2 > 0) | (d3 > 0)))
    return [int(i) for i in np.nonzero(inside.any(0))[0]]


def label(n, c, ext):
    ax = int(np.argmax(np.abs(n)))
    s = "+" if n[ax] > 0 else "-"
    return "%s%s@%s" % (s, "XYZ"[ax], ",".join("%d" % round(x) for x in c))


if mode == "flatten":
    OUT = os.path.abspath(opt("--out", os.path.join(ROOT, "Art", "Source", "chest", "clean", "chest_flat.fbx")))
    BUDGET = opt("--budget", 2950)
    WIDTH = opt("--width", 130.0)
    PROT_RINGS = opt("--rings", 2); PROT_FACTOR = opt("--pfactor", 1.0); PROT_INVERT = bool(opt("--pinvert", 1))
    FP = ["--dist", opt("--dist", "3"), "--ang", opt("--ang", "25"), "--min-area", "20", "--max-move", opt("--maxmove", "3")]
    DISS = opt("--dissolve", 1.0)
    tmp = tempfile.mkdtemp(prefix="chestflat_")
    # 1. original: planes at full resolution
    r1 = run_flatten(SRC_FBX, os.path.join(tmp, "s1.fbx"), os.path.join(tmp, "s1.json"), FP)
    orig_tris = r1["tris_before"]
    obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
    # 1b. merge the faces that are now coplanar (n-gons, UV seams and the material kept), triangulate again
    if DISS > 0:
        bm = bmesh.new(); bm.from_mesh(obj.data)
        bmesh.ops.dissolve_limit(bm, angle_limit=math.radians(DISS), use_dissolve_boundaries=False, verts=bm.verts, edges=bm.edges, delimit={"UV", "MATERIAL"})
        bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")
        bm.to_mesh(obj.data); bm.free(); obj.data.update()
    tris_after_dissolve = tri_count(obj.data)
    print("DISSOLVED", orig_tris, "->", tris_after_dissolve)
    # 2. decimation to the budget (UVs follow the collapse)
    ratio = BUDGET / tris_after_dissolve
    lock = red_faces(obj)
    bm = bmesh.new(); bm.from_mesh(obj.data); bm.faces.ensure_lookup_table(); bm.verts.ensure_lookup_table()
    ring = {v.index for fi in lock for v in bm.faces[fi].verts}
    for _ in range(PROT_RINGS):
        ring |= {w.index for vi in list(ring) for e in bm.verts[vi].link_edges for w in (e.other_vert(bm.verts[vi]),)}
    bm.free()
    vg = obj.vertex_groups.new(name="prot"); vg.add(sorted(ring), 1.0, "REPLACE")
    for _ in range(6):
        bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
        me0 = obj.data.copy()
        md = obj.modifiers.new("dec", "DECIMATE"); md.decimate_type = "COLLAPSE"; md.ratio = ratio; md.use_collapse_triangulate = True
        md.vertex_group = "prot"; md.vertex_group_factor = PROT_FACTOR; md.invert_vertex_group = PROT_INVERT
        bpy.ops.object.modifier_apply(modifier="dec")
        t = tri_count(obj.data)
        if t <= BUDGET:
            break
        obj.data = me0; ratio *= 0.97
    dec_tris = tri_count(obj.data)
    lock_after = red_faces(obj)
    print("LOCKFACES", len(lock), "->", len(lock_after))
    for g in list(obj.vertex_groups): obj.vertex_groups.remove(g)
    bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True)
    bpy.ops.export_scene.fbx(filepath=os.path.join(tmp, "s2.fbx"), use_selection=True, object_types={"MESH"},
                             apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="OFF", add_leaf_bones=False)
    # 3. planes again on the decimated mesh (its report: deviation_before = flatness left by the decimation)
    r3 = run_flatten(os.path.join(tmp, "s2.fbx"), os.path.join(tmp, "s3.fbx"), os.path.join(tmp, "s3.json"), FP)
    obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
    me = obj.data
    # 4. flat shading, scale, bottom-centre origin
    for p in me.polygons:
        p.use_smooth = False
    nv = len(me.vertices)
    V = np.empty(nv * 3); me.vertices.foreach_get("co", V); V = V.reshape(nv, 3)
    k = WIDTH / (V[:, 0].max() - V[:, 0].min())          # k: mesh units to final cm
    V = V * k
    mn, mx = V.min(axis=0), V.max(axis=0)
    off = np.array([-(mn[0] + mx[0]) / 2, -(mn[1] + mx[1]) / 2, -mn[2]])
    V = V + off
    me.vertices.foreach_set("co", V.reshape(-1)); me.update()
    obj.rotation_euler = (0, 0, 0); obj.scale = (1, 1, 1); obj.location = (0, 0, 0)
    sc = bpy.context.scene
    sc.unit_settings.scale_length = 1.0
    # 5. new UVs (smart project) and a re-bake of the Meshy base colour from the original mesh, same transform as the final mesh
    before = set(sc.objects)
    bpy.ops.import_scene.fbx(filepath=SRC_FBX)
    hi = next(o for o in sc.objects if o not in before and o.type == "MESH")
    fix_textures(hi)
    hv = np.array([v.co[:] for v in hi.data.vertices]) * hi.scale[0] * k + off
    hi.data.vertices.foreach_set("co", hv.reshape(-1)); hi.data.update(); hi.scale = (1, 1, 1); hi.rotation_euler = (0, 0, 0); hi.location = (0, 0, 0)
    bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
    bpy.ops.object.mode_set(mode="EDIT"); bpy.ops.mesh.select_all(action="SELECT")
    bpy.ops.uv.smart_project(angle_limit=math.radians(66), island_margin=0.004)
    bpy.ops.object.mode_set(mode="OBJECT")
    BAKE_PX = opt("--px", 2048)
    img = bpy.data.images.new("chest_flat_basecolor", BAKE_PX, BAKE_PX, alpha=False)
    mat = bpy.data.materials.new("M_BonusChest"); mat.use_nodes = True
    nt = mat.node_tree; bsdf = nt.nodes["Principled BSDF"]
    tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = img; nt.links.new(tx.outputs["Color"], bsdf.inputs["Base Color"])
    rm = []
    for suffix in ("_roughness", "_metallic"):
        a = bpy.data.images.load(os.path.join(SRC_DIR, STEM + suffix + ".png"), check_existing=True)
        rm.append(float(np.array(a.pixels[:]).reshape(-1, 4)[::97, 0].mean()))
    bsdf.inputs["Roughness"].default_value = rm[0]; bsdf.inputs["Metallic"].default_value = rm[1]
    obj.data.materials.clear(); obj.data.materials.append(mat)
    nt.nodes.active = tx
    sc.render.engine = "CYCLES"; sc.cycles.device = "CPU"; sc.cycles.samples = 4
    bpy.ops.object.select_all(action="DESELECT"); hi.select_set(True); obj.select_set(True); bpy.context.view_layer.objects.active = obj
    bpy.ops.object.bake(type="DIFFUSE", pass_filter={"COLOR"}, use_selected_to_active=True, cage_extrusion=2.0, max_ray_distance=6.0, margin=8, margin_type="EXTEND")
    clean_dir = os.path.dirname(OUT)
    img.filepath_raw = os.path.join(clean_dir, "chest_flat_basecolor.png"); img.file_format = "PNG"; img.save()
    bsdf.inputs["Roughness"].default_value = rm[0]
    bpy.data.objects.remove(hi, do_unlink=True)
    # UV sanity: triangles with flipped UV winding, UVs out of 0..1
    uvl = me.uv_layers[0].data
    flipped = 0; out_range = 0
    for p in me.polygons:
        a, b, c = (np.array(uvl[l].uv[:]) for l in p.loop_indices[:3])
        if (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0]) <= 0:
            flipped += 1
    uvs = np.array([l.uv[:] for l in uvl]); out_range = int(((uvs < 0) | (uvs > 1)).any(axis=1).sum())
    # planarity table in final cm: original regions (r1) against the final regions (r3)
    def table_rows(rep, kk):
        return [dict(label=label(np.array(r["normal"]), np.array(r["centroid_cm"]) * kk, None), area_cm2=r["area_cm2"] * kk * kk,
                     normal=r["normal"], centroid=(np.array(r["centroid_cm"]) / 100.0 * k + off).tolist(),
                     dev_before=r["deviation_before_cm"] / 100.0 * k, dev_after=r["deviation_after_cm"] / 100.0 * k, faces=r["faces"])
                for r in rep["regions"]]
    orig_rows = sorted(table_rows(r1, 0.01 * k), key=lambda r: -r["area_cm2"])
    fin_rows = sorted(table_rows(r3, 0.01 * k), key=lambda r: -r["area_cm2"])
    large = []
    for r in orig_rows[:16]:
        n = np.array(r["normal"]); c = np.array(r["centroid"])
        best = None
        for f in fin_rows:
            fn = np.array(f["normal"]); fc = np.array(f["centroid"])
            if np.dot(n, fn) < 0.98 or abs(np.dot(fc - c, n)) > 3.0:
                continue
            d = np.linalg.norm(fc - c)
            if best is None or d < best[0]:
                best = (d, f)
        large.append(dict(face=r["label"].split("@")[0] + " @ (%.0f,%.0f,%.0f) cm" % tuple(c), area_cm2_original=round(r["area_cm2"], 1),
                          planarity_original_uu=round(r["dev_before"], 3), planarity_original_after_first_fit_uu=round(r["dev_after"], 4),
                          planarity_decimated_before_refit_uu=round(best[1]["dev_before"], 3) if best else None,
                          planarity_final_uu=round(best[1]["dev_after"], 4) if best else None,
                          final_faces=best[1]["faces"] if best else None))
    # export
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    bpy.ops.object.select_all(action="DESELECT"); obj.select_set(True); bpy.context.view_layer.objects.active = obj
    bpy.ops.export_scene.fbx(filepath=OUT, use_selection=True, object_types={"MESH"}, apply_scale_options="FBX_SCALE_UNITS",
                             mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="COPY", embed_textures=False)
    dims = V.max(axis=0) - V.min(axis=0)
    rep = dict(
        script="Tools/art/blender/chest_flatten.py", source=os.path.relpath(SRC_FBX, ROOT).replace("\\", "/"), output=os.path.relpath(OUT, ROOT).replace("\\", "/"),
        tris_before=orig_tris, tris_after_dissolve=tris_after_dissolve, tris_after_decimation=dec_tris, tris_after=tri_count(me), budget=BUDGET, verts_before=r1["verts_before"], verts_after=len(me.vertices),
        scale_factor_to_cm=round(float(k), 6), dimensions_cm_xyz=[round(float(x), 2) for x in dims], source_dimensions_cm_xyz=[round(float(x) / 100.0 * k, 2) for x in r1["dimensions_cm_before"]],
        origin="bottom-centre (bbox centre in X and Y, min Z = 0), 1 Blender unit = 1 cm", front="lock face toward -Y (as in the Meshy file; Z up, X is the long side)",
        shading="flat (face smoothing off)", materials=[m.name for m in me.materials], uv_layers=[u.name for u in me.uv_layers],
        texture="RE-BAKED: new smart-project UVs, Meshy base colour baked from the original mesh (Cycles diffuse colour pass, %d px) to chest_flat_basecolor.png; roughness %.3f and metallic %.3f are the mean of the Meshy maps (constants); Meshy normal map dropped (baked from noisy geometry)" % (BAKE_PX, rm[0], rm[1]),
        uv_checks=dict(flipped_uv_triangles=flipped, triangles=len(me.polygons), triangles_with_uv_outside_0_1=out_range),
        regions=dict(original=r1["regions_found"], decimated=r3["regions_found"], faces_in_regions_decimated=r3["faces_in_regions"], faces_decimated=r3["faces_total"]),
        planarity_note="uu = cm in final scale; max distance of the region vertices to the best-fit plane of that region. original = Meshy mesh, regions found by meshy_flatten_panels (faces farther than 2.7 mesh-cm from the plane are pruned out of a region first); decimated_before_refit = after collapse decimation; final = after the second plane fit (what is exported).",
        large_faces=large,
        max_planarity_final_uu=round(max([r["planarity_final_uu"] for r in large if r["planarity_final_uu"] is not None] or [0]), 4),
        max_planarity_original_uu=round(max(r["planarity_original_uu"] for r in large), 3),
        flipped_normals_tool=r3["flipped_normals"], warnings=[])
    if rep["tris_after"] > BUDGET: rep["warnings"].append("tris above budget")
    if flipped: rep["warnings"].append("%d triangles with flipped UV winding" % flipped)
    json.dump(rep, open(os.path.splitext(OUT)[0] + ".report.json", "w", encoding="utf-8"), indent=2)
    print("DONE", json.dumps({k_: rep[k_] for k_ in ("tris_before", "tris_after", "dimensions_cm_xyz", "max_planarity_final_uu", "max_planarity_original_uu", "uv_checks", "warnings")}))
    shutil.rmtree(tmp, ignore_errors=True)

elif mode == "render":
    FBX, PREFIX = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=FBX)
    obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
    if os.path.basename(FBX).startswith("Meshy"):
        fix_textures(obj)
    else:
        for n in obj.data.materials[0].node_tree.nodes:
            if n.type == "TEX_IMAGE":
                n.image = bpy.data.images.load(os.path.join(os.path.dirname(FBX), "chest_flat_basecolor.png"))
    if "--clay" in argv:   # geometry check: plain grey, no texture
        for n in obj.data.materials[0].node_tree.nodes:
            if n.type == "BSDF_PRINCIPLED":
                for l in list(n.inputs["Base Color"].links) + list(n.inputs["Normal"].links): obj.data.materials[0].node_tree.links.remove(l)
                n.inputs["Base Color"].default_value = (0.3, 0.3, 0.3, 1); n.inputs["Roughness"].default_value = 0.6; n.inputs["Metallic"].default_value = 0.0
    V = np.array([v.co[:] for v in obj.data.vertices]) * obj.scale[0]
    k = 130.0 / (V[:, 0].max() - V[:, 0].min())
    # same normalisation for every file: 130 cm along X, bottom-centre; the original keeps its smooth shading
    me = obj.data
    V = np.array([v.co[:] for v in me.vertices]) * k
    mn, mx = V.min(axis=0), V.max(axis=0)
    V += np.array([-(mn[0] + mx[0]) / 2, -(mn[1] + mx[1]) / 2, -mn[2]])
    me.vertices.foreach_set("co", V.reshape(-1)); me.update(); obj.scale = (1, 1, 1); obj.rotation_euler = (0, 0, 0); obj.location = (0, 0, 0)
    sc = bpy.context.scene
    sc.render.engine = "BLENDER_EEVEE"
    sc.render.resolution_x = sc.render.resolution_y = 800
    sc.render.image_settings.file_format = "PNG"
    sc.view_settings.view_transform = "Standard"
    w = bpy.data.worlds.new("W"); w.use_nodes = True; sc.world = w
    bg = w.node_tree.nodes["Background"]; bg.inputs["Color"].default_value = (0.85, 0.85, 0.85, 1); bg.inputs["Strength"].default_value = 0.6
    sd = bpy.data.lights.new("S", "SUN"); sd.energy = 4.0
    s = bpy.data.objects.new("S", sd); sc.collection.objects.link(s); s.rotation_euler = Euler([math.radians(a) for a in (50, 10, -60)])
    cd = bpy.data.cameras.new("C"); cd.type = "ORTHO"; cd.ortho_scale = 170.0; cd.clip_start = 1; cd.clip_end = 3000
    cam = bpy.data.objects.new("C", cd); sc.collection.objects.link(cam); sc.camera = cam
    t = Vector((0, 0, 33))
    for name, az, el in (("front", 0, 10), ("side", 90, 10), ("back", 180, 10), ("q", -35, 28), ("top", 0, 89.9)):
        a, e = math.radians(az), math.radians(el)
        cam.location = t + Vector((math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e))) * 500
        cam.rotation_euler = (t - cam.location).to_track_quat("-Z", "Y").to_euler()
        sc.render.filepath = PREFIX + "_" + name + ".png"
        bpy.ops.render.render(write_still=True)

elif mode == "compare":
    D = os.path.abspath(argv[1])
    sheet = bpy.data.images.load(SHEET)
    sw, sh = sheet.size
    S = np.array(sheet.pixels[:]).reshape(sh, sw, 4)       # bottom row first
    # crops of the sheet in pixel fractions (x0, x1) per view, whole height
    crops = {"front": (0.012, 0.415), "side": (0.425, 0.592), "back": (0.605, 0.99)}
    H = 800

    def load(path):
        im = bpy.data.images.load(path)
        return np.array(im.pixels[:]).reshape(im.size[1], im.size[0], 4)

    def resize_h(a, h):
        w_ = max(1, int(round(a.shape[1] * h / a.shape[0])))
        yi = (np.arange(h) * a.shape[0] / h).astype(int); xi = (np.arange(w_) * a.shape[1] / w_).astype(int)
        return a[yi][:, xi]

    def save(a, path):
        im = bpy.data.images.new("o", a.shape[1], a.shape[0], alpha=True)
        im.pixels = a.reshape(-1).tolist(); im.filepath_raw = path; im.file_format = "PNG"; im.save()

    for view in ("front", "side", "back", "q", "top"):
        parts = []
        if view in crops:
            x0, x1 = crops[view]
            c = S[:, int(x0 * sw):int(x1 * sw)]
            # trim the white margin top and bottom, then pad to a square of the render height
            nonwhite = np.where((c[:, :, :3] < 0.93).any(axis=(1, 2)))[0]
            c = c[max(0, nonwhite.min() - 25):nonwhite.max() + 25]
            c = resize_h(c, int(H * 0.5)) if c.shape[1] * (H * 0.5) / c.shape[0] < 1000 else c
            pad = np.ones((H, c.shape[1], 4)); y0 = (H - c.shape[0]) // 2; pad[y0:y0 + c.shape[0]] = c
            parts.append(pad)
        else:
            parts.append(np.ones((H, 400, 4)))      # no sheet view for this angle
        parts.append(load(os.path.join(D, "orig_%s.png" % view)))
        parts.append(load(os.path.join(D, "flat_%s.png" % view)))
        save(np.concatenate(parts, axis=1), os.path.join(D, "compare_%s.png" % view))
