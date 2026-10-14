"""Chaser HeadTorso: chunky block with bevelled corners, deep hood, recess + eye, shield logo on a plate, cap box with rounded edges and two antennas, three bulged back plates in a dark backing, dark shoulder pods. Pivot at the waist, parent Pelvis. Edit ONLY this file for the head and torso."""
import math
import bpy
import numpy as np
import common as C
from common import h
from mathutils import Vector

PREVIEW = {"close": ("front", (25.0, 0.0, h(0.80)), 40.0)}

SHIELD = [(0.07, 0.98), (0.93, 0.98), (1.0, 0.62), (0.5, 0.02), (0.0, 0.62)]      # logo shield outline in (s, t) of the decal texture, convex, point down


def bil(a, b, c, d, s, t):
    """bilinear on a quad a(s=0,t=0) b(s=1,t=0) c(s=1,t=1) d(s=0,t=1)"""
    return a.co * (1 - s) * (1 - t) + b.co * s * (1 - t) + c.co * s * t + d.co * (1 - s) * t


def paint_logo():
    """The torso owns the logo look: overwrite T_ChaserLogo (made by common.setup) with a gunmetal-edged dark shield and a yellow star on the plate yellow, 256 px."""
    img = bpy.data.images.get("T_ChaserLogo")
    if img is None:
        return
    N = 256
    row, col = np.mgrid[0:N, 0:N]
    s = (col + 0.5) / N
    t = (row + 0.5) / N          # row 0 = bottom in Blender

    def inside(poly, k):
        cx = sum(p[0] for p in poly) / len(poly)
        cy = sum(p[1] for p in poly) / len(poly)
        pts = [(cx + (x - cx) * k, cy + (y - cy) * k) for x, y in poly]
        m = np.ones((N, N), bool)
        for i in range(len(pts)):
            x0, y0 = pts[i]
            x1, y1 = pts[(i + 1) % len(pts)]
            m &= ((x1 - x0) * (t - y0) - (y1 - y0) * (s - x0)) <= 0          # clockwise outline: inside = right of every edge
        return m
    plate = np.array(C.srgb(C.HEX["logo_plate"])[:3])
    rim = np.array(C.srgb("#555555")[:3])
    dark = np.array(C.srgb("#1C1C1C")[:3])
    out = np.ones((N, N, 4))
    out[:, :, :3] = plate
    sh = inside(SHIELD, 1.0)
    out[sh, :3] = rim
    out[inside(SHIELD, 0.82), :3] = dark
    u, v = np.abs(s - 0.5), np.abs(t - 0.56)
    star = (u ** 0.62 + v ** 0.62) < 0.27 ** 0.62
    out[star & inside(SHIELD, 0.82), :3] = plate
    img.scale(N, N)
    img.pixels = out.flatten().tolist()
    try:
        img.save()
    except Exception as ex:
        print("logo save", ex)


def plate_rings(ht, prof, y0, y1, ztop_scale, bev, bulge, extra_in_outer=0.0, outer_at_y1=True):
    """Bulged back plate: rings across y. Each ring: x shift toward the block (dx) and z scale about the profile centre. Rounded corners come from the end rings."""
    zc = sum(p[1] for p in prof) / len(prof)
    ym = (y0 + y1) / 2
    spec = [(y0, bev, 0.90), (y0 + bev * 0.8, 0.10, 0.975), (ym, -bulge, 1.0), (y1 - bev * 0.8, 0.10, 0.975), (y1, bev, 0.90)]
    rg = []
    for y, dx, zs in spec:
        out_end = (y == y1) if outer_at_y1 else (y == y0)
        near_out = (y in (y1, y1 - bev * 0.8)) if outer_at_y1 else (y in (y0, y0 + bev * 0.8))
        d2 = dx + (extra_in_outer if near_out else 0.0)
        rg.append([ht.P(x + d2 * (1 if 0 < i < len(prof) - 1 or True else 0), y, zc + (z - zc) * zs) for i, (x, z) in enumerate(prof)])
    ht.loft(rg, "yellow_body")


