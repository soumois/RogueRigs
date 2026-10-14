"""Chaser ShinFoot (left built, right = mirror through sy), pivot at the knee, parent Thigh. Edit ONLY this file for the shin and foot."""
import common as C
from common import h

PREVIEW = {"close": ("front", (3.0, 17.0, 24.0), 44.0)}


def build(side="L"):
    sy = C.SIGN[side]
    sf = C.Part("ShinFoot_" + side, C.pivot_knee(side), parent="Thigh_" + side, sy=sy)
    FY = h(0.1455)
    # one continuous tapering shin guard from the knee to the ankle
    sf.secs([(44.1, -0.2, 16.4, 8.0, 5.6), (40.2, -0.2, 16.6, 10.2, 9.6), (37.4, 0.0, 16.7, 10.6, 12.3), (30.0, -0.7, 17.2, 9.0, 8.8), (23.4, -0.8, 17.6, 7.0, 5.4)], "yellow_body", ch=0.3)
    sf.prism((7.0, h(0.130), h(0.300)), (2.6, h(0.142), 12.5), h(0.05), h(0.05), h(0.045), h(0.045), "gunmetal")     # dark leg bar in front of the shin
    sf.secs([(24.0, -2.6, 17.9, 5.0, 4.6), (14.0, -3.0, 18.0, 5.0, 5.0)], "gunmetal", ch=0.3)                        # dark neck behind the ankle
    sf.oct_y(1.3, FY, 13.3, 3.6, 15.6, "gunmetal")                                                                   # dark octagonal ankle joint
    sf.profile([(2.4, 22.6), (-1.0, 17.5), (3.5, 10.0), (8.3, 13.3)], FY - 3.2, FY + 3.2, "yellow_body")             # slanted instep shield
    for sgn in (-1, 1):                                                                                             # ankle cheek plates
        y0, y1 = sorted((FY + sgn * 3.3, FY + sgn * 5.9))
        sf.profile([(-3.0, 10.0), (-3.0, 19.5), (1.0, 21.0), (4.2, 18.5), (4.2, 10.0)], y0, y1, "yellow_body")
    sf.profile([(-0.5, 14.5), (-4.0, 18.6), (-6.8, 17.6), (-8.0, 8.0), (-0.5, 8.0)], FY - h(0.058), FY + h(0.058), "yellow_body")      # heel
    sf.profile([(17.0, h(0.030)), (17.0, 7.4), (8.5, 13.2), (-0.5, 13.2), (-0.5, h(0.030))], FY - 7.4, FY + 7.4, "yellow_body")        # foot upper
    sf.abox(-8.6, 21.5, FY - 8.3, FY + 8.3, 0.0, h(0.030), "gunmetal")                                              # sole
    for sgn in (-1, 1):                                                                                             # two angled toe blocks
        sf.profile([(15.0, 0.0), (21.5, 0.0), (19.8, 7.4), (15.0, 7.4)], FY + sgn * 5.4 - 2.6, FY + sgn * 5.4 + 2.6, "gunmetal")
    sf.abox(-8.6, -2.7, FY - 7.0, FY + 7.0, 0.0, 7.4, "gunmetal")                                                    # heel block
    return C.finish(sf)
