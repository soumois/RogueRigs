---
id: task-398-01-shop-backdrop-70
description: "Bp: WBP_ShopScreen: find why no dark layer shows, black backdrop at 70 %; report the WBP_UpgradeChoice backdrop state without editing it"
parent: task-398-upgrade-screen-backdrop
kind: task
status: todo
---
# Spare Parts screen: black backdrop at 70 %: bp

- goal: In WBP_ShopScreen (Spare Parts), read and report the real state: Backdrop Border (visibility, brush colour and alpha, render opacity, anchors and offsets, z-order in Root), any other border or image covering or replacing it, and the owner hand edit of 2026-10-29 (d1c7766d); say why nothing dark shows behind the screen in game (also check the shop widget Z-order against the HUD and whether the controller adds it as expected). Then make one visible full-screen black layer at alpha 0.70 behind every other child (reuse the existing Backdrop: brush black, alpha 0.70, render opacity 1, not collapsed, first child of Root, anchors 0,0 to 1,1, offsets 0; add a Border only if none exists). Nothing else changes: cards, buttons, bindings, number sources, task-396 labels. Then, read only, check WBP_UpgradeChoice: UpgradeScreen and Backdrop visibility, brush colour and alpha, render opacity, z-order, any graph that changes them at runtime; report whether a black 0.7 layer is actually rendered and why not if not. Do not edit WBP_UpgradeChoice or WBP_Intermission.
- agent: bp
- depends_on: task-397-01-extractor-out-of-sight
- inputs: agent-bp (vault/reference/agents/agent-bp.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), run-shop (vault/reference/gdd/run/run-shop.md), sys-run-transition (vault/reference/tech/systems/world/sys-run/sys-run-transition.md), commit d1c7766d (owner hand edit of WBP_ShopScreen and WBP_ShopCard), global request and gdd_check: vault/reference/tasks/task-398-upgrade-screen-backdrop.md
- expected_output: WBP_ShopScreen saved, compiled with warnings as errors; commit `task-398-01-shop-backdrop-70: ...`; change summary (state before, cause, values after, WBP_UpgradeChoice finding) for the doc subtask and the main session.
- done_when: WBP_ShopScreen Backdrop read back: black, alpha 0.70, render opacity 1, full screen, first (bottom) child of Root, not collapsed; cause reported; no other property or graph changed; WBP_UpgradeChoice state reported and the package unchanged (git status clean for it); compiled with warnings as errors, no error; saved; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-398-upgrade-screen-backdrop]] — parent global task.
