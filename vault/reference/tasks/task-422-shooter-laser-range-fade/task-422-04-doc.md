---
id: task-422-04-doc
description: "Doc: shooter laser docs: system, BP_Shooter runtime override, laser material use, tests"
parent: task-422-shooter-laser-range-fade
kind: task
status: todo
---
# Shooter targeting laser: maximum range and fade-out: doc

- goal: Update the docs of the shooter laser from the task-422-02 change summary and the global request.
- agent: doc
- depends_on: task-422-02-laser-range-fade
- inputs: task-422-shooter-laser-range-fade (vault/reference/tasks/task-422-shooter-laser-range-fade.md), agent-doc (vault/reference/agents/agent-doc.md), sys-enemies-shooter (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-shooter.md), bp-shooter (vault/reference/tech/blueprints/enemies/bp-shooter.md), bp-aim-laser-material (vault/reference/tech/blueprints/fx/bp-aim-laser-material.md), sys-automation-tests (vault/reference/tech/systems/tools/sys-automation-tests.md)
- expected_output: Commit task-422-04-doc updating sys-enemies-shooter (length rule, Plane plus M_AimLaser instance, defaults), bp-shooter (LaserBeam Cube and MI_GB_Laser now replaced at runtime by C++), bp-aim-laser-material (also used by the Shooter laser), sys-automation-tests (changed laser length check).
- done_when: The four nodes state the new rule and defaults with the commit hash; no gdd node edited; lint clean.
- editor_state: Unreal editor closed
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-422-shooter-laser-range-fade]] — parent global task.
