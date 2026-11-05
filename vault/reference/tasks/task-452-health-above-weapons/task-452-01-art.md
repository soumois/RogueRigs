---
id: task-452-01-art
description: "Art: Cut the health bar pieces of the retained mockup as UI sprites, or report that none is needed."
parent: task-452-health-above-weapons
kind: task
status: done
---
# Health bar just above the weapon slots: art

- goal: Cut the health bar pieces of the retained mockup as UI sprites, or report that none is needed.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/, kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md)
- expected_output: see done_when
- done_when: Sprites retained by the owner, keyed, committed and listed in the mockup art-prompts node, or a stated reason none is needed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-452
- usage: included in the executor run of task-452
- runs: 1
- milestone: M2
- started: 2026-11-05T23:43:51Z
- waiting_for: none
- result: No new sprite needed: the retained mockup plate (riveted gunmetal, bolted teal-trimmed ends, rust chips, dark inside) is the same frame family as the owner-retained Art/UI/Shop/T_Shop_ButtonReroll.png (640 x 180, bolted ends, 26 px transparent margin), already imported as /Game/RogueRigs/UI/Shop/T_Shop_ButtonReroll; task-452-02 uses it as a 9-slice Box brush behind the bar, so no Kie.ai run, no credit and no owner retention round. No generation, so no style-guide review.

## Relations
- [[task-452-health-above-weapons]] — parent global task.
