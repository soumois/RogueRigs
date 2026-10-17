---
id: task-228-01-bp-pickup-xp-big-pop
description: "bp: read the pop values of BP_PickupXP and set the same on BP_PickupXPBig class defaults; doc node."
parent: task-228-xp-big-pop
kind: task
status: done
---
# BP_PickupXPBig pop values

- goal: the big XP cube flies like the nut.
- agent: agent-bp
- depends_on: none
- inputs: `vault/reference/tech/blueprints/fx/bp-pickup-xp.md` (floaty_pop line), `vault/reference/agents/bp-mcp-lessons.md`.
- method: read on the class default of BP_PickupXP: PopDuration, PopArcHeight, PopMinDistance, PopMaxDistance, PopFallTimeRatio, PopBounceFraction, PopBounceHeight, HoverHeight, bAttractDuringPop, PopAttractDelay; set the same on BP_PickupXPBig; compile, save, read back. Nothing else changed. Update the doc node (floaty_pop line: now on both).
- done_when: values read back equal; doc updated; commit by path.
- editor_state: open (launched by the Orchestrator)
- token_ceiling: open
- model: sonnet
- tokens: input 26, cache write 32037, cache read 289390, output 493
- runs: 1
- milestone: M1
- result: BP_PickupXPBig pop values now equal BP_PickupXP (PopDuration 1.6, PopArcHeight 250, PopMinDistance 80, PopMaxDistance 300, PopFallTimeRatio 2, PopBounceFraction 0.25, PopBounceHeight 15, HoverHeight 30, bAttractDuringPop true, PopAttractDelay 1.2), read back after compile and save; doc floaty_pop line updated.

## Relations
- [[task-228-xp-big-pop]] — parent global task.
