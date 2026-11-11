---
id: task-498-02-slam-niagara
description: "Bp: Only if C1 or C2 is missing: the Niagara effect with the missing ring and or chips, in the task-434 and task-446 style."
parent: task-498-heavy-slam-impact-fx
kind: task
status: cancelled
---
# Heavy slam impact: dust ring and rock chips: bp

- goal: Only if C1 or C2 is missing: the Niagara effect with the missing ring and or chips, in the task-434 and task-446 style.
- agent: bp
- depends_on: task-498-01-capture-before
- inputs: enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md), color-code (vault/reference/gdd/art-audio/color-code.md), task-434-kamikaze-burst-and-ring (vault/reference/tasks/task-434-kamikaze-burst-and-ring.md), task-446-explosions-red (vault/reference/tasks/task-446-explosions-red.md), explosions (vault/reference/gdd/terrain/explosions.md)
- expected_output: see done_when
- done_when: Acceptance A3 and A4 for the missing parts, checked on an editor preview; skipped with a note when capture-before saw both and they match.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: Not run: owner keeps the existing slam ring and chips (2026-11-11).

## Relations
- [[task-498-heavy-slam-impact-fx]] — parent global task.
