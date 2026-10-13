---
id: task-109-01-weapon-slot-icons
description: "Active weapon slots show the weapon icon large without its name, like the reference game; the icon tool keeps the icon aspect ratio."
parent: task-109-weapon-slot-icons
kind: task
status: done
---
# Weapon icon larger, no name on active slots

- goal: Active weapon slots show the weapon icon large without its name, like the reference game; the icon tool keeps the icon aspect ratio.
- agent: cpp
- depends_on: none
- inputs: asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), placeholder-weapon-icons (vault/reference/gdd/art-audio/placeholder-weapon-icons.md), wbp-weapons (vault/reference/tech/blueprints/wbp-weapons.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: owner capture 2026-10-12: on an active slot the name is useless; the icon should fill the slot like the reference game (a big weapon silhouette, a status bar under it). Step 1 (this subtask, a Node tool change, no Unreal): add an option to Tools/art/ui_icon_prepare.js (for example --keep-aspect) that trims to the content and keeps the icon aspect ratio (no square canvas: longest side 256, margin 4 %), regenerate the three WEAPON icons (burst_rifle, shotgun, turret_launcher) from vault/assets/placeholder-icon-*.png into Art/UI/Icons/ (the padlock stays square), check each output by script (size, alpha, corners transparent, no magenta residue, content not clipped) and report the numbers; document the option in asset-pipeline. Step 2 (task-109-02, bp) reimports them and changes WBP_Weapons.
- done_when: three trimmed weapon icons with their aspect ratio kept, checked by script; tool option documented; lint clean; committed.
- editor_state: any
- token_ceiling: 30000
- model: opus
- tokens: input 24, cache write 30499, cache read 254508, output 7935
- runs: 1
- milestone: M2
- result: --keep-aspect option in ui_icon_prepare.js; three weapon icons trimmed 256 x 105 / 98 / 170 with a 10 px margin, checks pass (0 pink pixels, corners transparent); padlock unchanged. Commit 28f227e.

## Relations
- [[task-109-weapon-slot-icons]] — parent global task.
