"""M_RockSlab wall look of task-478 (owner image 3, "coupe de principe"): sides without dark strata
or visible facet cuts, soft vertical streaks, orange emissive on the outer edge of the top face.

Unreal Editor Python, headless, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path>/Tools/unreal/rock_slab_wall_section.py"
        -unattended -nopause -nosplash -stdout

Idempotent. Runs after rock_slab_wall_look.py (task-471). On M_RockSlab it:
  - removes the StrataParams parameter (horizontal strata of task-471);
  - adds vector parameters SideParams (x lit share of the side colour (the side is drawn as emissive), y side normal flatten 0..1,
    z streak scale uu, w streak darkness) and EdgeParams (x distance inside the outline where the top
    glow starts to fade uu, y fade width uu, z top glow gain, w cell size uu);
  - rewrites RockColor (sides: dark purple, soft vertical streaks, foot darkening; no strata);
  - rewrites RockRim (rim triangles, top-face edge glow from the distance to the piece outline
    computed from the per-instance solid mask,
    thin band just under the lip, faint side emissive with the same streaks);
  - adds RockSpec (specular 0 on the side faces) and RockNormal (world-space normal: sides bent toward +Z so facets light alike) and turns
    tangent_space_normal off;
then recompiles and saves, and writes the parameter values on MI_RockSlab. Each step prints a
line tagged WALLSECTION.
Override values: environment WALLSECTION_PARAMS="Name=r,g,b,a;Name=r,g,b,a".
"""

import os

import unreal

TAG = "WALLSECTION"
MEL = unreal.MaterialEditingLibrary
PATH = "/Game/RogueRigs/Environment/Rock/M_RockSlab"
MI_PATH = "/Game/RogueRigs/Environment/Rock/MI_RockSlab"

STREAK = r"""
float2 sw = WP.xy / max(SideP.z, 1.0);
float sa = sin(sw.x * 1.00 + 1.7 * sin(sw.x * 0.37 + 0.4)) + sin(sw.y * 1.13 + 1.3 * sin(sw.y * 0.29 + 2.1) + 0.8);
float sb = sin(sw.x * 2.71 + 0.9) * 0.5 + sin(sw.y * 2.39 + 2.6) * 0.5;
float sv = 0.5 + 0.25 * sa + 0.15 * sb;
float streak = 1.0 - SideP.w * smoothstep(0.55, 0.95, sv);
"""

COLOR_CODE = r"""// M_RockSlab base colour: top (world UV, two scales), rim, sides, cracks by quadrant damage.
// task-478: sides are one dark purple with soft vertical streaks (no strata), darker toward the foot over the block height RimP.w.
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
""" + STREAK + r"""
// Sides are drawn by RockRim's emissive; only a small lit share SideP.x stays, so light and shadow do not cut the facets.
float3 sideC = SideCol * SideP.x * lerp(TopP.w, 1.0, saturate(hz / max(RimP.w, 1.0))) * streak * (1.0 - CrackP.w * cd);
return isSide > 0.5 ? sideC : topC;
"""

RIM_CODE = r"""// M_RockSlab emissive (task-478, owner image 3): orange on the outer edge of the top face, fading inward
// (distance to the piece outline from the per-instance solid mask, continuous across pieces), on the rim triangles,
// a thin glow just under the lip, and the dark purple sides themselves (SideCol, streaks, foot darkening TopP.w).
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
float cs = max(EdgeP.w, 1.0);
float H = 0.5 * cs;
// Piece centre: the nearest cell corner (pieces sit on the corners of the 67 uu grid, origin 0).
float2 rel = WP.xy - floor(WP.xy / cs + 0.5) * cs;
float b[4];
[unroll] for (int j = 0; j < 4; j++) { b[j] = floor(fmod(floor(SolidMask / pow(2.0, (float)j) + 0.001), 2.0) + 0.5); }
float n = b[0] + b[1] + b[2] + b[3];
// Distance inside the rock to the piece outline (rock_slab_pieces.py: straight line through the centre for an edge,
// quarter circles of radius H around the lone solid corner or around the empty corners), wobble ignored.
float d = 1.0e4;
float2 c0 = float2(-H, -H); float2 c1 = float2(H, -H); float2 c2 = float2(-H, H); float2 c3 = float2(H, H);
if (n > 0.5 && n < 1.5) {
    float2 cc = b[0] > 0.5 ? c0 : (b[1] > 0.5 ? c1 : (b[2] > 0.5 ? c2 : c3));
    d = H - length(rel - cc);
} else if (n > 2.5 && n < 3.5) {
    float2 ce = b[0] < 0.5 ? c0 : (b[1] < 0.5 ? c1 : (b[2] < 0.5 ? c2 : c3));
    d = length(rel - ce) - H;
} else if (n > 1.5 && n < 2.5) {
    if (b[0] > 0.5 && b[1] > 0.5) d = -rel.y;
    else if (b[2] > 0.5 && b[3] > 0.5) d = rel.y;
    else if (b[0] > 0.5 && b[2] > 0.5) d = -rel.x;
    else if (b[1] > 0.5 && b[3] > 0.5) d = rel.x;
    else if (b[0] > 0.5) d = min(length(rel - c1), length(rel - c2)) - H;
    else d = min(length(rel - c0), length(rel - c3)) - H;
}
float e = 1.0 - smoothstep(EdgeP.x, EdgeP.x + max(EdgeP.y, 0.1), d);
float topGlow = (1.0 - isSide) * EdgeP.z * e * e * (1.0 - 0.5 * broken);
float rimGlow = RimP.x * isRim * (1.0 - isSide);
""" + STREAK + r"""
float3 sideE = SideCol * streak * lerp(TopP.w, 1.0, saturate(hz / max(RimP.w, 1.0))) * isSide;
return RimCol * (max(rimGlow, topGlow) + RimP.y * band) + sideE;
"""

