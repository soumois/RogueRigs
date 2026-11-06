"""M_BlobShadow actor shadow look of task-472-02 (owner 2026-11-05, start values of Q3 2026-11-06).

Unreal Editor Python, headless, editor closed (copy to a path without spaces first):

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<path without spaces>/blob_shadow_material.py"
        -unattended -nopause -nosplash -stdout

Idempotent. Rebuilds the graph of M_BlobShadow (Unlit, Translucent, two sided):
  - Tint (vector parameter) -> Emissive Color: dark desaturated purple-grey, aimed at sRGB (45, 35, 55) on screen (TINT_LINEAR);
    unlit, so no light changes it;
  - Opacity (scalar parameter, custom primitive data index 0, default 0.6): written per actor by URRBlobShadowComponent
    (per-type opacity x global multiplier);
  - EdgeStart (scalar parameter, default 0.8): the alpha is full up to EdgeStart x radius, then a smoothstep to 0 at the
    disc radius (soft edge on the outer 20 percent);
  - Custom node BlobShadowMask(UV, Edge, Op) -> Opacity.
MI_BlobShadow keeps no override (parent defaults). Recompiles, saves both assets, reads back; lines tagged BLOBSHADOW,
also written to the file named by env BLOBSHADOW_OUT.
"""

import os
import unreal

TAG = "BLOBSHADOW"
MEL = unreal.MaterialEditingLibrary
PATH = "/Game/RogueRigs/FX/BlobShadow/M_BlobShadow"
MI_PATH = "/Game/RogueRigs/FX/BlobShadow/MI_BlobShadow"
TINT_SRGB = (45, 35, 55)
# Tint actually stored (linear): the unlit emissive goes through the fixed exposure (-0.94 EV100), the tone mapper and the grading,
# which showed the plain sRGB (45, 35, 55) conversion about 3x brighter and bluer on screen (task-472-02 capture: tint seen about
# (80, 60, 110)). Divided per channel by the measured gain so the disc reads about (45, 35, 55) on screen.
TINT_LINEAR = (0.0092, 0.0065, 0.0102)
OPACITY = 0.6
EDGE_START = 0.8

MASK_CODE = r"""// Actor shadow disc (task-472-02): full alpha up to Edge x radius, smoothstep to 0 at the radius, times Op.
float d = length(UV - float2(0.5, 0.5)) * 2.0;
return (1.0 - smoothstep(Edge, 1.0, d)) * saturate(Op);
"""

LOG_LINES = []


def log(msg):
    line = "{} {}".format(TAG, msg)
    LOG_LINES.append(line)
    unreal.log_warning(line)


def srgb_to_linear(c):
    c = c / 255.0
    return c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4


def connect(src, dst, input_name, out_name=""):
    if not MEL.connect_material_expressions(src, out_name, dst, input_name):
        raise RuntimeError("connect {} -> {}.{} failed".format(src.get_name(), dst.get_name(), input_name))


