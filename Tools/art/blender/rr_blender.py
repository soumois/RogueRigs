"""Shared bpy helpers for Rogue Rigs 3d Blender-route scripts (task-363). Units: 1 Blender unit = 1 cm.
Use:  sys.path.insert(0, <Tools/art/blender>); import rr_blender as rr
Nothing runs at import time. Public functions (signatures are never changed; add new ones instead):
  import_fbx(path, clear=True) -> list of mesh objects        import an FBX (clear=True resets to an empty scene first)
  export_fbx(path, objects=None)                              export selected/given objects, centimetres (FBX_SCALE_UNITS), flat faces, no leaf bones
  tri_count(obj_or_mesh) -> int                               triangle count of a mesh or mesh object
  dimensions_cm(objects) -> [x, y, z]                         world bounding-box size in cm (rounded 3 decimals), one object or a list
  srgb(h) -> (r, g, b, 1.0)                                   "#RRGGBB" or 0-1 sRGB tuple to linear RGBA
  make_material(name, colour, roughness=0.8, metallic=0.0)    flat Principled material; colour = hex or 0-1 sRGB tuple
  setup_preview(res=1000, ortho_scale=200.0, azimuth=-35.0, elevation=28.0, distance=400.0, target=(0, 0, 67), sun_energy=4.0, sun_rotation=(50, 10, -60), world_strength=0.35) -> scene
                                                              EEVEE, Standard view transform, white world, orthographic camera and one sun (barrel_preview values)
  render_preview(path, **setup_kwargs) -> path                setup_preview then one PNG render
  render_turntable(path_prefix, n=8, **setup_kwargs) -> list  n renders at evenly spaced azimuths from `azimuth`, files <prefix>_<k>.png
  write_report(path, objects, materials=None, warnings=None, **extra) -> dict
                                                              report dict (tris, dimensions_cm, materials, uv_layers, warnings + extra keys), written as json when path is not None
"""
import json
import math
import os

import bpy
from mathutils import Euler, Vector


def import_fbx(path, clear=True):
    if clear:
        bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.fbx(filepath=path)
    return [o for o in bpy.data.objects if o.type == "MESH"]


def _as_list(objects):
    if objects is None:
        return []
    if isinstance(objects, (list, tuple, set)):
        return list(objects)
    return [objects]


def export_fbx(path, objects=None):
    objs = _as_list(objects)
    if objs:
        bpy.ops.object.select_all(action="DESELECT")
        for o in objs:
            o.select_set(True)
        bpy.context.view_layer.objects.active = objs[0]
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    bpy.ops.export_scene.fbx(
        filepath=path, use_selection=True, object_types={"MESH"},
        apply_scale_options="FBX_SCALE_UNITS", mesh_smooth_type="FACE",
        add_leaf_bones=False, path_mode="COPY", embed_textures=False)


def tri_count(obj_or_mesh):
    me = obj_or_mesh.data if hasattr(obj_or_mesh, "data") and hasattr(obj_or_mesh.data, "polygons") else obj_or_mesh
    return sum(len(p.vertices) - 2 for p in me.polygons)


def dimensions_cm(objects):
    objs = _as_list(objects)
    lo = [float("inf")] * 3
    hi = [float("-inf")] * 3
    for o in objs:
        for c in o.bound_box:
            w = o.matrix_world @ Vector(c)
            for i in range(3):
                lo[i] = min(lo[i], w[i])
                hi[i] = max(hi[i], w[i])
    return [round(hi[i] - lo[i], 3) for i in range(3)]


def _lin(x):
    return ((x + 0.055) / 1.055) ** 2.4 if x > 0.04045 else x / 12.92


