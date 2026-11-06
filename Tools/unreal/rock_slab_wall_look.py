"""M_RockSlab wall look of task-471 (owner image 2): orange glowing lip, horizontal side strata.

Unreal Editor Python, headless, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/rock_slab_wall_look.py"
        -unattended -nopause -nosplash -stdout

Idempotent. On M_RockSlab it:
  - adds vector parameters RimColor, RimParams (x rim-triangle glow, y side band glow, z side band
    height uu, w block top height uu) and StrataParams (x layer height uu, y line darkness,
    z line width as a fraction of a layer, w layer brightness spread);
  - rewrites the RockColor Custom node (sides: horizontal strata, foot darkening over the real
    block height) and adds the RimP and StrataP inputs;
  - adds a RockRim Custom node (emissive orange on the mesh rim triangles and on a band at the
    top of the side faces) summed with RockMarked into Emissive Color;
then recompiles and saves. Each step prints a line tagged WALLLOOK.
"""

import unreal

TAG = "WALLLOOK"
MEL = unreal.MaterialEditingLibrary
PATH = "/Game/RogueRigs/Environment/Rock/M_RockSlab"

COLOR_CODE = r"""// M_RockSlab base colour: top (world UV, two scales), rim, sides, cracks by quadrant damage.
// task-471: sides carry horizontal strata (StrataP) and darken toward the foot over the block height RimP.w.
float4 Dq = float4(Dv3, D3);
float broken = UV3.y > 0.0005 ? 1.0 : 0.0;
float2 uvA = WP.xy / TileP.x;
float cr = cos(radians(TileP.w));
float sr = sin(radians(TileP.w));
float2 uvB = WP.xy / TileP.y;
uvB = float2(cr * uvB.x - sr * uvB.y, sr * uvB.x + cr * uvB.y) + float2(0.37, 0.61);
float3 tA = Texture2DSample(TopTex, TopTexSampler, uvA).rgb;
float3 tB = Texture2DSample(TopTex, TopTexSampler, uvB).rgb;
float macro = dot(tB, float3(0.3333, 0.3333, 0.3334)) / max(TopP.y, 0.01);
float3 tex = tA * lerp(1.0, macro, TopP.x);
float tl0 = dot(tex, float3(0.3, 0.59, 0.11));
tex = tex * max(1.0 + (tl0 / 0.338 - 1.0) * Contr, 0.2);
float lum = dot(tex, float3(0.3, 0.59, 0.11));
float3 c = lerp(float3(lum, lum, lum), tex, 1.0 - Desat);
c = lerp(float3(lum, lum, lum), c, Sat);
c = c * Bright * TopTint;
float vr = VC.r;
// Side faces from the geometric normal; broken pieces from screen-space derivatives (task-230-22).
float3 gN = cross(ddx(WP), ddy(WP));
float gz = abs(gN.z) / max(length(gN), 0.000001);
float isSide = (broken > 0.5 ? gz : abs(VN.z)) < 0.5 ? 1.0 : 0.0;
float vcOk = (VC.g < 0.5 && VC.b < 0.5) ? 1.0 : 0.0;
float isRim = vcOk * (broken > 0.5 ? ((vr > 0.25 && vr < 0.75) ? 1.0 : 0.0) : (vr > 0.75 ? 1.0 : 0.0));
c = c * lerp(1.0, TopP.z, isRim);
float2 rel = WP.xy - OP.xy;
int q = (rel.x >= 0.0 ? 1 : 0) + (rel.y >= 0.0 ? 2 : 0);
float d = Dq[q];
float cd = saturate(d / max(CrackP.x, 0.01));
float ck = Texture2DSample(CrackTex, CrackTexSampler, WP.xy / TileP.z).r;
float cm = smoothstep(1.0 - cd - CrackP.y, 1.0 - cd + 0.0001, ck) * (d > 0.0005 ? 1.0 : 0.0) * CrackP.z;
float3 topC = lerp(c, CrackCol, cm);
float hz = WP.z - OP.z + 0.5 * RimP.w; // OP reads the instance bounds centre (half the block height above the floor top), not the pivot
float wob = 0.18 * sin(WP.x * 0.021 + WP.y * 0.017) + 0.12 * sin(WP.x * 0.047 - WP.y * 0.039);
float sv = hz / max(StrataP.x, 1.0) + wob;
float layer = floor(sv);
float fr = frac(sv);
float lw = max(StrataP.z, 0.001);
float ln = 1.0 - smoothstep(0.0, lw, fr) * smoothstep(0.0, lw, 1.0 - fr);
float lb = 1.0 + StrataP.w * (frac(sin(layer * 12.9898 + 4.1) * 43758.5453) - 0.5);
float strata = lb * (1.0 - StrataP.y * ln);
float3 sideC = SideCol * lerp(TopP.w, 1.0, saturate(hz / max(RimP.w, 1.0))) * strata * (1.0 - CrackP.w * cd);
return isSide > 0.5 ? sideC : topC;
"""

