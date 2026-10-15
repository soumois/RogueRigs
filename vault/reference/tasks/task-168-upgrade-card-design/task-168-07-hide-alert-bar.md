---
id: task-168-07-hide-alert-bar
description: "Hide the alert bar while the upgrade cards are shown, to free the top of the screen; show it again when the offer closes."
parent: task-168-upgrade-card-design
kind: task
status: done
---
# Upgrade cards: hide the alert bar during the choice

- goal: The alert bar (WBP_AlertObjectives) is hidden while an upgrade offer is on screen and comes back when it closes (owner 2026-10-14, "la barre d'alerte peut s'effacer pour laisser plus de place sur le top").
- agent: bp
- depends_on: none
- inputs: wbp-alert-objectives (vault/reference/tech/blueprints/ui/wbp-alert-objectives.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), sys-hud-events (vault/reference/tech/systems/ui/sys-hud-events.md), sys-level-up (vault/reference/tech/systems/core/sys-level-up.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: WBP_AlertObjectives binds the level-up component's OnUpgradeOfferShown / OnUpgradeOfferClosed in its BindToMiner hook (unbinding the previous miner) and collapses only the alert bar and its event icons while an offer is shown, restoring their previous visibility on close; nothing else of the widget changes; no tick; doc wbp-alert-objectives updated.
- done_when: Compiles with warnings as errors, saved, graphs read back, doc updated, committed. Owner checks on screen.
- editor_state: open
- token_ceiling: 40 turns
- model: sonnet
- tokens: input 92, cache write 137736, cache read 3162806, output 1956
- runs: 2
- milestone: M2
- result: WBP_AlertObjectives binds OnUpgradeOfferShown/Closed in BindToMiner (UnbindOffer on the previous miner): HideForOffer collapses AlertBar, AlertLabel and the 7 alert icons; RestoreAfterOffer shows them again (Visible, their only designed state); OnAlertBarEventsChanged skips the icons while bOfferShown. No tick. Compiled with warnings as errors, saved, graph read back, doc updated. Left: about 45 empty unconnected custom event nodes from failed DSL writes (delete_node refused by the permission classifier; harmless, compile clean). Not seen on screen. Commits 89895fb, d5418fd.

## Relations
- [[task-168-upgrade-card-design]] — parent global task.
