---
id: art-3d-lessons
description: "Owner art preferences, art and 3D agent limits, Kie.ai runs, FBX and import traps. Read by both agents and before art or 3D work."
parent: agents-index
kind: note
---
# Art and 3D lessons

From the barrel (2026-10-11), XP meshes, chaser and card sessions (to 2026-10-14). General owner rules: [[orchestrator-lessons]].

## Owner preferences
- Show images: every generated image or render goes to the owner with `SendUserFile`, as soon as it exists; reading a file shows it to nobody. Send a close-up when a detail is discussed. He may reject every proposal and pick another asset as the base (the stat card frame became the weapon frame).
- Judge by looking: when he says a result differs from the reference, open the reference and crop it close before replying (five wear attempts failed on a misread target).
- One change at a time per pass; he decides each next step.
- Meshy is not the default; he wants to see what Claude can do in Blender ([[blender-asset-route]]).
- He changes the charter when it blocks the result: state the consequences, offer levels, default to the lightest, then amend the rule and the pipeline node ([[art-style-guide]] A1, A14).
- Only the retained image or mesh is committed; rejected candidates stay untracked. The Orchestrator copies a chosen image to `vault/assets/` only when he says so.

## Agent limits
- The agents check by numbers and file contents; the Orchestrator renders and shows ([[blender-asset-route]]).
- A third-party image from the owner is a visual reference only: draw an own version and check it does not resemble it too closely.
- `Tools/art/kie_image.js` names a run by its timestamp to the second: never run it in parallel, even for different assets (task-168-02 lost 22 of 24 images, about 144 credits).
- A headless Blender run can outlive the agent (task-230-06 left `blender -b ... rock_slab_broken.py` at 13 GB for hours): the 3d agent stops its Blender processes before reporting; the Orchestrator checks `Get-Process blender` when closing a 3d subtask.
- A resumed agent keeps its first label: later subtasks are costed under it. One agent per subtask when cost matters.

- A Blender-route script imports `rr_blender` and never redefines its helpers; it reads only the module header docstring for the API ([[blender-asset-route]]).

## Engine and import traps
- Blender FBX export uses `FBX_SCALE_UNITS`; `FBX_SCALE_NONE` imports 100 times too large ([[chaser-meshy-pipeline]]). For an existing asset, reimport with [[tool-unreal-reimport]], editor closed.
- Generate Lightmap UVs can overwrite an extra UV channel in the render data only: read RENDER_DATA with Geometry Script (`copy_mesh_from_static_mesh`) and check every triangle corner.
- Static mesh build settings headless: `UnrealEditor-Cmd ... -ExecutePythonScript=<file>`; plain `-run=pythonscript` applies nothing.
- Meshy multi-view: up to three extra views (Front, Left, Back, Right), same size, angle and framing, subject centred with margin; 90 degree profiles worked.

## Brief mistakes to avoid
- Do not specify a position rule from a distant case ("bottom of the block, floorless cells included" sank the barrel 200 uu, task-108): read the code constants first.
- Specify camera, light and scale of every preview so renders stay comparable.

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-lessons]] — the general owner rules.
- [[agent-art]] — the art agent.
- [[agent-3d]] — the 3D agent.
- [[chaser-meshy-pipeline]] — the Meshy pipeline these traps come from.
- [[barrel-asset]] — the first asset these lessons come from.
