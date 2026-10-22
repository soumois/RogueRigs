"""task-284-01: planted-foot walk for a rigid-part biped (heavy). Pure numpy, no bpy; imported by meshy_bake_anim.py.

Model (robot space, cm: X forward, Y left, Z up; a positive rotation about Y swings a hanging limb backward and tips a foot toe-down):
  * the pelvis only translates in Z (pz); the body moves forward at `speed` cm/s, the ground is fixed in the world;
  * stance leg (half a cycle): thigh and shin-foot rotate as ONE rigid body by psi about the hip (knee rotation 0). The foot pivots on one vertex
    that stays on the same ground point: the heel vertex until the sole is flat (psi = 0), then the toe vertex. psi(t) and pz(t) solve
    "pivot vertex on the floor at a fixed ground x" exactly, so the contact vertex does not slide;
  * the instant the sole is flat inside the stance (tau_h) is solved so that the pelvis height at heel strike equals the pelvis height at toe off
    (continuity at the hand-over between legs, a half cycle apart);
  * swing leg (the other half cycle): thigh angle goes from the toe-off angle to the next heel-strike angle (smoothstep, optionally early) and the knee
    flexes (shin rotates backward about the knee) by a sine bump of at most knee_deg; the exponent and the early factor are searched so that no foot
    vertex goes below the floor.
Poses are returned in the format of meshy_bake_anim.py: part -> dict(rot=(rx, ry, rz) degrees Euler XYZ, loc=(dx, dy, dz) cm)."""
import math
import numpy as np


def ry(a):
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def smooth(x):
    x = min(1.0, max(0.0, x))
    return x * x * (3 - 2 * x)


def bisect(f, lo, hi, n=80):
    flo = f(lo)
    for _ in range(n):
        mid = 0.5 * (lo + hi)
        fm = f(mid)
        if (fm > 0) == (flo > 0):
            lo, flo = mid, fm
        else:
            hi = mid
    return 0.5 * (lo + hi)


