---
id: task-463-02-doc
description: "Doc: Record the aim assist and its data value."
parent: task-463-aim-assist
kind: task
status: done
---
# Light aim assist on miner shots: doc

- goal: Record the aim assist and its data value.
- agent: doc
- depends_on: task-463-01-aim-assist
- inputs: sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md)
- expected_output: see done_when
- done_when: The node states the assist rules, where the angle lives and that 0 turns it off; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-463
- usage: included in the executor run of task-463
- runs: 1
- milestone: M2
- started: 2026-11-06T08:51:05Z
- waiting_for: none
- result: Commit 09ab3ce5. New node sys-weapons-aim-assist (child of sys-weapons): rules per bullet, AimAssistDeg column of WeaponStats.csv (0 = off), what is not affected, test RogueRigs.Weapons.AimAssist; sys-weapons links it in Decisions, data and Relations. Lint clean (0 problems).

## Relations
- [[task-463-aim-assist]] — parent global task.