RIM_CODE = r"""// M_RockSlab orange lip glow (task-471, owner image 2): emissive on the mesh rim triangles
// (vertex colour R, as RockColor) and on a band at the top of the side faces, fading downward.
float broken = UV3.y > 0.0005 ? 1.0 : 0.0;
float3 gN = cross(ddx(WP), ddy(WP));
float gz = abs(gN.z) / max(length(gN), 0.000001);
float isSide = (broken > 0.5 ? gz : abs(VN.z)) < 0.5 ? 1.0 : 0.0;
float vr = VC.r;
float vcOk = (VC.g < 0.5 && VC.b < 0.5) ? 1.0 : 0.0;
float isRim = vcOk * (broken > 0.5 ? ((vr > 0.25 && vr < 0.75) ? 1.0 : 0.0) : (vr > 0.75 ? 1.0 : 0.0));
float hz = WP.z - OP.z + 0.5 * RimP.w; // OP reads the instance bounds centre (half the block height above the floor top), not the pivot
float t = saturate(1.0 - (RimP.w - hz) / max(RimP.z, 1.0));
float band = isSide * t * t * (1.0 - 0.5 * broken);
return RimCol * (RimP.x * isRim * (1.0 - isSide) + RimP.y * band);
"""

DEFAULTS = {
    "RimColor": (1.0, 0.32, 0.04, 1.0),
    "RimParams": (1.0, 6.0, 14.0, 153.0),
    "StrataParams": (34.0, 0.6, 0.1, 0.35),
}


LOG_LINES = []


def log(msg):
    line = "{} {}".format(TAG, msg)
    LOG_LINES.append(line)
    unreal.log_warning(line)


def find_exprs(m):
    return list(MEL.get_material_expressions(m))


def find_custom(m, desc):
    for e in find_exprs(m):
        if isinstance(e, unreal.MaterialExpressionCustom) and str(e.get_editor_property("description")) == desc:
            return e
    return None


def find_param(m, name):
    for e in find_exprs(m):
        if isinstance(e, unreal.MaterialExpressionVectorParameter) and str(e.get_editor_property("parameter_name")) == name:
            return e
    return None


def find_by_name(m, name):
    for e in find_exprs(m):
        if e.get_name() == name:
            return e
    raise RuntimeError("expression {} not found".format(name))


def vector_param(m, name, x, y):
    p = find_param(m, name)
    if p is None:
        p = MEL.create_material_expression(m, unreal.MaterialExpressionVectorParameter, x, y)
        p.set_editor_property("parameter_name", name)
        log("created param {}".format(name))
    r, g, b, a = DEFAULTS[name]
    p.set_editor_property("default_value", unreal.LinearColor(r, g, b, a))
    return p


def ensure_inputs(custom, names):
    ins = list(custom.get_editor_property("inputs"))
    have = [str(i.get_editor_property("input_name")) for i in ins]
    changed = False
    for n in names:
        if n not in have:
            ci = unreal.CustomInput()
            ci.set_editor_property("input_name", n)
            ins.append(ci)
            changed = True
    if changed:
        custom.set_editor_property("inputs", ins)
    return changed


def connect(src, dst, input_name, out_name=""):
    if not MEL.connect_material_expressions(src, out_name, dst, input_name):
        raise RuntimeError("connect {} -> {}.{} failed".format(src.get_name(), dst.get_name(), input_name))


