---
id: mesh-fidelity-loop
description: "Validation loop for Blender-route meshes built from a source image: criteria list in the subtask, comparison render, max returns, report fields. Open before a 3D subtask."
parent: pipelines-index
kind: doc
---
# Mesh fidelity loop

Why (owner 2026-10-13, task-128 and task-129): a mesh built blind from a verbal summary of the image passed every numeric check (tris, size, pivot) and still looked far from the image. A mesh is now judged against the image, criterion by criterion, with a bounded number of returns.

## What the subtask holds
- `fidelity_criteria`: a numbered list C1..Cn written by the Orchestrator from the source image. Each criterion is a yes/no check on a render (shape, proportion, count of parts, position, detail kind). A proportion is given as a ratio, with a tolerance.
- `max_returns`: 5 (owner 2026-10-13, provisional; "if necessary", a lower count is fine when all criteria pass earlier).
- `render`: camera yaw and elevation close to the source image, light-grey background (A16), the light of [[blender-asset-route]].

## Loop (the 3D agent, Blender route)
1. Build the first version from the criteria, not from a summary of the image.
2. Render it with the subtask camera; open the render and the source image in the same step.
3. For each criterion write `pass` or `fail` and one sentence of evidence. A criterion is never judged from the script, only from the render.
4. If every criterion passes: stop. If some fail and fewer than `max_returns` returns were used: change only what the failed criteria name, rebuild, count one return, go to 2.
5. After `max_returns` returns with failures left: stop, keep the best version, report the open failures. No sixth try, no workaround.

## What is not judged
- Glow and halo: Blender's standard render does not bloom an emissive material; Unreal does ([[blender-asset-route]]). Judge the emissive colour and the shape that carries it.
- Painted base texture is not used. Details that are painted in the image (traces, marks, signs) are geometry or textured decals ([[art-style-guide]] A1; owner 2026-10-13: textures serve details).

## Budget
The triangle budget is a ceiling, not a target. Using less is fine only when all criteria pass.

## Report
`.report.json` gets a `fidelity` object: `returns_used`, and per criterion `verdict` and `evidence`; plus `final_render` (path). The Orchestrator sends the final render with `SendUserFile`; the owner makes the last call.

## Relations
- [[pipelines-index]] — parent index.
- [[blender-asset-route]] — scripts, camera, light, UV conventions.
- [[agent-3d]] — the agent that runs the loop.
- [[art-style-guide]] — A1 texture charter the criteria respect.
- [[art-3d-lessons]] — agents cannot see unprompted: the loop forces the comparison.
