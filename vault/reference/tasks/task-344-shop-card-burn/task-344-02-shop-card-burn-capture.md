---
id: task-344-02-shop-card-burn-capture
description: "Cpp: -game frame capture of the bought card burning."
parent: task-344-shop-card-burn
kind: task
status: todo
---
# Shop purchase: the bought card burns like a level-up pick: -game frame capture of the bought card burning

- goal: -game frames of a purchase through the whole sequence, contact sheet sent to the owner, compared with the level-up pick burn.
- agent: cpp
- depends_on: task-344-01-shop-card-burn
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md)
- expected_output: frames and contact sheet in `Saved/ShopCardBurn/` (not committed), report.
- done_when: contact sheet shows the tinted burn on the bought card only, then the sold look; unseen items listed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-344-shop-card-burn]] — parent global task.
