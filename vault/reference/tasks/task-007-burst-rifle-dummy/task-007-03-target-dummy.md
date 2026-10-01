---
id: task-007-03-target-dummy
description: "Write ARRTargetDummy: immobile test target with an ASC and health, white hit flash, debug floating damage text, refill 3 s after the last hit."
parent: task-007-burst-rifle-dummy
kind: task
status: todo
---
# Target dummy

- goal: a punching ball to test weapons and hit feedback.
- agent: cpp
- depends_on: task-007-02-projectile-damage
- inputs: [[sys-damage]], [[sys-gas-core]], [[damage-numbers]], [[color-code]], [[naming]], [[cpp-standard]].
- expected_output: `Source/RogueRigs/Debug/RRTargetDummy.h/.cpp`: `ARRTargetDummy` (abstract, APawn or AActor, immobile) with a mesh root that overlaps the `Projectile` channel, a `URRAbilitySystemComponent` and `URRHealthAttributeSet` (`MaxHealth` editable, default 1000). On each damage (listen to `Health` changes or the damage execution): flash the mesh white for an editable 0.1 s (dynamic material instance of the `Color` parameter of `M_GreyBox`), spawn a floating text (world-space text render, rises and fades over about 0.8 s, faces the camera): white for normal hits, larger and white-gold for crits, value rounded. Refill `Health` to `MaxHealth` 3 s (editable) after the last hit; at 0 it refills instantly; never dies. Log each hit on `LogRogueRigs` (damage, crit, health left). New doc `sys-target-dummy` (template `system.md`) linked from [[systems-index]].
- done_when: build exits 0 with the editor closed; no new warnings in touched files; doc matches; lint clean.
- editor_state: closed
- token_ceiling: 60000
- milestone: M1
- result: open

## Relations
- [[task-007-burst-rifle-dummy]] — parent global task.
