"""M_RockSlab look of task-480: thin orange-yellow emissive band on the geometric top chamfer, subtle glow on the top
near the edge, lit faceted sides with one violet tint per facet.

Unreal Editor Python, headless, editor closed:

    UnrealEditor-Cmd.exe <RogueRigs.uproject>
        -ExecutePythonScript="<abs path without spaces>/rock_slab_chamfer_facets.py"
        -unattended -nopause -nosplash -stdout

Idempotent. Runs after rock_slab_wall_section.py (task-478). On M_RockSlab it:
  - adds vector parameters FacetParams (x dark facet multiplier, y light facet multiplier, z share of dark facets,
    w share of light facets) and BandParams (x unused, y band start across the chamfer 0 top side..1 outer edge,
    z band softness, w intensity variation along the edge);
  - rewrites RockColor (sides: SideColor x facet tint x foot darkening, fully lit), RockRim (band on the chamfer triangles,
    glow on the top near the edge, side emissive share SideParams.x), RockNormal (real side normals unless SideParams.y) and
    RockSpec; sides are read from vertex colour G (Blender marks them), meshes without colours from derivatives;
then recompiles, saves and writes the values on MI_RockSlab.
SideParams now: x emissive share of the side colour, y side normal flatten 0..1, z period of the band variation uu, w unused.
Override values: environment ROCKCHAMFER_PARAMS="Name=r,g,b,a;Name=r,g,b,a". Lines tagged ROCKCHAMFER, also written to the
file named by env ROCKCHAMFER_OUT.
"""

import os

import unreal

TAG = "ROCKCHAMFER"
MEL = unreal.MaterialEditingLibrary
PATH = "/Game/RogueRigs/Environment/Rock/M_RockSlab"
MI_PATH = "/Game/RogueRigs/Environment/Rock/MI_RockSlab"

SIDE_TEST = r"""
// Never test UV3 here: intact pieces have 2 UV channels and read UV3.y above 0 (task-480 diagnostic: all pieces read as broken).
// Rock pieces mark side faces G = 1 (a lip face can lean past a normal test). Meshes without vertex colours read white
// (rock slot of the heal deposits): derivatives.
float3 gN = cross(ddx(WP), ddy(WP));
float gz = abs(gN.z) / max(length(gN), 0.000001);
float noVC = (VC.r > 0.99 && VC.g > 0.99 && VC.b > 0.99) ? 1.0 : 0.0;
float isSide = (noVC > 0.5 ? gz < 0.5 : VC.g > 0.5) ? 1.0 : 0.0;
"""

FACET = r"""
// One tint per facet: side faces carry a facet random value in vertex R, mixed with the piece position so pieces differ.
float2 fCell = floor(OP.xy / 10.0 + 0.5);
float fh = frac(sin(dot(fCell, float2(12.9898, 78.233)) + VC.r * 37.719) * 43758.5453);
float facet = fh < FacetP.z ? FacetP.x : (fh > 1.0 - FacetP.w ? FacetP.y : 1.0);
// Foot darkening from vertex colour B on intact sides (1 - height share); broken sides have B = 0.
float foot = lerp(1.0, TopP.w, saturate(VC.b));
"""

COLOR_CODE = r"""// M_RockSlab base colour (task-480): top (world UV, two scales), chamfer, lit sides with a tint per facet, cracks by quadrant damage.
float4 Dq = float4(Dv3, D3);
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
""" + SIDE_TEST + r"""
float isRim = (VC.g < 0.5 && VC.r > 0.75) ? 1.0 : 0.0;
c = c * lerp(1.0, TopP.z, isRim);
float2 rel = WP.xy - OP.xy;
int q = (rel.x >= 0.0 ? 1 : 0) + (rel.y >= 0.0 ? 2 : 0);
float d = Dq[q];
float cd = saturate(d / max(CrackP.x, 0.01));
float ck = Texture2DSample(CrackTex, CrackTexSampler, WP.xy / TileP.z).r;
float cm = smoothstep(1.0 - cd - CrackP.y, 1.0 - cd + 0.0001, ck) * (d > 0.0005 ? 1.0 : 0.0) * CrackP.z;
float3 topC = lerp(c, CrackCol, cm);
""" + FACET + r"""
float3 sideC = SideCol * facet * foot * (1.0 - CrackP.w * cd);
return isSide > 0.5 ? sideC : topC;
"""

