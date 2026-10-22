"""task-287-01: keyed poses of the shooter aim (clip 1), rest hold (clip 2) and fire recoil (clip 3) on the ten rigid parts, feet fixed (every pose leaves legs, pelvis and torso at rest).
Used by meshy_bake_anim.py when the config bake object has an "aimfire" key. Space: robot space, cm, X forward, Y left (Unreal Y = -Blender Y), Z up.
The cannon is ForearmHand_R. Cannon axis = first principal axis (sign toward +X) of the forearm vertices farther than cannon_fraction of the elbow reach; cannon tip = centroid of the vertices
within tip_depth_cm of the farthest point along the axis (muzzle face centre).
Aim: UpperArm_R rotated by (0, u, yaw) (Euler XYZ, about the shoulder): u solved so the axis elevation is 0, yaw so its azimuth is 0 (horizontal toward +X). The elbow stays at rest.
Fire: from the aim pose, UpperArm_R slides back along X by shoulder_back_cm, ForearmHand_R slides back along the barrel by forearm_back_cm and its elbow pitches by forearm_pitch_deg (muzzle up, negative);
recoil weight w(f) = smoothstep(f / peak_frame) up to the peak, then 1 - smoothstep((f - peak_frame) / (return_frame - peak_frame)), 0 from return_frame on. Blender only."""
import math
import numpy as np


class AimFire:
    def __init__(self, cfg, apply_pose, world_verts, rest_p, rest_v):
        self.c = cfg
        self.apply_pose, self.world_verts = apply_pose, world_verts
        el = np.array(rest_p["ForearmHand_R"], float)
        d = np.linalg.norm(rest_v["ForearmHand_R"] - el, axis=1)
        self.cannon = d > float(cfg.get("cannon_fraction", 0.35)) * d.max()
        self.n_aim, self.n_hold, self.n_fire = int(cfg["telegraph_frames"]), int(cfg["strike_frames"]), int(cfg["recover_frames"])
        self.u, self.yaw = self.solve()
        self.keys = dict(rest=dict(u=0.0, yaw=0.0), aim=dict(u=self.u, yaw=self.yaw))

    def axis(self):
        v = self.world_verts("ForearmHand_R")[self.cannon]
        c = v.mean(0)
        w, vec = np.linalg.eigh(np.cov((v - c).T))
        a = vec[:, int(np.argmax(w))]
        return (a if a[0] >= 0 else -a), v

    def elevation_azimuth(self, u, yaw=0.0):
        self.apply_pose({"UpperArm_R": dict(rot=(0, u, yaw))})
        a, _ = self.axis()
        return math.degrees(math.asin(a[2])), math.degrees(math.atan2(a[1], a[0]))

    def solve(self):
        lo, hi = -90.0, 20.0                       # u negative raises the arm (muzzle up); elevation grows as u decreases
        el_lo, _ = self.elevation_azimuth(lo)
        el_hi, _ = self.elevation_azimuth(hi)
        if not (el_lo > 0 > el_hi):
            raise SystemExit("aimfire: elevation does not bracket 0 (%.2f at %.0f, %.2f at %.0f)" % (el_lo, lo, el_hi, hi))
        for _ in range(50):
            mid = 0.5 * (lo + hi)
            if self.elevation_azimuth(mid)[0] > 0:
                lo = mid
            else:
                hi = mid
        u = 0.5 * (lo + hi)
        az = self.elevation_azimuth(u)[1]
        return u, -az

    def tip(self, pos=None):
        """muzzle face centre and farthest vertex along the cannon axis, from world vertices (pos = array of the forearm world vertices) or the current pose"""
        v = np.asarray(pos if pos is not None else self.world_verts("ForearmHand_R"))[self.cannon]
        c = v.mean(0)
        w, vec = np.linalg.eigh(np.cov((v - c).T))
        a = vec[:, int(np.argmax(w))]
        a = a if a[0] >= 0 else -a
        s = (v - c) @ a
        near = v[s >= s.max() - float(self.c.get("tip_depth_cm", 3.0))]
        return near.mean(0), v[int(np.argmax(s))], a

    @staticmethod
    def smooth(x):
        x = min(1.0, max(0.0, x))
        return x * x * (3 - 2 * x)

    def recoil_w(self, f):
        pk, rt = float(self.c["peak_frame"]), float(self.c["return_frame"])
        if f >= rt:
            return 0.0
        return self.smooth(f / pk) if f <= pk else 1.0 - self.smooth((f - pk) / (rt - pk))

    def pose(self, wa, wr):
        c = self.c
        u, yaw = wa * self.u, wa * self.yaw
        P = {"Pelvis": dict(rot=(0, 0, 0), loc=(0.0, 0.0, 0.0))}
        P["UpperArm_R"] = dict(rot=(0, u, yaw), loc=(-wr * float(c["shoulder_back_cm"]), 0.0, 0.0))
        ur, uy = math.radians(u), math.radians(yaw)
        Ry = np.array([[math.cos(ur), 0, math.sin(ur)], [0, 1, 0], [-math.sin(ur), 0, math.cos(ur)]])
        Rz = np.array([[math.cos(uy), -math.sin(uy), 0], [math.sin(uy), math.cos(uy), 0], [0, 0, 1]])
        back = Rz.dot(Ry).T.dot(np.array([-wr * float(c["forearm_back_cm"]), 0.0, 0.0]))        # world -X as a UpperArm_R local offset
        P["ForearmHand_R"] = dict(rot=(0, wr * float(c["forearm_pitch_deg"]), 0), loc=tuple(float(x) for x in back))
        return P

    def poses(self):
        res = []
        for f in range(self.n_aim):
            w = self.smooth(f / float(self.c["aim_frames"]))
            res.append(dict(clip="aim", row_in_clip=f, params=dict(aim=w, recoil=0.0), weight=w))
        for f in range(self.n_hold):
            res.append(dict(clip="rest", row_in_clip=f, params=dict(aim=0.0, recoil=0.0), weight=0.0))
        for f in range(self.n_fire):
            res.append(dict(clip="fire", row_in_clip=f, params=dict(aim=1.0, recoil=self.recoil_w(f)), weight=self.recoil_w(f)))
        for r in res:
            r["pose"] = self.pose(r["params"]["aim"], r["params"]["recoil"])
        return res
