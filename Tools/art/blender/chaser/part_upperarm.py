"""Chaser UpperArm (left built, right = mirror through sy), pivot at the shoulder, parent HeadTorso. Edit ONLY this file for the upper arm."""
import common as C
from common import h

PREVIEW = {"close": ("front", (-3.0, 24.0, h(0.86)), 30.0)}


def build(side="L"):
    sy = C.SIGN[side]
    ua_ = C.Part("UpperArm_" + side, C.pivot_arm(side), parent="HeadTorso", sy=sy)
    ua_.abox(-7.0, 3.0, h(0.113), h(0.170), h(0.815), h(0.905), "gunmetal")                      # dark shoulder connector
    # faceted upper arm: narrow top, wide shoulder cap just below, taper to the elbow
    ua_.secs([(h(0.930), -2.5, h(0.196), 10.0, 11.0), (h(0.903), -2.8, h(0.202), 14.0, 14.6), (h(0.848), -3.3, h(0.218), 13.4, 14.0), (h(0.775), -4.0, h(0.243), 10.2, 12.2)], "yellow_body", ch=0.3)
    return C.finish(ua_)