RIM_CODE = r"""// M_RockSlab emissive (task-480): thin orange-yellow band on the chamfer triangles with a slow variation along the edge,
// subtle glow on the top face near the outline (distance from the per-instance solid mask), small emissive share of the sides.
""" + SIDE_TEST + r"""
float isRim = (VC.g < 0.5 && VC.r > 0.75) ? 1.0 : 0.0;
""" + FACET + r"""
float2 nq = WP.xy / max(SideP.z, 1.0);
float nA = sin(nq.x + 1.7 * sin(nq.y * 0.63 + 0.4));
float nB = sin(nq.y * 1.13 + 1.3 * sin(nq.x * 0.71 + 2.1) + 0.8);
float nv = saturate(0.5 + 0.25 * (nA + nB));
float gain = 1.0 + BandP.w * (2.0 * nv - 1.0);
// Band across the chamfer: u (vertex colour B) = 0 at the top, 1 at the outer edge; its start moves a little along the edge.
float u = saturate(VC.b);
float st = BandP.y + 0.3 * nB;
float bandT = smoothstep(st, st + max(BandP.z, 0.01), u);
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
float topGlow = (1.0 - isSide) * EdgeP.z * e * e * gain;
float band = RimP.x * isRim * bandT * gain;
float3 bandCol = RimCol * float3(1.0, 0.85 + 0.5 * nv, 1.0);
float3 sideE = SideCol * facet * foot * SideP.x * isSide;
return bandCol * band + RimCol * topGlow * (1.0 - isRim) + sideE;
"""

NORMAL_CODE = r"""// M_RockSlab world normal (task-480): real normals, side faces bent toward +Z only by SideP.y (0 in MI_RockSlab).
""" + SIDE_TEST + r"""
float3 n = normalize(lerp(VN, float3(0.0, 0.0, 1.0), saturate(SideP.y)));
return isSide > 0.5 ? n : VN;
"""

SPEC_CODE = r"""// M_RockSlab specular (task-480): none on the side faces, the engine default 0.5 elsewhere.
""" + SIDE_TEST + r"""
return isSide > 0.5 ? 0.0 : 0.5;
"""

# Parent defaults (M_RockSlab) and MI_RockSlab values. All provisional, owner 2026-11-06.
DEFAULTS = {
    "SideParams": (0.20, 0.0, 90.0, 0.0),
    "EdgeParams": (8.0, 16.0, 0.14, 67.0),
    "RimParams": (1.5, 0.0, 5.0, 153.0),
    "FacetParams": (0.55, 1.7, 0.38, 0.16),
    "BandParams": (8.0, 0.45, 0.4, 0.5),
}

# Instance-only values, on parameters that already exist.
MI_ONLY = {
    "SideColor": (0.05, 0.016, 0.13, 1.0),
    "RimColor": (1.0, 0.2, 0.015, 1.0),
    "TopParams": (0.5, 0.4, 0.55, 0.55),
}

LOG_LINES = []


def log(msg):
    line = "{} {}".format(TAG, msg)
    LOG_LINES.append(line)
    unreal.log_warning(line)


def parse_overrides():
    raw = os.environ.get("ROCKCHAMFER_PARAMS", "").strip()
    for item in raw.split(";"):
        if not item.strip():
            continue
        name, vals = item.split("=", 1)
        v = tuple(float(x) for x in vals.split(","))
        (MI_ONLY if name.strip() in MI_ONLY else DEFAULTS)[name.strip()] = v