def run():
    m = unreal.EditorAssetLibrary.load_asset(PATH)
    rim_col = vector_param(m, "RimColor", -900, 900)
    rim_p = vector_param(m, "RimParams", -900, 1100)
    strata_p = vector_param(m, "StrataParams", -900, 1300)

    color = find_custom(m, "RockColor")
    if color is None:
        raise RuntimeError("RockColor not found")
    # Keep the existing input connections: read their sources before touching the array.
    before = [str(i.get_editor_property("input_name")) for i in color.get_editor_property("inputs")]
    srcs = MEL.get_inputs_for_material_expression(m, color)
    outs = [MEL.get_input_node_output_name_for_material_expression(color, s) if s is not None else "" for s in srcs]
    log("RockColor old outputs " + ", ".join("{}={}".format(n, o) for n, o in zip(before, outs)))
    color.set_editor_property("code", COLOR_CODE)
    ensure_inputs(color, ["RimP", "StrataP"])
    for name, src, out in zip(before, srcs, outs):
        if src is not None:
            connect(src, color, name, out)
    connect(rim_p, color, "RimP", "RGBA")
    connect(strata_p, color, "StrataP", "RGBA")
    log("RockColor inputs " + ", ".join(str(i.get_editor_property("input_name")) for i in color.get_editor_property("inputs")))

    marked = find_custom(m, "RockMarked")
    rim = find_custom(m, "RockRim")
    if rim is None:
        rim = MEL.create_material_expression(m, unreal.MaterialExpressionCustom, -400, 900)
        rim.set_editor_property("description", "RockRim")
        rim.set_editor_property("output_type", unreal.CustomMaterialOutputType.CMOT_FLOAT3)
        log("created RockRim")
    rim.set_editor_property("code", RIM_CODE)
    ins = []
    for n in ("WP", "OP", "VC", "UV3", "VN", "RimCol", "RimP"):
        ci = unreal.CustomInput()
        ci.set_editor_property("input_name", n)
        ins.append(ci)
    rim.set_editor_property("inputs", ins)
    connect(find_by_name(m, "MaterialExpressionWorldPosition_1"), rim, "WP")
    connect(find_by_name(m, "MaterialExpressionObjectPositionWS_0"), rim, "OP")
    connect(find_by_name(m, "MaterialExpressionVertexColor_0"), rim, "VC")
    connect(find_by_name(m, "MaterialExpressionTextureCoordinate_1"), rim, "UV3")
    connect(find_by_name(m, "MaterialExpressionVertexNormalWS_0"), rim, "VN")
    connect(rim_col, rim, "RimCol")
    connect(rim_p, rim, "RimP", "RGBA")

    add = None
    node = MEL.get_material_property_input_node(m, unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    if isinstance(node, unreal.MaterialExpressionAdd):
        add = node
    else:
        add = MEL.create_material_expression(m, unreal.MaterialExpressionAdd, -150, 700)
        log("created emissive Add")
    connect(marked, add, "A")
    connect(rim, add, "B")
    if not MEL.connect_material_property(add, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR):
        raise RuntimeError("emissive connect failed")

    MEL.recompile_material(m)
    if not unreal.EditorAssetLibrary.save_loaded_asset(m, only_if_is_dirty=False):
        raise RuntimeError("save failed")
    m2 = unreal.EditorAssetLibrary.load_asset(PATH)
    em = MEL.get_material_property_input_node(m2, unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    log("saved; emissive <- {} inputs {}".format(em.get_name(), [x.get_name() if x else None for x in MEL.get_inputs_for_material_expression(m2, em)]))
    log("RockColor sources " + ", ".join(x.get_name() if x else "None" for x in MEL.get_inputs_for_material_expression(m2, find_custom(m2, "RockColor"))))
    log("RockRim sources " + ", ".join(x.get_name() if x else "None" for x in MEL.get_inputs_for_material_expression(m2, find_custom(m2, "RockRim"))))


try:
    run()
    log("done")
except Exception as exc:
    log("error {}".format(exc))
    raise
finally:
    import os
    out = os.environ.get("WALLLOOK_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LOG_LINES) + "\n")
