---
id: task-332-space-background
description: "Global task: a layered procedural space background in the void around the arena, with real depth parallax, instead of a flat pasted image."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Layered procedural space background for the void

- request: owner, 2026-10-27 (verbatim, French): first "Il faudrait maintenant le fond 'espace' pour le vide, mais que ça ne fasse pas une simple image 'collée'. Recherche ce qui fonctionne bien comme technique pour ça", then, after the research below was shown to him, "fond d'espace procédural en couches pour le vide". No file given. Research shown to him (not validated as design): 3 to 4 large Unlit planes under the arena at real depths, parallax given by the perspective camera (arm 1400); roles far stars, wide nebula, near dust; procedural material (thresholded-noise stars with slow twinkle, nebula from 2 to 3 noise octaves with slow flow and a 2 to 3 hue ramp from the game palette, world-position noise so no tiling seam); optional Niagara dust and drifting debris; optional dark glow where floor meets void; constraints: darkening mask, fixed exposure -0.94 EV100, Unlit, no sun or shadow, no parallax occlusion mapping. Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so Q1 to Q9 take their defaults (see `gdd_check`, decided values written in [[visual-style]] and [[color-code]]).
- goal: looking past the arena edge, the player sees a dark space background built of separate layers at different depths that shift against each other as the camera follows the miner, with slow procedural motion, no visible tiling, no plane edge, and the floor and units still read first.
- acceptance: M_SpaceBackground and its instances exist and compile (subtask 01); BP_SpaceBackground holds the layers at their depths and one instance is placed and saved in L_ArenaM1 (subtask 02); layers cast no shadow and receive no decal; no plane edge in view with the miner on any arena cell (computed from the camera FOV, arm and aspect); a `-game` capture near an arena edge shows the layers and is sent to the owner (subtask 03, per Q9); the background is darker than the floor next to it in that capture (Q4); doc nodes written (subtask 04); committed by explicit paths.
- gdd_check: read [[visual-style]] and [[arena-grid]] (arena edge "is a cliff into space with stars and a planet"; void cells have no floor and no block), [[art-style-guide]] A13 (same line), [[color-code]] (no background or void colour defined), [[readability-rules]] (acid green, steel blue reserved for enemies; red = enemy fire or healing; orange = explodes; rarity colours UI only), [[milestones]] (M3 polish: in scope), [[mpp-player-light-mask]] (screen-space darkener on world XY of each pixel, gain FarBrightness 0.08 to 1, Radius 200, Exponent 1.8: the void background, far from the miner in XY, will be multiplied by about 0.08), [[handover-2026-10-21-lighting-floor]] (sun 3 with shadows, sky 0.3, exposure -0.94 EV100, far floor about 10/255), [[bp-floor-plates-material]] (HLSL value noise on world XY, the technique to reuse), [[bp-arena-grid]] (L_ArenaM1 has no sky atmosphere, void is plain dark). Conflict: the GDD promises a planet, the request does not name it (Q1). Open values: Q2 to Q8. Questions, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - Q1 planet: default no planet in this task (stars, nebula, dust only); the GDD line stays, a planet is a later request.
  - Q2 layers: default 3 planes, stars at Z -12000 (opaque, black base), nebula at Z -5000 (additive), sparse dust specks at Z -2000 (additive); provisional.
  - Q3 palette: default nebula in deep indigo-violet, violet and dusty magenta (the floor's dark violet-magenta family), low saturation; stars white with a slight warm or cool shift; no acid green, steel blue, red, orange or cyan.
  - Q4 brightness: default background always darker than the floor beside it; nebula peak at most the far floor value of the same frame; stars are the only bright points; all values provisional, tuned by eye in the instances.
  - Q5 darkening mask: default the mask also darkens the background (one rule, emissive compensates); no stencil exception, M_PP_PlayerLightMask untouched.
  - Q6 motion: default material only, slow star twinkle and very slow nebula flow; no Niagara dust, no drifting debris in this task.
  - Q7 edge: default no glow or gradient at the floor-void edge and no rock sag toward the void in this task (separate points).
  - Q8 who builds it: default editor work by the bp agent (one material with instances, one data-only Blueprint actor placed in L_ArenaM1), no C++ change.
  - Q9 capture: default one `-game` capture after the build, near an arena edge, by cpp, shown to the owner (lessons rule 2026-10-16, visible change); no before capture.
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: task-332-04-docs
- started: 2026-10-27T21:27:42Z
- waiting_for: none
- result: open

## Subtasks
1. task-332-01-space-material (vault/reference/tasks/task-332-space-background/task-332-01-space-material.md) — bp: M_SpaceBackground (Unlit procedural) with MI_Space_Stars, MI_Space_Nebula, MI_Space_Dust.
2. task-332-02-space-layers-actor (vault/reference/tasks/task-332-space-background/task-332-02-space-layers-actor.md) — bp: BP_SpaceBackground with the layer planes at their depths, one instance placed in L_ArenaM1.
3. task-332-03-capture (vault/reference/tasks/task-332-space-background/task-332-03-capture.md) — cpp: one `-game` capture near an arena edge, brightness numbers, sheet for the owner.
4. task-332-04-docs (vault/reference/tasks/task-332-space-background/task-332-04-docs.md) — doc: doc nodes of the material and the Blueprint, world index and bp-arena-grid placement line.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-332-01-space-material]] — step 1, bp.
- [[task-332-02-space-layers-actor]] — step 2, bp.
- [[task-332-03-capture]] — step 3, cpp.
- [[task-332-04-docs]] — step 4, doc.
- [[visual-style]] — the void edge line of the GDD.
- [[mpp-player-light-mask]] — the darkener that also covers the background.
- [[bp-floor-plates-material]] — the world-position noise technique reused.
