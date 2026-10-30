---
id: pipelines-index
description: "Asset pipeline (image, 3D generation, cleanup), run log schema, build and toolchain. Open for asset import, logs or build issues."
parent: tech-index
kind: note
---
# Pipelines

## Children
- [[asset-pipeline]] — image, 3D generation, cleanup, rigid-part split.
- [[blender-asset-route]] — Blender scripts for simple props: headless command, UV channels, mask contract, preview render, FBX quirks.
- [[mesh-fidelity-loop]] — validation of a Blender-route mesh against its source image: criteria, comparison render, max returns, report fields.
- [[chaser-model-pipeline]] — chaser Blender model split by part: files, commands, contract per part, part preview, how to edit one part.
- [[chaser-meshy-pipeline]] — the animated chaser from the Meshy mesh: Blender scripts, bake format, Unreal assets and parameters.
- [[robots-meshy-raw-2026-10-22]] — raw Meshy meshes of heavy, kamikaze, shooter and chaser v2 (owner 2026-10-22): files, measures, texture defects.
- [[robot-parts-bake-pipeline]] — per-robot config of the part cut, walk bake and check (heavy): commands, planted-foot walk, texture rows.
- [[level-layout-schema]] — layout JSON format, brief format, validator. Read before writing or loading a layout.
- [[layout-generator-recipe]] — algorithm behind arena_m1 (outline, organic paths, hidden pockets, heal ore); read before writing the C++ generator.
- [[layout-review-rules]] — review rules for generated 240 layouts: seed pick, dead zones, straight lanes, pocket distance. Open before writing a layout.
- [[run-log-schema]] — what each run log records, for the balance agent.
- [[text-content-schema]] — player-facing text JSON files, retrieval log and critic review; read before writing, validating or loading game text.
- [[style-loop]] — evaluate-and-refine loop on generated text: command, script, run layout, score file, gate; open to run it.
- [[unreal-mcp]] — Epic's MCP plugin: start, connection, toolsets, limits.
- [[build-toolchain]] — engine, compiler, plugins, regenerating project files.
- [[tool-unreal-reimport]] — headless reimport of static meshes in place from their FBX; use after a Blender re-export.
- [[tool-capture]] — Tools/capture scripts: capture.js, sheet.js comparison sheets and crops, commands.js rr.* list, ffmpeg finder.
- [[barrel-asset]] — the barrel as built and integrated: files, slots, decals, wear, rejected approaches, Unreal side.
- [[asteroid-asset]] — the four background asteroid meshes (Blender route): files, budget, seeds, rebuild command.
- [[kie-image-workflow]] — Kie.ai image generation in practice: hosts, variables, costs, permissions, rules.
- [[miner-mixamo-clips]] — the nine Mixamo clips of the miner: measures per clip (direction, speed), in-place method, FBX export settings.
- [[barrel-asset-history]] — task map of the barrel work, what each step superseded, Kie credits spent.

## Relations
- [[tech-index]] — parent index.
