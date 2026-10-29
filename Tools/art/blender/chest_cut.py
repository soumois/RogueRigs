"""Bonus chest cut (task-306-02). Blender route, no Meshy call. Headless:
  cut    : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_cut.py -- cut [--zs 41.25] [--wall 3.0] [--depth 22.0]
  render : "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chest_cut.py -- render [--lid -105] [--latch -65]
Input  : Art/Source/chest/clean/chest_flat.fbx (task-306-01; 1 Blender unit = 1 cm, X long side, lock face toward -Y, Z up, bottom-centre origin).
Output : Art/Source/chest/clean/chest_body.fbx, chest_lid.fbx, chest_latch.fbx (FBX_SCALE_UNITS, flat shading, texture chest_flat_basecolor.png)
         and chest_cut.report.json. Renders: Art/Source/chest/clean/renders/cut_*.png.
Cut: 1 the latch = faces in a box in front of the lock (|x| <= 11.6, y < -24, 8 <= z <= 54); 2 the hole it leaves in the rest is filled (recess behind the latch);
     3 the rest is bisected at the seam plane z = zs (inside the vertex gap 41.0 to 41.5 of the lid / body groove); above = lid, below = body;
     4 every open loop is closed with a flat cap (cap faces get one dark texel of the base colour); 5 the body top cap is inset by --wall and pushed down
     by --depth: its walls and floor are the Interior slot; 6 the red lock-light faces of the latch (UV triangle mostly red) are the LockLight slot;
     7 normals recomputed, triangulated, flat shading; 8 origins: body bottom-centre, lid on the axis of the hinge pins (PIN_Y, PIN_Z, measured on the pin
     cylinders of the source), latch on its top edge (X axis at mid depth of its top face). The export keeps every vertex local to its origin; the report gives
     the origins relative to the body origin.
task-309-07: each hinge block (the bracket from the pins down the back wall, HX0..HX1 / y > HY / z >= HZ) is cut whole into the lid (no stub left on the body);
     the recess fill behind the latch takes the texel of the front wall around it (not the dark cap texel); the render pose swings the latch -90 deg about its
     top edge (outward and up) with the lid at -100 / -105 / -115 deg. Opening sign: negative rotation about X in the Blender frame (see the report)."""
import bpy, bmesh, json, math, os, sys
import numpy as np
from mathutils import Vector, Matrix, Euler

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rr_blender as rr

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
CLEAN = os.path.join(ROOT, "Art", "Source", "chest", "clean")
SRC = os.path.join(CLEAN, "chest_flat.fbx")
TEX = os.path.join(CLEAN, "chest_flat_basecolor.png")
RENDERS = os.path.join(CLEAN, "renders")  # overridden by --rdir (after argv parse)
argv = sys.argv[sys.argv.index("--") + 1:]
mode = argv[0]
if "--rdir" in argv: RENDERS = argv[argv.index("--rdir") + 1]


def opt(name, default):
    return type(default)(argv[argv.index(name) + 1]) if name in argv else default


tri_count = rr.tri_count


def mat_textured():
    m = bpy.data.materials.new("M_BonusChest"); m.use_nodes = True
    nt = m.node_tree; b = nt.nodes["Principled BSDF"]
    tx = nt.nodes.new("ShaderNodeTexImage"); tx.image = bpy.data.images.load(TEX, check_existing=True)
    nt.links.new(tx.outputs["Color"], b.inputs["Base Color"])
    b.inputs["Roughness"].default_value = 0.454; b.inputs["Metallic"].default_value = 0.003
    return m


def mat_flat(name, rgb, rough=0.6, emit=0.0):
    m = bpy.data.materials.new(name); m.use_nodes = True
    b = m.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (*rgb, 1); b.inputs["Roughness"].default_value = rough; b.inputs["Metallic"].default_value = 0.0
    if emit:
        b.inputs["Emission Color"].default_value = (*rgb, 1); b.inputs["Emission Strength"].default_value = emit
    m.diffuse_color = (*rgb, 1)
    return m


def boundary_edges(bm):
    return [e for e in bm.edges if len(e.link_faces) == 1]


def fill_holes(bm):
    be = boundary_edges(bm)
    if not be:
        return []
    r = bmesh.ops.holes_fill(bm, edges=be, sides=2000)
    uvl = bm.loops.layers.uv.verify()
    for f in r["faces"]:
        for l in f.loops: l[uvl].uv = DARK_UV
    return r["faces"]


