---
id: task-108-02-weapon-icons-placeholder
description: "The validated icon images become transparent 256 x 256 PNGs, ready to import."
parent: task-108-weapon-icons-placeholder
kind: task
status: in-progress
---
# Prepare the icons

- goal: The validated icon images become transparent 256 x 256 PNGs, ready to import.
- agent: cpp
- depends_on: task-108-01-weapon-icons-placeholder (owner validation)
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: a small tool Tools/art/ui_icon_prepare.js (Node; or headless Blender if the repo already uses it for images; no new heavy dependency, say which and why) that takes a validated icon from vault/assets/ (or Art/Source/weapon-icons/), cuts out the flat background by colour distance from the corners with a tolerance (soft edge, no halo), trims to the content, centres it with a 10 % margin on a 256 x 256 transparent canvas and writes Art/UI/Icons/T_Icon_<name>.png (names: padlock, burst_rifle, shotgun, turret_launcher). Run it on the four validated icons, check each output (size, alpha present, corner pixels fully transparent, content not clipped) by script and report the numbers; no Unreal import here. Document the tool in tech/pipelines/asset-pipeline (one short paragraph). Git: commit the tool and the four PNGs by name.
- done_when: four transparent 256 x 256 PNGs in Art/UI/Icons, checked by script; tool documented; lint clean; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-108-weapon-icons-placeholder]] — parent global task.
