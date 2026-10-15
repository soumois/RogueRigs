---
id: task-183-03-ore-hud-icon
description: "Art: the HUD ore icon, a Duskite crystal drawn from the owner-validated in-game render, same style as the stat icons."
parent: task-183-ore-counter
kind: task
status: done
---
# Ore counter: HUD icon

- goal: the HUD ore icon, drawn from the in-game render the owner validated in task-183-02 (image-to-image with that render and the stat icons as references), square, readable at 48 px, on a background the bp agent can key or a transparent one.
- agent: art
- depends_on: task-183-02-ore-hud-render (owner-validated render in `vault/assets/`)
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md); the validated render `vault/assets/ore-hud-render.png` (candidate 2, chosen by the owner 2026-10-14); the stat icons in `Art/Source/stat-icons/`.
- expected_output: at most 2 candidate PNGs (1:1, 1K) with run log and prompt file in `Art/Source/ore-hud/`; self-review per image, including a 48 px downscale check.
- done_when: images sent to the owner; style-guide review done; the owner picks one; it is copied to `vault/assets/` only on his word.
- editor_state: any
- token_ceiling: open
- model: opus+sonnet
- tokens: input 24, cache write 53375, cache read 198731, output 3459
- runs: 1
- milestone: M1
- result: 2 icons (12 credits, run 20261014-233627-ore-hud, green key background); owner chose candidate 2, copied to vault/assets/ore-hud-icon.png; style-guide fails (A1 gloss, magenta lean, no glow) kept by owner; commits 951a9bf4 and this one

## Relations
- [[task-183-ore-counter]] — parent global task.
