"""Chaser ForearmHand (left built, right = mirror through sy), pivot at the elbow, parent UpperArm. Edit ONLY this file for the forearm and claw."""
import common as C
from common import h

PREVIEW = {"close": ("front", (3.5, 36.0, 65.0), 22.0)}


def build(side="L"):
    sy = C.SIGN[side]
    fa_ = C.Part("ForearmHand_" + side, C.pivot_elbow(side), parent="UpperArm_" + side, sy=sy)
    fa_.abox(-8.0, 0.0, h(0.210), h(0.294), h(0.728), h(0.768), "gunmetal")                     # dark elbow connector
    fa_.secs([(h(0.735), -4.5, h(0.264), 11.4, 12.0), (h(0.697), -3.2, h(0.275), 12.9, 13.4), (h(0.637), -1.2, h(0.286), 11.8, 12.2), (h(0.595), 0.3, h(0.294), 9.6, 11.2)], "yellow_body", ch=0.3)
    fa_.prism((2.0, h(0.272), h(0.715)), (5.4, h(0.290), h(0.623)), 6.8, 2.6, 6.0, 2.4, "yellow_body", ch=0.3, simple=True)    # guard plate on the front face of the forearm
    # curled claw: wrist ring, narrow palm, three fingers (two segments) side by side in depth hooking inward and forward, thumb on the inner side
    fa_.secs([(h(0.608), 0.8, 37.3, 6.2, 7.6), (h(0.578), 1.2, 37.8, 6.6, 8.2)], "gunmetal", ch=0.3)
    fa_.secs([(h(0.578), 1.8, 39.0, 5.8, 10.2), (h(0.563), 2.0, 39.4, 6.4, 11.0), (63.2, 2.4, 39.6, 5.4, 9.6)], "gunmetal", ch=0.3)
    for xi in (5.6, 2.3, -1.0):
        fa_.prism((xi, 39.2, 63.8), (xi + 0.9, 38.0, 59.4), 4.4, 3.4, 4.0, 3.2, "gunmetal", ch=0.25, simple=True)
        fa_.prism((xi + 0.9, 38.0, 59.8), (xi + 3.0, 36.4, 55.4), 4.0, 3.2, 3.4, 2.8, "gunmetal", ch=0.25, simple=True)
    fa_.prism((6.0, 36.6, 70.0), (8.0, 34.6, 65.6), 4.2, 3.6, 3.8, 3.2, "gunmetal", ch=0.25, simple=True)                  # thumb, two segments
    fa_.prism((8.0, 34.6, 65.9), (9.6, 32.4, 61.4), 3.8, 3.2, 3.2, 2.8, "gunmetal", ch=0.25, simple=True)
    return C.finish(fa_)