def srgb(h):
    if isinstance(h, str):
        c = [int(h[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    else:
        c = list(h[:3])
    return tuple(_lin(x) for x in c) + (1.0,)


def make_material(name, colour, roughness=0.8, metallic=0.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    bsdf = m.node_tree.nodes.get("Principled BSDF") or m.node_tree.nodes.new("ShaderNodeBsdfPrincipled")
    rgba = srgb(colour)
    bsdf.inputs["Base Color"].default_value = rgba
    bsdf.inputs["Roughness"].default_value = roughness
    bsdf.inputs["Metallic"].default_value = metallic
    m.diffuse_color = rgba
    return m


def _preview_cam_and_sun(sc, ortho_scale, azimuth, elevation, distance, target, sun_energy, sun_rotation):
    az, el = math.radians(azimuth), math.radians(elevation)
    tgt = Vector(target)
    cam_d = bpy.data.cameras.new("RRCam")
    cam_d.type = "ORTHO"
    cam_d.ortho_scale = ortho_scale
    cam_d.clip_start = 1
    cam_d.clip_end = 2000
    cam = bpy.data.objects.new("RRCam", cam_d)
    sc.collection.objects.link(cam)
    cam.location = tgt + Vector((math.sin(az) * math.cos(el), -math.cos(az) * math.cos(el), math.sin(el))) * distance
    cam.rotation_euler = (tgt - cam.location).to_track_quat("-Z", "Y").to_euler()
    sc.camera = cam
    return cam


def setup_preview(res=1000, ortho_scale=200.0, azimuth=-35.0, elevation=28.0, distance=400.0,
                  target=(0, 0, 67), sun_energy=4.0, sun_rotation=(50, 10, -60), world_strength=0.35):
    sc = bpy.context.scene
    for name in ("BLENDER_EEVEE", "BLENDER_EEVEE_NEXT"):
        try:
            sc.render.engine = name
            break
        except TypeError:
            continue
    sc.render.resolution_x = sc.render.resolution_y = res
    sc.render.resolution_percentage = 100
    sc.render.image_settings.file_format = "PNG"
    sc.view_settings.view_transform = "Standard"
    sc.view_settings.look = "None"
    sc.view_settings.exposure = 0.0
    sc.view_settings.gamma = 1.0
    w = bpy.data.worlds.new("RRWorld")
    w.use_nodes = True
    sc.world = w
    bg = w.node_tree.nodes["Background"]
    bg.inputs["Color"].default_value = (1, 1, 1, 1)
    bg.inputs["Strength"].default_value = world_strength
    sun_d = bpy.data.lights.new("RRSun", "SUN")
    sun_d.energy = sun_energy
    sun = bpy.data.objects.new("RRSun", sun_d)
    sc.collection.objects.link(sun)
    sun.rotation_euler = Euler([math.radians(a) for a in sun_rotation], "XYZ")
    _preview_cam_and_sun(sc, ortho_scale, azimuth, elevation, distance, target, sun_energy, sun_rotation)
    return sc


def render_preview(path, **setup_kwargs):
    sc = setup_preview(**setup_kwargs)
    sc.render.filepath = path
    bpy.ops.render.render(write_still=True)
    return path


def render_turntable(path_prefix, n=8, **setup_kwargs):
    base = setup_kwargs.pop("azimuth", -35.0)
    sc = setup_preview(azimuth=base, **setup_kwargs)
    kw = dict(ortho_scale=setup_kwargs.get("ortho_scale", 200.0), elevation=setup_kwargs.get("elevation", 28.0),
              distance=setup_kwargs.get("distance", 400.0), target=setup_kwargs.get("target", (0, 0, 67)),
              sun_energy=0, sun_rotation=(0, 0, 0))
    paths = []
    for k in range(n):
        old = sc.camera
        if old is not None:
            bpy.data.objects.remove(old)
        _preview_cam_and_sun(sc, azimuth=base + 360.0 * k / n, **kw)
        p = "%s_%d.png" % (path_prefix, k)
        sc.render.filepath = p
        bpy.ops.render.render(write_still=True)
        paths.append(p)
    return paths


def write_report(path, objects, materials=None, warnings=None, **extra):
    objs = _as_list(objects)
    mats = materials
    if mats is None:
        mats = sorted({s.material.name for o in objs for s in o.material_slots if s.material})
    report = {
        "tris": sum(tri_count(o) for o in objs),
        "dimensions_cm": dimensions_cm(objs),
        "materials": list(mats),
        "uv_layers": sorted({u.name for o in objs for u in o.data.uv_layers}),
        "warnings": list(warnings or []),
    }
    report.update(extra)
    if path is not None:
        os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
        with open(path, "w", encoding="utf-8") as f:
            json.dump(report, f, indent=2)
    return report
