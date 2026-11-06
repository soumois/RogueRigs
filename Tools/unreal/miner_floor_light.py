"""Read or set the FloorWallLight point light of BP_Miner (task-473-01, miner light calibration).

Unreal Editor Python, headless, editor closed (copy to a path without spaces first):

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<path without spaces>/miner_floor_light.py intensity=10 radius=500 exponent=4 color=255,140,40"
        -unattended -nopause -nosplash -stdout

Arguments (all optional; none = read only):
  intensity=<float>   PointLightComponent Intensity (Unitless units in BP_Miner)
  radius=<float>      AttenuationRadius (uu)
  exponent=<float>    LightFalloffExponent (used because bUseInverseSquaredFalloff is false)
  color=<r>,<g>,<b>   LightColor, 8-bit sRGB
The relative location (owner-tuned height) is never written. Prints lines tagged FLOORLIGHT before and after,
compiles and saves BP_Miner when something was set. Lines are also written to the file named by env FLOORLIGHT_OUT.
"""

import os
import sys

import unreal

TAG = "FLOORLIGHT"
BP_PATH = "/Game/RogueRigs/Blueprints/Player/BP_Miner"
COMPONENT = "FloorWallLight"
LINES = []


def log(msg):
    LINES.append(msg)
    unreal.log(msg)


def _find_template(bp):
    sub = unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
    for handle in sub.k2_gather_subobject_data_for_blueprint(bp):
        data = unreal.SubobjectDataBlueprintFunctionLibrary.get_data(handle)
        obj = unreal.SubobjectDataBlueprintFunctionLibrary.get_object(data)
        if obj and isinstance(obj, unreal.PointLightComponent) and obj.get_name().startswith(COMPONENT):
            return obj
    return None


def _dump(label, c):
    col = c.get_editor_property("light_color")
    loc = c.get_editor_property("relative_location")
    log("{} {} intensity={:g} radius={:g} exponent={:g} inverse_sq={} color={},{},{} cast_shadows={} location=({:g},{:g},{:g}) channels={}".format(
        TAG, label, c.get_editor_property("intensity"), c.get_editor_property("attenuation_radius"),
        c.get_editor_property("light_falloff_exponent"), c.get_editor_property("use_inverse_squared_falloff"),
        col.r, col.g, col.b, c.get_editor_property("cast_shadows"), loc.x, loc.y, loc.z,
        [c.get_editor_property("lighting_channels").get_editor_property(n) for n in ("channel0", "channel1", "channel2")]))


def main(args):
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    comp = _find_template(bp)
    if comp is None:
        raise RuntimeError("{} error: {} not found".format(TAG, COMPONENT))
    _dump("before", comp)
    changed = False
    for a in args:
        key, _, val = a.partition("=")
        if key == "intensity":
            comp.set_editor_property("intensity", float(val))
        elif key == "radius":
            comp.set_editor_property("attenuation_radius", float(val))
        elif key == "exponent":
            comp.set_editor_property("light_falloff_exponent", float(val))
        elif key == "color":
            r, g, b = (int(x) for x in val.split(","))
            comp.set_editor_property("light_color", unreal.Color(r=r, g=g, b=b, a=255))
        else:
            raise RuntimeError("{} error: unknown argument {}".format(TAG, a))
        changed = True
    if changed:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(BP_PATH, only_if_is_dirty=False)
        comp = _find_template(unreal.EditorAssetLibrary.load_asset(BP_PATH))
        _dump("after", comp)


try:
    main(sys.argv[1:])
except Exception as exc:
    log("{} error {}".format(TAG, exc))
    raise
finally:
    out = os.environ.get("FLOORLIGHT_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LINES) + "\n")