def build(side=None):
    paint_logo()
    ht = C.Part("HeadTorso", C.PIV_WAIST, parent="Pelvis")
    W = h(0.120)            # block half width
    PROF = [(-8.4, h(0.655)), (19.5, h(0.672)), (24.2, h(0.696)), (33.5, h(0.790)), (26.5, h(0.865)), (9.0, h(0.925)), (-3.5, h(0.925)), (-6.9, h(0.903)), (-8.9, h(0.840))]
    # 1-2 = bevel of the lower front corner, 2-3 = lower front face (recess), 3-4 = upper front face (logo), 4-5 = long hood slope, 6-7 = bevel of the rear top corner
    cxz = (sum(p[0] for p in PROF) / len(PROF), sum(p[1] for p in PROF) / len(PROF))
    LAY = [(-W, 0.80), (-W + 1.8, 0.93), (-W + 4.2, 1.0), (W - 4.2, 1.0), (W - 1.8, 0.93), (W, 0.80)]      # rounded shoulder line: three steps per side
    L = []
    for y, sc in LAY:
        L.append([ht.bm.verts.new((cxz[0] + (x - cxz[0]) * sc, y, cxz[1] + (z - cxz[1]) * sc)) for x, z in PROF])
    n = len(PROF)
    SPECIAL = (2, 3)
    for li in range(len(LAY) - 1):
        for k in range(n):
            if li == 2 and k in SPECIAL:
                continue
            ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
    ht.quad(L[0][::-1], "yellow_body")
    ht.quad(L[-1], "yellow_body")

    # lower front face: frame + tapered recess walls + floor
    fa, fb, fc, fd = L[2][2], L[3][2], L[3][3], L[2][3]            # bottom-left, bottom-right, top-right, top-left (y -, y +)
    nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
    if nrm.x < 0:
        nrm = -nrm
    RS = (0.5 - 0.388, 0.5 + 0.388)    # recess across 0.776 of the centre face width
    RT = (0.12, 0.70)
    DEPTH = 3.5
    outer = [fa, fd, fc, fb]

    def ring(shrink, depth):
        sm, tm = (RS[0] + RS[1]) / 2, (RT[0] + RT[1]) / 2
        pts = [(RS[0], RT[0]), (RS[0], RT[1]), (RS[1], RT[1]), (RS[1], RT[0])]
        return [ht.bm.verts.new(bil(fa, fb, fc, fd, sm + (s_ - sm) * shrink, tm + (t_ - tm) * shrink) - nrm * depth) for s_, t_ in pts]
    inner = ring(1.0, 0.0)
    mid = ring(1.03, 1.6)
    floor = ring(0.84, DEPTH)
    for k in range(4):
        ht.quad((outer[k], outer[(k + 1) % 4], inner[(k + 1) % 4], inner[k]), "yellow_body")
        ht.quad((inner[k], inner[(k + 1) % 4], mid[(k + 1) % 4], mid[k]), "gunmetal")
        ht.quad((mid[k], mid[(k + 1) % 4], floor[(k + 1) % 4], floor[k]), "gunmetal")
    ht.quad(floor[::-1], "gunmetal")
    EYE_C = sum((v.co for v in floor), Vector()) / 4 + nrm * 0.2
    EYE_R = 3.2
    fa_, fb_ = L[2][2].co, L[3][2].co
    ytan = (fb_ - fa_).normalized()
    ztan = nrm.cross(ytan)
    if ztan.z < 0:
        ztan = -ztan
    # upper front face: plain yellow quad (the plate and the shield sit on top of it)
    ua, ub, uc, ud = L[2][3], L[3][3], L[3][4], L[2][4]
    ht.quad((ua, ub, uc, ud), "yellow_body")
    unrm = (ub.co - ua.co).cross(ud.co - ua.co).normalized()
    if unrm.x < 0:
        unrm = -unrm

    # cap: box on the hood, bevelled top edges (rings: z, x back, x front, half width)
    CAP = [(0.860, -7.5, 24.5, 0.086), (0.930, -6.0, 21.0, 0.086), (0.985, -1.6, 15.6, 0.084), (1.000, 0.2, 13.8, 0.066)]
    rg = []
    for z, x0, x1, hw in CAP:
        rg.append([ht.bm.verts.new(p) for p in ((x0, -h(hw), h(z)), (x1, -h(hw), h(z)), (x1, h(hw), h(z)), (x0, h(hw), h(z)))])
    ht.loft(rg, "yellow_body")
    # two small antennas, dark, beside the cap on the rear top corners, leaning outward
    for sg in (1, -1):
        for _ in range(1):
            ht.bm.verts.ensure_lookup_table()
        base = Vector((-2.5, sg * h(0.093), h(0.912)))
        tip = Vector((-2.8, sg * h(0.100), h(0.960)))
        a0 = base
        ax = (tip - a0).normalized()
        lat = Vector((0, 1, 0))
        dep = ax.cross(lat).normalized()
        rings_ = []
        for c_, w_, d_ in ((a0, 1.9, 1.5), (tip, 0.9, 0.8)):
            hw_, hd_ = w_ / 2, d_ / 2
            rings_.append([ht.bm.verts.new(c_ + lat * u_ + dep * v_) for u_, v_ in ((hw_, -hd_), (hw_, hd_), (-hw_, hd_), (-hw_, -hd_))])
        ht.loft(rings_, "gunmetal")

    # back: dark backing slab, then three bulged plates (centre tall, two sides) with rounded corners and 0.4 cm gaps
    BACK = [(-7.0, h(0.685)), (-9.8, h(0.690)), (-10.2, h(0.760)), (-9.8, h(0.850)), (-7.6, h(0.905)), (-5.0, h(0.905))]
    ht.profile(BACK, -(h(0.112) + 0.5), h(0.112) + 0.5, "gunmetal")
    CEN = [(-7.0, h(0.685)), (-10.6, h(0.695)), (-11.6, h(0.760)), (-11.3, h(0.860)), (-9.2, h(0.925)), (-6.0, h(0.975)), (-3.0, h(0.975))]
    SIDE = [(-7.0, h(0.685)), (-11.4, h(0.695)), (-12.4, h(0.760)), (-12.0, h(0.850)), (-9.0, h(0.900)), (-6.0, h(0.915)), (-5.0, h(0.915))]
    GAP = 0.4
    cw = h(0.058)
    plate_rings(ht, CEN, -cw, cw, 1.0, 1.5, 0.7)
    sw0, sw1 = cw + GAP, h(0.112)
    plate_rings(ht, SIDE, sw0, sw1, 1.0, 1.4, 0.7, extra_in_outer=1.8, outer_at_y1=True)
    plate_rings(ht, SIDE, -sw1, -sw0, 1.0, 1.4, 0.7, extra_in_outer=1.8, outer_at_y1=False)

    # dark shoulder pods behind the arms: disc on the side of the block and a vertical band behind it
    for sg in (1, -1):
        ht.oct_y(-3.0, sg * (W + 0.1), h(0.835), 7.4, 3.2, "gunmetal")
        ht.abox(-10.0, -2.0, sg * (W - 1.0), sg * (W + 0.9), h(0.69), h(0.895), "gunmetal")

    def post(bm, uv0):
        """eye octagon (emissive) on the recess floor, yellow plate and shield decal (one planar UV); returns the shield face"""
        ev_ = [bm.verts.new(EYE_C + ytan * EYE_R * math.cos(math.radians(22.5 + 45 * k)) + ztan * EYE_R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
        f = bm.faces.new(ev_)
        if f.normal.dot(nrm) < 0:
            f.normal_flip()
        f.material_index = C.MI["eye_red"]

        def poly(k, off, uvs=False):
            vs = [bm.verts.new(bil(ua, ub, uc, ud, 0.5 + (s_ - 0.5) * 0.375 * k, 0.5 + (t_ - 0.5) * 0.56 * k) + unrm * off) for s_, t_ in SHIELD]
            fc_ = bm.faces.new(vs)
            if fc_.normal.dot(unrm) < 0:
                fc_.normal_flip()
            fc_.material_index = C.MI["logo_plate"]
            if uvs:
                for l in fc_.loops:
                    l[uv0].uv = SHIELD[vs.index(l.vert)]
            return fc_
        poly(1.22, 0.15)                 # plate under the shield (UV0 left at 0,0: plate yellow)
        return poly(1.0, 0.32, True)
    ht.post = post
    return C.finish(ht)
