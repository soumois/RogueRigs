---
id: task-469-03-capture
description: "Capture: In-game capture of the green aim line next to the owner capture, Shooter on screen, sent to the owner."
parent: task-469-aim-laser-green
kind: task
status: todo
---
# Aim laser in the lime green of the owner capture: capture

- goal: In-game capture of the green aim line next to the owner capture, Shooter on screen, sent to the owner.
- agent: capture
- depends_on: task-469-02-doc
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), agent-capture (vault/reference/agents/agent-capture.md), Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp
- expected_output: see done_when
- done_when: Frames from a -game run sent to the owner next to Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp: C1 the aim line is lime green like the reference, not white-gold; C2 width and glow look as before; C3 a Shooter on screen reads apart from the line; unverified points stated plainly.
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
- [[task-469-aim-laser-green]] — parent global task.
