---
id: task-478-02-capture
description: "Capture: Final independent check of the walls against image 3 and the before."
parent: task-478-wall-lines-emissive-edge
kind: task
status: todo
---
# Walls without black lines, emissive on the top edge: capture

- goal: Final independent check of the walls against image 3 and the before.
- agent: capture
- depends_on: task-478-01-wall-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/cartoon-look/reference/owner-2026-11-06-wall-black-lines.png, Art/Source/cartoon-look/reference/owner-2026-11-06-wall-section-target.png
- expected_output: see done_when
- done_when: C1 no dark line on wall sides; C2 no visible cut between facets or blocks; C3 orange glow on the top face edge, fading inward, thin; C4 top surface without seams; C5 damage cracks and plates still show; C6 walls separate from the floor; frames at the game camera and a low side view; sheets sent to the owner.
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
- [[task-478-wall-lines-emissive-edge]] — parent global task.
