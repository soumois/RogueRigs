---
id: task-189-ore-chunk-look
description: "Global task: ore chunk pickup in game as a glowing Duskite crystal cluster (image proposals, Meshy mesh, violet materials and trail)."
parent: tasks-done-index
kind: task
status: done
---
# Ore chunk in-game look

- request: Tu as l'icone du minerai violet ( et il est déjà intégré dans l'écran ), maintenant il faudrait le rendu in game; answers: 1. ok (the dropped ore chunk), 2. Normalement les deux mauves sont assez différent, 3. Non, j'attends des propositions d'image in game, je vais utiliser Meshy ( maintenant que j'ai payé l'abonnement ), 4. On pourra faire une trainée (owner, 2026-10-15)
- goal: In-game look of the ore chunk pickup: a glowing Duskite violet crystal cluster like the HUD ore icon, Meshy mesh, violet trail, replacing the cyan cube.
- acceptance: an ore chunk dropped in game shows SM_OreChunk with violet glow and, when magnetised, a violet trail; BP_PickupOre compiles; existing tests pass; docs updated; lint clean; committed.
- gdd_check: ore-blocks, ore-economy, color-code read 2026-10-15. Conflict: color-code gave resource pickups the cyan nut look; owner decided the ore chunk is Duskite violet (color-code updated 2026-10-15) and judges it distinct from the lilac big XP chip. Ore blocks keep their look. Size kept at 30 cm, glow pulse like the XP nut (defaults accepted). Route: art image proposals, then Meshy (owner).
- confirmed_by_owner: yes (2026-10-15: answers to the plan, expects image proposals)
- milestone: M1
- next_step: none
- result: Done. BP_PickupOre: SM_OreChunk_01 at scale 1, OverrideMaterials cleared (slot defaults MI_OreChunk_Bright/Dark), RestOrientation Full, MeshVariants 01-03, CastShadow false, TrailFX NS_OreTrail (scale 0.3, auto activate off) activated on OnMagnetStart, no look-at (Orchestrator choice, keeps the random orientation); BP_ArenaGrid OrePickupClass unchanged; compiled and saved; docs updated; look, trail and runtime draw not seen in PIE; commit 8cf8486b

## Subtasks
1. task-189-01-ore-chunk-look — art
2. task-189-02-ore-chunk-mesh — 3d
3. task-189-03-ore-chunk-materials — bp
4. task-189-04-ore-chunk-trail — bp
5. task-189-05-ore-chunk-blueprint — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-189-01-ore-chunk-look]] — step 1, art.
- [[task-189-02-ore-chunk-mesh]] — step 2, 3d.
- [[task-189-03-ore-chunk-materials]] — step 3, bp.
- [[task-189-04-ore-chunk-trail]] — step 4, bp.
- [[task-189-05-ore-chunk-blueprint]] — step 5, bp.
- [[color-code]] — the ore pickup colour decided for this task.
