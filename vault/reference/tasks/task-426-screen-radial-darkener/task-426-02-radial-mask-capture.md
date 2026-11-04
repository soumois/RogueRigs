---
id: task-426-02-radial-mask-capture
description: "Capture: check the screen circles darkening"
parent: task-426-screen-radial-darkener
kind: task
status: done
---
# Darkener shader: circles around the screen centre, min at centre, max at corners: capture

- goal: One -game run on L_ArenaM1 at 1920x1080: a frame with Enable 1 and one with Enable 0 at the same spot, and one after the miner has moved, then check the list.
- agent: capture
- depends_on: task-426-01-radial-mask-material
- inputs: agent-capture (vault/reference/agents/agent-capture.md), vault/reference/tasks/task-426-screen-radial-darkener.md (acceptance)
- expected_output: Report per point seen / not seen / ambiguous with frame or crop paths in Saved/.
- done_when: C1 centre pixels equal between Enable 1 and Enable 0 (CenterDarkness 0). C2 corner pixels about 15 % of their Enable 0 value. C3 equal darkness along circles around the screen centre, linear fade. C4 the pattern stays fixed on screen when the miner moves. C5 void pixels at most halved. Each point reported; ambiguous points go to the owner.
- editor_state: Unreal editor closed (-game run).
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 46, cache write 60558, cache read 874782, output 14724
- usage: turns 23, context start 13k peak 61k, resumes 0, 3 min, units 0.24 M
- runs: 1
- milestone: M2
- started: 2026-11-04T17:06:06Z
- waiting_for: none
- result: Capture: C3 circles with smooth monotonic fade, C4 fixed on screen, C7 (task-425) seen; C1 centre near equal (ratio 0.96-0.99) ambiguous; C2 corners measured 0.27-0.60 of Enable 0 not 0.15 (likely display-space/tonemapped), ambiguous, owner call; C5, C6, C8, C9 not verified (no void in view, no light-off baseline, no barrel). Frames Saved/Captures/task426-02b/.

## Relations
- [[task-426-screen-radial-darkener]] — parent global task.
