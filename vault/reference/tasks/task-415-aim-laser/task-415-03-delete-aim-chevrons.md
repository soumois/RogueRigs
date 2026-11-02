---
id: task-415-03-delete-aim-chevrons
description: "Bp: delete the unused M_AimChevrons material"
parent: task-415-aim-laser
kind: task
status: todo
---
# Aim laser replaces the white aim chevrons: bp

- goal: Delete /Game/RogueRigs/FX/M_AimChevrons after checking in the reference viewer that nothing references it (task-415-02 removed the C++ path); leave no redirector; touch no other asset.
- agent: bp
- depends_on: task-415-02-aim-laser-component
- inputs: task-415-aim-laser (vault/reference/tasks/task-415-aim-laser.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: Content/RogueRigs/FX/M_AimChevrons.uasset removed, one commit by path; change summary.
- done_when: M_AimChevrons.uasset no longer exists, no redirector left, the editor opens the level without a missing-material warning for the miner; committed by path as `task-415-03-delete-aim-chevrons: <summary>`.
- editor_state: Unreal editor open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-415-aim-laser]] — parent global task.
