---
id: task-246-02-card-pick-destination
description: "Create the flight overlay widget that outlives the card screen and wire WBP_UpgradeChoice: on pick, fly the icon from the card to the destination (targeted weapon slot, each owned slot for all weap..."
parent: task-246-card-pick-destination
kind: task
status: done
---
# 246-02 bp

- goal: Create the flight overlay widget that outlives the card screen and wire WBP_UpgradeChoice: on pick, fly the icon from the card to the destination (targeted weapon slot, each owned slot for all weapons, new slot for a new weapon, XP bar for a player stat) while the other cards dissolve; the click still calls the level-up component and the game resumes as before; update the doc nodes.
- agent: bp
- depends_on: task-246-01-card-pick-destination
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), wbp-weapons (vault/reference/tech/blueprints/ui/wbp-weapons.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), wbp-xp-status (vault/reference/tech/blueprints/ui/wbp-xp-status.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), sys-hud (vault/reference/tech/systems/ui/sys-hud.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: overlay widget, updated widgets, doc nodes
- done_when: compiles with warnings as errors, saved, flight wiring read back; doc nodes updated
- editor_state: open
- token_ceiling: 70000 (provisional)
- model: open
- tokens: open
- runs: 2
- milestone: M2
- result: WBP_UpgradeChoice: StartPick/PickStep/FlyStep/PickReset: click blocks buttons (HitTestInvisible), 0.55 s pick in the paused game then ChooseCard and CloseCards; icon copies FlyIcon0..3 fly 0.5 s (arc 140 px, shrink 0.55) to the weapon slot (targeted), each owned slot (all weapons), XPBar (player stat), first free slot (new weapon); BurnOverlay0..2 with MI_CardBurn on the discarded cards, chosen card fades; destinations found by GetAllWidgetsOfClass on WBP_Weapons and WBP_XPStatus (Frame0..3 made variables in WBP_Weapons, metadata only); commits dc74f0be, ab0fb86e; compiled warnings as errors, saved; nothing seen (capture in 246-03). 2 runs.

## Relations
- [[task-246-card-pick-destination]] — parent global task.
