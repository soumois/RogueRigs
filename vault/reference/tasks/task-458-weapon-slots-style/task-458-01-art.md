---
id: task-458-01-art
description: "Art: Cut the weapon slot frame sprite of the retained mockup."
parent: task-458-weapon-slots-style
kind: task
status: done
---
# Weapon slots in the unified look: art

- goal: Cut the weapon slot frame sprite of the retained mockup.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/, kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md)
- expected_output: see done_when
- done_when: Sprite retained by the owner, keyed, committed and listed in the mockup art-prompts node.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-458
- usage: included in the executor run of task-458
- runs: 1
- milestone: M2
- started: 2026-11-06T01:13:02Z
- waiting_for: none
- result: No new sprite: each mockup slot frame (dark inside, gunmetal riveted edges, teal chamfered corner plates, rust) matches the owner-retained Art/UI/LevelEnd/T_LevelEnd_Frame.png (task-380, keyed, imported, 9-slice margins measured); the bolted base under the row reuses T_Shop_ButtonReroll like the health plate (task-452). Listed in art-prompts-hud-unified-mockup (commit ef831bd5). No Kie.ai run, no credit, no style-guide review (no generation).

## Relations
- [[task-458-weapon-slots-style]] — parent global task.
