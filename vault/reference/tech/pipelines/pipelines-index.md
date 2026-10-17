---
id: pipelines-index
description: "Asset pipeline (image, 3D generation, cleanup), run log schema, build and toolchain. Open for asset import, logs or build issues."
parent: tech-index
kind: note
---
# Pipelines

## Relations
- [[tech-index]] — parent index.
- [[asset-pipeline]] — image, 3D generation, cleanup, rigid-part split.
- [[blender-asset-route]] — Blender scripts for simple props: headless command, UV channels, mask contract, preview render, FBX quirks.
- [[mesh-fidelity-loop]] — validation of a Blender-route mesh against its source image: criteria, comparison render, max returns, report fields.
- [[chaser-model-pipeline]] — chaser Blender model split by part: files, commands, contract per part, part preview, how to edit one part.
- [[chaser-meshy-pipeline]] — the animated chaser from the Meshy mesh: Blender scripts, bake format, Unreal assets and parameters.
- [[level-layout-schema]] — layout JSON format, brief format, validator. Read before writing or loading a layout.
- [[layout-generator-recipe]] — algorithm behind arena_m1 (outline, organic paths, hidden pockets, heal ore); read before writing the C++ generator.
- [[layout-review-rules]] — review rules for generated 240 layouts: seed pick, dead zones, straight lanes, pocket distance. Open before writing a layout.
- [[run-log-schema]] — what each run log records, for the balance agent.
- [[text-content-schema]] — player-facing text JSON files, retrieval log and critic review; read before writing, validating or loading game text.
- [[unreal-mcp]] — Epic's MCP plugin: start, connection, toolsets, limits.
- [[build-toolchain]] — engine, compiler, plugins, regenerating project files.
- [[tool-unreal-reimport]] — headless reimport of static meshes in place from their FBX; use after a Blender re-export.
- [[barrel-asset]] — the barrel as built and integrated: files, slots, decals, wear, rejected approaches, Unreal side.
- [[kie-image-workflow]] — Kie.ai image generation in practice: hosts, variables, costs, permissions, rules.
- [[barrel-asset-history]] — task map of the barrel work, what each step superseded, Kie credits spent.
