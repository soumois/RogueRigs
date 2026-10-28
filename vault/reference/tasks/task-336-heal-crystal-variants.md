---
id: task-336-heal-crystal-variants
description: "Global task: Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Heal crystal variants: images

- request: "Il me faut les images, le minerai de soin sera rouge. pas de différenciation, je ne veux pas de forme très différente. C'est juste pour ne pas avoir tout le temps le même minerai." (relayed verbatim by the main session). Earlier in the same exchange the owner asked for 3 different minerals for healing (what is picked up on the ground); the main session had proposed three provisional ideas (Rubéline: tall faceted hexagonal prisms; Cinabrite: rounded veined nodules; Grenat-géode: round geodes), which this answer replaces: red only, no gameplay difference, no very different shapes (owner, 2026-10-28)
- goal: Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same.
- acceptance: images sent to the owner; style-guide review done; the owner picks or rejects; the chosen image is copied to vault/assets/ only on his word; prompt node written; committed.
- gdd_check: read [[heal-ore]] (approved: red heal crystals and red pickups, 15 HP per pickup, 2 x 2 veins), [[color-code]] and [[readability-rules]] (heal = red, static on the floor; red also enemy fire), [[art-prompts-ore-deposit]] (heal deposits already have 3 owner-validated structural variants: cracked mound, inlaid slab, half-buried cluster, `vault/assets/heal-deposit-sheet.png`, meshes SM_HealDeposit_01..03 in game since task-234), [[bp-pickup-heal]] (the heal pickup is still a single grey-box red cube, 30 uu, `MeshVariants` empty), [[bp-pickup-ore]] (precedent: the ore pickup draws one of 3 chunk meshes per drop, task-189), [[agent-art]] and [[kie-image-workflow]] (Kie.ai route; owner allowed the art agent to call the API himself in task-335), [[milestones]]. Rules: no change (same 15 HP, same rules, "pas de différenciation"); colour: red, no conflict. Conflict / ambiguity: "le minerai" can be the dropped heal pickup or the heal deposits; the deposits already have 3 strongly different validated variants, which "je ne veux pas de forme très différente" would contradict if they were the target. Open values: Q1 to Q5 below, asked 2026-10-28 with defaults. Owner answer (2026-10-28, relayed by the main session): "ok" = every default below is DECIDED (provisional values); the alternatives are not taken. Written in [[heal-ore]] (Look line); [[color-code]] unchanged (heal stays red).
  - Q1 target: the heal pickup (dropped on the ground, today a red cube) gets 3 near-identical red crystal variants, drawn at random per drop like the ore chunk; the 3 heal deposits stay as validated. Alternative: new images for the deposits instead.
  - Q2 scope: images only in this task; the mesh (Blender), materials and in-game assignment come as a new request once he has picked. Alternative: full chain now (art, 3d, bp, capture, doc) as in task-189.
  - Q3 route: the art agent generates through Kie.ai (GPT Image 2, 1K, about 6 credits per image). Alternative: prompts handed to the owner for ChatGPT.
  - Q4 look: same red crystal as the heal deposit crystals (`vault/assets/heal-deposit-sheet.png` as image reference), a compact cluster of 3 to 5 chunky faceted red points, no rock, 30 cm pickup scale; the 3 variants share size, proportions and crystal count, only the arrangement and angles of the points change; not a red copy of the violet ore chunk shape. Pure red, no orange, no pink.
  - Q5 images: one 3-up sheet (three isolated variants, same camera, three-quarter top view, plain light grey background, usable for Blender), at most 2 correction passes (up to about 18 credits). The names Rubéline, Cinabrite, Grenat-géode are dropped: it stays "heal ore".
- owner_pick: 2026-10-28, owner "ok" to Q1 of [[task-339-heal-chunk-mesh]] (relayed by the main session): the pass 2 sheet `Art/Source/heal-pickup/20261027-165219-heal-pickup-1.png` is validated despite the style-guide A1 fail; copy it to `vault/assets/heal-pickup-sheet.png`, commit it with `Art/Source/heal-pickup/20261027-165219-heal-pickup.review.json`, then close this task. Earlier sheets stay untracked.
- confirmed_by_owner: yes (2026-10-28, "ok" to Q1 to Q5, relayed by the main session)
- milestone: M2
- next_step: task-336-01-heal-crystal-variants
- started: 2026-10-28T00:37:24Z
- waiting_for: none from the owner: sheet picked (owner_pick); an Executor copies it and closes
- result: open

## Subtasks
1. task-336-01-heal-crystal-variants (vault/reference/tasks/task-336-heal-crystal-variants/task-336-01-heal-crystal-variants.md) — art: Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-336-01-heal-crystal-variants]] — step 1, art.
