---
id: task-330-aim-chevrons-sharp
description: "Global task: make the aim chevron outlines sharp instead of blurry, in M_AimChevrons, after the distance fade task."
parent: tasks-done-index
kind: task
status: done
---
# Aim chevrons: sharp outlines

- request: owner, 2026-10-27, after testing [[task-328-aim-chevrons]] in the editor (verbatim): "La longueur est bonne, mais cela doit perdre en opacité ( plus la fléche est loin du perso ) / Aussi, les countours des fléches sont floues". This task covers the second point only (blurry outlines); the opacity point is [[task-329-aim-chevrons-fade]]. Separate global task because it is a separate owner point ("Aussi"), checked separately by the owner, even though both touch the same material (owner rule: never bundle points to save editor switches). Owner screenshot: `Art/Source/aim-indicator/owner-test-2026-10-27.webp` (stored by task-329-01). Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default, so: Q2 the soft glow is removed (sharp edges, like the ground telegraph); Q3 no capture, the owner tests himself in the open editor; Q4 no work on the fifth chevron (a passing shot streak). "ok" also confirms the plan; this task runs after task-329.
- goal: in game, every chevron has crisp outlines, about one screen pixel of anti-aliasing, like the ground telegraph rings.
- acceptance: the cause of the soft edges is named in the bp result; M_AimChevrons edges use a one-pixel screen-space AA (fwidth) and the glow is removed (owner Q2); line width, arm length, opening and the task-329 fade unchanged; compiles, saved; sys-aim-indicator updated (aim-indicator already says no glow); committed by explicit paths. Look judged by the owner in the open editor (no capture, owner Q3).
- gdd_check: read [[aim-indicator]] (values say "with a soft glow": conflicts with "contours nets" if the halo is what reads as blur; asked as Q2, default remove the glow), [[sys-aim-indicator]] (GlowWidth 10, GlowOpacity 0.35 are material defaults not pushed by C++: a material-only change, no C++ build), [[sys-ground-telegraph]] (sharp lines with smoothstep over fwidth, no glow: the model to copy). Colour unchanged (white-gold), [[color-code]] and [[readability-rules]] as checked in task-328. Milestone M3, in scope. Open values asked and decided (owner "ok", 2026-10-27): glow removed (Q2, aim-indicator updated); no capture (Q3).
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: none
- started: 2026-10-27T16:38:12Z
- waiting_for: none
- result: Done. sys-aim-indicator updated: edge AA (fwidth one pixel), glow cause, GlowWidth/GlowOpacity unused. Commit e53e7966. Word count not checked.

## Subtasks
1. task-330-01-sharp-edges-material (vault/reference/tasks/task-330-aim-chevrons-sharp/task-330-01-sharp-edges-material.md) — bp: find why the chevron edges are soft and make them sharp in M_AimChevrons.
2. task-330-02-docs (vault/reference/tasks/task-330-aim-chevrons-sharp/task-330-02-docs.md) — doc: update sys-aim-indicator (edge AA, glow).

## Relations
- [[tasks-done-index]] — parent index.
- [[task-330-01-sharp-edges-material]] — step 1, bp.
- [[task-330-02-docs]] — step 2, doc.
- [[task-329-aim-chevrons-fade]] — first owner point, same material, must be done before this one.
- [[task-328-aim-chevrons]] — the task whose build the owner tested.
- [[aim-indicator]] — the GDD spec.