NORMAL_CODE = r"""// M_RockSlab world normal (task-478): side faces bent toward +Z by SideP.y so neighbouring facets
// and blocks light alike (no visible cut); top and rim keep the vertex normal.
float broken = UV3.y > 0.0005 ? 1.0 : 0.0;
float3 gN = cross(ddx(WP), ddy(WP));
float gz = abs(gN.z) / max(length(gN), 0.000001);
float isSide = (broken > 0.5 ? gz : abs(VN.z)) < 0.5 ? 1.0 : 0.0;
float3 n = normalize(lerp(VN, float3(0.0, 0.0, 1.0), saturate(SideP.y)));
return isSide > 0.5 ? n : VN;
"""

SPEC_CODE = r"""// M_RockSlab specular (task-478): none on the side faces, whose bent-up normal would otherwise
// catch a grazing sheen; the engine default 0.5 elsewhere.
float broken = UV3.y > 0.0005 ? 1.0 : 0.0;
float3 gN = cross(ddx(WP), ddy(WP));
float gz = abs(gN.z) / max(length(gN), 0.000001);
float isSide = (broken > 0.5 ? gz : abs(VN.z)) < 0.5 ? 1.0 : 0.0;
return isSide > 0.5 ? 0.0 : 0.5;
"""

DEFAULTS = {
    "SideParams": (0.03, 1.0, 25.0, 0.6),
    "EdgeParams": (-1.0, 16.0, 3.0, 67.0),
    "RimParams": (1.6, 2.0, 5.0, 153.0),
}

# Instance-only values (MI_RockSlab), on parameters that already exist.
MI_ONLY = {
    "SideColor": (0.06, 0.018, 0.085, 1.0),
}


def parse_overrides():
    raw = os.environ.get("WALLSECTION_PARAMS", "").strip()
    if not raw:
        return
    for item in raw.split(";"):
        if not item.strip():
            continue
        name, vals = item.split("=", 1)
        v = [float(x) for x in vals.split(",")]
        target = MI_ONLY if name.strip() in MI_ONLY else DEFAULTS
        target[name.strip()] = tuple(v)


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
    if name in DEFAULTS:
        r, g, b, a = DEFAULTS[name]
        p.set_editor_property("default_value", unreal.LinearColor(r, g, b, a))
    return p


def connect(src, dst, input_name, out_name=""):
    if not MEL.connect_material_expressions(src, out_name, dst, input_name):
        raise RuntimeError("connect {} -> {}.{} failed".format(src.get_name(), dst.get_name(), input_name))


def set_inputs(custom, names):
    ins = []
    for n in names:
        ci = unreal.CustomInput()
        ci.set_editor_property("input_name", n)
        ins.append(ci)
    custom.set_editor_property("inputs", ins)


def custom_node(m, desc, x, y, out_type):
    c = find_custom(m, desc)
    if c is None:
        c = MEL.create_material_expression(m, unreal.MaterialExpressionCustom, x, y)
        c.set_editor_property("description", desc)
        log("created {}".format(desc))
    c.set_editor_property("output_type", out_type)
    return c


