---
id: task-309-04-bp-chest-open-fx
description: "NS_BonusChestOpen, turning rays and floating sparks (bp): bonus chest opening, step 04."
parent: task-309-chest-opening
kind: task
status: done
---
# NS_BonusChestOpen, turning rays and floating sparks

- goal: Niagara NS_BonusChestOpen, played once at the open lid: emitter Rays, 4 long thin additive light rays rising from the chest interior and turning slowly about the vertical axis (about 30 deg/s), fading out after 3 s (Q22); emitter Sparks, about 20 small sparks floating up slowly from the open chest for 2.5 s and fading (Q25); both in the company yellow of MI_ChestInterior (task-309-02, Q23); no flash (replaces Q12); doc node bp-bonus-chest-open under blueprints-fx-index. Wiring is task-309-05.
- agent: bp
- depends_on: task-309-02-bp-chest-interior-material
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-bonus-chest (vault/reference/tech/blueprints/world/bp-bonus-chest.md), color-code (vault/reference/gdd/art-audio/color-code.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md); files: Art/Source/chest/reference/egor-chest-juice.mp4 (techniques F and H)
- expected_output: /Game/RogueRigs/FX/NS_BonusChestOpen, doc node bp-bonus-chest-open
- done_when: system compiles with 0 errors; emitter values read back; doc node and index relation; lint clean; committed
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 68, cache write 84424, cache read 1682124, output 23166
- usage: turns 34, context start 12k peak 90k, resumes 0, 4 min, units 0.39 M
- runs: 1
- milestone: M2
- started: 2026-10-29T19:03:56Z
- result: Done (13349d35): NS_BonusChestOpen, Rays (4 blades, 3 s, 30 deg/s turn) and Sparks (20, 2-2.5 s) in linear yellow (1,0.58,0), additive M_DroneThruster, no params, no flash; 0 errors. Look unverified; ray spacing assumes UniqueID 0..3.

## Relations
- [[task-309-chest-opening]] — parent global task.
