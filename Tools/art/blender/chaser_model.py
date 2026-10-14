"""Thin wrapper (task-151-05): the chaser model now lives in Tools/art/blender/chaser/ (see vault chaser-model-pipeline). Same command line as before:
  "$BLENDER_EXE" -b --factory-startup -P Tools/art/blender/chaser_model.py -- --out Art/Meshes/chaser/SM_Chaser_parts.fbx [--render-dir Art/Meshes/chaser]"""
import os, runpy
_here = os.path.dirname(os.path.abspath(__file__)) if "__file__" in globals() else os.path.abspath("Tools/art/blender")
runpy.run_path(os.path.join(_here, "chaser", "assemble.py"), run_name="__main__")
