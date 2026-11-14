---
id: task-548-01-frame-art
description: "Art: New shop panel frame image for its ratio, 9-slice ready, per Q14"
parent: task-548-shop-frame-layout
kind: task
status: done
---
# Spare Parts shop: own panel frame, layout as owner image 8: art

- goal: New shop panel frame image for its ratio, 9-slice ready, per Q14
- agent: art
- depends_on: none
- inputs: run-shop (vault/reference/gdd/run/run-shop.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), Saved/Scratch/owner-ui-batch-2026-11-13/7.webp, Saved/Scratch/owner-ui-batch-2026-11-13/8.webp
- expected_output: see done_when
- done_when: A4 holds; margins of the 9-slice recorded.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 14, cache write 48908, cache read 85678, output 9208
- usage: turns 7, context start 7k peak 27k, resumes 0, 2 min, units 0.12 M, USD 0.45
- runs: 1
- milestone: M2
- started: 2026-11-14T04:19:49Z
- waiting_for: none
- result: Kie.ai image-to-image, 2 rounds x 1 variant, 12 credits (round 1 temporary, Saved/Scratch/task-548). Kept round 2: Art/Source/shop-panel-frame/20261114-052340-shop-panel-frame-1.png with log, prompts 01-02, script scripts/cut_frame.py, sprites T_Shop_PanelFrame.png 484x484 (9-slice margins 0.4339 all sides = corner 210 px, 64 px plain bar piece, dark interior #0E1014), T_Shop_PanelRivet.png 32x32, T_Shop_PanelClamp.png 82x79 (right clamp = mirror); rivets and clamps are separate sprites so they never stretch. Owner image 8 kept as reference/owner-shop-target-2026-11-13.webp. Style-guide: round 1 fail A6 (no rivets, patch, clamps); round 2 pass A1 A2 A7 A14 colour readability, fail A6 (patch plates not mismatched, no white plate; owner target has no white either); kept the best per Q14. Vault listing done in task-548-04-doc.

## Relations
- [[task-548-shop-frame-layout]] — parent global task.
