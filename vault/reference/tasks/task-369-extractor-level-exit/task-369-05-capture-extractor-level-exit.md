---
id: task-369-05-capture-extractor-level-exit
description: "Capture: -game capture of the extractor landed at extraction and of the success screen on contact (Q8)."
parent: task-369-extractor-level-exit
kind: task
status: todo
---
# Extractor in game: the level exit you walk up to: capture

- goal: Frames show the extractor at the extraction site and the validation on contact.
- agent: capture
- depends_on: task-369-03-bp-shuttle-extractor-look
- inputs: agent-capture (vault/reference/agents/agent-capture.md), tool-capture (vault/reference/tech/pipelines/tool-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), orchestrator-lessons (vault/reference/agents/orchestrator-lessons.md)
- expected_output: frames and a per-point report (seen, not seen, ambiguous with frame paths) in Saved/Captures/task369/.
- done_when: C1 the landed extractor shows a body and four rotors, no white box; C2 the countdown chevrons point from the miner toward it; C3 walking next to it opens the success or stats screen; report written.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-369-extractor-level-exit]] — parent global task.
