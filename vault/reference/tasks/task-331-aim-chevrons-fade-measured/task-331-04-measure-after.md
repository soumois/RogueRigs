---
id: task-331-04-measure-after
description: "Repeat the aim chevron -game capture after the material fix and check each chevron's contrast ratio against the 0.78^i target."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: done
---
# Aim chevrons: measure the fade after the fix

- goal: prove on the rendered image that the fix works, with the same method as subtask 02, and give the owner the image.
- agent: cpp
- depends_on: task-331-03-fade-material
- inputs: task-331-02 result (method, pixel boxes, exec commands, before table); task-331-03 result (what changed); agent-cpp (vault/reference/agents/agent-cpp.md: `capture_script`, `capture_rules`); the global `acceptance` of task-331-aim-chevrons-fade-measured (vault/reference/tasks/task-331-aim-chevrons-fade-measured.md); image `Art/Source/aim-indicator/mockup-chevrons.png`.
- expected_output: one capture with the same exec commands as subtask 02, output in `Saved/Captures/AimFadeAfter/`; the same five-row table (line luma, floor luma, C_i, C_i / C_0) beside the before table and the target 1.00 / 0.78 / 0.61 / 0.47 / 0.37; pass or fail per row against C_i / C_0 at most 0.78^i + 0.08 and C_4 > 0; whether the colour reads white-gold or beige; PNG path for the Executor to show the owner. No code, asset or config change. A failing row is reported, not fixed (capture_rules 3).
- done_when: the PNG exists; before and after tables in the report with pass or fail per row; at most 5 launches; nothing committed.
- editor_state: open (capture.js runs beside the editor; do not close it, do not start PIE)
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 32, cache write 37204, cache read 365312, output 9973
- usage: turns 16, context start 11k peak 37k, resumes 0, 3 min, units 0.13 M
- runs: 1
- milestone: M3
- started: 2026-10-27T20:39:21Z
- waiting_for: none
- result: One -game capture (1 launch), Saved/Captures/AimFadeAfter/aimfade_after_03.png. C_i/C_0 after = 1.000/0.883/0.712/0.511/0.358 (limits 1.08/0.860/0.688/0.555/0.450): rows 1 and 2 FAIL by +0.023/+0.024, rows 3, 4 pass, C_4=52.5 visible. Colour reads cream/beige, not gold. Boxes re-placed (chevrons pointed another way); floors differ from before. Not fixed (capture_rules 3).

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
