---
id: task-485-07-upgrade-reroll
description: "Bp: Same hover on the WBP_UpgradeChoice Reroll button; the cards keep their own hover."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Same hover on the WBP_UpgradeChoice Reroll button; the cards keep their own hover.
- agent: bp
- depends_on: task-485-06-shop-card
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md)
- expected_output: see done_when
- done_when: Reroll shows the hover of ui-button-hover after the deal, none while disabled; card hover, deal, rarity suspense and pick animation unchanged; compiled with warnings as errors, saved.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T03:02:52Z
- waiting_for: none
- result: Done (4667f271). URRHUDPanelWidget gains HoverButtonNames (EditDefaultsOnly) applied in NativeOnInitialized; WBP_UpgradeChoice default = RerollButton, compiled with warnings as errors and saved. The Reroll is an untextured grey rounded box with a white label (not a cream plate): first capture with the display +25% made the plate near white and the label hard to read, one correction applied (linear tint x1.25 on untextured plates); recapture: lighter, grown, label readable (Saved/Captures/task485_resA/c_lu.png). Card hover unchanged (task485_a1080/lu.png). Disabled Reroll during the deal: same IsEnabled gate as the tested disabled Buy, not captured separately. Closed after 08 and 09 (a locked usage-snapshot.json made the first close fail).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
