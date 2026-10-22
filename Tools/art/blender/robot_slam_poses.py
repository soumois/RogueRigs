"""task-285-01: keyed poses of the heavy slam (telegraph, strike, recover) on the ten rigid parts, planted feet.
Used by meshy_bake_anim.py when the config bake object has a "slam" key. Space: robot space, cm, X forward, Z up; a positive rotation about Y swings a hanging limb backward and leans a torso forward.
A pose is five numbers (degrees), the same for both sides (sagittal move):
  alpha = world angle of both thighs (negative = knee forward = knee bend), p = pelvis pitch (hips hinge), phi = world lean of the torso (positive = forward),
  u = local rotation of the upper arms (world angle phi + u), e = local rotation of the forearms (elbow, negative = forward flexion).
Feet fixed: the shin keeps world rotation 0 (local -alpha) and the knee stays at its rest position, so the hip lies on the circle around the knee: hip = K + R(alpha) (H - K); the pelvis translation is hip - H.
The contact pose solves u so that the lowest vertex of the hand end (forearm vertices farther than hand_end_fraction of the elbow reach) is at fist_z_cm above the floor.
Blender only."""
import math
import numpy as np


class Slam:
    def __init__(self, cfg, apply_pose, world_verts, rest_p, rest_v):
        self.c = cfg
        self.apply_pose, self.world_verts = apply_pose, world_verts
        self.H = np.array(rest_p["Thigh_L"], float)          # hip pivot (x, z used)
        self.K = np.array(rest_p["ShinFoot_L"], float)       # knee pivot
        self.hands = {}
        for s_ in "LR":
            el = np.array(rest_p["ForearmHand_" + s_], float)
            d = np.linalg.norm(rest_v["ForearmHand_" + s_] - el, axis=1)
            self.hands["ForearmHand_" + s_] = d > float(cfg.get("hand_end_fraction", 0.65)) * d.max()
        self.hand = self.hands["ForearmHand_L"]
        self.keys = {k: dict(v) for k, v in cfg["keys"].items()}
        self.keys["rest"] = dict(alpha=0.0, p=0.0, phi=0.0, u=0.0, e=0.0)
        ck = self.keys["contact"]
        if ck.get("u") == "solve":
            ck["u"] = self.solve_u(ck, float(cfg.get("fist_z_cm", 1.0)))
        self.contact_u = ck["u"]

    def pose(self, a):
        r = math.radians(a["alpha"])
        v = self.H - self.K
        hx = self.K[0] + v[0] * math.cos(r) + v[2] * math.sin(r)
        hz = self.K[2] - v[0] * math.sin(r) + v[2] * math.cos(r)
        d = (hx - self.H[0], hz - self.H[2])
        P = {"Pelvis": dict(rot=(0, a["p"], 0), loc=(d[0], 0.0, d[1])), "HeadTorso": dict(rot=(0, a["phi"] - a["p"], 0))}
        for s in "LR":
            P["Thigh_" + s] = dict(rot=(0, a["alpha"] - a["p"], 0))
            P["ShinFoot_" + s] = dict(rot=(0, -a["alpha"], 0))
            P["UpperArm_" + s] = dict(rot=(0, a["u"], 0))
            P["ForearmHand_" + s] = dict(rot=(0, a["e"], 0))
        return P

    def hand_z(self, a):
        self.apply_pose(self.pose(a))
        return float(self.world_verts("ForearmHand_L")[self.hand][:, 2].min())

    def solve_u(self, a, z):
        """the forward-most u (most negative) at which the hand end is at z: bisection on the branch where z grows as u gets more negative"""
        us = np.arange(float(self.c.get("solve_u_range", [-120, 20])[1]), float(self.c.get("solve_u_range", [-120, 20])[0]), -0.25)
        zs = [self.hand_z(dict(a, u=float(u))) for u in us]
        i0 = int(np.argmin(zs))
        for i in range(i0, len(us) - 1):
            if zs[i] <= z < zs[i + 1]:
                lo, hi = float(us[i + 1]), float(us[i])           # z(lo) > z, z(hi) <= z
                for _ in range(40):
                    mid = 0.5 * (lo + hi)
                    if self.hand_z(dict(a, u=mid)) > z:
                        lo = mid
                    else:
                        hi = mid
                return 0.5 * (lo + hi)
        raise SystemExit("slam: contact pose cannot reach z = %.2f cm (lowest hand z %.2f)" % (z, min(zs)))

    @staticmethod
    def smooth(x):
        x = min(1.0, max(0.0, x))
        return x * x * (3 - 2 * x)

    def blend(self, k0, k1, w):
        a, b = self.keys[k0], self.keys[k1]
        return {n: a[n] + (b[n] - a[n]) * w for n in ("alpha", "p", "phi", "u", "e")}

    def weights(self):
        """list of (clip name, row in clip, from key, to key, weight) for the 46 rows"""
        c = self.c
        nt, ns, nr = int(c["telegraph_frames"]), int(c["strike_frames"]), int(c["recover_frames"])
        raise_f = float(c["raise_s"]) * 30.0                    # frames of the raise (13.5 for 0.45 s); the last telegraph frames are clamped at the raised pose
        cf = int(c["contact_frame"])
        pw = float(c.get("strike_ease_power", 2.0))
        hold = int(c.get("recover_hold_frames", 3))
        out = []
        for f in range(nt):
            out.append(("telegraph", f, "rest", "raised", self.smooth(f / raise_f)))
        for f in range(ns):
            out.append(("strike", f, "raised", "contact", min(1.0, f / cf) ** pw))
        for f in range(nr):
            w = 1.0 - self.smooth((f - hold + 1) / float(nr - hold)) if f >= hold else 1.0
            out.append(("recover", f, "contact", "rest", 1.0 - w))
        return out

    def poses(self):
        res = []
        for clip, f, k0, k1, w in self.weights():
            res.append(dict(clip=clip, row_in_clip=f, params=self.blend(k0, k1, w), weight=w))
        for r in res:
            r["pose"] = self.pose(r["params"])
        return res
