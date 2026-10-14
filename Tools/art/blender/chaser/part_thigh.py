"""Chaser Thigh (left built, right = mirror through sy), pivot at the hip, parent Pelvis. Edit ONLY this file for the thigh."""
import common as C
from common import h

PREVIEW = {"close": ("front", (6.0, 13.0, h(0.45)), 36.0)}


def build(side="L"):
    sy = C.SIGN[side]
    th = C.Part("Thigh_" + side, C.pivot_hip(side), parent="Pelvis", sy=sy)
    th.abox(-2.5, 9.0, h(0.074), h(0.115), h(0.435), h(0.590), "gunmetal")                         # upper leg bar
    th.secs([(h(0.431), 7.3, h(0.104), 9.5, 6.0), (h(0.379), 6.8, h(0.104), 11.8, 9.8), (h(0.335), 7.2, h(0.104), 10.3, 7.0), (h(0.308), 7.8, h(0.104), 8.0, 4.6)], "yellow_body", ch=0.3)   # faceted knee guard
    for y0, y1 in ((0.058, 0.079), (0.112, 0.138)):                                                # hip guard plates, chamfered
        th.profile([(4.0, h(0.580)), (14.0, h(0.585)), (15.5, h(0.615)), (12.0, h(0.640)), (4.0, h(0.628))], h(y0), h(y1), "yellow_body")
    th.oct_y(1.5, h(0.108), h(0.383), h(0.034), h(0.124), "gunmetal")                              # knee prism
    return C.finish(th)
