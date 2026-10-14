"""Chaser HeadTorso: chunky block, hood front, cap, recess + eye, logo decal, three back plates. Pivot at the waist, parent Pelvis. Edit ONLY this file for the head and torso."""
import math
import common as C
from common import h
from mathutils import Vector

PREVIEW = {"close": ("front", (25.0, 0.0, h(0.80)), 40.0)}


def bil(a, b, c, d, s, t):
    """bilinear on a quad a(s=0,t=0) b(s=1,t=0) c(s=1,t=1) d(s=0,t=1)"""
    return a.co * (1 - s) * (1 - t) + b.co * s * (1 - t) + c.co * s * t + d.co * (1 - s) * t


def build(side=None):
    ht = C.Part("HeadTorso", C.PIV_WAIST, parent="Pelvis")
    W = h(0.120)            # block half width
    PROF = [(-8.4, h(0.655)), (22.5, h(0.672)), (32.2, h(0.840)), (27.4, h(0.903)), (20.5, h(0.925)), (-5.7, h(0.925)), (-8.9, h(0.840))]      # (27.4, .903) = chamfered top edge of the hood
    cxz = (sum(p[0] for p in PROF) / len(PROF), sum(p[1] for p in PROF) / len(PROF))
    LAY = [(-W, 0.80), (-W + 1.8, 0.93), (-W + 4.2, 1.0), (W - 4.2, 1.0), (W - 1.8, 0.93), (W, 0.80)]      # rounded shoulder line: three steps per side
    L = []
    for y, sc in LAY:
        L.append([ht.bm.verts.new((cxz[0] + (x - cxz[0]) * sc, y, cxz[1] + (z - cxz[1]) * sc)) for x, z in PROF])
    n = len(PROF)
    SPECIAL = (1, 2)        # edge 1 = lower front face (recess), edge 2 = upper front face (logo)
    for li in range(len(LAY) - 1):
        for k in range(n):
            if li == 2 and k in SPECIAL:
                continue
            ht.quad((L[li][k], L[li][(k + 1) % n], L[li + 1][(k + 1) % n], L[li + 1][k]), "yellow_body")
    ht.quad(L[0][::-1], "yellow_body")
    ht.quad(L[-1], "yellow_body")

    # lower front face: frame + recess + floor
    fa, fb, fc, fd = L[2][1], L[3][1], L[3][2], L[2][2]            # bottom-left, bottom-right, top-right, top-left (y -, y +)
    nrm = (fb.co - fa.co).cross(fd.co - fa.co).normalized()
    if nrm.x < 0:
        nrm = -nrm
    RS = (0.5 - 0.388, 0.5 + 0.388)    # recess across 0.776 of the centre face width = 62 % of the block width
    RT = (0.12, 0.70)
    DEPTH = 3.5
    outer = [fa, fd, fc, fb]
    st = [(RS[0], RT[0]), (RS[0], RT[1]), (RS[1], RT[1]), (RS[1], RT[0])]
    inner = [ht.bm.verts.new(bil(fa, fb, fc, fd, s_, t_)) for s_, t_ in st]
    floor = [ht.bm.verts.new(bil(fa, fb, fc, fd, s_, t_) - nrm * DEPTH) for s_, t_ in st]
    for k in range(4):
        ht.quad((outer[k], outer[(k + 1) % 4], inner[(k + 1) % 4], inner[k]), "yellow_body")
        ht.quad((inner[k], inner[(k + 1) % 4], floor[(k + 1) % 4], floor[k]), "gunmetal")
    ht.quad(floor[::-1], "gunmetal")
    EYE_C = sum((v.co for v in floor), Vector()) / 4 + nrm * 0.2
    EYE_R = 3.2
    fa_, fb_ = L[2][1].co, L[3][1].co
    ytan = (fb_ - fa_).normalized()
    ztan = nrm.cross(ytan)
    if ztan.z < 0:
        ztan = -ztan
    # upper front face: plain yellow quad (the logo decal sits on top of it)
    ua, ub, uc, ud = L[2][2], L[3][2], L[3][3], L[2][3]
    ht.quad((ua, ub, uc, ud), "yellow_body")
    unrm = (ub.co - ua.co).cross(ud.co - ua.co).normalized()
    if unrm.x < 0:
        unrm = -unrm
    # cap: small box on top of the block
    cap_b = [ht.bm.verts.new(p) for p in ((-6.5, -h(0.086), h(0.925) - 0.2), (22.5, -h(0.086), h(0.925) - 0.2), (22.5, h(0.086), h(0.925) - 0.2), (-6.5, h(0.086), h(0.925) - 0.2))]
    cap_t = [ht.bm.verts.new(p) for p in ((-1.0, -h(0.070), h(1.0)), (15.0, -h(0.070), h(1.0)), (15.0, h(0.070), h(1.0)), (-1.0, h(0.070), h(1.0)))]
    ht.loft([cap_b, cap_t], "yellow_body")
    # back: three large raised plates (centre plate and one on each side), profile (x, z) extruded across
    CEN = [(-7.0, h(0.680)), (-11.0, h(0.690)), (-11.3, h(0.790)), (-8.2, h(0.885)), (-6.2, h(0.919)), (-5.0, h(0.919))]      # centre plate
    SIDE = [(-7.0, h(0.680)), (-12.6, h(0.690)), (-13.2, h(0.770)), (-9.4, h(0.880)), (-6.0, h(0.917)), (-5.0, h(0.917))]            # side plates
    for y0, y1, base, ia, ib in ((-h(0.054), h(0.054), CEN, 1.0, 1.0), (h(0.062), h(0.104), SIDE, 1.0, 0.0), (-h(0.104), -h(0.062), SIDE, 0.0, 1.0)):
        rg = []
        for y, k in ((y0, ia), (y1, ib)):          # side plates tilt outward: the outer edge sits closer to the block
            rg.append([ht.P(x + (1.0 - k) * 3.6 * (1 if 0 < i < len(base) - 1 else 0), y, z) for i, (x, z) in enumerate(base)])
        ht.loft(rg, "yellow_body")

    def post(bm, uv0):
        """eye octagon (emissive) on the recess floor and the logo decal; returns the decal face"""
        ev_ = [bm.verts.new(EYE_C + ytan * EYE_R * math.cos(math.radians(22.5 + 45 * k)) + ztan * EYE_R * math.sin(math.radians(22.5 + 45 * k))) for k in range(8)]
        f = bm.faces.new(ev_)
        if f.normal.dot(nrm) < 0:
            f.normal_flip()
        f.material_index = C.MI["eye_red"]
        # logo decal: copy of the covered region of the upper face, offset 0.25 cm, ONE planar UV
        s0, s1, t0, t1 = 0.5 - 0.17, 0.5 + 0.17, 0.46 - 0.40, 0.46 + 0.40
        cs = [(s0, t0), (s1, t0), (s1, t1), (s0, t1)]
        dv = [bm.verts.new(bil(ua, ub, uc, ud, s_, t_) + unrm * 0.25) for s_, t_ in cs]
        f = bm.faces.new(dv)
        if f.normal.dot(unrm) < 0:
            f.normal_flip()
        f.material_index = C.MI["logo_plate"]
        uvs = [(0, 0), (1, 0), (1, 1), (0, 1)]
        for l in f.loops:
            l[uv0].uv = uvs[dv.index(l.vert)]
        return f
    ht.post = post
    return C.finish(ht)
