---
id: task-342-04-shop-ore-flight-capture
description: "Cpp: -game frame capture of a purchase."
parent: task-342-shop-ore-flight
kind: task
status: todo
---
# Shop purchase: ore flies from the balance to the Buy button: -game frame capture of a purchase

- goal: -game capture of a purchase (GrantOre, rr.Run.EndLevel, rr.Run.Stats.Next, rr.Shop.Buy): frames through the 1.2 s sequence, contact sheet sent to the owner, compared with the old instant sold look.
- agent: cpp
- depends_on: task-342-03-shop-ore-flight
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md)
- expected_output: frames and contact sheet in `Saved/ShopOreFlight/` (not committed), report.
- done_when: contact sheet shows icons leaving the balance and reaching the Buy button, the balance dropping, then the sold look; unseen items listed.
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
- [[task-342-shop-ore-flight]] — parent global task.
