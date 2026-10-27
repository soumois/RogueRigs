---
id: task-331-aim-chevrons-fade-measured
description: "Global task: make the aim chevron distance fade really visible, measured on the final image, since task-329 left it barely visible."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Aim chevrons: measured distance fade

- request: owner, 2026-10-27, after testing [[task-329-aim-chevrons-fade]] and [[task-330-aim-chevrons-sharp]] in the open editor (verbatim): "Il n'y a pas vraiment une perte d'opacité". Owner screenshot: `C:\Users\soumo\AppData\Local\Temp\claude\C--Users-soumo-Documents-Unreal-Projects-RogueRigs\1466b655-e1bf-4b72-a154-24c3dea0507d\images\3.webp`, stored by subtask 01. Planner reading (description, not a diagnosis): edges now sharp (task-330 accepted); the five chevrons are beige, not white-gold; from first to fifth only a slight dimming, nowhere near 100 to 37 %; a short bright streak above the fourth chevron (a passing shot, as in task-329 Q4). Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so: Q1 = A, one `-game` capture before (subtask 02) and one after (subtask 04), the after image sent to him; Q2 = EmissiveGain back to 1.0 (white-gold of the mockup), and if the far chevrons stay washed out at 1.0, report it to him, he decides; Q3 = keep 0.78 per chevron (100 / 78 / 61 / 47 / 37 %), measured on the final image with the contrast measure C_i and tolerance +0.08 (see `acceptance`). "ok" also confirms the plan.
- goal: in game, each chevron is clearly dimmer than the previous one, as in `Art/Source/aim-indicator/mockup-chevrons.png`, and this is measured on the rendered image, not inferred from the material.
- acceptance: the cause is named from measurements (subtask 02 numbers, PIE read-back), not from code alone; M_AimChevrons fixed and saved; after-fix measure (subtask 04): per-chevron contrast ratio C_i / C_0 at most 0.78^i + 0.08 (1.00 / 0.78 / 0.61 / 0.47 / 0.37) and the fifth chevron still visible (C_4 > 0); colour white-gold per [[color-code]] (EmissiveGain per Q2); owner screenshot stored and listed in [[aim-indicator]]; sys-aim-indicator updated; committed by explicit paths. C_i = mean sRGB luma of chevron i line pixels minus mean luma of the floor just beside it, in a `-game` capture. Q1, Q2, Q3 decided by the owner ("ok", 2026-10-27).
- gdd_check: read [[aim-indicator]] (fade 0.78 per chevron already specified and owner-approved; no conflict, the build does not show it), [[sys-aim-indicator]] (HLSL weights chevron i by Decay^i on `MP_Opacity`; `MP_EmissiveColor` = Color x EmissiveGain 0.5; C++ pushes Decay 0.78, Opacity 1; the task-329 cause "exposure + tonemapping" is marked not measured), [[color-code]] (player side white-gold; the beige look contradicts it, Q2), [[readability-rules]] (no red, orange, acid green, steel blue: unchanged). Engine fact checked by the Planner in the UE 5.8 source: on the deferred path every decal emissive is drawn in the Emissive stage with blend `SrcAlpha, One` whatever the material blend mode (`Engine/Source/Runtime/Renderer/Private/DecalRenderingCommon.cpp` line 919; `Engine/Shaders/Private/DecalCommon.ush` line 106 writes `(Emissive x PreExposure, Opacity)`): the emissive is already scaled by opacity and added on the floor, the floor is never darkened. So the opacity does reach the colour; switching Translucent to AlphaComposite does not change this stage; multiplying the emissive by Decay^i again would square the fade. The real cause is not known: candidates for subtask 03 include a weight not per chevron in practice, an alpha above 1, tonemap compression, the post-process (`PPV_XPGlow` fixed exposure -0.94 EV100, bloom 1.3) and the floor brightness under each chevron. EmissiveGain 0.5 (task-329, owner option A) rested on that unmeasured diagnosis. Milestone M3, in scope. Open values asked and decided (owner "ok" to every default, 2026-10-27): Q1 how to measure (default: one capture before and one after the fix, by cpp, editor stays open); Q2 EmissiveGain (default back to 1.0, white-gold; if the after measure shows washed-out far chevrons at 1.0, report, owner decides); Q3 fade target (default 0.78 per chevron measured on the final image, tolerance +0.08). No C++ change foreseen: if the cause is outside the material (C++, post-process, level), subtask 03 stops and reports.
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: task-331-05-docs
- started: 2026-10-27T20:44:08Z
- waiting_for: none
- result: open

## Subtasks
1. task-331-01-owner-screenshot (vault/reference/tasks/task-331-aim-chevrons-fade-measured/task-331-01-owner-screenshot.md) — art: store the owner's new test screenshot in the repo and list it in aim-indicator.
2. task-331-02-measure-before (vault/reference/tasks/task-331-aim-chevrons-fade-measured/task-331-02-measure-before.md) — cpp: one `-game` capture of the current chevrons and the per-chevron contrast numbers.
3. task-331-03-fade-material (vault/reference/tasks/task-331-aim-chevrons-fade-measured/task-331-03-fade-material.md) — bp: name the cause from the numbers and fix the fade in M_AimChevrons, EmissiveGain per Q2.
4. task-331-04-measure-after (vault/reference/tasks/task-331-aim-chevrons-fade-measured/task-331-04-measure-after.md) — cpp: same capture after the fix, numbers against the target, sheet for the owner.
5. task-331-05-docs (vault/reference/tasks/task-331-aim-chevrons-fade-measured/task-331-05-docs.md) — doc: update sys-aim-indicator (measured cause, fix, EmissiveGain).

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-331-01-owner-screenshot]] — step 1, art.
- [[task-331-02-measure-before]] — step 2, cpp.
- [[task-331-03-fade-material]] — step 3, bp.
- [[task-331-04-measure-after]] — step 4, cpp.
- [[task-331-05-docs]] — step 5, doc.
- [[task-329-aim-chevrons-fade]] — the fade fix the owner tested; its diagnosis was not measured.
- [[task-330-aim-chevrons-sharp]] — sharp edges, accepted.
- [[aim-indicator]] — the GDD spec.