def find_custom(m, desc):
    for e in MEL.get_material_expressions(m):
        if isinstance(e, unreal.MaterialExpressionCustom) and str(e.get_editor_property("description")) == desc:
            return e
    raise RuntimeError("{} not found".format(desc))


def find_param(m, name):
    for e in MEL.get_material_expressions(m):
        if isinstance(e, unreal.MaterialExpressionVectorParameter) and str(e.get_editor_property("parameter_name")) == name:
            return e
    return None


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


def inputs_of(m, custom):
    names = [str(i.get_editor_property("input_name")) for i in custom.get_editor_property("inputs")]
    srcs = MEL.get_inputs_for_material_expression(m, custom)
    outs = [MEL.get_input_node_output_name_for_material_expression(custom, s) if s is not None else "" for s in srcs]
    return dict((n, (s, o)) for n, s, o in zip(names, srcs, outs))


def rewire(m, custom, code, wanted, known):
    """Sets the code and the input list `wanted`; a pin keeps its source, else takes it from `known`."""
    old = inputs_of(m, custom)
    custom.set_editor_property("code", code)
    ins = []
    for n in wanted:
        ci = unreal.CustomInput()
        ci.set_editor_property("input_name", n)
        ins.append(ci)
    custom.set_editor_property("inputs", ins)
    for n in wanted:
        src, out = old.get(n, known.get(n, (None, "")))
        if src is None and n in known:
            src, out = known[n]
        if src is None:
            raise RuntimeError("no source for {}.{}".format(custom.get_name(), n))
        connect(src, custom, n, out)


def run():
    parse_overrides()
    m = unreal.EditorAssetLibrary.load_asset(PATH)
    side_p = vector_param(m, "SideParams", -900, 1300)
    edge_p = vector_param(m, "EdgeParams", -900, 1500)
    rim_p = vector_param(m, "RimParams", -900, 1100)
    facet_p = vector_param(m, "FacetParams", -900, 1700)
    band_p = vector_param(m, "BandParams", -900, 1900)
    color = find_custom(m, "RockColor")
    rim = find_custom(m, "RockRim")
    nrm = find_custom(m, "RockNormal")
    spec = find_custom(m, "RockSpec")
    pins = inputs_of(m, rim)
    known = dict(pins)
    known["FacetP"] = (facet_p, "RGBA")
    known["BandP"] = (band_p, "RGBA")
    known["SideP"] = (side_p, "RGBA")
    known["EdgeP"] = (edge_p, "RGBA")
    known["RimP"] = (rim_p, "RGBA")
    color_pins = inputs_of(m, color)
    rewire(m, color, COLOR_CODE, list(color_pins.keys()) + [n for n in ("FacetP",) if n not in color_pins], known)
    rewire(m, rim, RIM_CODE, list(pins.keys()) + [n for n in ("FacetP", "BandP") if n not in pins], known)
    for node, code in ((nrm, NORMAL_CODE), (spec, SPEC_CODE)):
        names = list(inputs_of(m, node).keys())
        rewire(m, node, code, names + [n for n in ("VC",) if n not in names], known)
    for c in (color, rim, nrm, spec):
        log("{} inputs {}".format(c.get_editor_property("description"), ", ".join(str(i.get_editor_property("input_name")) for i in c.get_editor_property("inputs"))))
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
    for name in vals:
        c = MEL.get_material_instance_vector_parameter_value(mi, name)
        log("MI {} = ({:g}, {:g}, {:g}, {:g})".format(name, c.r, c.g, c.b, c.a))


try:
    run()
    log("done")
except Exception as exc:
    log("error {}".format(exc))
    raise
finally:
    out = os.environ.get("ROCKCHAMFER_OUT")
    if out:
        with open(out, "w") as f:
            f.write("\n".join(LOG_LINES) + "\n")
