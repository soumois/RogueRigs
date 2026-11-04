---
id: task-426-screen-radial-darkener
description: "Global task: Step 2: rework M_PP_PlayerLightMask into a screen-space radial darkener (min black at centre, max at corners, linear fade), no miner position."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Darkener shader: circles around the screen centre, min at centre, max at corners

- request: Owner 2026-11-04 (verbatim): "Le but de cette conversation est de retravailler le shader d'assombrissement, on va travailler par étape. Plutôt qu'avoir par shader le fait d'illuminer le centre, j'aimerais une light sur le personnage qui ne touche que le sol et les murs. Ensuite, je voudrais que le shader actuel soit modifié pour retirer l'effet lumineux et me propose par cercle autour du centre de l'écran avec une valeur max et min. ( min = valeur du noir rajouté au centre de l'écran, max = valeur de noir rajouté au coin de l'écran et un fade entre ces deux valeurs ) Le radius = distance entre le centre et un coin de l'écran. N'hésite pas à me poser des questions, je ne suis pas technical artiste, donc je n'ai peut être pas exprimé mon idée avec le bon vocabulaire technique" This task is step 2 ("Ensuite ... retirer l'effet lumineux ... par cercle autour du centre de l'écran"); it starts after task-425-miner-floor-wall-light is done and the owner gives his go (Q5 of task-425-miner-floor-wall-light). Owner answer 2026-11-04 (relayed by the main session, verbatim): "Tout par défaut" = the plan and the defaults of Q6, Q7, Q8, Q9, Q10 accepted.
- goal: M_PP_PlayerLightMask no longer uses the miner position: black added = lerp(CenterDarkness, CornerDarkness, t), t = saturate(length((UV - 0.5) * ViewSize) / (0.5 * length(ViewSize))) (true circles, t = 1 at the corners); out = SceneColor * (1 - black * Enable). Void pixels (WorldPos.z < VoidZ) use min(black, VoidMaxDarkness). MPC_PlayerLight keeps Enable, VoidZ and PlayerPosition, gets CenterDarkness, CornerDarkness, VoidMaxDarkness, and loses FarBrightness, VoidFarBrightness, Radius, Exponent. Asset names unchanged.
- acceptance: A1 screen centre not darkened at CenterDarkness 0. A2 corners darkened by CornerDarkness (0.85 = 15 % of the colour left). A3 darkness forms circles around the screen centre and fades linearly between them. A4 the dark pattern does not move with the world or the miner. A5 void pixels at most 0.5 black. A6 Enable 0 gives the plain image. Values per Questions (defaults until answered).
- gdd_check: visual-style void sentence (task-335 far gain for the void, provisional) is rewritten in screen terms; Q9 asks whether to keep the void exception. The current world-space mask (mpp-player-light-mask) is replaced, not extended. No open gdd value blocks; start values are Q7. Milestone M2.
- confirmed_by_owner: yes (2026-11-04, "Tout par défaut")
- milestone: M2
- next_step: task-426-03-radial-mask-doc
- started: 2026-11-04T17:03:55Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-04 (owner "Tout par défaut"): every default below is accepted; values provisional, owner 2026-11-04.
- Q6 Unit: Default: both values are the share of black added, 0 = nothing added, 1 = fully black (pixel = colour x (1 - value)).
- Q7 Start values: Default: min (centre) 0, max (corners) 0.85, close to today's corners; you tune them in MPC_PlayerLight in the editor.
- Q8 Fade shape: Default: straight (linear) fade along true circles. On a 16:9 screen the circle that touches the corners passes outside the screen, so the middle of the top and bottom edges is about halfway (about 0.5 x max) and the middle of the left and right edges about 0.87 x max. Other choice: an oval that follows the screen shape (every edge middle at the same value).
- Q9 Space background (stars, clouds): today it is darkened less (at most 0.5 black). Default: keep that limit. Other choice: same circles for everything.
- Q10 The C++ code that sends the miner position to the shader becomes useless. Default: leave it as is (no C++ change, harmless). Other choice: remove it (one more C++ subtask).

## Subtasks
1. task-426-01-radial-mask-material (vault/reference/tasks/task-426-screen-radial-darkener/task-426-01-radial-mask-material.md) — bp: M_PP_PlayerLightMask and MPC_PlayerLight: screen radial darkener
2. task-426-02-radial-mask-capture (vault/reference/tasks/task-426-screen-radial-darkener/task-426-02-radial-mask-capture.md) — capture: check the screen circles darkening
3. task-426-03-radial-mask-doc (vault/reference/tasks/task-426-screen-radial-darkener/task-426-03-radial-mask-doc.md) — doc: document the new darkener

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-426-01-radial-mask-material]] — step 1, bp.
- [[task-426-02-radial-mask-capture]] — step 2, capture.
- [[task-426-03-radial-mask-doc]] — step 3, doc.
- [[visual-style]] — gdd look node updated with the decided darkener
- [[mpp-player-light-mask]] — the material and collection reworked here
- [[ppv-xp-glow]] — volume carrying the material
- [[task-425-miner-floor-wall-light]] — step 1, done before this one
