---
id: task-339-heal-chunk-mesh
description: "Global task: Blender meshes of 3 red heal pickup chunk variants from the chosen sheet, then in game on BP_PickupHeal, random per drop."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heal pickup chunks: Blender meshes and in-game look

- request: "Ok, fait l'implémentation 3D en blender de ces 3 mineraux" (owner, 2026-10-28, relayed verbatim by the main session, said after seeing the pass 2 sheet `Art/Source/heal-pickup/20261027-165219-heal-pickup-1.png` of [[task-336-heal-crystal-variants]]). Earlier owner constraints (task-336): the pickup is thrown out of the broken vein and may roll a little, so it reads as a freshly broken-off chunk (compact, resting base, jagged fracture faces); red; 3 near-identical visual variants, no gameplay difference (15 HP), drawn at random per drop like the Duskite ore chunks ([[task-189-ore-chunk-look]]). Planner reading: "ok" = the pass 2 sheet is the validated image; "en blender" = Blender route, no Meshy. Owner answer to Q1 to Q3 (2026-10-28, relayed by the main session): "ok" = every default accepted (pass 2 sheet validated, full chain, default values; shapes follow the sheet with whole prisms, not stumps).
- goal: Blender meshes of the 3 red heal pickup chunk variants from the chosen sheet, then in game on BP_PickupHeal, drawn at random per drop.
- acceptance: the owner accepts the three Blender meshes (preview render); a heal pickup dropped in game shows one of SM_HealChunk_01..03 in red with the pulse, at random per drop, resting on a face under physics; BP_PickupHeal compiles; the two existing heal pickup tests pass; a -game contact sheet is sent to the owner; bp-pickup-heal updated; lint clean; committed.
- gdd_check: read [[heal-ore]] (approved; Pickup variants line, provisional 2026-10-28: 3 near-identical red clusters "3 to 5 chunky faceted points, no rock", "mesh and in-game assignment are a later request": this is that request), [[color-code]] (heal pickups red, no conflict), [[art-prompts-heal-pickup]] (pass 2 self-review and style-guide verdict: A1 fail, reads as two whole prisms on a red fracture base, not a snapped-off chunk), [[bp-pickup-heal]] (grey-box cube 30 uu, override MI_GB_PickupHeal, RestOrientation Full, MeshVariants empty), [[bp-pickup-ore]] and [[task-189-ore-chunk-look]] (precedent chain: art, 3d, bp import and materials, bp trail, bp Blueprint; 30 cm, centred pivot, 300/500 tris, slot defaults, no shadow), [[sys-pickups-lifecycle]] (Chaos physics on landing uses the mesh simple collision; variants share slot layout or overrides cleared), [[blender-asset-route]], [[mesh-fidelity-loop]], [[asset-pipeline]], [[milestones]] (heal ore is M2, no scope issue). Rules unchanged (15 HP, same pop, same refusal at full health). Conflict: the sheet's look (two whole prisms on a fracture body) differs from the heal-ore Look line ("3 to 5 points, no rock") and failed the style-guide A1 check; settled by Q1 (sheet look kept, heal-ore Pickup variants line rewritten 2026-10-28). Questions asked with defaults; owner "ok" 2026-10-28: every default below is DECIDED (provisional values), the alternatives are not taken:
  - Q1 image and shape: the pass 2 sheet is the validated image, copied to `vault/assets/heal-pickup-sheet.png`, and task-336 closes on it; the Blender meshes follow the sheet (two whole pointed prisms side by side on a wide body of red fracture chips, a small third tip on the left and right pieces) plus a flat resting face underneath. Alternative: in Blender, shorten the prisms to snapped-off stubs so it reads more as a torn-off chunk (style-guide correction).
  - Q2 scope: full chain like the Duskite chunk: 3D Blender meshes, import and materials, collision, BP_PickupHeal with the 3 variants at random, the 2 existing heal tests and one in-game capture, doc. No trail (not requested). Alternative: the 3 meshes only (FBX, report, render), the in-game part as a later request.
  - Q3 values (provisional): 30 cm like today's cube and the Duskite chunk; centred pivot; 300 tris target, 500 max per variant (Duskite chunk budget); two materials, bright red prism faces and darker red fracture faces, as new instances MI_HealChunk_Bright and MI_HealChunk_Dark copied from the floor heal crystals (same values, tunable alone); one simple collision primitive per variant (Duskite rule); no cast shadow (as the Duskite chunk); RestOrientation Full kept. Alternative: reuse MI_HealCrystal_Bright and _Dark directly (one tuning then changes the floor crystals too).
- confirmed_by_owner: yes (2026-10-28, "ok" to Q1 to Q3, relayed by the main session)
- milestone: M2
- next_step: task-339-01-heal-chunk-mesh
- started: 2026-10-28T01:25:52Z
- waiting_for: none
- result: open

## Subtasks
1. task-339-01-heal-chunk-mesh (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-01-heal-chunk-mesh.md) — 3d: Blender route, SM_HealChunk_01..03 from the chosen sheet, FBX, reports, fidelity loop, preview the owner accepts.
2. task-339-02-heal-chunk-import (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-02-heal-chunk-import.md) — bp: import the 3 meshes, new MI_HealChunk_Bright and _Dark as slot defaults.
3. task-339-03-heal-chunk-collision (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-03-heal-chunk-collision.md) — cpp: one simple collision primitive per variant, editor closed.
4. task-339-04-heal-pickup-blueprint (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-04-heal-pickup-blueprint.md) — bp: BP_PickupHeal uses the 3 chunks at random, overrides cleared, no shadow.
5. task-339-05-heal-chunk-capture (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-05-heal-chunk-capture.md) — cpp: the 2 existing heal pickup tests, one -game capture sent to the owner.
6. task-339-06-heal-chunk-doc (vault/reference/tasks/task-339-heal-chunk-mesh/task-339-06-heal-chunk-doc.md) — doc: rewrite bp-pickup-heal.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-339-01-heal-chunk-mesh]] — step 1, 3d.
- [[task-339-02-heal-chunk-import]] — step 2, bp.
- [[task-339-03-heal-chunk-collision]] — step 3, cpp.
- [[task-339-04-heal-pickup-blueprint]] — step 4, bp.
- [[task-339-05-heal-chunk-capture]] — step 5, cpp.
- [[task-339-06-heal-chunk-doc]] — step 6, doc.
- [[task-336-heal-crystal-variants]] — the image task whose sheet this builds.
