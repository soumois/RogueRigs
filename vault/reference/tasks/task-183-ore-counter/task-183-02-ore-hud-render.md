---
id: task-183-02-ore-hud-render
description: "Art: proposed in-game render of the HUD top-left corner with the ore counter (Duskite icon and count) next to the round radar."
parent: task-183-ore-counter
kind: task
status: done
---
# Ore counter: in-game HUD render

- goal: a proposed in-game render showing the ore counter next to the round radar (minimap disc 200 × 200 at 20, 20 on a 1920 × 1080 screen; health bar below it at y 232–260; the space to the right of the disc is free up to the alert bar at x 660). The counter is a Duskite ore icon plus the carried count, readable at a glance during a fight. It is the reference the icon (task-183-03) is drawn from (owner 2026-10-14).
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-minimap-locator (vault/reference/tech/blueprints/ui/wbp-minimap-locator.md); style references: the stat icons in `Art/Source/stat-icons/` and an in-game screenshot if one exists under `Saved/Screenshots/`.
- expected_output: at most 2 candidate PNGs (16:9, 1K) with their run log and prompt file in `Art/Source/ore-hud/`; self-review per image; one line per candidate on what the icon looks like.
- done_when: images exist and are sent to the owner; style-guide review done; the owner picks one (or rejects all); the chosen image is copied to `vault/assets/` only on his word.
- editor_state: any
- token_ceiling: open
- model: opus+sonnet
- tokens: input 28, cache write 93149, cache read 239290, output 5790
- runs: 1
- milestone: M1
- result: 2 renders (12 credits, run 20261014-231429-ore-hud); owner chose candidate 2 for the icon, copied to vault/assets/ore-hud-render.png; style-guide fails kept by owner; placement changed to an inline top-left counter (task-183-04); commits f5698be8 and this one

## Relations
- [[task-183-ore-counter]] — parent global task.
