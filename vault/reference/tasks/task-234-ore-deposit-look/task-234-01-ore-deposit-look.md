---
id: task-234-01-ore-deposit-look
description: "Image prompts for the owner: 3 Duskite and 3 heal 2 x 2 crystal formations in the image C style."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Ore deposit look

- goal: Each 2 x 2 ore or heal deposit is drawn as one crystal formation, at least 3 variants per type, from images the owner generates with prompts we write.
- agent: art
- depends_on: none
- inputs: art-prompts-rock (vault/reference/gdd/art-audio/art-prompts-rock.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), rock-render-damage (vault/reference/tech/systems/world/sys-grid/rock-state-render-options/rock-render-damage.md), agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: Prompts only, no generation: 3 prompts for Duskite deposits and 3 for heal deposits (one 2 x 2 crystal formation covering the four cells of a deposit, top-down three-quarter view matching image C, locked style), plus one shared negative block and the framing rules for later 3D (isolated object, neutral background, same scale); saved in a new vault node under gdd/art-audio linked from art-prompts-rock; self-review against the art rules.
- done_when: prompt node written and linted; prompts sent to the owner; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
