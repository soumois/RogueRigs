---
id: task-428-xp-glow-from-spawn
description: "Global task: the XP pickup camera-facing glow disc shows from spawn, during the pop arc, instead of fading in only on landing."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# XP pickup glow disc on from spawn

- request: Owner 2026-11-04 (verbatim): "Ok, l'effet de glow qui s'active au sol pour les XP, j'aimerais essayer de voir ce que ça donne s'ils sont directement activés ( comme ils sont quand même face caméra )". Current logic (ARRPickup, task-148-01): with bGlowFade (true on BP_PickupXP and BP_PickupXPBig only) the C++ writes a 0..1 fade to PickupMesh Custom Primitive Data index 0, read as Fade by M_XPGlowDisc (camera-facing billboard disc, task-147-01): 0 at life start and during the Pop (about 1.6 s floaty arc), fades to 1 in GlowFadeDuration 0.2 s on landing (Pop to Rest, ReturnToRest), back to 0 at Magnet start and Collected, 0 at pool release. Reading: an experiment ("essayer"), so it stays switchable. Owner answer 2026-11-04 (relayed by the main session, verbatim): "ok pour tout sauf le point 5, je vérifie par moi même" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted; Q5 other choice: no capture run, the owner checks the glow in game himself (task-428-02 is closed on his word, no agent launched).
- goal: New ARRPickup class default bGlowDuringPop (Category Pickup|Glow, EditCondition bGlowFade; C++ default per Q4). When bGlowFade and bGlowDuringPop, StartLife keeps ResetGlowFade (writes 0, no flash from a reused pickup) then sets the glow target to 1, so the disc fades in over GlowFadeDuration from spawn and stays on through the Pop and the Rest; Magnet, Collected and pool release are unchanged (target 0, per Q1). Fade-in shape per Q2, pickups per Q3. bGlowDuringPop false gives exactly the task-148 behaviour. No material, Niagara or Blueprint asset change (the BP class defaults inherit the C++ default).
- acceptance: A1 a freshly dropped XP nut shows its glow disc while in the air during its pop arc, within about 0.2 s of spawn. A2 the glow disc is still on at rest on the floor, no visible dip or second fade at landing. A3 when the magnet takes the nut, the disc fades out as before. A4 same for the big XP cube (BP_PickupXPBig) per Q3. A5 unchecking bGlowDuringPop in the Blueprint class defaults restores the old behaviour (off in the air, fade-in at landing). A6 the C++ builds and the existing pickup automation tests still pass.
- gdd_check: xp-level-up (approved spec): no rule about when the glow shows; the Look line gets the provisional sentence on confirmation (edit in the defaults file). Rules versus rendering: rendering only, pop, magnet, collection and XP values unchanged. color-code: colours unchanged. No open gdd value blocks. Milestone M2. Tech: bp-pickup-xp glow_disc_fade (task-148-02) and sys-pickups describe the current fade and are updated by the doc subtask.
- confirmed_by_owner: yes (2026-11-04, "ok pour tout sauf le point 5, je vérifie par moi même")
- milestone: M2
- next_step: task-428-02-glow-from-spawn-capture
- started: 2026-11-04T18:53:53Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-04 (owner "ok pour tout sauf le point 5, je vérifie par moi même"): the defaults of Q1 to Q4 are accepted; Q5 takes the other choice (no capture, the owner judges in game); values provisional, owner 2026-11-04.
- Q1 Magnet: Default: the glow shows from spawn and stays on at rest, then still fades out when the magnet takes the pickup (as today). Other choice: keep it on during the flight to the miner too, until collection.
- Q2 How it appears at spawn: Default: quick fade-in over the existing 0.2 s (GlowFadeDuration) from spawn. Other choice: instantly at full at spawn.
- Q3 Which pickups: Default: both XP pickups, the small nut and the big XP cube (both use the glow disc). Other choice: the small nut only.
- Q4 Test switch: Default: a new checkbox bGlowDuringPop in the pickup class defaults, ticked by default, so going back to the old look is one untick in BP_PickupXP / BP_PickupXPBig (no code). Other choice: no switch, the glow simply always shows from spawn.
- Q5 Visual check: Default: a capture agent run in game (-game) checks the glow in the air, at rest and on magnet and sends you the frames (rule: a change on screen ends with a capture). Other choice: no capture, you judge it yourself in game.

## Subtasks
1. task-428-01-glow-from-spawn-cpp (vault/reference/tasks/task-428-xp-glow-from-spawn/task-428-01-glow-from-spawn-cpp.md) — cpp: ARRPickup bGlowDuringPop: glow target 1 from life start
2. task-428-02-glow-from-spawn-capture (vault/reference/tasks/task-428-xp-glow-from-spawn/task-428-02-glow-from-spawn-capture.md) — capture: no run (Q5, owner checks himself); closed on the owner word
3. task-428-03-glow-from-spawn-doc (vault/reference/tasks/task-428-xp-glow-from-spawn/task-428-03-glow-from-spawn-doc.md) — doc: document bGlowDuringPop in sys-pickups and bp-pickup-xp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-428-01-glow-from-spawn-cpp]] — step 1, cpp.
- [[task-428-02-glow-from-spawn-capture]] — step 2, capture.
- [[task-428-03-glow-from-spawn-doc]] — step 3, doc.
- [[xp-level-up]] — gdd XP node, Look line gets the provisional glow sentence
- [[bp-pickup-xp]] — BP_PickupXP and BP_PickupXPBig, glow disc material and current fade
- [[sys-pickups]] — C++ pickup system holding the glow fade
