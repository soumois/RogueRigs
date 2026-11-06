---
id: task-465-04-doc
description: "Doc: Update the bp-* and sys-* nodes of the impact effects and check color-code, burst-rifle, shotgun and turret match the result"
parent: task-465-shot-impact-burst
kind: task
status: done
---
# Player shot impact burst: doc

- goal: Update the bp-* and sys-* nodes of the impact effects and check color-code, burst-rifle, shotgun and turret match the result
- agent: doc
- depends_on: task-465-03-impact-expand
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), shotgun (vault/reference/gdd/weapons/shotgun.md), sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), task-432-player-shot-impact-sparks (vault/reference/tasks/task-432-player-shot-impact-sparks.md)
- expected_output: see done_when
- done_when: The bp-* and sys-* nodes describing the player shot impact effects state the new burst and its two colours; color-code, burst-rifle, shotgun and turret describe the impact look as built; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-465
- usage: included in the executor run of task-465
- runs: 1
- milestone: M2
- started: 2026-11-06T09:47:35Z
- waiting_for: none
- result: Done (0812e474). New bp-shot-impact-burst (three systems, two materials, emitters, sizes, colours, 8-sprite bound, spawn, verified frames). Updated bp-projectile, bp-turret, bp-shotgun (ImpactSparkSystem per shot BP), sys-projectiles (burst per BP, pool bound 24 x 8 = 192 sprites), bp-shot-impact-spark (superseded, still C++ default and asset test reference), blueprints-fx-index. color-code already held the player shot impacts row (planner, matches the build); burst-rifle, shotgun and turret vfx_sfx now state the impact look (A9). sys-weapons has no impact look, unchanged. Lint clean.

## Relations
- [[task-465-shot-impact-burst]] — parent global task.
