"""Chaser Pelvis (root), pivot at the hip height on the centre line. Edit ONLY this file for the pelvis; contract (name, pivot, parent, slots) is in common.py."""
import common as C
from common import h

# preview: optional close-up (view, centre world cm, window cm)
PREVIEW = {"close": ("front", (1.5, 0.0, h(0.60)), 30.0)}


def build(side=None):
    pel = C.Part("Pelvis", C.PIV_PELVIS)
    pel.prism((1.5, 0, h(0.662)), (1.5, 0, h(0.585)), h(0.135), h(0.165), h(0.085), h(0.150), "gunmetal", bulge=0.0)
    pel.secs([(h(0.558), 2.25, 0.0, 11.5, 18.0), (h(0.572), 2.25, 0.0, 14.0, 21.5), (h(0.590), 2.25, 0.0, 13.0, 20.0)], "yellow_body", ch=0.3)        # yellow lower rim, chamfered
    return C.finish(pel)