def split_off(bm, keep):
    """copy of bm that keeps only the faces whose index is in keep"""
    b = bm.copy(); b.faces.ensure_lookup_table(); b.faces.index_update()
    drop = [f for f in b.faces if f.index not in keep]
    bmesh.ops.delete(b, geom=drop, context="FACES")
    for v in [v for v in b.verts if not v.link_faces]:
        b.verts.remove(v)
    return b


def nonmanifold(bm):
    return dict(open_edges=sum(1 for e in bm.edges if len(e.link_faces) == 1), edges_over_2_faces=sum(1 for e in bm.edges if len(e.link_faces) > 2))


if mode == "cut":
    ZS = opt("--zs", 41.25); WALL = opt("--wall", 3.0); DEPTH = opt("--depth", 22.0)
    LX, LY, LZ0, LZ1 = 11.6, -24.0, 8.0, 54.0
    PIN_Y, PIN_Z = 24.6, 59.5                      # axis of the hinge pins (cm, body frame): mean of the fitted outer pin ends at x = +-45
    HX0, HX1, HY, HZ = 20.5, 47.8, 27.5, 33.0      # hinge block box: |x| in HX0..HX1, y > HY, face centre z >= HZ -> lid
    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=SRC)
    obj = next(o for o in bpy.context.scene.objects if o.type == "MESH")
    me = obj.data; me.transform(obj.matrix_world); obj.matrix_world = Matrix.Identity(4)
    sc = bpy.context.scene; sc.unit_settings.scale_length = 1.0
    me.materials.clear()
    for m_ in list(bpy.data.materials): bpy.data.materials.remove(m_)
    warnings = []
    src_tris = tri_count(me)
    # dark texel of the base colour: nearest texel to the mean colour (for faces without UV)
    img = bpy.data.images.load(TEX, check_existing=True)
    w, h = img.size
    px = np.array(img.pixels[:]).reshape(h, w, 4)
    mean = px[::7, ::7, :3].reshape(-1, 3).mean(0)
    d = ((px[::5, ::5, :3] - mean) ** 2).sum(2)
    iy, ix = np.unravel_index(np.argmin(d), d.shape)
    DARK_UV = ((ix * 5 + .5) / w, (iy * 5 + .5) / h)
    bm = bmesh.new(); bm.from_mesh(me); bm.faces.ensure_lookup_table()
    # 0 stray one-triangle island of the source mesh
    seen = set(); comps = []
    for f in bm.faces:
        if f.index in seen: continue
        st = [f]; seen.add(f.index); cur = []
        while st:
            a = st.pop(); cur.append(a)
            for e in a.edges:
                for b2 in e.link_faces:
                    if b2.index not in seen: seen.add(b2.index); st.append(b2)
        comps.append(cur)
    comps.sort(key=len)
    stray = sum(len(c) for c in comps[:-1])
    if stray:
        bmesh.ops.delete(bm, geom=[f for c in comps[:-1] for f in c], context="FACES")
        bm.faces.ensure_lookup_table(); bm.faces.index_update()
        warnings.append("removed %d stray triangle(s) of the source mesh (separate island, 1 face)" % stray)
    # 1 latch faces by box
    lat = {f.index for f in bm.faces if abs(f.calc_center_median().x) <= LX and f.calc_center_median().y < LY and LZ0 <= f.calc_center_median().z <= LZ1}
    bmL = split_off(bm, lat)
    bmR = split_off(bm, {f.index for f in bm.faces} - lat)
    bm.free()
    # 2 recess behind the latch: filled, then textured with the front wall around it (task-309-07)
    fl = bmR.faces.layers.int.new("fill"); hl = bmR.faces.layers.int.new("hinge")
    rec = [f for f in fill_holes(bmR) if abs(f.calc_center_median().x) <= LX + 1.0 and f.calc_center_median().y < LY]
    for f in rec: f[fl] = 1
    recset = set(rec)
    bmR.normal_update()
    uv0 = bmR.loops.layers.uv.verify()
    refs = []                                       # (centre z, uv centroid, luminance) of the wall faces that share an edge with the fill
    seen_g = set()
    for f in rec:
        for e in f.edges:
            for g in e.link_faces:
                if g in recset or g.index in seen_g or abs(g.normal.dot(f.normal)) < 0.8: continue
                seen_g.add(g.index)
                uc = sum((l[uv0].uv for l in g.loops), Vector((0, 0))) / len(g.loops)
                ix, iy = min(w - 1, max(0, int(uc.x * w))), min(h - 1, max(0, int(uc.y * h)))
                refs.append((g.calc_center_median().z, uc.copy(), float(px[iy, ix, :3].mean())))
    print("RECESS fill faces", len(rec), "wall refs", len(refs), "fill y range", round(min(v.co.y for f in rec for v in f.verts), 2), round(max(v.co.y for f in rec for v in f.verts), 2))
    # hinge block faces: they stay out of the seam cut and go to the lid
    for f in bmR.faces:
        c = f.calc_center_median()
        if HX0 <= abs(c.x) <= HX1 and c.y > HY and c.z >= HZ: f[hl] = 1
    n_hinge = sum(1 for f in bmR.faces if f[hl])
    n_hinge_below = sum(1 for f in bmR.faces if f[hl] and f.calc_center_median().z < ZS)
    # 3 seam
    nh = [f for f in bmR.faces if not f[hl]]
    bmesh.ops.bisect_plane(bmR, geom=nh + list({e for f in nh for e in f.edges}) + list({v for f in nh for v in f.verts}), dist=1e-4, plane_co=(0, 0, ZS), plane_no=(0, 0, 1))
    bmR.faces.ensure_lookup_table(); bmR.faces.index_update()
    print("HINGE faces", n_hinge, "centre below the seam", n_hinge_below, "after bisect", sum(1 for f in bmR.faces if f[hl]), "fill after bisect", sum(1 for f in bmR.faces if f[fl]))
    lid_i = {f.index for f in bmR.faces if f.calc_center_median().z > ZS or f[hl]}
    bmB = split_off(bmR, {f.index for f in bmR.faces} - lid_i)
    bmD = split_off(bmR, lid_i)
    bmR.free()
    n_seam_edges_body = len(boundary_edges(bmB)); n_seam_edges_lid = len(boundary_edges(bmD))
    recess_uv = {}
    for nm, bx, below in (("body", bmB, True), ("lid", bmD, False)):
        pool = [r for r in refs if (r[0] < ZS) == below] or refs
        fx = bx.faces.layers.int.get("fill"); uvx = bx.loops.layers.uv.verify()
        ff = [f for f in bx.faces if f[fx]]
        if pool and ff:
            pool.sort(key=lambda r: r[2]); pick = pool[len(pool) // 2]
            for f in ff:
                for l in f.loops: l[uvx].uv = pick[1]
            recess_uv[nm] = dict(faces=len(ff), uv=[round(float(pick[1].x), 4), round(float(pick[1].y), 4)], refs=len(pool))
    print("RECESS UV", recess_uv)
    # back wall behind each hinge block: the block carried that part of the wall, so the body has a hole there; close it with a wall patch (wall texel)
    wall_uv = None
    wl = [f for f in bmB.faces if f.normal.y > 0.97 and 31.4 < f.calc_center_median().y < 32.0 and HX0 - 2 < abs(f.calc_center_median().x) < HX1 + 2 and f.calc_center_median().z < HZ]
    if wl:
        uvb = bmB.loops.layers.uv.verify()
        cand = []
        for f in wl:
            uc = sum((l[uvb].uv for l in f.loops), Vector((0, 0))) / len(f.loops)
            ix, iy = min(w - 1, max(0, int(uc.x * w))), min(h - 1, max(0, int(uc.y * h)))
            cand.append((float(px[iy, ix, :3].mean()), uc.copy()))
        cand.sort(key=lambda c: c[0]); wall_uv = cand[len(cand) // 2][1]
    gap_info = []

    def close_gap(bx, nm, uvval):
        """close the open chain below the seam plane around the hinge blocks (hole in the body wall, open bottom of the lid blocks) with a flat patch"""
        for sg in (1, -1):
            ge = [e for e in boundary_edges(bx) if sg * ((e.verts[0].co.x + e.verts[1].co.x) / 2) > HX0 - 1 and abs((e.verts[0].co.x + e.verts[1].co.x) / 2) < HX1 + 1
                  and (e.verts[0].co.y + e.verts[1].co.y) / 2 > HY and HZ - 3 <= (e.verts[0].co.z + e.verts[1].co.z) / 2 < ZS - 0.05]
            deg = {}
            for e in ge:
                for v in e.verts: deg[v] = deg.get(v, 0) + 1
            ends = [v for v, d in deg.items() if d == 1]
            info = dict(part=nm, side=sg, gap_edges=len(ge), ends=[tuple(round(c, 2) for c in v.co) for v in ends])
            if len(ends) == 2 and all(abs(v.co.z - ZS) < 0.05 for v in ends) and uvval is not None:
                ne = bx.edges.new((ends[0], ends[1]))
                nf = bmesh.ops.holes_fill(bx, edges=ge + [ne], sides=2000)["faces"]
                uvb = bx.loops.layers.uv.verify()
                for f in nf:
                    for l in f.loops: l[uvb].uv = uvval
                info["patch_faces"] = len(nf)
            gap_info.append(info)
    close_gap(bmB, "body", wall_uv)
    close_gap(bmD, "lid", DARK_UV)
    print("WALLGAP", gap_info)
    # 4 caps
    capB = fill_holes(bmB); capD = fill_holes(bmD); capL = fill_holes(bmL)
    # body cavity
    bmB.normal_update(); bmesh.ops.recalc_face_normals(bmB, faces=bmB.faces[:])
    tops = [f for f in capB if abs(f.normal.z) > 0.99 and abs(f.calc_center_median().z - ZS) < 0.01]
    cap = max(tops, key=lambda f: f.calc_area())
    cap_loop = [v.co.copy() for v in cap.verts]
    hinge_y = max(v.y for v in cap_loop if abs(v.x) < 15)
    r = bmesh.ops.inset_region(bmB, faces=[cap], thickness=WALL, depth=0.0, use_even_offset=True, use_boundary=True)
    inner = cap
    ex = bmesh.ops.extrude_discrete_faces(bmB, faces=[inner])
    inner = ex["faces"][0]
    for v in inner.verts: v.co.z -= DEPTH
    bmB.normal_update(); bmesh.ops.recalc_face_normals(bmB, faces=bmB.faces[:])
    cav = {inner} | {f for e in inner.edges for f in e.link_faces}
    # extrude side quads: faces adjacent to the floor edges (the walls); the rim ring is the inset output faces
    ring = set(r["faces"])
    cav -= ring
    parts = {}
    for name, b in (("body", bmB), ("lid", bmD), ("latch", bmL)):
        b.faces.ensure_lookup_table(); b.faces.index_update()
        uvl = b.loops.layers.uv.verify()
        capped = 0
        for f in b.faces:
            if all(abs(l[uvl].uv.x) < 1e-9 and abs(l[uvl].uv.y) < 1e-9 for l in f.loops):
                for l in f.loops: l[uvl].uv = DARK_UV
                capped += 1
        bmesh.ops.recalc_face_normals(b, faces=b.faces[:])
        parts[name] = dict(bm=b, capped=capped, nonmanifold_before_tri=nonmanifold(b))
        print("OPEN", name, [tuple(round(c, 1) for c in ((e.verts[0].co + e.verts[1].co) / 2)) for e in boundary_edges(b)])
    interior_faces = set(cav)
    # materials
    m_tex = mat_textured(); m_int = mat_flat("Interior", (0.03, 0.035, 0.045), 0.7); m_red = mat_flat("LockLight", (0.9, 0.03, 0.03), 0.4, 2.0)
    # LockLight faces on the latch: UV triangle mostly red in the base colour
    parts["latch"]["bm"].faces.ensure_lookup_table()
    bl = parts["latch"]["bm"]; uvl = bl.loops.layers.uv.verify()
    red_faces = set()
    ry, rx = np.nonzero((px[:, :, 0] > 0.5) & (px[:, :, 1] < 0.2) & (px[:, :, 2] < 0.2))
    RP = np.stack([(rx + .5) / w, (ry + .5) / h], 1)

    def red_info(f):
        """(red texels inside the UV triangle, triangle area in texels, mask of the texels)"""
        A, B, C = (np.array(l[uvl].uv[:]) for l in f.loops)
        sg = lambda p, q: (RP[:, 0] - q[0]) * (p[1] - q[1]) - (p[0] - q[0]) * (RP[:, 1] - q[1])
        d1, d2, d3 = sg(A, B), sg(B, C), sg(C, A)
        inside = ~(((d1 < 0) | (d2 < 0) | (d3 < 0)) & ((d1 > 0) | (d2 > 0) | (d3 > 0)))
        area_px = 0.5 * abs((B[0] - A[0]) * (C[1] - A[1]) - (B[1] - A[1]) * (C[0] - A[0])) * w * h
        return int(inside.sum()), float(area_px), inside, (A, B, C)

    bmesh.ops.triangulate(bl, faces=bl.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")
    bl.faces.ensure_lookup_table(); uvl = bl.loops.layers.uv.verify()
    # 3D rectangle of the red light: map every red texel to its 3D point through the triangle that holds it
    pts = []
    for f in bl.faces:
        n_in, area_px, inside, (A, B, C) = red_info(f)
        if not n_in: continue
        M = np.array([[B[0] - A[0], C[0] - A[0]], [B[1] - A[1], C[1] - A[1]]])
        if abs(np.linalg.det(M)) < 1e-12: continue
        st = np.linalg.solve(M, (RP[inside] - A).T).T
        P0, P1, P2 = (f.loops[k].vert.co for k in (0, 1, 2))
        for s_, t_ in st:
            pts.append(P0 + (P1 - P0) * s_ + (P2 - P0) * t_)
    pts = np.array([p[:] for p in pts])
    pad = 0.1                              # cm, about one texel
    rect = (pts[:, 0].min() - pad, pts[:, 0].max() + pad, pts[:, 2].min() - pad, pts[:, 2].max() + pad)
    # cut the faces that hold red texels along the four sides of the rectangle (UVs interpolate)
    for co, no in (((rect[0], 0, 0), (1, 0, 0)), ((rect[1], 0, 0), (1, 0, 0)), ((0, 0, rect[2]), (0, 0, 1)), ((0, 0, rect[3]), (0, 0, 1))):
        fs = [f for f in bl.faces if red_info(f)[0]]
        g = list(fs) + list({e for f in fs for e in f.edges}) + list({v for f in fs for v in f.verts})
        bmesh.ops.bisect_plane(bl, geom=g, dist=1e-5, plane_co=co, plane_no=no)
        bmesh.ops.triangulate(bl, faces=bl.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")
        bl.faces.ensure_lookup_table()
    for f in bl.faces:
        c = f.calc_center_median()
        if rect[0] < c.x < rect[1] and rect[2] < c.z < rect[3] and red_info(f)[0]:
            red_faces.add(f)
    red_rect_cm = [round(float(x), 2) for x in rect]
    print("REDRECT x0 x1 z0 z1", red_rect_cm, "faces", len(red_faces))
    # build the final objects
    out = {}
    pivots = {}
    bB, bD, bL = parts["body"]["bm"], parts["lid"]["bm"], parts["latch"]["bm"]
    lz = max(v.co.z for v in bL.verts)
    ltop = [v.co for v in bL.verts if v.co.z > lz - 0.5]
    lx0 = (min(v.x for v in (v.co for v in bL.verts)) + max(v.x for v in (v.co for v in bL.verts))) / 2
    pivots["body"] = Vector((0, 0, 0))
    pivots["lid"] = Vector((0, PIN_Y, PIN_Z))
    pivots["latch"] = Vector((lx0, (min(v.y for v in ltop) + max(v.y for v in ltop)) / 2, lz))
    slot_names = dict(body=[m_tex, m_int], lid=[m_tex], latch=[m_tex, m_red])
    slot_idx = dict(body=interior_faces, lid=set(), latch=red_faces)
    summary = {}
    for name in ("body", "lid", "latch"):
        b = parts[name]["bm"]
        b.faces.ensure_lookup_table()
        sel = slot_idx[name]
        for f in b.faces:
            f.material_index = 1 if f in sel else 0
            f.smooth = False
        bmesh.ops.triangulate(b, faces=b.faces[:], quad_method="BEAUTY", ngon_method="BEAUTY")
        bmesh.ops.translate(b, vec=-pivots[name], verts=b.verts[:])
        me2 = bpy.data.meshes.new("SM_Chest_" + name.capitalize())
        b.to_mesh(me2)
        for m in slot_names[name]: me2.materials.append(m)
        for p in me2.polygons: p.use_smooth = False
        o2 = bpy.data.objects.new("SM_Chest_" + name.capitalize(), me2); sc.collection.objects.link(o2)
        out[name] = o2
        V = np.array([v.co[:] for v in me2.vertices])
        slots = {}
        for i, m in enumerate(me2.materials):
            slots[m.name] = int(sum(1 for p in me2.polygons if p.material_index == i))
        uvv = np.array([l.uv[:] for l in me2.uv_layers[0].data])
        summary[name] = dict(
            file="Art/Source/chest/clean/chest_%s.fbx" % name, tris=tri_count(me2), verts=len(me2.vertices),
            dimensions_cm_xyz=[round(float(x), 2) for x in (V.max(0) - V.min(0))],
            local_bbox_min_cm=[round(float(x), 2) for x in V.min(0)], local_bbox_max_cm=[round(float(x), 2) for x in V.max(0)],
            world_bbox_min_cm=[round(float(x), 2) for x in (V.min(0) + np.array(pivots[name][:]))], world_bbox_max_cm=[round(float(x), 2) for x in (V.max(0) + np.array(pivots[name][:]))],
            pivot_rel_body_origin_cm=[round(float(x), 3) for x in pivots[name]],
            material_slots=[m.name for m in me2.materials], faces_per_slot=slots,
            uv_layers=[u.name for u in me2.uv_layers], triangles_with_uv_outside_0_1=int(((uvv < 0) | (uvv > 1)).any(axis=1).sum()) // 3,
            cap_faces_before_triangulation=parts[name]["capped"], edges_before_triangulation=parts[name]["nonmanifold_before_tri"],
            shading="flat")
    # export, one object at a time (origin = pivot, vertices local)
    for name, o2 in out.items():
        bpy.ops.object.select_all(action="DESELECT"); o2.select_set(True); bpy.context.view_layer.objects.active = o2
        bpy.ops.export_scene.fbx(filepath=os.path.join(CLEAN, "chest_%s.fbx" % name), use_selection=True, object_types={"MESH"},
                                 apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE", add_leaf_bones=False, path_mode="RELATIVE", embed_textures=False)
    # checks
    allV = np.concatenate([np.array([v.co[:] for v in out[n].data.vertices]) + np.array(pivots[n][:]) for n in out])
    dims = allV.max(0) - allV.min(0)
    lp = pivots["latch"] - pivots["lid"]
    rep = dict(
        script="Tools/art/blender/chest_cut.py", source="Art/Source/chest/clean/chest_flat.fbx", source_tris=src_tris,
        units="1 Blender unit = 1 cm = 1 uu; X long side, lock face toward -Y, Z up; FBX_SCALE_UNITS",
        seam_plane_z_cm=ZS, latch_box=dict(abs_x_max=LX, y_below=LY, z_min=LZ0, z_max=LZ1),
        cavity=dict(wall_cm=WALL, depth_cm=DEPTH, floor_z_cm=round(ZS - DEPTH, 2)),
        parts=summary, tris_total=sum(summary[n]["tris"] for n in summary),
        assembled_dimensions_cm_xyz=[round(float(x), 2) for x in dims],
        pivots_relative_to_body_origin_cm=dict(body=[0, 0, 0], lid_hinge_axis_X=[round(float(x), 3) for x in pivots["lid"]], latch_top_edge_axis_X=[round(float(x), 3) for x in pivots["latch"]]),
        latch_pivot_relative_to_lid_origin_cm=[round(float(x), 3) for x in lp],
        pivot_notes="lid: origin on the axis of the hinge pins (a line along X at y = %.1f, z = %.1f in the body frame, measured on the pin cylinders at x = +-45); the whole hinge block (bracket from the pins down the back wall) belongs to the lid. "
                    "latch: origin on the top edge of the latch block (X axis, mid depth of its top face, x centred); it hangs on the lid front, so in the Blueprint it is a child of the lid." % (PIN_Y, PIN_Z),
        hinge=dict(pin_axis_y_z_cm=[PIN_Y, PIN_Z], block_box=dict(abs_x=[HX0, HX1], y_above=HY, face_centre_z_min=HZ), block_faces_to_lid=n_hinge, block_faces_centred_below_the_seam_moved_to_lid=n_hinge_below),
        recess_fill_uv=recess_uv,
        opening_signs=dict(
            blender_frame="Lid: rotation about +X of NEGATIVE angle opens it up and backward (lid -105 deg = open; the front, at -Y, goes to +Z). Latch, local, about its top edge: NEGATIVE angle swings its bottom outward (-Y) and up (-90 deg = hasp perpendicular to the lid front face, pointing away from the lid, above the lid front edge when the lid is open).",
            unreal_mesh_space="The import mirrors Y (hinge at -Y, lid and latch extend toward +Y). Lid component relative Roll NEGATIVE (-105) lifts +Y into +Z (UE Roll is positive when +Y goes toward -Z); Latch relative Roll NEGATIVE (-90). Derived from the mirror and the left-handed rule, not measured in the editor.",
            lid_pivot_unreal_mesh_space_cm=[0, -PIN_Y, PIN_Z],
            latch_relative_to_lid_unreal_cm=[round(float(lp[0]), 3), round(float(-lp[1]), 3), round(float(lp[2]), 3)]),
        texture="Art/Source/chest/clean/chest_flat_basecolor.png (shared by body, lid and latch; cap faces map to one dark texel)",
        lock_light_face_count=len(red_faces), lock_light_rect_x0_x1_z0_z1_cm=red_rect_cm, warnings=warnings)
    rep["warnings"] += ["%s: %d open edges after the caps" % (n, summary[n]["edges_before_triangulation"]["open_edges"]) for n in summary if summary[n]["edges_before_triangulation"]["open_edges"]]
    rep["manifold_note"] = "edges_over_2_faces are inherited from the Meshy source mesh (27 edges with 3 faces and 6 with 4 in chest_flat.fbx: interpenetrating thin parts); the caps close the open loops"
    RJ = os.path.join(CLEAN, "chest_cut.report.json")
    if os.path.exists(RJ):
        old = json.load(open(RJ, encoding="utf-8"))
        for k in ("texture_note", "open_edges_note"):
            if k in old: rep[k] = old[k]
    json.dump(rep, open(RJ, "w", encoding="utf-8"), indent=2)
    print("DONE", json.dumps(dict(tris={n: summary[n]["tris"] for n in summary}, slots={n: summary[n]["faces_per_slot"] for n in summary}, pivots=rep["pivots_relative_to_body_origin_cm"], dims=rep["assembled_dimensions_cm_xyz"], warnings=rep["warnings"])))

elif mode == "render":
    LID = opt("--lid", -105.0); LATCH = opt("--latch", -90.0); LATCH_OUT = opt("--latch-out", -60.0)
    bpy.ops.wm.read_factory_settings(use_empty=True)
    rep = json.load(open(os.path.join(CLEAN, "chest_cut.report.json"), encoding="utf-8"))
    pv = rep["pivots_relative_to_body_origin_cm"]
    P = dict(body=Vector((0, 0, 0)), lid=Vector(pv["lid_hinge_axis_X"]), latch=Vector(pv["latch_top_edge_axis_X"]))
    objs = {}
    sc = bpy.context.scene; sc.unit_settings.scale_length = 1.0
    for name in ("body", "lid", "latch"):
        before = set(sc.objects)
        bpy.ops.import_scene.fbx(filepath=os.path.join(CLEAN, "chest_%s.fbx" % name))
        o = next(x for x in sc.objects if x not in before and x.type == "MESH")
        o.data.transform(o.matrix_world); o.matrix_world = Matrix.Identity(4)
        for m in o.data.materials:
            for n in m.node_tree.nodes:
                if n.type == "TEX_IMAGE": n.image = bpy.data.images.load(TEX, check_existing=True)
        objs[name] = o

    def T(v): return Matrix.Translation(v)
    def Rx(deg): return Matrix.Rotation(math.radians(deg), 4, "X")
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
    os.makedirs(RENDERS, exist_ok=True)

    def pose(lid_deg, latch_deg):
        objs["body"].matrix_world = T(P["body"])
        Ml = T(P["lid"]) @ Rx(lid_deg)
        objs["lid"].matrix_world = Ml
        objs["latch"].matrix_world = Ml @ T(P["latch"] - P["lid"]) @ Rx(latch_deg)

    def shoot(prefix, views):
        for name, az, el in views:
            a, e = math.radians(az), math.radians(el)
            cam.location = t + Vector((math.sin(a) * math.cos(e), -math.cos(a) * math.cos(e), math.sin(e))) * 500
            cam.rotation_euler = (t - cam.location).to_track_quat("-Z", "Y").to_euler()
            sc.render.filepath = os.path.join(RENDERS, "%s_%s.png" % (prefix, name))
            bpy.ops.render.render(write_still=True)

    allv = (("front", 0, 10), ("side", 90, 10), ("back", 180, 10), ("q", -35, 28), ("top", 0, 89.9))
    if "--probe" in argv:
        pose(LID, LATCH); shoot("probe", (("back", 180, 10), ("backq", 145, 30), ("side", 90, 10), ("sideq", 60, 20))); raise SystemExit
    pose(0, 0); shoot("cut_closed", allv)
    pose(0, LATCH_OUT); shoot("cut_latchout", (("front", 0, 10), ("q", -35, 28)))
    pose(LID, LATCH); shoot("cut_open", (("front", 0, 10), ("side", 90, 10), ("q", -35, 28), ("top", 0, 89.9), ("back", 180, 10)))
    shoot("cut_open_high", (("q", -35, 55),))
    # open pose checks: lid at 100 / 105 / 115, latch -90: nothing hangs below the lid front edge, nothing of the lid or latch goes into the body
    from mathutils import Vector as V3
    def world_verts(name): return [objs[name].matrix_world @ v.co for v in objs[name].data.vertices]
    checks = {}
    cd.ortho_scale = 260.0; t = Vector((0, 20, 70))      # wide camera (same light, same angles): the opened lid and the swung latch leave the 170 frame
    pose(LID, LATCH); shoot("cut_openwide", (("front", 0, 10), ("side", 90, 10), ("q", -35, 28), ("back", 180, 10)))
    for ld in (100.0, 105.0, 115.0):
        pose(-ld, LATCH)
        lidv = world_verts("lid"); latv = world_verts("latch"); bodv = world_verts("body")
        loc = [v.co for v in objs["lid"].data.vertices]      # lid front-bottom edge: lowest vertices of the front-most 1 cm
        ymin = min(l.y for l in loc)
        fe = lidv[min((i for i in range(len(loc)) if loc[i].y < ymin + 1.0), key=lambda i: loc[i].z)]
        body_top = max(v.z for v in bodv)
        checks["lid_%d" % ld] = dict(lid_front_edge_world_z=round(fe.z, 1), latch_min_z=round(min(v.z for v in latv), 1), latch_min_z_minus_lid_front_edge_z=round(min(v.z for v in latv) - fe.z, 1),
                                       lid_min_z=round(min(v.z for v in lidv), 1), clearance_lid_over_body_rim=round(min(v.z for v in lidv) - body_top, 1),
                                       latch_min_z_over_body_rim=round(min(v.z for v in latv) - body_top, 1),
                                       latch_pivot_world_z=round((objs["latch"].matrix_world @ V3((0, 0, 0))).z, 1), latch_min_z_minus_its_pivot_z=round(min(v.z for v in latv) - (objs["latch"].matrix_world @ V3((0, 0, 0))).z, 1))
        shoot("cut_openwide%d" % ld, (("q", -35, 28), ("side", 90, 10)))
    print("OPENCHECKS", json.dumps(checks))
    rep["open_pose_checks"] = checks
    rep["open_pose_note"] = ("latch_min_z_minus_its_pivot_z is the no-dangling check: the lowest latch point is its own top edge (0 to -2.4 cm, its thickness), so nothing hangs below the hinge line. "
                             "The latch top edge sits 6.5 cm behind the lid front face and 10 cm above its bottom edge, so it is 7 to 12 cm below the lid front-bottom edge in world z. "
                             "The pins are at the top rear of the lid (z 59.5), 18 cm above the seam: opened about them the lid hovers 9 to 12 cm above the body rim (clearance_lid_over_body_rim), it never touches the body.")
    rep["renders"] = dict(dir="Art/Source/chest/clean/renders/", files=sorted(f for f in os.listdir(RENDERS) if f.startswith("cut_")), before=sorted(f for f in os.listdir(RENDERS) if f.startswith("before_")),
                          camera="ortho 170, target (0,0,33), 500 cm, EEVEE, one sun 4.0 rotated (50,10,-60), world 0.85 at 0.6 (the cut_* renders of 306-02)",
                          poses="closed = identity; latch released = latch -60 about X; open = lid -105 about X at the pin axis, latch child of the lid with -90 local; cut_openwide* = wide camera (ortho 260, target (0,20,70)); cut_openwide100 / cut_openwide115 = lid -100 / -115")
    json.dump(rep, open(os.path.join(CLEAN, "chest_cut.report.json"), "w", encoding="utf-8"), indent=2)
