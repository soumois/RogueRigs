---
id: task-331-01-owner-screenshot
description: "Copy the owner's second 2026-10-27 aim chevron test screenshot into Art/Source/aim-indicator and list it in the aim-indicator node."
parent: task-331-aim-chevrons-fade-measured
kind: task
status: todo
---
# Aim chevrons: store the owner's second test screenshot

- goal: the owner's screenshot of the task-329/330 build is in the repo and referenced in the vault (standing owner rule: every owner image goes into the repo and a vault node).
- agent: art
- depends_on: none
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md, line "Reference images"); source file `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\images\3.webp` (temporary folder: if it is gone, stop and report, do not recreate it).
- expected_output: `Art/Source/aim-indicator/owner-test-2026-10-27-b.webp`, a byte copy of the source (no edit, no conversion); one entry added to the "Reference images" line of aim-indicator: `owner-test-2026-10-27-b.webp` (owner screenshot of the task-329/330 build, 2026-10-27: sharp edges, beige chevrons, fade barely visible; source of [[task-331-aim-chevrons-fade-measured]]). No other change.
- done_when: the file exists at that path with the same size as the source; aim-indicator lists it; lint clean; committed by explicit paths.
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-331-aim-chevrons-fade-measured]] — parent global task.
