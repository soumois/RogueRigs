---
id: task-218-shield-emblem
description: "Put the Kessler-Voss emblem on a dark rounded plate as a decal at the front centre of the validated shield chaser riot shield."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Shield chaser riot shield: emblem decal

- request: "Voilà le logo (mais pas détouré je pense). Sur le bouclier, pour qu'il soit bien visible et qu'il ressorte, tu peux poser l'image sur un rectangle de coin arrondi de la couleur de l'image 2" (owner 2026-10-17)
- goal: the emblem (variation 4) cut out from its white background, laid on a rounded rectangle of #1A1F25, as one textured decal on the front centre of `SM_ChaserShield`.
- acceptance: shield FBX re-exported with the decal as an extra slot, emblem texture with alpha, renders sent to the owner.
- gdd_check: [[art-prompts-shield]], [[kessler-voss-logo]] (stickers and signs are textured decals, [[art-style-guide]] A1 and A14), [[blender-asset-route]] (decal = copy of covered faces offset 0.25 cm, one planar UV, Masked in Unreal). Plate colour sampled from the owner's swatch: #1A1F25 (mean 26, 31, 37). The emblem charcoal (#181820) is nearly the plate colour, so the rock outline merges into the plate; only the yellow KV and facets stand out (reported to the owner). Plate size and corner radius: provisional, set by the Orchestrator in the subtask.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-218-01-shield-emblem-decal
- result: open

## Subtasks
- [[task-218-01-shield-emblem-decal]] — 3D agent: emblem cut-out, plate texture, decal on the shield, export, renders.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[art-prompts-shield]] — the shield.
- [[kessler-voss-logo]] — the emblem.
