"""Set parameters of a material instance constant, save, read back.

Unreal Editor Python (task-231-01). Run in the headless editor, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/set_material_instance_params.py /Game/Path/MI_A ShakeParams.r=8 ShakeParams.g=3"
        -unattended -nopause -nosplash -stdout -FullStdOutLogOutput

Arguments: the material instance package path, then one or more assignments:
  - <VectorParam>.<r|g|b|a>=<value>: one channel of a vector parameter; the other channels keep
    their current value (instance override, else the parent value).
  - <ScalarParam>=<value>: a scalar parameter.
Every parameter must exist on the parent material. Each assignment prints one line
"MIPARAM <path> <param> before=<value> after=<value>" read back after the save; a failure prints
"MIPARAM <path> error <reason>" (also when the read back differs from the request; the
engine setters return False even on success, so only the read back is trusted) and the script raises (log "Python script executed with errors").
The editor quits after the script.
"""

import sys

import unreal

LINE_TAG = "MIPARAM"
CHANNELS = ("r", "g", "b", "a")
MEL = unreal.MaterialEditingLibrary


def _fmt_color(c):
    return "({:g}, {:g}, {:g}, {:g})".format(c.r, c.g, c.b, c.a)


def _parse(assignments):
    """Return {param: {channel or None: value}} from the argument list."""
    changes = {}
    for item in assignments:
        if "=" not in item:
            raise ValueError("bad assignment '{}' (expected Param=value or Param.r=value)".format(item))
        key, value = item.split("=", 1)
        channel = None
        if "." in key:
            key, channel = key.rsplit(".", 1)
            if channel not in CHANNELS:
                raise ValueError("bad channel '{}' in '{}'".format(channel, item))
        changes.setdefault(key, {})[channel] = float(value)
    return changes


def run(argv):
    if len(argv) < 2:
        raise ValueError("usage: set_material_instance_params.py <MI path> <Param[.c]=value> ...")
    path = argv[0]
    changes = _parse(argv[1:])
    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        raise ValueError("{} not found".format(path))
    mi = unreal.EditorAssetLibrary.load_asset(path)
    if not isinstance(mi, unreal.MaterialInstanceConstant):
        raise ValueError("{} is not a MaterialInstanceConstant ({})".format(path, type(mi).__name__))

    vector_names = [str(n) for n in MEL.get_vector_parameter_names(mi)]
    scalar_names = [str(n) for n in MEL.get_scalar_parameter_names(mi)]
    before = {}
    for param, channels in changes.items():
        if None in channels:
            if param not in scalar_names:
                raise ValueError("scalar parameter {} not on the material".format(param))
            before[param] = MEL.get_material_instance_scalar_parameter_value(mi, param)
            # The setter returns False even on success in 5.8 (MaterialEditingLibrary.cpp): checked by the read back.
            MEL.set_material_instance_scalar_parameter_value(mi, param, channels[None])
        else:
            if param not in vector_names:
                raise ValueError("vector parameter {} not on the material".format(param))
            color = MEL.get_material_instance_vector_parameter_value(mi, param)
            before[param] = _fmt_color(color)
            new = unreal.LinearColor(color.r, color.g, color.b, color.a)
            for channel, value in channels.items():
                setattr(new, channel, value)
            MEL.set_material_instance_vector_parameter_value(mi, param, new)

    MEL.update_material_instance(mi)
    if not unreal.EditorAssetLibrary.save_loaded_asset(mi, only_if_is_dirty=False):
        raise RuntimeError("save failed")

    # Read back from a fresh load of the saved package.
    mi = unreal.EditorAssetLibrary.load_asset(path)
    for param, channels in changes.items():
        if None in channels:
            after = MEL.get_material_instance_scalar_parameter_value(mi, param)
            mismatch = abs(after - channels[None]) > 1e-4
            after_text = "{:g}".format(after)
        else:
            color = MEL.get_material_instance_vector_parameter_value(mi, param)
            mismatch = any(abs(getattr(color, c) - v) > 1e-4 for c, v in channels.items())
            after_text = _fmt_color(color)
        if mismatch:
            raise RuntimeError("{} read back {} does not match the request".format(param, after_text))
        unreal.log("{} {} {} before={} after={}".format(LINE_TAG, path, param, before[param], after_text))


try:
    run(sys.argv[1:])
except Exception as error:  # report then fail the run
    unreal.log_error("{} {} error {}".format(LINE_TAG, sys.argv[1] if len(sys.argv) > 1 else "?", error))
    raise
