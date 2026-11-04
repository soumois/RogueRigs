---
id: task-427-02-clear-zone-capture
description: "Capture: one before/after pair, check the clear zone"
parent: task-427-darkener-clear-zone
kind: task
status: in-progress
---
# Darkener shader: clear zone around the screen centre with a transition radius: capture

- goal: One -game run on L_ArenaM1 at 1920x1080 and ONE before/after pair only (owner 2026-11-04): a frame with Enable 0 (before) and a frame with Enable 1 (after), same spot, same camera, nothing else changed between them. Use the owner's current light setup as it is: do not reset, revert, edit, save or commit any light, level or asset (his uncommitted edits stay untouched). No other run (no tonemapper-off pair, no moved-miner frame, no light on/off frames). Then check the list from that pair.
- agent: capture
- depends_on: task-427-01-clear-zone-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), vault/reference/tasks/task-427-darkener-clear-zone.md (acceptance, Questions)
- expected_output: Report per point seen / not seen / ambiguous with frame or crop paths in Saved/Captures/task427-02/.
- done_when: All points read from the single pair. C1 pixels within about 165 px of the screen centre equal between after and before. C2 between about 165 and 385 px the darkening grows smoothly, no ring or hard edge. C3 beyond about 385 px darkening grows in circles around the screen centre, darkest at the corners (if visible). C4 corner ratio after / before reported against the expected on-screen value about 0.4 (Q16), no extra run. Each point reported seen / not seen / ambiguous; ambiguous points go to the owner.
- editor_state: Unreal editor closed (-game run).
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-04T17:45:37Z
- waiting_for: none
- result: open

## Relations
- [[task-427-darkener-clear-zone]] — parent global task.