class Leg:
    def __init__(self, side, rest_v, rest_p):
        self.side = side
        self.foot = rest_v["ShinFoot_" + side]
        self.hip = rest_p["Thigh_" + side]
        self.knee = rest_p["ShinFoot_" + side]
        zmin = float(self.foot[:, 2].min())
        sole = self.foot[self.foot[:, 2] <= zmin + 0.5]
        self.heel = sole[sole[:, 0].argmin()]
        self.toe = sole[sole[:, 0].argmax()]
        self.zmin = zmin

    def rel(self, v):
        d = v - self.hip
        return d[0], d[2]

    def psi_for(self, piv, target_x):
        """rigid leg angle psi (rad) such that hip_x + X(psi) = target_x, X = x of Ry(psi) (piv - hip)"""
        dx, dz = self.rel(piv)
        hx = self.hip[0]
        return bisect(lambda p: hx + math.cos(p) * dx + math.sin(p) * dz - target_x, math.radians(-45), math.radians(45))

    def pz_for(self, piv, psi):
        dx, dz = self.rel(piv)
        z = -math.sin(psi) * dx + math.cos(psi) * dz
        return -(self.hip[2] + z)      # pivot vertex z' = hip_z + pz + Z = 0

    def lower_hull(self):
        """lower convex hull of the foot in the (x, z) plane (rolling contact can only touch these corners)"""
        pts = sorted({(round(float(p[0]), 3), round(float(p[2]), 3)) for p in self.foot})
        hull = []
        for p in pts:
            while len(hull) >= 2 and (hull[-1][0] - hull[-2][0]) * (p[1] - hull[-2][1]) - (hull[-1][1] - hull[-2][1]) * (p[0] - hull[-2][0]) <= 0:
                hull.pop()
            hull.append(p)
        return np.array(hull)

    def hull_xz(self, psi, k):
        """hull corners (x, z) relative to the hip for thigh angle psi and knee flexion k (positive = shin rotates backward)"""
        if not hasattr(self, "_hull"):
            self._hull = self.lower_hull()
        H = self._hull
        kn = np.array([self.knee[0], self.knee[2]])
        hp = np.array([self.hip[0], self.hip[2]])

        def rot(a, v):
            c, s = math.cos(a), math.sin(a)
            return np.stack([c * v[..., 0] + s * v[..., 1], -s * v[..., 0] + c * v[..., 1]], axis=-1)
        return rot(psi, kn - hp) + rot(psi + k, H - kn)

    def kfun(self, t, half, ke):
        return ke * (t / half) ** 2          # stance knee flexion: 0 at heel strike, ke at toe off (shortens the leg to match the pelvis height of the next heel strike)

    def sim(self, psi_s, half, speed, n=400, ke=0.0):
        """roll the rigid thigh + shin-foot over the floor for `half` seconds from a heel strike at thigh angle psi_s, knee flexion ramping 0 -> ke. The pivot is a hull corner that
        stays on one ground point; when another corner would go below the floor the pivot switches to it (rolling without slipping). returns arrays t, psi, pz, pivot index, k"""
        hx, hz = self.hip[0], self.hip[2]
        k0 = self.kfun(0.0, half, ke)
        P = self.hull_xz(psi_s, k0)
        p = int(np.argmin(P[:, 1]))
        cx = hx + P[p, 0]
        ts = np.linspace(0.0, half, n + 1)
        out = [[], [], [], [], []]
        psi = psi_s
        for t in ts:
            k = self.kfun(t, half, ke)
            if t > 0:
                psi = bisect(lambda a: hx + self.hull_xz(a, k)[p, 0] - (cx - speed * t), math.radians(-60), math.radians(60), 45)
            P = self.hull_xz(psi, k)
            j = int(np.argmin(P[:, 1]))
            if P[j, 1] < P[p, 1] - 1e-7:
                cx = hx + P[j, 0] + speed * t
                p = j
            out[0].append(t); out[1].append(psi); out[2].append(-(hz + P[p, 1])); out[3].append(p); out[4].append(k)
        return tuple(np.array(a) for a in out)

    def solve_stance(self, half, speed, kmax):
        """psi_s and ke such that (1) the pelvis height at toe off equals the one at heel strike (hand-over to the other leg) and (2) the thigh angle at toe off is minus the one at heel strike"""
        def inner(ps):
            def g(ke):
                _, _, pz, _, _ = self.sim(ps, half, speed, 60, ke)
                return pz[0] - pz[-1]
            g0 = g(0.0)
            if abs(g0) < 1e-4:
                return 0.0, True
            for lim in (kmax, -kmax):      # flexion first, then extension (shin rotates forward: knee straighter than the rest pose)
                if (g(lim) > 0) != (g0 > 0):
                    return bisect(g, 0.0, lim, 14) if lim > 0 else bisect(g, lim, 0.0, 14), True
            return (kmax if abs(g(kmax)) < abs(g(-kmax)) else -kmax), False

        def asym(ps):
            ke, _ = inner(ps)
            _, psi, _, _, _ = self.sim(ps, half, speed, 60, ke)
            return psi[-1] + ps
        lo, hi = math.radians(-25), math.radians(-1)
        ok = (asym(lo) > 0) != (asym(hi) > 0)
        ps = bisect(asym, lo, hi, 12) if ok else math.radians(-8)
        ke, ok2 = inner(ps)
        self.tab = self.sim(ps, half, speed, 400, ke)
        self.ke = ke
        return ps, (ok and ok2)

    def stance(self, t, *_):
        ts, psis, pzs, pivs, ks = self.tab
        psi = float(np.interp(t, ts, psis))
        pz = float(np.interp(t, ts, pzs))
        k = float(np.interp(t, ts, ks))
        return psi, pz, k

    def solve_tau(self, half, speed, kmax=0.26):
        ps, ok = self.solve_stance(half, speed, kmax)
        piv = self.tab[3]
        sw = np.where(piv != piv[0])[0]
        return (float(self.tab[0][sw[0]]) if len(sw) else half), ok

    def world_foot(self, theta, k, pz):
        """foot vertices for thigh angle theta, knee flexion k (rad, shin rotates backward when positive), pelvis dz pz"""
        kp = self.hip + ry(theta) @ (self.knee - self.hip)
        return kp + (self.foot - self.knee) @ ry(theta + k).T + np.array([0, 0, pz])


