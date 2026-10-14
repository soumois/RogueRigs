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
    u, v = np.abs(s - 0.5), np.abs(t - 0.58)
    star = (u ** 0.62 + v ** 0.62) < 0.36 ** 0.62          # large four-point emblem
    stem = (u < 0.055 * (1.0 + (0.5 - t) * 0.0)) & (t < 0.58) & (t > 0.14)      # stem down to the shield tip
    out[(star | stem) & inner, :3] = plate
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
    PROF = [(-8.4, h(0.655)), (24.0, h(0.668)), (27.5, h(0.690)), (31.1, h(0.780)), (32.5, h(0.835)), (0.0, h(0.984)), (-5.5, h(0.965)), (-8.4, h(0.925)), (-8.9, h(0.840))]
    # 2-3-4 = near-upright front panel (a slight crease at 3: recess below, logo above), 4 = brow, 4-5 = one long diagonal hood up to the rear top, 1-2 = bevel of the lower front edge
    cxz = (sum(p[0] for p in PROF) / len(PROF), sum(p[1] for p in PROF) / len(PROF))
    LAY = [(-W, 0.78), (-W + 1.4, 0.88), (-W + 3.0, 0.96), (-W + 5.0, 1.0), (W - 5.0, 1.0), (W - 3.0, 0.96), (W - 1.4, 0.88), (W, 0.78)]      # angled chest planes at the corners
    CL = 3          # first centre layer
    zlo, zhi = h(0.655), h(0.984)
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
            if li == CL and k in SPECIAL:
                continue
            ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
    ht.quad(L[0][::-1], "yellow_body")
    ht.quad(L[-1], "yellow_body")

    # lower part of the front panel: open at the bottom, tall recess lit red (walls and floor), two corner pieces left and right
    fa, fb, fc, fd = L[CL][2], L[CL + 1][2], L[CL + 1][3], L[CL][3]
    nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
    if nrm.x < 0:
        nrm = -nrm
    RS = (0.5 - 0.48, 0.5 + 0.48)
    RT = (0.0, 0.90)
    BACKV = Vector((-4.0, 0.0, 0.0))
    outer = [fa, fd, fc, fb]

    def ring(shs, back):
        sm = (RS[0] + RS[1]) / 2
        pts = [(RS[0], RT[0]), (RS[0], RT[1]), (RS[1], RT[1]), (RS[1], RT[0])]
        return [ht.bm.verts.new(bil(fa, fb, fc, fd, sm + (s_ - sm) * shs, t_) + BACKV * back) for s_, t_ in pts]
    inner = ring(1.0, 0.0)
    mid = ring(0.97, 0.45)
    floor = ring(0.90, 1.0)
    for k in range(3):                 # left, top, right; no sill at the bottom
        ht.quad((outer[k], outer[k + 1], inner[k + 1], inner[k]), "yellow_body")
        ht.quad((inner[k], inner[k + 1], mid[k + 1], mid[k]), "eye_red")
        ht.quad((mid[k], mid[k + 1], floor[k + 1], floor[k]), "eye_red")
    ht.quad(floor[::-1], "eye_red")
    FC = sum((v.co for v in floor), Vector()) / 4
    ytan = (fb.co - fa.co).normalized()
    ztan = nrm.cross(ytan)
    if ztan.z < 0:
        ztan = -ztan
    EYE_C = FC + nrm * 0.3 + ztan * 1.2
    EYE_R = 4.0
    # chin: narrow dark stem below the eye, and a small red-lit lip jutting forward at the bottom
    zf = min(fa.co.z, fb.co.z)
    xf = fa.co.x
    ht.abox(xf - 3.4, xf + 1.2, -1.6, 1.6, zf - 1.4, zf + 2.4, "gunmetal")
    ht.abox(xf - 0.5, xf + 2.6, -6.0, 6.0, zf - 1.6, zf + 0.5, "eye_red")
    # upper part of the front panel: plain yellow quad (plate and shield sit on it)
    ua, ub, uc, ud = L[CL][3], L[CL + 1][3], L[CL + 1][4], L[CL][4]
    ht.quad((ua, ub, uc, ud), "yellow_body")
    unrm = (ub.co - ua.co).cross(ud.co - ua.co).normalized()
    if unrm.x < 0:
        unrm = -unrm

    # cap plate: very shallow raised plate on the hood diagonal, vertical front, chamfered top corners
    z4, z5 = h(0.835), h(0.984)

    def zd(x):
        return z5 + (z4 - z5) * x / 32.5
    xa, xb, hw = 3.0, 22.0, h(0.076)
    rg = []
    for yy, ch in ((-hw, 0.5), (-hw + 0.5, 0.0), (hw - 0.5, 0.0), (hw, 0.5)):
        pts = [(xb, zd(xb) - 0.8), (xb, zd(xb) + 0.8 - ch), (xb - ch, zd(xb) + 0.8), (xa + ch, zd(xa) + 0.8), (xa, zd(xa) + 0.8 - ch), (xa, zd(xa) - 0.8)]
        rg.append([ht.P(x, yy, z) for x, z in pts])
    ht.loft(rg, "yellow_body")
    # two small dark nubs at the upper back corners
    for sg in (1, -1):
        a0 = Vector((-2.5, sg * h(0.100), h(0.940)))
        tip = Vector((-2.8, sg * h(0.104), h(0.940) + 3.0))
        ax = (tip - a0).normalized()
        lat = Vector((0, 1, 0))
        dep = ax.cross(lat).normalized()
        rings_ = []
        for c_, w_, d_ in ((a0, 2.4, 1.4), (tip, 1.4, 0.8)):
            hw_, hd_ = w_ / 2, d_ / 2
            rings_.append([ht.bm.verts.new(c_ + lat * u_ + dep * v_) for u_, v_ in ((hw_, -hd_), (hw_, hd_), (-hw_, hd_), (-hw_, -hd_))])
        ht.loft(rings_, "gunmetal")

    # back: three flat faceted slabs, 0.3 cm seams, wide side slabs (about 40 % of the centre one), faceted bottoms right above the waist
    BACK = [(-7.0, h(0.664)), (-9.8, h(0.668)), (-10.2, h(0.760)), (-9.8, h(0.850)), (-7.6, h(0.905)), (-5.0, h(0.905))]
    ht.profile(BACK, -14.6, 14.6, "gunmetal")
    CEN = [(-7.0, h(0.664)), (-10.4, h(0.672)), (-11.6, h(0.740)), (-11.3, h(0.860)), (-9.2, h(0.930)), (-6.0, h(0.975)), (-3.0, h(0.975))]
    SIDE = [(-7.0, h(0.664)), (-11.2, h(0.672)), (-12.4, h(0.745)), (-12.0, h(0.850)), (-9.0, h(0.900)), (-6.0, h(0.920)), (-5.0, h(0.920))]
    cw = h(0.060)
    slab(ht, CEN, [(-cw, 0.6, 0.93), (-cw + 0.5, 0.0, 1.0), (cw - 0.5, 0.0, 1.0), (cw, 0.6, 0.93)])
    y0, y1 = cw + 0.3, 14.9
    for sg in (1, -1):
        slab(ht, SIDE, [(sg * y0, 0.6, 0.93), (sg * (y0 + 0.5), 0.0, 1.0), (sg * (y1 - 1.0), 0.0, 1.0), (sg * y1, 3.0, 0.88)])

    # shoulder joints: small dark octagonal masses sunk behind the arm line, in a vertical dark channel
    for sg in (1, -1):
        ht.abox(-9.0, -1.0, sg * (W - 0.3), sg * (W + 0.9), h(0.68), h(0.90), "gunmetal")
        ht.oct_y(-6.5, sg * (W - 0.3), h(0.835), 5.4, 3.4, "gunmetal")

    def post(bm, uv0):
        """eye octagon on a dark socket plus a core octagon (emissive) on the recess floor, yellow plate and shield decal (one planar UV); returns the shield face"""
        for R, off, mat in ((EYE_R + 0.8, 0.12, "gunmetal"), (EYE_R, 0.3, "eye_red"), (EYE_R * 0.5, 0.55, "eye_red")):
            ev_ = [bm.verts.new(EYE_C + nrm * (off - 0.3) + ytan * R * math.cos(math.radians(22.5 + 45 * k)) + ztan * R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
            f = bm.faces.new(ev_)
            if f.normal.dot(nrm) < 0:
                f.normal_flip()
            f.material_index = C.MI[mat]

        def poly(k, off, uvs=False):
            vs = [bm.verts.new(bil(ua, ub, uc, ud, 0.5 + (s_ - 0.5) * 0.40 * k, 0.5 + (t_ - 0.5) * 0.72 * k) + unrm * off) for s_, t_ in SHIELD]
            fc_ = bm.faces.new(vs)
            if fc_.normal.dot(unrm) < 0:
                fc_.normal_flip()
            fc_.material_index = C.MI["logo_plate"]
            if uvs:
                for l in fc_.loops:
                    l[uv0].uv = SHIELD[vs.index(l.vert)]
            return fc_
        poly(1.10, 0.15)                 # plate under the shield (UV0 left at 0,0: plate yellow)
        return poly(1.0, 0.32, True)
    ht.post = post
    return C.finish(ht)
