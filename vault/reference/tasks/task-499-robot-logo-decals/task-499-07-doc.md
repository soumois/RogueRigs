---
id: task-499-07-doc
description: "Doc: Document the emblem decal: asset, placement per robot, how it follows the animation."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: doc

- goal: Document the emblem decal: asset, placement per robot, how it follows the animation.
- agent: doc
- depends_on: task-499-06-style-review
- inputs: sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), robots-meshy-raw-2026-10-22 (vault/reference/tech/pipelines/robots-meshy-raw-2026-10-22.md)
- expected_output: see done_when
- done_when: The nodes name the emblem asset and how each robot carries it; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run)
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: 2026-11-10T21:20:58Z
- waiting_for: none
- result: Commit f4e332da. New bp-robot-emblem-decal (script, meshes, masters and instances, debris, slot order rule, sync rule, gotcha on BlendMode set by property, revert); emblem_499 entries and relations added to bp-heavy, bp-kamikaze, bp-chaser-v2, bp-shield-chaser-v2, bp-robot-meshy-material; index updated; lint clean (vault OK). gdd nodes (color-code, kessler-voss-logo) not edited by the executor: the planner had already written the task-499 lines; the dark-rock variant on the chasers is not yet in color-code (critic note, owner decision). Cost not recorded (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