def run():
    parse_overrides()
    m = unreal.EditorAssetLibrary.load_asset(PATH)
    side_p = vector_param(m, "SideParams", -900, 1300)
    edge_p = vector_param(m, "EdgeParams", -900, 1500)
    rim_p = vector_param(m, "RimParams", -900, 1100)
    rim_col = find_param(m, "RimColor")
    side_col = find_param(m, "SideColor")
    if rim_col is None or side_col is None:
        raise RuntimeError("RimColor or SideColor not found")

    color = find_custom(m, "RockColor")
    if color is None:
        raise RuntimeError("RockColor not found")
    before = [str(i.get_editor_property("input_name")) for i in color.get_editor_property("inputs")]
    srcs = MEL.get_inputs_for_material_expression(m, color)
    outs = [MEL.get_input_node_output_name_for_material_expression(color, s) if s is not None else "" for s in srcs]
    keep = [(n, s, o) for n, s, o in zip(before, srcs, outs) if n not in ("StrataP", "SideP")]
    color.set_editor_property("code", COLOR_CODE)
    set_inputs(color, [n for n, _, _ in keep] + ["SideP"])
    for name, src, out in keep:
        if src is not None:
            connect(src, color, name, out)
    connect(side_p, color, "SideP", "RGBA")
    log("RockColor inputs " + ", ".join(str(i.get_editor_property("input_name")) for i in color.get_editor_property("inputs")))

    strata = find_param(m, "StrataParams")
    if strata is not None:
        MEL.delete_material_expression(m, strata)
        log("deleted StrataParams")

    wp = find_by_name(m, "MaterialExpressionWorldPosition_1")
    op = find_by_name(m, "MaterialExpressionObjectPositionWS_0")
    vc = find_by_name(m, "MaterialExpressionVertexColor_0")
    uv3 = find_by_name(m, "MaterialExpressionTextureCoordinate_1")
    vn = find_by_name(m, "MaterialExpressionVertexNormalWS_0")
    wpo = find_custom(m, "RockWPO")
    wpo_names = [str(i.get_editor_property("input_name")) for i in wpo.get_editor_property("inputs")]
    mask = MEL.get_inputs_for_material_expression(m, wpo)[wpo_names.index("SolidMask")]
    log("SolidMask source {}".format(mask.get_name()))

    rim = custom_node(m, "RockRim", -400, 900, unreal.CustomMaterialOutputType.CMOT_FLOAT3)
    rim.set_editor_property("code", RIM_CODE)
    set_inputs(rim, ["WP", "OP", "VC", "UV3", "VN", "RimCol", "RimP", "SideCol", "SideP", "EdgeP", "SolidMask", "TopP"])
    connect(wp, rim, "WP")
    connect(op, rim, "OP")
    connect(vc, rim, "VC")
    connect(uv3, rim, "UV3")
    connect(vn, rim, "VN")
    connect(rim_col, rim, "RimCol")
    connect(rim_p, rim, "RimP", "RGBA")
    connect(side_col, rim, "SideCol")
    connect(side_p, rim, "SideP", "RGBA")
    connect(edge_p, rim, "EdgeP", "RGBA")
    connect(mask, rim, "SolidMask")
    connect(find_param(m, "TopParams"), rim, "TopP", "RGBA")

    node = MEL.get_material_property_input_node(m, unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    if not isinstance(node, unreal.MaterialExpressionAdd):
        raise RuntimeError("emissive Add not found")
    connect(rim, node, "B")

    nrm = custom_node(m, "RockNormal", -400, 1500, unreal.CustomMaterialOutputType.CMOT_FLOAT3)
    nrm.set_editor_property("code", NORMAL_CODE)
    set_inputs(nrm, ["WP", "UV3", "VN", "SideP"])
    connect(wp, nrm, "WP")
    connect(uv3, nrm, "UV3")
    connect(vn, nrm, "VN")
    connect(side_p, nrm, "SideP", "RGBA")
    m.set_editor_property("tangent_space_normal", False)
    if not MEL.connect_material_property(nrm, "", unreal.MaterialProperty.MP_NORMAL):
        raise RuntimeError("normal connect failed")

    spec = custom_node(m, "RockSpec", -400, 1700, unreal.CustomMaterialOutputType.CMOT_FLOAT1)
    spec.set_editor_property("code", SPEC_CODE)
    set_inputs(spec, ["WP", "UV3", "VN"])
    connect(wp, spec, "WP")
    connect(uv3, spec, "UV3")
    connect(vn, spec, "VN")
    if not MEL.connect_material_property(spec, "", unreal.MaterialProperty.MP_SPECULAR):
        raise RuntimeError("specular connect failed")

    MEL.recompile_material(m)
    if not unreal.EditorAssetLibrary.save_loaded_asset(m, only_if_is_dirty=False):
        raise RuntimeError("save failed")
    log("material saved")

    mi = unreal.EditorAssetLibrary.load_asset(MI_PATH)
    vals = dict(DEFAULTS)
    vals.update(MI_ONLY)
    for name, (r, g, b, a) in vals.items():
        MEL.set_material_instance_vector_parameter_value(mi, name, unreal.LinearColor(r, g, b, a))
    MEL.update_material_instance(mi)
    if not unreal.EditorAssetLibrary.save_loaded_asset(mi, only_if_is_dirty=False):
        raise RuntimeError("MI save failed")
    mi = unreal.EditorAssetLibrary.load_asset(MI_PATH)
    for name in ("SideParams", "EdgeParams", "RimParams", "SideColor", "RimColor"):
        c = MEL.get_material_instance_vector_parameter_value(mi, name)
        log("MI {} = ({:g}, {:g}, {:g}, {:g})".format(name, c.r, c.g, c.b, c.a))
    m2 = unreal.EditorAssetLibrary.load_asset(PATH)
    log("tangent_space_normal {}".format(m2.get_editor_property("tangent_space_normal")))
    for desc in ("RockColor", "RockRim", "RockNormal", "RockSpec"):
        log("{} sources ".format(desc) + ", ".join(x.get_name() if x else "None" for x in MEL.get_inputs_for_material_expression(m2, find_custom(m2, desc))))


try:
    run()
    log("done")
except Exception as exc:
    log("error {}".format(exc))
    raise
finally:
    out = os.environ.get("WALLSECTION_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LOG_LINES) + "\n")
