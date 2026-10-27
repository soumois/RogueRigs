---
id: task-331-02-measure-before
description: "Run one -game capture of the current aim chevrons and measure each chevron's contrast against the floor, before any fix."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: in-progress
---
# Aim chevrons: measure the fade before the fix

- goal: numbers, not impressions: how much each of the five chevrons stands out from the floor in the rendered image of the current build.
- agent: cpp
- depends_on: task-331-01-owner-screenshot
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md: `capture_script`, `capture_rules`, works with the editor open); sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md: geometry, 5 chevrons from 120 uu, spacing 74 uu); aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md); images `Art/Source/aim-indicator/mockup-chevrons.png` (target) and `Art/Source/aim-indicator/owner-test-2026-10-27-b.webp` (what the owner saw).
- expected_output: one capture with `node Tools/capture/capture.js` (miner alive, chevrons on open floor, no enemy, shots and streaks away from the chevrons if possible, e.g. `rr.Stress.Peace 1`), output in `Saved/Captures/AimFadeBefore/`. For each chevron i (0 = nearest the miner): mean sRGB luma of its line pixels, mean luma of the floor just beside it, contrast C_i = difference, ratio C_i / C_0 (sampling with ffmpeg crops or a small Node script in the scratchpad, nothing committed). Report the table, the pixel boxes used, the PNG path, and one sentence on what the image shows. No code, asset or config change.
- done_when: the PNG exists; the five-row table (line luma, floor luma, C_i, C_i / C_0) is in the report with the sampled boxes; at most 5 launches (capture_rules); nothing committed (no repo file changes).
- editor_state: open (capture.js runs beside the editor; do not close it, do not start PIE)
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-10-27T16:59:56Z
- waiting_for: none
- result: open

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