class Walk:
    def __init__(self, rest_v, rest_p, nrun, fps, run_rate, speed, knee_deg, arm_deg, elbow_deg, lean_deg, twist_deg):
        self.legs = {s: Leg(s, rest_v, rest_p) for s in ("L", "R")}
        self.T = nrun / (fps * run_rate)
        self.half = self.T / 2
        self.speed = speed
        self.kmax = math.radians(knee_deg)
        self.arm, self.elbow, self.lean, self.twist = arm_deg, elbow_deg, lean_deg, twist_deg
        self.tau, self.tau_ok, self.ends = {}, {}, {}
        for s, lg in self.legs.items():
            tau, ok = lg.solve_tau(self.half, speed, 0.0) if s == "L" else (self.tau["L"], self.tau_ok["L"])      # R mirrors L: one pelvis height curve for both legs
            self.tau[s], self.tau_ok[s] = tau, ok
            src = self.legs["L"]
            psi_s, pz_s, k_s = src.stance(0.0)
            psi_e, pz_e, k_e = src.stance(self.half)
            self.ends[s] = dict(psi_s=psi_s, psi_e=psi_e, pz_s=pz_s, pz_e=pz_e, k_s=k_s, k_e=k_e)
        self.psi_max = max(max(abs(e["psi_s"]), abs(e["psi_e"])) for e in self.ends.values())
        self.search_swing(rest_v)

    def pelvis_z(self, u):
        wl = u % 1.0
        w = wl if wl < 0.5 else (wl - 0.5)        # R mirrors L half a cycle later
        return self.legs["L"].stance(w * self.T)[1]

    def swing_angles(self, s, side, pz):
        """sole angle alpha = theta + k goes from the toe-off angle to the next heel-strike angle (smoothstep); the knee flexion k (0..kmax) is the smallest that lifts the lowest foot vertex to the clearance curve H sin(pi s)^2"""
        e = self.ends[side]
        alpha = (e["psi_e"] + e["k_e"]) + ((e["psi_s"] + e["k_s"]) - (e["psi_e"] + e["k_e"])) * smooth(s)
        kmin = e["k_e"] * (1 - smooth(s)) + e["k_s"] * smooth(s)      # the knee flexion of the stance end blends into the one of the next heel strike
        h = self.clear_cm * math.sin(math.pi * s) ** 2
        lg = self.legs[side]
        def g(k):
            return float(lg.world_foot(alpha - k, k, pz)[:, 2].min()) - h
        if g(kmin) >= 0:
            return alpha - kmin, kmin
        if g(self.kmax) < 0:
            return alpha - self.kmax, self.kmax
        k = bisect(g, kmin, self.kmax, 40)
        return alpha - k, k

    def leg_state(self, u, side):
        """u = cycle phase 0..1. returns dict(kind, theta, k, pz)"""
        w = (u - (0.0 if side == "L" else 0.5)) % 1.0
        if w < 0.5:
            t = w * self.T
            psi, pz, kk = self.legs["L"].stance(t)      # R mirrors L
            return dict(kind="stance", theta=psi, k=kk, pz=pz, s=None)
        s = (w - 0.5) / 0.5
        th, k = self.swing_angles(s, side, self.pelvis_z(u))
        return dict(kind="swing", theta=th, k=k, pz=None, s=s)

    def search_swing(self, rest_v):
        self.clear_cm = 3.0
        self.swing_min_z = self.min_swing_z()

    def min_swing_z(self):
        m = 1e9
        for u in np.linspace(0, 1, 201)[:-1]:
            pz = self.pelvis_z(u)
            for side in ("L", "R"):
                st = self.leg_state(u, side)
                if st["kind"] == "swing":
                    m = min(m, float(self.legs[side].world_foot(st["theta"], st["k"], pz)[:, 2].min()))
        return m

    def pose_at(self, u):
        pz = self.pelvis_z(u)
        st = {s: self.leg_state(u, s) for s in ("L", "R")}
        deg = math.degrees
        gl = st["L"]["theta"]
        gnorm = max(1e-9, self.psi_max)
        g = gl / gnorm          # -1 when the left leg is the forward one (heel strike), +1 at toe off
        pose = {}
        pose["Pelvis"] = dict(rot=(0, 0, 0), loc=(0.0, 0.0, pz))
        pose["HeadTorso"] = dict(rot=(0, self.lean, -self.twist * g))
        for s in ("L", "R"):
            pose["Thigh_" + s] = dict(rot=(0, deg(st[s]["theta"]), 0))
            pose["ShinFoot_" + s] = dict(rot=(0, deg(st[s]["k"]), 0))
        pose["UpperArm_L"] = dict(rot=(0, -self.arm * g, 0))
        pose["UpperArm_R"] = dict(rot=(0, self.arm * g, 0))
        pose["ForearmHand_L"] = dict(rot=(0, -self.elbow, 0))
        pose["ForearmHand_R"] = dict(rot=(0, -self.elbow, 0))
        return pose

    def info(self):
        d = dict(cycle_s=round(self.T, 4), stance_s=round(self.half, 4), step_travel_cm=round(self.speed * self.half, 2), first_pivot_switch_s_after_heel_strike={s: round(self.tau[s], 4) for s in self.tau},
                 flat_instant_solved={s: bool(self.tau_ok[s]) for s in self.tau_ok},
                 rigid_leg_angle_deg={s: dict(heel_strike=round(math.degrees(e["psi_s"]), 3), toe_off=round(math.degrees(e["psi_e"]), 3), stance_knee_flexion_at_toe_off=round(math.degrees(e["k_e"]), 3)) for s, e in self.ends.items()},
                 pelvis_z_cm_at_heel_strike_and_toe_off={s: dict(heel_strike=round(e["pz_s"], 3), toe_off=round(e["pz_e"], 3)) for s, e in self.ends.items()},
                 swing=dict(clearance_target_cm=self.clear_cm, knee_flexion_max_deg=round(math.degrees(self.kmax), 3), knee_limit_deg=15, min_foot_vertex_z_cm=round(self.swing_min_z, 4)))
        return d
