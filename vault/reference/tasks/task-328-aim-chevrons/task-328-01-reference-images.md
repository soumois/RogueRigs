---
id: task-328-01-reference-images
description: "Copy the owner-seen chevron mockup and its base screenshot into Art/Source/aim-indicator/ and list them in the aim-indicator node."
parent: task-328-aim-chevrons
kind: task
status: done
---
# Aim chevrons: reference images into the repo

- goal: the mockup the owner validated and its base screenshot live in the repo and are listed in the GDD node (owner rule: art files and owner images never stay on disk).
- agent: art
- depends_on: none
- inputs: aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md); files `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\scratchpad\mockup-chevrons.png` (mockup), `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\scratchpad\base.png` (owner screenshot of the build, base of the mockup), `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\scratchpad\chevrons.ps1` (script that drew the mockup), and the original owner image `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\images\1.webp` if present.
- expected_output: `Art/Source/aim-indicator/mockup-chevrons.png`, `Art/Source/aim-indicator/base-screenshot.png`, `Art/Source/aim-indicator/mockup-chevrons.ps1`, `Art/Source/aim-indicator/owner-screenshot.webp` (if found); the "Reference images" line of aim-indicator lists each file with one gloss and states the mockup values are approximate, not game values. No image generation, no Kie.ai call. Location decided (owner 2026-10-27, Q6 default).
- done_when: the files exist at those paths with the same size as the sources; aim-indicator lists them; lint clean; committed by explicit path.
- editor_state: any
- token_ceiling: 0.15 M units
- model: sonnet
- tokens: input 12, cache write 17673, cache read 71901, output 1984
- usage: turns 6, context start 9k peak 18k, resumes 0, 0 min, units 0.04 M
- runs: 1
- milestone: M3
- started: 2026-10-27T15:31:14Z
- waiting_for: none
- result: 4 reference files copied to Art/Source/aim-indicator (commit 1e9c8116), listed in aim-indicator

## Relations
- [[task-328-aim-chevrons]] — parent global task.
