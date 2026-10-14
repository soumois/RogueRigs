"""Chaser HeadTorso: tapered block with bevelled corners, raked hood, one front panel with a dark tall recess + one red octagon eye, company logo decal (texture) on the top of the hood, low cap box and two low fins, three flat faceted back slabs, dark shoulder joints in a vertical channel. Pivot at the waist, parent Pelvis. Edit ONLY this file for the head and torso."""
import math
import os
import bpy
import numpy as np
import common as C
from common import h
from mathutils import Vector

PREVIEW = {"close": ("front", (25.0, 0.0, h(0.80)), 40.0)}

LOGO_SRC = "Art/Source/kessler-voss-logo/20261011-201828-kessler-voss-logo-1.png"
LOGO_BOX = (55, 412, 425, 782)      # emblem crop in source pixels (x0, y0 from the top, x1, y1), square, thin yellow plate border kept
LOGO_N = 256
LOGO_W = 17.6                       # decal width (cm, across) and length (cm, along the slope): the decal fills the flat top of the cap plate (17.8 x 21.9 cm)
LOGO_L = 21.4


def bil(a, b, c, d, s, t):
    """bilinear on a quad a(s=0,t=0) b(s=1,t=0) c(s=1,t=1) d(s=0,t=1)"""
    return a.co * (1 - s) * (1 - t) + b.co * s * (1 - t) + c.co * s * t + d.co * (1 - s) * t


def paint_logo():
    """Company logo as a texture: crop the emblem (charcoal asteroid with the KV monogram, thin yellow plate border) from the Kessler-Voss source image into a 256 px square and write it over T_ChaserLogo (made by common.setup). No logo geometry."""
    img = next((i for i in bpy.data.images if i.name.startswith("T_ChaserLogo")), None)
    if img is None:
        return
    root = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
    src = bpy.data.images.load(os.path.join(root, LOGO_SRC), check_existing=False)
    W_, H_ = src.size
    px = np.array(src.pixels[:], dtype=np.float32).reshape(H_, W_, 4)          # row 0 = bottom
    x0, y0, x1, y1 = LOGO_BOX
    crop = px[H_ - y1:H_ - y0, x0:x1].copy()
    ch_, cw_ = crop.shape[:2]
    body = np.array([int(C.HEX["yellow_body"][i:i + 2], 16) / 255 for i in (1, 3, 5)])      # encoded values, like the material colour in common.setup
    dark = np.array([26, 28, 34]) / 255
    al = np.clip((crop[:, :, 0] - 0.12) / 0.78, 0.0, 1.0)             # yellow-ness from the red channel (yellow 1.0, charcoal 0.1)
    rr, cc = np.mgrid[0:ch_, 0:cw_]
    for cy_ in (ch_ - 28, ch_ - 333):                                 # paint out the two left rivets: the plate border is symmetric
        al[(rr - cy_) ** 2 + (cc - 25) ** 2 < 34 ** 2] = 1.0
        al[(np.abs(rr - cy_) < 40) & (cc < 30)] = 1.0
    rgb = dark[None, None, :] * (1 - al[:, :, None]) + body[None, None, :] * al[:, :, None]
    CWd = 410                                                         # canvas with the decal aspect, body yellow plate, thin charcoal rim
    CHd = int(round(CWd * LOGO_L / LOGO_W))
    canvas = np.ones((CHd, CWd, 4))
    canvas[:, :, :3] = body
    oy, ox = (CHd - ch_) // 2, (CWd - cw_) // 2
    canvas[oy:oy + ch_, ox:ox + cw_, :3] = rgb
    RIM = 8
    canvas[:RIM, :, :3] = dark
    canvas[-RIM:, :, :3] = dark
    canvas[:, :RIM, :3] = dark
    canvas[:, -RIM:, :3] = dark
    tmp = bpy.data.images.new("logo_crop_tmp", CWd, CHd, alpha=False)
    tmp.pixels = canvas.flatten().tolist()
    tmp.scale(LOGO_N, LOGO_N)                                         # 256 x 256; the UV stretch of the decal restores the aspect
    img.scale(LOGO_N, LOGO_N)
    img.pixels = list(tmp.pixels)
    try:
        img.save()
    except Exception as ex:
        print("logo save", ex)
    bpy.data.images.remove(tmp)
    bpy.data.images.remove(src)


