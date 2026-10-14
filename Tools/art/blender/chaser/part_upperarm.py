"""Chaser UpperArm (left built, right = mirror through sy), pivot at the shoulder, parent HeadTorso. Edit ONLY this file for the upper arm."""
import common as C
from common import h

PREVIEW = {"close": ("front", (-3.0, 24.0, h(0.86)), 30.0)}


def build(side="L"):
    sy = C.SIGN[side]
    ua_ = C.Part("UpperArm_" + side, C.pivot_arm(side), parent="HeadTorso", sy=sy)
    ua_.oct_y(-2.0, (h(0.113) + h(0.170)) / 2, h(0.860), 5.8, h(0.170) - h(0.113), "gunmetal")      # dark shoulder connector (cylinder across the shoulder)
    # faceted upper arm: narrow top, wide shoulder cap just below, taper to the elbow
    ua_.secs([(h(0.930), -2.5, h(0.196), 10.0, 11.0), (h(0.903), -2.8, h(0.202), 14.0, 14.6), (h(0.862), -3.2, h(0.214), 14.4, 14.9), (h(0.815), -3.7, h(0.230), 12.6, 13.6), (h(0.784), -4.0, h(0.243), 10.4, 11.4), (h(0.772), -4.0, h(0.246), 8.4, 9.6)], "yellow_body", ch=0.32)
    return C.finish(ua_)
