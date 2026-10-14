"""Chaser Thigh (left built, right = mirror through sy), pivot at the hip, parent Pelvis. Edit ONLY this file for the thigh."""
import common as C
from common import h

PREVIEW = {"close": ("front", (6.0, 13.0, h(0.45)), 36.0)}


def build(side="L"):
    sy = C.SIGN[side]
    th = C.Part("Thigh_" + side, C.pivot_hip(side), parent="Pelvis", sy=sy)
    th.prism((3.2, h(0.095), h(0.592)), (3.2, h(0.101), h(0.505)), 8.8, 11.6, 8.6, 11.0, "gunmetal", ch=0.32, simple=True)        # widened upper leg block, splays outward
    th.prism((3.0, h(0.101), h(0.510)), (2.8, h(0.108), h(0.425)), 6.0, 8.4, 6.2, 8.0, "gunmetal", ch=0.3, simple=True)         # narrower link bar down to the knee guard
    th.secs([(h(0.431), 7.0, h(0.106), 8.6, 5.8), (h(0.392), 6.8, h(0.109), 10.6, 9.6), (h(0.350), 7.2, h(0.111), 10.2, 7.6), (h(0.306), 7.8, h(0.112), 7.6, 4.6)], "yellow_body", ch=0.32)   # faceted knee guard
    for y0, y1 in ((0.060, 0.080), (0.111, 0.134)):                                                # hip guard plates, chamfered in side and front
        th.profile([(4.5, h(0.574)), (12.5, h(0.580)), (15.2, h(0.600)), (14.5, h(0.628)), (11.5, h(0.645)), (6.0, h(0.640)), (3.6, h(0.615))], h(y0), h(y1), "yellow_body")
    th.oct_y(1.5, h(0.110), h(0.383), h(0.034), h(0.124), "gunmetal")                              # knee prism
    return C.finish(th)