CAP = [(-9.0, 112.5), (-9.6, 117.0), (-7.4, 120.2), (-3.0, 120.4), (-3.0, 112.5)]          # small yellow shoulder cap behind and above the side plate (profile x, z)


def plate2(ht, prof, rings, mat, sg=1.0):
    """Back plate: rings across y = (y, top drop, bottom lift, barrel factor, rearward pull of the chamfer); the outer edge bows out at the top and curves in toward the bottom."""
    zmin, zmax = min(p[1] for p in prof), max(p[1] for p in prof)
    out = []
    for y, dT, dB, yb, xb in rings:
        ring = []
        for x, z in prof:
            f = (z - zmin) / (zmax - zmin)
            wT, wB = f ** 4, (1 - f) ** 4
            bow = yb * (0.5 * f ** 2 - 2.6 * (1 - f) ** 2)
            ring.append(ht.P(x + xb * (wT + wB), sg * (abs(y) + bow) if y > 0 else y, z - dT * wT + dB * wB))
        out.append(ring)
    ht.loft(out, mat)


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
            if li == CL and k == 1:
                continue            # lower bevel under the recess: built below, dark in the opening
            ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
    # back (round 7): cut the rear top of the block so that, from behind, the outline steps down beside the centre tower (large chamfer in plan and elevation); the front, the brow and the diagonal up to x 3 are not moved
    for li, (z5, z6, z7, dxr) in {0: (112.0, 112.0, 112.0, 3.0), 1: (113.5, 113.0, 112.5, 2.0), 2: (116.0, 114.5, 113.5, 1.0), 3: (120.5, 117.5, 200.0, 0.0)}.items():
        for lj in (li, 7 - li):
            for k, zt in ((5, z5), (6, z6), (7, z7)):
                v = L[lj][k]
                v.co.z = min(v.co.z, zt)
                if k >= 6:
                    v.co.x += dxr
    ht.quad(L[0][::-1], "yellow_body")
    ht.quad(L[-1], "yellow_body")

    # lower part of the front panel: open at the bottom, tall dark recess in gunmetal (walls and floor, non-emissive), two corner pieces left and right
    fa, fb, fc, fd = L[CL][2], L[CL + 1][2], L[CL + 1][3], L[CL][3]
    nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
    if nrm.x < 0:
        nrm = -nrm
    RS = (0.5 - 0.48, 0.5 + 0.48)
    RT = (0.0, 0.90)
    BACKV = Vector((-5.5, 0.0, 0.0))
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
        ht.quad((inner[k], inner[k + 1], mid[k + 1], mid[k]), "gunmetal")
        ht.quad((mid[k], mid[k + 1], floor[k + 1], floor[k]), "gunmetal")
    ht.quad(floor[::-1], "gunmetal")
    FC = sum((v.co for v in floor), Vector()) / 4
    ytan = (fb.co - fa.co).normalized()
    ztan = nrm.cross(ytan)
    if ztan.z < 0:
        ztan = -ztan
    EYE_C = FC + nrm * 0.3 + ztan * 1.2
    EYE_R = 3.6
    # bottom: the lower bevel under the opening is gunmetal between the recess edges (reads open), the two shell corners stay yellow
    A_, B_, C_, D_ = L[CL][1], L[CL][2], L[CL + 1][2], L[CL + 1][1]
    q0 = ht.bm.verts.new(bil(A_, D_, C_, B_, RS[0], 0.0))
    q1 = ht.bm.verts.new(bil(A_, D_, C_, B_, RS[1], 0.0))
    ht.quad((A_, q0, inner[0], B_), "yellow_body")
    ht.quad((q0, q1, inner[3], inner[0]), "gunmetal")
    ht.quad((q1, D_, C_, inner[3]), "yellow_body")
    # Y-shaped dark neck on the recess floor: a narrow vertical stem (3 cm) splitting into two short diagonal arms that end under the eye; raised 1 cm, stops below the eye plane
    zb = (sum((v.co for v in (floor[0], floor[3])), Vector()) / 2 - EYE_C).dot(ztan)
    zt, zj = -EYE_R * 0.92 - 0.1, -EYE_R * 0.92 - 2.4

    def yz(y_, z_, off):
        return ht.bm.verts.new(EYE_C - nrm * 0.3 + ytan * y_ + ztan * z_ + nrm * off)
    for pts in (((-1.5, zb), (1.5, zb), (1.5, zj), (-1.5, zj)),
                ((0.0, zj), (1.5, zj), (3.0, zt), (1.4, zt)),
                ((-1.5, zj), (0.0, zj), (-1.4, zt), (-3.0, zt))):
        ht.loft([[yz(y_, z_, 0.05) for y_, z_ in pts], [yz(y_, z_, 1.0) for y_, z_ in pts]], "gunmetal")
    # upper part of the front panel: plain yellow quad
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

    # back (round 6): a tall centre tower (53 % of the back width) with chamfered top and bottom corners, two barrel-shaped side plates (41 % of the tower) with slanted tops, tapered bottoms, one facet change at 55 % of the height, two small yellow shoulder caps; seams 0.3 cm, dark only in the seams
    BACK = [(-7.0, 85.0), (-9.8, 85.2), (-10.2, h(0.760)), (-9.8, h(0.850)), (-7.6, h(0.905)), (-5.0, h(0.905))]          # bottom raised to 85: no dark strip under the plates
    ht.profile(BACK, -8.6, 8.6, "gunmetal")
    TW = 7.9                                        # tower half width
    ZT = 123.0
    TOWER = [(-7.0, h(0.664)), (-10.6, 83.3), (-12.2, 104.7), (-10.4, 119.0), (-9.4, ZT), (-1.0, ZT)]       # facet change at 104.7 (55 % of 82.3..123)
    plate2(ht, TOWER, [(-TW, 1.6, 1.0, 0, 0.5), (-TW + 1.6, 0.0, 0.0, 0, 0.0), (TW - 1.6, 0.0, 0.0, 0, 0.0), (TW, 1.6, 1.0, 0, 0.5)], "yellow_body")
    SIDE = [(-7.0, h(0.664)), (-11.0, 83.2), (-12.4, 100.3), (-10.8, 115.0), (-5.0, 115.0)]
    y0 = TW + 0.3
    for sg in (1, -1):
        plate2(ht, SIDE, [(y0, 0.0, 0.0, 0.0, 0.0), (11.4, 1.0, 0.0, 0.4, 0.0), (13.8, 2.4, 0.5, 1.0, 0.4), (14.6, 3.4, 1.5, 1.0, 0.8)], "yellow_body", sg)
        plate2(ht, CAP, [(12.4, 0.0, 0.0, 0.0, 0.0), (14.0, 1.6, 0.0, 0.0, 0.4), (14.6, 2.6, 0.0, 0.0, 0.8)], "yellow_body", sg)

    # shoulder joints: small dark octagonal masses sunk behind the arm line, in a vertical dark channel
    for sg in (1, -1):
        ht.abox(-9.0, -1.0, sg * (W - 0.3), sg * (W + 0.9), h(0.80), h(0.90), "gunmetal")
        ht.oct_y(-6.5, sg * (W - 0.3), h(0.835), 5.4, 3.4, "gunmetal")

    # logo decal on the hood top: one quad 0.25 cm above the flat top of the cap plate, one planar UV, up toward the robot back (-X)
    pa = Vector((xb - 0.5, 0.0, zd(xb - 0.5) + 0.8))
    pb = Vector((xa + 0.5, 0.0, zd(xa + 0.5) + 0.8))
    cen = (pa + pb) / 2
    up = (pb - pa).normalized()
    right = Vector((0, 1, 0))
    dn = right.cross(up).normalized()
    if dn.z < 0:
        dn = -dn

    def post(bm, uv0):
        """one solid eye octagon plus a core octagon (both eye_red) on the recess floor; then the logo decal quad (returned)"""
        for R, off in ((EYE_R, 0.0), (EYE_R * 0.5, 0.25)):
            ev_ = [bm.verts.new(EYE_C + nrm * off + ytan * R * math.cos(math.radians(22.5 + 45 * k)) + ztan * R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
            f = bm.faces.new(ev_)
            if f.normal.dot(nrm) < 0:
                f.normal_flip()
            f.material_index = C.MI["eye_red"]
        c = cen + dn * 0.25
        vs = [bm.verts.new(c + right * sx * LOGO_W / 2 + up * sy * LOGO_L / 2) for sx, sy in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
        f = bm.faces.new(vs)
        if f.normal.dot(dn) < 0:
            f.normal_flip()
        f.material_index = C.MI["logo_plate"]
        for l in f.loops:
            l[uv0].uv = (0.5 + (l.vert.co - c).dot(right) / LOGO_W, 0.5 + (l.vert.co - c).dot(up) / LOGO_L)
        return f
    ht.post = post
    return C.finish(ht)
