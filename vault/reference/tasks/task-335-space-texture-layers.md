---
id: task-335-space-texture-layers
description: "Global task: replace the procedural nebula of the void background with bold layered AI-generated tileable space textures at several depths, procedural stars kept."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Layered generated space textures for the void

- request: owner, 2026-10-27 (verbatim, French): "ça ne marche pas vraiment, peut être jouer avec des textures superposé géneré par chatgpt". Feedback on the procedural background of [[task-332-space-background]] after the [[task-333-nebula-visible]] retune (MI_Space_Nebula Intensity 0.06 to 1.6, capture `Saved/Captures/task333_nebula/contact_sheet.png`: soft blurry violet blobs, no structure). Reading: keep a layered background with depth, but the nebula comes from AI-generated images layered on each other instead of procedural noise. No file given. Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so S1 to S10 take their defaults (S10 written in [[visual-style]]; task-333 cancelled per S1). Second answer, to the B questions of `feedback_2` (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the changed plan, so B1 to B7 take their defaults (written in [[visual-style]] and [[color-code]]); they override S3 ("very low contrast" deep washes) and S6. Third answer, after the task-335-06 capture failed acceptance (clouds p99 180, max 232 against 70 to 100; void mean 45.5 above floor strips 34.6 to 37.3; deep layer not distinguishable; clouds near-black share 14.2 percent) (2026-10-27, relayed verbatim by the main session): "Oui, ajoute un recalage de l'intensité des nuages (et de la couche profonde si besoin) suivi d'une nouvelle capture, avant les docs." Added subtasks 07 (bp, Intensity only) and 08 (cpp, second capture); docs moved to 09.
- goal: looking past the arena edge, the player sees bold, saturated painted space clouds with real structure (dense filaments, bright cores, dark dust lanes) in several layers that shift against each other with depth and drift slowly, procedural stars behind; bright cores may exceed the floor beside the edge, the void mean stays below the floor mean.
- acceptance: bold prompts and retained images saved in the repo, owner-validated (02); two tileable textures without seam in a 3 x 3 preview (02); material and instances compile (03); BP_SpaceBackground shows the texture layers at their depths, no plane edge in view, read back in L_ArenaM1 (04); M_PP_PlayerLightMask gives void pixels a far gain of 0.5 (05); a `-game` capture near the east edge shows the layers, compared with the task-333 capture, cloud peak in the B1 band, void mean below floor mean (06); Intensity retuned from the 06 numbers (07); a second capture shows the cloud peak in the B1 band and the void mean below the floor strips (08); doc nodes updated (09); committed by explicit paths.
- gdd_check: read [[visual-style]] (void line: "procedural, layered, never a pasted image", three procedural planes, motion in the material only, nebula peak 20 to 35 luma, mask applies), [[color-code]] (void palette), [[bp-space-background-material]] ("no texture"), [[bp-space-background]] (planes, coverage arithmetic), [[mpp-player-light-mask]] (about 0.08 far from the miner), [[art-prompts-floor-texture]] (precedent: art agent writes ChatGPT prompts, owner generates, `make_tileable.js` at 2048), [[agent-art]] and [[kie-image-workflow]] (Kie.ai route, 1K cap), [[milestones]] (M3 polish, in scope). Conflicts: the request supersedes "procedural" in the void line and Q2 of task-332 (three procedural planes), and N3 of task-333 (parameters only, no new material); "never a pasted image" stays (layers, depth, drift). Open values: S1 to S10. Questions asked 2026-10-27, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - S1 task-333: cancelled, superseded by this task; Intensity 1.6 stays on MI_Space_Nebula, which leaves its plane; its capture is the "before".
  - S2 source: CHANGED by the owner 2026-10-27 ("Tu peux contacter l'api toi même"): the art agent generates the images through Kie.ai (GPT Image 2, 1K, about 6 credits per image) from the written prompts; one candidate per layer, at most one correction each; the main session shows them to the owner for validation before subtask 02 continues. Was: owner generates in ChatGPT.
  - S3 layers: two images. Deep: wide dark colour washes, very low contrast, Z -8000 (new plane). Clouds: wisps and filaments with dark gaps, Z -5000, replaces the procedural nebula. Procedural stars (Z -12000) and dust (Z -2000) untouched. No star, planet, asteroid or text in the images.
  - S4 format: square, tileable (`make_tileable.js`, 2048), world-XY UV, one tile about 20000 uu (deep) and 12000 uu (clouds). Alternative: one large non-repeating image per layer.
  - S5 blend and motion: Additive over near-black images; slow UV drift, deep about 5 uu/s, clouds about 12 uu/s, different directions; no rotation.
  - S6 brightness: unchanged: cloud peak about 20 to 35 sRGB luma near an edge, below the floor strip beside it, stars brightest, mask applies.
  - S7 procedural base: M_SpaceBackground graph and stars and dust instances untouched; nothing deleted.
  - S8 task-334: unchanged (12 Blender 3D asteroids), runs after this task. Alternative: asteroids as a generated texture layer.
  - S9 capture: one `-game` capture near the east edge, two shots 3 s apart, compared with the task-333 capture.
  - S10 GDD: void line becomes "layered: procedural stars and generated tileable cloud textures at depths, slow drift, never a single pasted image".
- feedback_2: owner, 2026-10-27 (verbatim, French), after seeing the two Kie.ai raw images (deep 20261027-141216, clouds 20261027-141558 after the desaturation correction; 18 credits spent): "ça reste trés timide". Reading: too dim, too flat, too little presence, in the images and in the expected game look. GDD re-check: the timidity comes from decided values, not from a defect: (a) S6 and [[visual-style]]: cloud peak 20 to 35 sRGB luma, below the floor strip beside the edge (18 to 49); (b) [[color-code]] void line: one hue family, low saturation, which made the style review reject the brighter pinker try 141320; (c) [[mpp-player-light-mask]]: gain 1/(1+(r/200)^1.8) toward 0.08, about 0.13 at 1000 uu and 0.09 at 2000 uu from the miner, so the void is always at about a tenth of its value; (d) [[art-prompts-space-background]] prompts: hex caps (deep below #4A2850, clouds below #8A4A84) and 40 percent near-black asked. Retained images: deep max #341741, almost a flat wash; clouds max #6A4286, 43 percent near-black. Reserved colours limiting a second hue ([[readability-rules]], [[color-code]]): acid green, steel blue (enemies), red (enemy fire, healing), orange (explodes), cyan (XP), teal (player side), magenta-pink (big XP core). Questions B1 to B7 asked 2026-10-27, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - B1 brightness: cloud peak about 70 to 100 sRGB luma near an edge; bright cores may exceed the floor strip beside the edge, the void mean stays below the floor mean. Replaces S6. Alternative: stay below the floor strip (peak about 45).
  - B2 mask: void pixels (world Z below -200: stars plane, layers, later asteroids) get their own far gain 0.5 instead of 0.08 in M_PP_PlayerLightMask; new bp subtask before the capture. Alternatives: full exemption (gain 1); mask unchanged, Intensity compensates (corners stay near black).
  - B3 colour: stronger saturation within the family (indigo, electric violet, magenta) plus a second hue, deep ultramarine blue (saturated, not grey steel blue), mainly in the deep layer. Alternatives: warm rose (close to the big XP magenta-pink and the heal red); single family, saturation only.
  - B4 structure: near-black share about 20 to 25 percent (was 40 to 50); denser filaments, bright cores, dark dust lanes cutting the clouds; deep layer with visible large shapes, not a flat wash.
  - B5 images: regenerate both through Kie.ai with new prompts, one image each, at most one correction each (12 to 24 credits); current images kept as fallback; M_SpaceTexture also gets Contrast and Saturation parameters. Alternative: keep the current images, boost in the material only (no credit, structure and colour limited).
  - B6 depth: no third texture layer now (the task-334 asteroids are the near layer). Alternative: a third layer of sparse bright filaments near Z -3000.
  - B7 GDD: [[visual-style]] void line and [[color-code]] void row rewritten with B1 to B4 (provisional), replacing S6 and "low saturation".
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session; changed plan B1 to B7 confirmed by a second "ok" the same day)
- milestone: M3
- next_step: task-335-07-intensity-retune
- started: 2026-10-27T22:40:11Z
- waiting_for: none
- result: open

## Subtasks
1. task-335-01-space-prompts (vault/reference/tasks/task-335-space-texture-layers/task-335-01-space-prompts.md) — art: ChatGPT prompts for the deep and cloud layers, prompt node, handed to the owner.
2. task-335-02-space-textures (vault/reference/tasks/task-335-space-texture-layers/task-335-02-space-textures.md) — art: bold prompts (B1 to B5), both images regenerated via Kie.ai, owner validation, tileable at 2048, 3 x 3 previews, review.
3. task-335-03-space-texture-material (vault/reference/tasks/task-335-space-texture-layers/task-335-03-space-texture-material.md) — bp: import the two textures, M_SpaceTexture with MI_Space_Deep and MI_Space_Clouds.
4. task-335-04-space-layers (vault/reference/tasks/task-335-space-texture-layers/task-335-04-space-layers.md) — bp: BP_SpaceBackground gets the texture layers at their depths.
5. task-335-05-void-mask-gain (vault/reference/tasks/task-335-space-texture-layers/task-335-05-void-mask-gain.md) — bp: M_PP_PlayerLightMask gives void pixels (world Z below -200) a far gain of 0.5.
6. task-335-06-capture (vault/reference/tasks/task-335-space-texture-layers/task-335-06-capture.md) — cpp: one `-game` capture near the east edge, numbers, comparison, sheet for the owner.
7. task-335-07-intensity-retune (vault/reference/tasks/task-335-space-texture-layers/task-335-07-intensity-retune.md) — bp: retune Intensity of MI_Space_Clouds (and MI_Space_Deep if needed) from the 06 numbers.
8. task-335-08-recapture (vault/reference/tasks/task-335-space-texture-layers/task-335-08-recapture.md) — cpp: second `-game` capture at the same east edge, numbers, side-by-side with 06.
9. task-335-09-docs (vault/reference/tasks/task-335-space-texture-layers/task-335-09-docs.md) — doc: material, Blueprint and mask doc nodes (final intensities, FarBrightness 0.06), world index.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-335-01-space-prompts]] — step 1, art.
- [[task-335-02-space-textures]] — step 2, art.
- [[task-335-03-space-texture-material]] — step 3, bp.
- [[task-335-04-space-layers]] — step 4, bp.
- [[task-335-05-void-mask-gain]] — step 5, bp.
- [[task-335-06-capture]] — step 6, cpp.
- [[task-335-07-intensity-retune]] — step 7, bp.
- [[task-335-08-recapture]] — step 8, cpp.
- [[task-335-09-docs]] — step 9, doc.
- [[task-332-space-background]] — the procedural background this changes; its Q2 is superseded.
- [[task-333-nebula-visible]] — the retune the owner judged not working.
- [[task-334-near-asteroids]] — the 3D asteroids in front of these layers.
