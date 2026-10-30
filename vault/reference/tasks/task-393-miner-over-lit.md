---
id: task-393-miner-over-lit
description: "Global task: The new Meshy miner looks burnt out (blown highlights) in the arena; reduce his brightness with the lever the owner picks."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Miner too bright in the arena

- request: Owner, 2026-10-30, relayed by the main session (translated): the player character (miner) looks very over-lit, "cramé" (burnt out, blown highlights) in the arena; he asks HOW it can be changed. One logical point, own global task. Planner findings (vault and assets, not seen on screen): the miner is now the Meshy mesh SK_Miner (task-386, BP_Miner mesh swap 46f17540) with M_Miner, a material made by the import script: Default Lit, one texture sample (T_Miner_BaseColor); T_Miner_Normal is imported but no second texture sample exists in M_Miner, so the normal map is probably not wired; roughness, specular and the albedo level are not documented. The arena lighting was tuned on 2026-10-21 for the old Quinn mannequin: sun 3 lux white with shadows, sky light 0.3 white, exposure fixed at -0.94 EV100, bloom 1.3 with threshold -1 (every pixel blooms), darkening mask gain 1 at the miner (it darkens away from him, never brightens) ([[lights-dark-ambient]], [[ppv-xp-glow]], [[mpp-player-light-mask]]). No player light exists (removed task-272-15). Likely cause: a bright Meshy albedo (often baked light) under lights tuned for the grey mannequin, plus the full-image bloom; to confirm by reading M_Miner and the texture. Levers and default in Q9, Q10. bp-miner doc is out of date (still says Quinn): the doc subtask fixes it. Owner answer 2026-10-30 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q9, Q10 accepted.
- goal: The miner reads less burnt out in the arena with the chosen lever, the rest of the scene unchanged (default: miner material only), with one parameter the owner can retouch in the editor.
- acceptance: bp: M_Miner read and reported (albedo mean of T_Miner_BaseColor, roughness, specular, metallic, normal map wired or not); MI_Miner (child of M_Miner) with scalar BaseColorScale (default per Q10) multiplying the base colour, assigned to SK_Miner or BP_Miner; nothing else in the scene changed; compiled and saved; docs updated (bp-miner refreshed: SK_Miner, M_Miner, MI_Miner); lint clean; committed by path; owner judges the look himself and retouches BaseColorScale.
- gdd_check: Read 2026-10-30: [[visual-style]] and [[color-code]] not changed (rendering only; rules versus rendering: no game rule changes). [[handover-2026-10-21-lighting-floor]]: lighting values decided by the owner (sun 3, sky 0.3, exposure -0.94, mask 0.08 / 200 / 1.8), "the later player light" still an open owner decision (not touched here). [[lights-dark-ambient]], [[ppv-xp-glow]], [[mpp-player-light-mask]], [[bp-miner]] (out of date: Quinn mesh), [[task-386-miner-mixamo-animation]] (Meshy miner import and mesh swap). Open values: the lever (Q9) and the brightness value (Q10, provisional default, owner tunes). Scope M2. No capture subtask (owner Q1, 2026-10-30): the owner checks the result himself in the packaged build. Repo state 2026-10-30: RRPlayerController input fix committed by the owner (091bf23c); Config/DefaultGame.ini (DirectoriesToAlwaysCook) and Config/DefaultEditor.ini have uncommitted owner changes: leave them untouched and never commit them with this task (commit by path).
- confirmed_by_owner: yes (2026-10-30, "ok")
- milestone: M2
- next_step: task-393-01-miner-material-scale
- started: 2026-10-30T22:02:49Z
- waiting_for: none
- result: open

## Questions
Decided 2026-10-30 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-30.
- Q9 Le levier. Cause probable : la texture couleur du mineur Meshy est claire (lumière souvent « cuite » dedans) sous un éclairage réglé le 21/10 pour l'ancien mannequin gris (soleil 3, ciel 0,3, exposition fixe -0,94), plus le bloom sur toute l'image (seuil -1). Choix : A) le matériau du mineur seul : une instance MI_Miner avec un multiplicateur de luminosité de la couleur (ne change rien d'autre dans la scène) ; B) le bloom : seuil relevé pour que seules les surfaces très claires aient un halo (change aussi le halo des XP, cristaux, effets) ; C) toute la scène plus sombre (exposition ou soleil). Default: A.
- Q10 La valeur : multiplicateur 0,6 (provisoire), paramètre BaseColorScale sur MI_Miner que vous retouchez à l'œil dans l'éditeur (effet en direct en PIE) ; l'agent relève aussi rugosité, spéculaire et la carte normale de M_Miner et les rapporte sans les changer. Default: 0,6 et rapport.

## Subtasks
1. task-393-01-miner-material-scale (vault/reference/tasks/task-393-miner-over-lit/task-393-01-miner-material-scale.md) — bp: read M_Miner, add MI_Miner with BaseColorScale 0.6 and assign it to the miner
2. task-393-02-doc-miner-material (vault/reference/tasks/task-393-miner-over-lit/task-393-02-doc-miner-material.md) — doc: refresh bp-miner (SK_Miner, ABP_Miner, M_Miner, MI_Miner, BaseColorScale, how to tune)

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-393-01-miner-material-scale]] — step 1, bp.
- [[task-393-02-doc-miner-material]] — step 2, doc.
- [[bp-miner]] — the miner Blueprint whose material changes; doc refreshed here.
- [[lights-dark-ambient]] — the scene lighting kept unchanged by the default.
- [[ppv-xp-glow]] — the bloom and exposure of option B and C.
- [[task-386-miner-mixamo-animation]] — the task that imported the Meshy miner and M_Miner.
