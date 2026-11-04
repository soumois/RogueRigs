---
id: task-427-darkener-clear-zone
description: "Global task: Step 3 of the darkener rework: ClearRadius cancels the screen darkener near the centre, FadeRadius blends back to the current circles."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Darkener shader: clear zone around the screen centre with a transition radius

- request: Owner 2026-11-04 (verbatim): "Mais deuxième étape du shader, pour pouvoir mieux contrôler ce shader. Il me faudrait un radius où cela annule l'effet du shader et un autre radius pour faire la transition entre l'effet actuel du shader et le radius où l'effet est annulé." Follows task-426-screen-radial-darkener. Owner (relayed by the main session): do not remove the unused PlayerPosition parameter for now. Main session note: the capture should also settle the task-426 corner measurement question and the unverified task-425 light checks if it can (Q16, Q17). Owner answer 2026-11-04 changes the capture: one before/after pair only, in his current light setup (he changed the lights by hand); Q16 and Q17 reduced accordingly (see Questions). Owner answer 2026-11-04 (relayed by the main session, verbatim): "Ok pour tout, fait juste une capture avant après. Car j'ai pas mal changé les lights manuellement (sinon tu n'as pas une comparaison correcte)" = the plan and the defaults of Q11, Q12, Q13, Q14, Q15 accepted.
- goal: M_PP_PlayerLightMask keeps the task-426 darkener and adds two MPC_PlayerLight scalars, ClearRadius and FadeRadius, in the same unit as t (share of the centre-to-corner distance, 0 at centre, 1 at corners). With t as today: base = lerp(CenterDarkness, CornerDarkness, t); m = smoothstep(ClearRadius, FadeRadius, t) (if FadeRadius <= ClearRadius, m = t >= ClearRadius ? 1 : 0); black = base * m; void cap min(black, VoidMaxDarkness) after it; out unchanged (SceneCol * (1 - black * Enable)). Defaults per Questions (Q11 to Q15); any non-default answer recorded below replaces this formula. PlayerPosition stays in the MPC, unused. Asset names unchanged.
- acceptance: A1 inside ClearRadius the image equals the Enable 0 image. A2 between ClearRadius and FadeRadius the black grows from none to the current effect with no visible ring or hard edge. A3 beyond FadeRadius the darkening equals task-426 (circles around the screen centre, CornerDarkness at the corners). A4 the pattern stays fixed on screen when the miner moves (seen in task-426; not re-captured, owner 2026-11-04: one before/after pair only). A5 Enable 0 gives the plain image. A6 ClearRadius and FadeRadius are tunable in MPC_PlayerLight and apply live. Values per Questions (defaults until answered).
- gdd_check: visual-style (approved spec, provisional screen darkener sentence, task-426): no conflict; the clear zone is added to that sentence (edit applied on confirmation). No open gdd value blocks; unit, start values, remap and fade shape are Q11 to Q15. task-426 open point (corner measured 0.27 to 0.60 of the undarkened frame instead of 0.15) is Q16; task-425 capture was waived until after task-426 (Q17). Milestone M2.
- confirmed_by_owner: yes (2026-11-04, "Ok pour tout, fait juste une capture avant après. Car j'ai pas mal changé les lights manuellement (sinon tu n'as pas une comparaison correcte)")
- milestone: M2
- next_step: task-427-01-clear-zone-material
- started: 2026-11-04T17:41:06Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-04 (owner "Ok pour tout, fait juste une capture avant après. Car j'ai pas mal changé les lights manuellement (sinon tu n'as pas une comparaison correcte)"): the defaults of Q11, Q12, Q13, Q14, Q15 are accepted; values provisional, owner 2026-11-04.
- Q11 Reading: inside the first radius (ClearRadius) no black is added at all; between ClearRadius and the second radius (FadeRadius, also measured from the screen centre, not a width) the black grows from none to the current effect; beyond FadeRadius the current circles are unchanged. Default: this reading.
- Q12 Unit of the radii: Default: share of the centre-to-corner distance, 0 to 1, like the current shader (0.5 = halfway to a corner), same look at any resolution. Other choice: pixels.
- Q13 Start values: Default: ClearRadius 0.15 (about 165 px at 1920x1080), FadeRadius 0.35 (about 385 px, about the edge of the miner light pool); you tune them in MPC_PlayerLight.
- Q14 Beyond FadeRadius: Default: the current circles stay exactly as today, the clear zone is just cut out of them (at 0.35 the current effect is already about 0.30 black, so the transition goes from 0 to about 0.30). Other choice: restart the min-max fade at FadeRadius (CenterDarkness at FadeRadius, CornerDarkness at the corners), which makes the ring area lighter than today.
- Q15 Shape of the transition: Default: soft (slow start and slow end, no visible ring). Other choice: straight (linear), like the main fade.
- Q16 (decided 2026-11-04, owner "fait juste une capture avant après": no tonemapper-off run; the capture only reports the corner ratio read from its single before/after pair against the expected on-screen value about 0.4; values kept as they are) task-426 open point, corners lighter than 0.85 promises: the black is added before the screen brightness curve, so 15 % of the colour left shows as roughly 40 % on screen. Default: keep the values as they are; the capture checks the corner against that expected on-screen value and once with the brightness curve off (expected about 0.15), and you tune by eye. Other choice: make the values mean what the eye sees (extra shader change).
- Q17 (decided 2026-11-04, owner "fait juste une capture avant après. Car j'ai pas mal changé les lights manuellement": not checked by the capture; the owner judges the light himself; task-425 points stay unverified) The task-425 miner light checks were never seen in game (capture waived until after task-426). Default: this task's capture also checks them (light on and off at the same spot: L1 to L5). Other choice: leave them unchecked.

## Subtasks
1. task-427-01-clear-zone-material (vault/reference/tasks/task-427-darkener-clear-zone/task-427-01-clear-zone-material.md) — bp: M_PP_PlayerLightMask and MPC_PlayerLight: ClearRadius and FadeRadius
2. task-427-02-clear-zone-capture (vault/reference/tasks/task-427-darkener-clear-zone/task-427-02-clear-zone-capture.md) — capture: one before/after pair, check the clear zone
3. task-427-03-clear-zone-doc (vault/reference/tasks/task-427-darkener-clear-zone/task-427-03-clear-zone-doc.md) — doc: document ClearRadius and FadeRadius

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-427-01-clear-zone-material]] — step 1, bp.
- [[task-427-02-clear-zone-capture]] — step 2, capture.
- [[task-427-03-clear-zone-doc]] — step 3, doc.
- [[visual-style]] — gdd look node, screen darkener sentence extended with the clear zone
- [[mpp-player-light-mask]] — the material and collection extended here
- [[task-426-screen-radial-darkener]] — previous step, the darkener this task extends
- [[task-425-miner-floor-wall-light]] — miner light; its checks stay with the owner (Q17)
