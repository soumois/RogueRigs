---
id: task-070-02-ore-in-rock
description: "Gold ore blocks show glowing amber-gold crystals on the rock base, readable against rock and distinct from the chaser yellow and the white-gold shots."
parent: task-070-ore-in-rock
kind: task
status: todo
---
# Gold ore look

- goal: Gold ore blocks show glowing amber-gold crystals on the rock base, readable against rock and distinct from the chaser yellow and the white-gold shots.
- agent: bp
- depends_on: task-070-01-ore-in-rock
- inputs: ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), sys-grid (vault/reference/tech/systems/sys-grid.md)
- expected_output: the ore block material/instance for gold: rock base and cracks, glowing amber-gold crystals (darker and warmer than company yellow and white-gold shots), set through the ore table colour parameter from 070-01; the gold pickup stays cyan like every resource pickup; thin Blueprint layer, Blueprint doc nodes in tech/blueprints updated; no screenshot calls (nobody sees the screen: the owner judges the look); group the work, commit after each group.
- done_when: material/blueprint compiled 0 errors and saved; colour set from the table row; Blueprint doc nodes updated; lint clean; committed.
- editor_state: open
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-070-ore-in-rock]] — parent global task.
