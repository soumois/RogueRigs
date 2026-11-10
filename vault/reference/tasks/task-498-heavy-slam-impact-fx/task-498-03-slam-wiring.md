---
id: task-498-03-slam-wiring
description: "Bp: Only if slam-niagara ran: the effect plays on the heavy slam frame at the heavy, sized to its slam radius."
parent: task-498-heavy-slam-impact-fx
kind: task
status: todo
---
# Heavy slam impact: dust ring and rock chips: bp

- goal: Only if slam-niagara ran: the effect plays on the heavy slam frame at the heavy, sized to its slam radius.
- agent: bp
- depends_on: task-498-02-slam-niagara
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), task-419-camera-shake-on-heavy-slam (vault/reference/tasks/task-419-camera-shake-on-heavy-slam.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md)
- expected_output: see done_when
- done_when: Acceptance A5 in a -game log run: one effect per slam, none on a death during the telegraph, cleared at the next-level clear; skipped with a note otherwise.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-498-heavy-slam-impact-fx]] — parent global task.
