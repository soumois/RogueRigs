---
id: task-189-01-ore-chunk-look
description: "Art: in-game image proposals of the ore chunk pickup, a glowing Duskite crystal cluster drawn from the HUD ore icon, plus Meshy input views."
parent: task-189-ore-chunk-look
kind: task
status: in-progress
---
# Ore chunk: in-game image proposals

- goal: proposed in-game images of the ore chunk pickup (dropped by a broken Duskite deposit, 30 cm, replaces the cyan cube of BP_PickupOre): a glowing Duskite violet crystal cluster drawn from the owner-validated HUD icon `vault/assets/ore-hud-icon.png` (image-to-image reference), readable from the top-down game camera, distinct from the cyan XP nut and the lilac big XP chip. The chosen image is the Meshy input of task-189-02 (owner 2026-10-15).
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), chaser-meshy-pipeline (vault/reference/tech/pipelines/chaser-meshy-pipeline.md); the validated icon vault/assets/ore-hud-icon.png and render vault/assets/ore-hud-render.png; in-game references vault/assets/chaser-ingame-run.png and vault/assets/xp-nut.png
- expected_output: at most 2 candidate PNGs (1K) of the ore chunk as it would look in game (top-down game camera, on the arena floor beside broken rock, scale of a 30 cm pickup next to the miner if useful), plus one clean isolated view per candidate usable as Meshy input (subject centred, plain background, margin); run log and prompt file in Art/Source/ore-chunk/; self-review per image; one line per candidate on the shape.
- done_when: images sent to the owner; style-guide review done; the owner picks one (or rejects all); the chosen image is copied to vault/assets/ only on his word.
- editor_state: any
- token_ceiling: open
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-189-ore-chunk-look]] — parent global task.
