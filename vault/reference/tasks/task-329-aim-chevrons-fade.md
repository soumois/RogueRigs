---
id: task-329-aim-chevrons-fade
description: "Global task: make the aim chevrons visibly lose opacity with distance from the miner, as in the mockup; the task-328 build shows them equally bright."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Aim chevrons: distance fade

- request: owner, 2026-10-27, after testing [[task-328-aim-chevrons]] in the editor (verbatim): "La longueur est bonne, mais cela doit perdre en opacité ( plus la fléche est loin du perso ) / Aussi, les countours des fléches sont floues". This task covers the first point only (opacity); the blurry outlines are [[task-330-aim-chevrons-sharp]]. "La longueur est bonne": the fixed length 416 uu is now the owner's value (written in [[aim-indicator]]), no length change. Owner screenshot of that test: `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\images\2.webp`, to be stored by subtask 01. Planner reading of it (description, not a diagnosis): the five chevrons look equally bright; the fifth looks smeared, probably a white-gold shot streak passing over it (same streak as the shots further right). Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default, so: Q1 fade as the mockup, each chevron at 0.78 x the previous (100 / 78 / 61 / 47 / 37 %), one uniform opacity per chevron; Q2 glow removed (task-330); Q3 no capture, the owner tests himself in the open editor; Q4 no work on the fifth chevron (a passing shot streak). "ok" also confirms the plan; run task-329 before task-330.
- goal: in game, the first chevron is nearly opaque and each next one clearly dimmer, the last about 37 % (0.78^4), as in `Art/Source/aim-indicator/mockup-chevrons.png`.
- acceptance: the cause of the missing fade is found and named in the bp result; M_AimChevrons draws chevron i at Opacity x Decay^i on the colour the decal really shows (read back from the material); compiles, saved; owner screenshot stored and listed in [[aim-indicator]]; sys-aim-indicator updated; committed by explicit paths. The look is judged by the owner in the open editor (no capture, owner Q3).
- gdd_check: read [[aim-indicator]] (opacity x0.78 per chevron already specified; no conflict, the build does not show it), [[sys-aim-indicator]] (C++ pushes `Decay` 0.78 and `Opacity` 1 every BeginPlay, checked in `RRAimIndicatorComponent.cpp` lines 128-140: the defect is in the material or the render, not the component), [[sys-ground-telegraph]] (same decal technique: translucent unlit, opacity = mask x Opacity, emissive = colour), [[mpp-player-light-mask]] (darkening around the miner; "let it fade", no compensation, unchanged). Colour unchanged (white-gold), so [[color-code]] and [[readability-rules]] as checked in task-328. Milestone M3 (juice), in scope. Conflicts: none. Open values asked and decided (owner "ok", 2026-10-27): fade 0.78 per chevron, one flat opacity per chevron as in the mockup (Q1); no capture (Q3). No C++ change planned: if the bp agent finds the cause outside the material (C++, post-process, level), it stops and reports, no other fix.
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: task-329-03-docs
- started: 2026-10-27T16:36:04Z
- waiting_for: none
- result: open

## Subtasks
1. task-329-01-owner-screenshot (vault/reference/tasks/task-329-aim-chevrons-fade/task-329-01-owner-screenshot.md) — art: store the owner's test screenshot in the repo and list it in aim-indicator.
2. task-329-02-fade-material (vault/reference/tasks/task-329-aim-chevrons-fade/task-329-02-fade-material.md) — bp: find why M_AimChevrons shows no distance fade and fix it in the material.
3. task-329-03-docs (vault/reference/tasks/task-329-aim-chevrons-fade/task-329-03-docs.md) — doc: update sys-aim-indicator (fade cause and fix, fixed length now owner-decided).

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-329-01-owner-screenshot]] — step 1, art.
- [[task-329-02-fade-material]] — step 2, bp.
- [[task-329-03-docs]] — step 3, doc.
- [[task-328-aim-chevrons]] — the task whose build the owner tested.
- [[task-330-aim-chevrons-sharp]] — the second owner point, same material, runs after this one.
- [[aim-indicator]] — the GDD spec.
