---
id: task-192-pickup-spawn-orientation-variants
description: "Global task: Pickup spawn: random rest orientation per Blueprint (none, yaw only, full) and a random mesh variant picked per drop, pool-safe."
parent: tasks-done-index
kind: task
status: done
---
# Pickup spawn orientation and mesh variants

- request: prépare une tâche pour changer la maniére de spawn les collectables, mais ne lance rien (context: ore chunks must land in different positions and use 3 mesh variants; owner: yaw only was meant for the chips, "il n'y a qu'une face intéressante") (owner, 2026-10-15)
- goal: Pickup spawn: random rest orientation per Blueprint (none, yaw only, full) and a random mesh variant picked per drop, pool-safe.
- acceptance: a pickup with mode Full rests in a random orientation, YawOnly turns only around Z, None is unchanged; a new draw on every pool reuse; a variant list picks one mesh per drop; build clean; existing pickup and pool tests pass; docs updated; lint clean; committed.
- gdd_check: ore-blocks, color-code and sys-pickups read 2026-10-15. Today every pickup spawns with zero rotation (RRArenaGrid SpawnActor, pool restores the template); PopYaw is only the ejection direction. No GDD rule on pickup orientation: the per-Blueprint values are provisional defaults to confirm at launch (ore Full with 3 variants via task-189-05, chip YawOnly per owner, nut and heal Full). Not decided: whether orientation is drawn at ejection or at landing (default: at ejection, no tumble in flight).
- confirmed_by_owner: yes (2026-10-15: "Lance la tâche 192 avec les réglages par défaut, puis termine la 189 jusqu'au bout")
- milestone: M1
- next_step: none
- result: Done. RestOrientation set and read back: BP_PickupXP Full, BP_PickupXPBig YawOnly, BP_PickupHeal Full, MeshVariants empty; compiled (no error returned, warning count not exposed) and saved; docs updated; not seen in PIE; commit 2deb17ef

## Subtasks
1. task-192-01-pickup-spawn-orientation-variants — cpp
2. task-192-02-pickup-orientation-defaults — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-192-01-pickup-spawn-orientation-variants]] — step 1, cpp.
- [[task-192-02-pickup-orientation-defaults]] — step 2, bp.
- [[task-189-ore-chunk-look]] — the ore chunk task whose Blueprint step uses this mechanism.
