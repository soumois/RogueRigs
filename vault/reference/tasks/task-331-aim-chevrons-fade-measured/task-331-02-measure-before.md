---
id: task-331-02-measure-before
description: "Run one -game capture of the current aim chevrons and measure each chevron's contrast against the floor, before any fix."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: done
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
- model: opus
- tokens: input 46, cache write 50212, cache read 746993, output 12774
- usage: turns 23, context start 11k peak 50k, resumes 0, 4 min, units 0.20 M
- runs: 1
- milestone: M3
- started: 2026-10-27T16:59:56Z
- waiting_for: none
- result: One -game capture (2 launches), Saved/Captures/AimFadeBefore/aimfade_before_03.png. C_i/C_0 = 1.000/0.989/0.931/0.836/0.695 (target 1/0.78/0.61/0.47/0.37); line luma 225.7/210.2/192.6/170.6/142.9, floor luma 84.5/70.6/61.2/52.6/44.8. Fade far weaker than 0.78^i.

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
