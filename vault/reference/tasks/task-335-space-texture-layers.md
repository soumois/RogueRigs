---
id: task-335-space-texture-layers
description: "Global task: replace the procedural nebula of the void background with layered ChatGPT-generated tileable space textures at several depths, procedural stars kept."
parent: tasks-to-do-index
kind: task
status: todo
---
# Layered generated space textures for the void

- request: owner, 2026-10-27 (verbatim, French): "ça ne marche pas vraiment, peut être jouer avec des textures superposé géneré par chatgpt". Feedback on the procedural background of [[task-332-space-background]] after the [[task-333-nebula-visible]] retune (MI_Space_Nebula Intensity 0.06 to 1.6, capture `Saved/Captures/task333_nebula/contact_sheet.png`: soft blurry violet blobs, no structure). Reading: keep a layered background with depth, but the nebula comes from AI-generated images layered on each other instead of procedural noise. No file given. Owner answer (2026-10-27, relayed verbatim by the main session): "ok", after being told "ok" accepts every default and confirms the plan, so S1 to S10 take their defaults (S10 written in [[visual-style]]; task-333 cancelled per S1).
- goal: looking past the arena edge, the player sees painted space clouds with real structure (wisps, filaments, dark gaps) in several layers that shift against each other with depth and drift slowly, procedural stars behind, still darker than the floor beside the edge.
- acceptance: prompt node and retained images saved in the repo (01, 02); two tileable textures without seam in a 3 x 3 preview (02); material and instances compile (03); BP_SpaceBackground shows the texture layers at their depths, no plane edge in view, read back in L_ArenaM1 (04); a `-game` capture near the east edge shows the layers, compared with the task-333 capture, numbers in the S6 band (05); doc nodes updated (06); committed by explicit paths.
- gdd_check: read [[visual-style]] (void line: "procedural, layered, never a pasted image", three procedural planes, motion in the material only, nebula peak 20 to 35 luma, mask applies), [[color-code]] (void palette), [[bp-space-background-material]] ("no texture"), [[bp-space-background]] (planes, coverage arithmetic), [[mpp-player-light-mask]] (about 0.08 far from the miner), [[art-prompts-floor-texture]] (precedent: art agent writes ChatGPT prompts, owner generates, `make_tileable.js` at 2048), [[agent-art]] and [[kie-image-workflow]] (Kie.ai route, 1K cap), [[milestones]] (M3 polish, in scope). Conflicts: the request supersedes "procedural" in the void line and Q2 of task-332 (three procedural planes), and N3 of task-333 (parameters only, no new material); "never a pasted image" stays (layers, depth, drift). Open values: S1 to S10. Questions asked 2026-10-27, each with its default; owner "ok" (2026-10-27) = every default below is DECIDED (provisional values, tuned by eye later):
  - S1 task-333: cancelled, superseded by this task; Intensity 1.6 stays on MI_Space_Nebula, which leaves its plane; its capture is the "before".
  - S2 source: owner generates in ChatGPT from the art agent's prompts (floor-texture route). Alternative: art agent through Kie.ai (GPT Image 2, 1K, about 6 credits per image).
  - S3 layers: two images. Deep: wide dark colour washes, very low contrast, Z -8000 (new plane). Clouds: wisps and filaments with dark gaps, Z -5000, replaces the procedural nebula. Procedural stars (Z -12000) and dust (Z -2000) untouched. No star, planet, asteroid or text in the images.
  - S4 format: square, tileable (`make_tileable.js`, 2048), world-XY UV, one tile about 20000 uu (deep) and 12000 uu (clouds). Alternative: one large non-repeating image per layer.
  - S5 blend and motion: Additive over near-black images; slow UV drift, deep about 5 uu/s, clouds about 12 uu/s, different directions; no rotation.
  - S6 brightness: unchanged: cloud peak about 20 to 35 sRGB luma near an edge, below the floor strip beside it, stars brightest, mask applies.
  - S7 procedural base: M_SpaceBackground graph and stars and dust instances untouched; nothing deleted.
  - S8 task-334: unchanged (12 Blender 3D asteroids), runs after this task. Alternative: asteroids as a generated texture layer.
  - S9 capture: one `-game` capture near the east edge, two shots 3 s apart, compared with the task-333 capture.
  - S10 GDD: void line becomes "layered: procedural stars and generated tileable cloud textures at depths, slow drift, never a single pasted image".
- confirmed_by_owner: yes (2026-10-27, "ok", relayed by the main session)
- milestone: M3
- next_step: task-335-01-space-prompts
- started: open
- waiting_for: none
- result: open

## Subtasks
1. task-335-01-space-prompts (vault/reference/tasks/task-335-space-texture-layers/task-335-01-space-prompts.md) — art: ChatGPT prompts for the deep and cloud layers, prompt node, handed to the owner.
2. task-335-02-space-textures (vault/reference/tasks/task-335-space-texture-layers/task-335-02-space-textures.md) — art: owner images saved, made tileable at 2048, 3 x 3 previews, review.
3. task-335-03-space-texture-material (vault/reference/tasks/task-335-space-texture-layers/task-335-03-space-texture-material.md) — bp: import the two textures, M_SpaceTexture with MI_Space_Deep and MI_Space_Clouds.
4. task-335-04-space-layers (vault/reference/tasks/task-335-space-texture-layers/task-335-04-space-layers.md) — bp: BP_SpaceBackground gets the texture layers at their depths.
5. task-335-05-capture (vault/reference/tasks/task-335-space-texture-layers/task-335-05-capture.md) — cpp: one `-game` capture near the east edge, numbers, comparison, sheet for the owner.
6. task-335-06-docs (vault/reference/tasks/task-335-space-texture-layers/task-335-06-docs.md) — doc: material and Blueprint doc nodes, world index.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-335-01-space-prompts]] — step 1, art.
- [[task-335-02-space-textures]] — step 2, art.
- [[task-335-03-space-texture-material]] — step 3, bp.
- [[task-335-04-space-layers]] — step 4, bp.
- [[task-335-05-capture]] — step 5, cpp.
- [[task-335-06-docs]] — step 6, doc.
- [[task-332-space-background]] — the procedural background this changes; its Q2 is superseded.
- [[task-333-nebula-visible]] — the retune the owner judged not working.
- [[task-334-near-asteroids]] — the 3D asteroids in front of these layers.
