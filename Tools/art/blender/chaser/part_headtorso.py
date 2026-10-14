"""Chaser HeadTorso: tapered block with bevelled corners, raked hood, one front panel with a tall recess + eye, shield logo on a plate, low cap box and two low fins, three flat faceted back slabs, dark shoulder joints in a vertical channel. Pivot at the waist, parent Pelvis. Edit ONLY this file for the head and torso."""
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
    """The torso owns the logo look: overwrite T_ChaserLogo (made by common.setup) with a dark shield (gunmetal rim) and a yellow four-point emblem, plate yellow outside, 256 px."""
    img = next((i for i in bpy.data.images if i.name.startswith("T_ChaserLogo")), None)
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

    def raw(hx):
        return np.array([int(hx[i:i + 2], 16) / 255 for i in (1, 3, 5)])      # encoded values, like common.setup
    plate = raw(C.HEX["logo_plate"])
    rim = raw("#5A5A5A")
    dark = raw("#1A1A1A")
    out = np.ones((N, N, 4))
    out[:, :, :3] = plate
    out[inside(SHIELD, 1.0), :3] = rim
    inner = inside(SHIELD, 0.84)
    out[inner, :3] = dark
    u, v = np.abs(s - 0.5), np.abs(t - 0.56)
    star = (u ** 0.62 + v ** 0.62) < 0.25 ** 0.62          # four-point emblem
    out[star & inner, :3] = plate
    img.scale(N, N)
    img.pixels = out.flatten().tolist()
    try:
        img.save()
    except Exception as ex:
        print("logo save", ex)


def slab(ht, prof, spec):
    """Flat faceted back slab: rings across y; spec = (y, dx toward the block, z scale about the profile centre)."""
    zc = sum(p[1] for p in prof) / len(prof)
    ht.loft([[ht.P(x + dx, y, zc + (z - zc) * zs) for x, z in prof] for y, dx, zs in spec], "yellow_body")