def run():
    m = unreal.EditorAssetLibrary.load_asset(PATH)
    if not m:
        raise RuntimeError("missing " + PATH)
    # Deleting expressions asserts (!IsRooted) in 5.8 headless (delete_all and one by one): reuse the task-264 nodes (TexCoord,
    # Tint, Opacity, Softness renamed EdgeStart) and add the mask node; the old falloff chain is left unconnected (not compiled).
    exprs = list(MEL.get_material_expressions(m))

    def find(cls, name=None):
        for e in exprs:
            if isinstance(e, cls) and (name is None or str(e.get_editor_property("parameter_name")) == name):
                return e
        return None

    m.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    m.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    m.set_editor_property("two_sided", True)

    tint = find(unreal.MaterialExpressionVectorParameter, "Tint") or MEL.create_material_expression(m, unreal.MaterialExpressionVectorParameter, -700, -200)
    tint.set_editor_property("parameter_name", "Tint")
    r, g, b = TINT_LINEAR
    tint.set_editor_property("default_value", unreal.LinearColor(r, g, b, 1.0))

    uv = find(unreal.MaterialExpressionTextureCoordinate) or MEL.create_material_expression(m, unreal.MaterialExpressionTextureCoordinate, -700, 100)

    edge = find(unreal.MaterialExpressionScalarParameter, "EdgeStart") or find(unreal.MaterialExpressionScalarParameter, "Softness")         or MEL.create_material_expression(m, unreal.MaterialExpressionScalarParameter, -700, 250)
    edge.set_editor_property("parameter_name", "EdgeStart")
    edge.set_editor_property("default_value", EDGE_START)

    op = find(unreal.MaterialExpressionScalarParameter, "Opacity") or MEL.create_material_expression(m, unreal.MaterialExpressionScalarParameter, -700, 400)
    op.set_editor_property("parameter_name", "Opacity")
    op.set_editor_property("default_value", OPACITY)
    op.set_editor_property("use_custom_primitive_data", True)
    op.set_editor_property("primitive_data_index", 0)

    old_mask = None
    for e in exprs:
        if isinstance(e, unreal.MaterialExpressionCustom) and str(e.get_editor_property("description")) == "BlobShadowMask":
            old_mask = e
    mask = old_mask or MEL.create_material_expression(m, unreal.MaterialExpressionCustom, -350, 200)
    mask.set_editor_property("description", "BlobShadowMask")
    mask.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT1)
    mask.set_editor_property("code", MASK_CODE)
    ins = []
    for n in ("UV", "Edge", "Op"):
        ci = unreal.CustomInput()
        ci.set_editor_property("input_name", n)
        ins.append(ci)
    mask.set_editor_property("inputs", ins)
    connect(uv, mask, "UV")
    connect(edge, mask, "Edge")
    connect(op, mask, "Op")

    if not MEL.connect_material_property(tint, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("emissive connect failed")
    if not MEL.connect_material_property(mask, "", unreal.MaterialProperty.MP_OPACITY):
        raise RuntimeError("opacity connect failed")

    MEL.recompile_material(m)
    if not unreal.EditorAssetLibrary.save_loaded_asset(m, only_if_is_dirty=False):
        raise RuntimeError("save failed")

    mi = unreal.EditorAssetLibrary.load_asset(MI_PATH)
    if mi:
        MEL.clear_all_material_instance_parameters(mi)
        MEL.update_material_instance(mi)
        unreal.EditorAssetLibrary.save_loaded_asset(mi, only_if_is_dirty=False)

    m2 = unreal.EditorAssetLibrary.load_asset(PATH)
    log("shading {} blend {} two_sided {}".format(m2.get_editor_property("shading_model"), m2.get_editor_property("blend_mode"),
                                                   m2.get_editor_property("two_sided")))
    log("expressions " + ", ".join(e.get_name() for e in MEL.get_material_expressions(m2)))
    em = MEL.get_material_property_input_node(m2, unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    opn = MEL.get_material_property_input_node(m2, unreal.MaterialProperty.MP_OPACITY)
    log("emissive <- {}; opacity <- {}".format(em.get_name() if em else None, opn.get_name() if opn else None))
    for e in MEL.get_material_expressions(m2):
        if isinstance(e, unreal.MaterialExpressionScalarParameter):
            log("scalar {} default {} cpd {} index {}".format(e.get_editor_property("parameter_name"), e.get_editor_property("default_value"),
                                                           e.get_editor_property("use_custom_primitive_data"), e.get_editor_property("primitive_data_index")))
        if isinstance(e, unreal.MaterialExpressionVectorParameter):
            log("vector {} default {}".format(e.get_editor_property("parameter_name"), e.get_editor_property("default_value")))
    if mi:
        log("MI scalar params {}".format(MEL.get_scalar_parameter_names(mi)))
        log("MI Tint {}".format(MEL.get_material_instance_vector_parameter_value(mi, "Tint")))


try:
    run()
    log("done")
except Exception as exc:
    log("error {}".format(exc))
    raise
finally:
    out = os.environ.get("BLOBSHADOW_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LOG_LINES) + "\n")
