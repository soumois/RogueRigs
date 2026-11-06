---
id: task-468-03-capture
description: "Capture: In-game capture of the aim line in the four stop cases, sent to the owner with his capture as before."
parent: task-468-aim-laser-unlimited-length
kind: task
status: todo
---
# Aim laser without length limit, stopping at walls and robots: capture

- goal: In-game capture of the aim line in the four stop cases, sent to the owner with his capture as before.
- agent: capture
- depends_on: task-468-02-doc
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), agent-capture (vault/reference/agents/agent-capture.md), Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp
- expected_output: see done_when
- done_when: Frames from a -game run sent to the owner with Art/Source/aim-indicator/owner-capture-green-unlimited-2026-11-06.webp as the before: C1 aimed over open floor the line reaches the screen edge; C2 aimed at a border cliff the line ends at its face, not drawn over it; C3 aimed at a breakable block it ends at the block; C4 aimed at a robot it ends on the robot; C5 the tip is sharp in C2 to C4; unverified points stated plainly.
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
- [[task-468-aim-laser-unlimited-length]] — parent global task.