def build(side=None):
    paint_logo()
    ht = C.Part("HeadTorso", C.PIV_WAIST, parent="Pelvis")
    W = h(0.120)            # block half width
    PROF = [(-8.4, h(0.655)), (19.5, h(0.672)), (24.2, h(0.696)), (29.0, h(0.7823)), (33.5, h(0.862)), (9.0, h(0.925)), (-3.5, h(0.925)), (-6.9, h(0.903)), (-8.9, h(0.840))]
    # 2-3-4 are collinear: one front panel (recess below the split, logo above it); 4 = leading point of the hood; 4-5 = raked hood; 1-2 and 6-7 = bevels
    cxz = (sum(p[0] for p in PROF) / len(PROF), sum(p[1] for p in PROF) / len(PROF))
    LAY = [(-W, 0.80), (-W + 1.8, 0.93), (-W + 4.2, 1.0), (W - 4.2, 1.0), (W - 1.8, 0.93), (W, 0.80)]      # rounded shoulder line: three steps per side
    zlo, zhi = h(0.655), h(0.925)
    L = []
    for y, sc in LAY:
        row_ = []
        for x, z in PROF:
            zz = cxz[1] + (z - cxz[1]) * sc
            taper = 0.95 + 0.05 * min(1.0, max(0.0, (zz - zlo) / (zhi - zlo)))      # slightly narrower at the bottom
            row_.append(ht.bm.verts.new((cxz[0] + (x - cxz[0]) * sc, y * taper, zz)))
        L.append(row_)
    n = len(PROF)
    SPECIAL = (2, 3)
    for li in range(len(LAY) - 1):
        for k in range(n):
            if li == 2 and k in SPECIAL:
                continue
            ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
    ht.quad(L[0][::-1], "yellow_body")
    ht.quad(L[-1], "yellow_body")

    # lower part of the front panel: frame + tall recess (walls go straight back) + floor
    fa, fb, fc, fd = L[2][2], L[3][2], L[3][3], L[2][3]
    nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
    if nrm.x < 0:
        nrm = -nrm
    RS = (0.5 - 0.47, 0.5 + 0.47)
    RT = (0.10, 0.88)
    BACKV = Vector((-4.0, 0.0, 0.0))
    outer = [fa, fd, fc, fb]

    def ring(shrink, back):
        sm, tm = (RS[0] + RS[1]) / 2, (RT[0] + RT[1]) / 2
        pts = [(RS[0], RT[0]), (RS[0], RT[1]), (RS[1], RT[1]), (RS[1], RT[0])]
        return [ht.bm.verts.new(bil(fa, fb, fc, fd, sm + (s_ - sm) * shrink, tm + (t_ - tm) * shrink) + BACKV * back) for s_, t_ in pts]
    inner = ring(1.0, 0.0)
    mid = ring(0.97, 0.45)
    floor = ring(0.90, 1.0)
    for k in range(4):
        ht.quad((outer[k], outer[(k + 1) % 4], inner[(k + 1) % 4], inner[k]), "yellow_body")
        ht.quad((inner[k], inner[(k + 1) % 4], mid[(k + 1) % 4], mid[k]), "gunmetal")
        ht.quad((mid[k], mid[(k + 1) % 4], floor[(k + 1) % 4], floor[k]), "gunmetal")
    ht.quad(floor[::-1], "gunmetal")
    FC = sum((v.co for v in floor), Vector()) / 4
    EYE_C = FC + nrm * 0.3
    EYE_R = 4.0
    ytan = (fb.co - fa.co).normalized()
    ztan = nrm.cross(ytan)
    if ztan.z < 0:
        ztan = -ztan
    # chin piece: dark block at the bottom centre of the recess, standing on the floor
    zb = min(v.co.z for v in floor)
    xb = min(v.co.x for v in floor)
    ht.abox(xb - 0.2, xb + 3.2, -4.5, 4.5, zb - 0.3, zb + 2.4, "gunmetal")
    # upper part of the front panel: plain yellow quad (plate and shield sit on it)
    ua, ub, uc, ud = L[2][3], L[3][3], L[3][4], L[2][4]
    ht.quad((ua, ub, uc, ud), "yellow_body")
    unrm = (ub.co - ua.co).cross(ud.co - ua.co).normalized()
    if unrm.x < 0:
        unrm = -unrm

    # cap: low box about as wide as the centre back slab, small chamfers (rings: z, x back, x front, half width)
    CAP = [(0.860, -7.5, 21.5, 0.078), (0.925, -6.5, 19.5, 0.078), (0.965, -3.0, 15.5, 0.076), (0.984, -1.6, 13.4, 0.066)]
    rg = []
    for z, x0, x1, hw in CAP:
        rg.append([ht.bm.verts.new(p) for p in ((x0, -h(hw), h(z)), (x1, -h(hw), h(z)), (x1, h(hw), h(z)), (x0, h(hw), h(z)))])
    ht.loft(rg, "yellow_body")
    # two short low fins at the upper back corners
    for sg in (1, -1):
        a0 = Vector((-2.5, sg * h(0.090), h(0.915)))
        tip = Vector((-2.8, sg * h(0.096), h(0.945)))
        ax = (tip - a0).normalized()
        lat = Vector((0, 1, 0))
        dep = ax.cross(lat).normalized()
        rings_ = []
        for c_, w_, d_ in ((a0, 2.6, 1.4), (tip, 1.6, 0.8)):
            hw_, hd_ = w_ / 2, d_ / 2
            rings_.append([ht.bm.verts.new(c_ + lat * u_ + dep * v_) for u_, v_ in ((hw_, -hd_), (hw_, hd_), (-hw_, hd_), (-hw_, -hd_))])
        ht.loft(rings_, "gunmetal")

    # back: three flat faceted slabs, 0.3 cm seams, side slabs wrap the corners; a dark backing only shows in the seams
    BACK = [(-7.0, h(0.685)), (-9.8, h(0.690)), (-10.2, h(0.760)), (-9.8, h(0.850)), (-7.6, h(0.905)), (-5.0, h(0.905))]
    ht.profile(BACK, -14.6, 14.6, "gunmetal")
    CEN = [(-7.0, h(0.685)), (-10.6, h(0.695)), (-11.6, h(0.760)), (-11.3, h(0.860)), (-9.2, h(0.925)), (-6.0, h(0.960)), (-3.0, h(0.960))]
    SIDE = [(-7.0, h(0.685)), (-11.4, h(0.695)), (-12.4, h(0.760)), (-12.0, h(0.850)), (-9.0, h(0.900)), (-6.0, h(0.915)), (-5.0, h(0.915))]
    cw = h(0.068)
    slab(ht, CEN, [(-cw, 0.6, 0.93), (-cw + 0.5, 0.0, 1.0), (cw - 0.5, 0.0, 1.0), (cw, 0.6, 0.93)])
    y0, y1 = cw + 0.3, 14.9
    for sg in (1, -1):
        slab(ht, SIDE, [(sg * y0, 0.6, 0.93), (sg * (y0 + 0.5), 0.0, 1.0), (sg * (y1 - 3.2), 0.0, 1.0), (sg * y1, 4.5, 0.88)])

    # shoulder joints: thick dark octagonal masses seated in a vertical dark channel
    for sg in (1, -1):
        ht.abox(-9.0, -1.0, sg * (W - 0.3), sg * (W + 0.9), h(0.68), h(0.90), "gunmetal")
        ht.oct_y(-4.5, sg * (W - 0.1), h(0.835), 7.0, 3.6, "gunmetal")
        ht.oct_y(-4.5, sg * (W + 1.2), h(0.835), 5.2, 1.4, "gunmetal")

    def post(bm, uv0):
        """eye octagon plus a smaller core octagon (emissive) on the recess floor, yellow plate and shield decal (one planar UV); returns the shield face"""
        for R, off in ((EYE_R, 0.0), (EYE_R * 0.5, 0.25)):
            ev_ = [bm.verts.new(EYE_C + nrm * off + ytan * R * math.cos(math.radians(22.5 + 45 * k)) + ztan * R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
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
