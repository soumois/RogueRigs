---
id: task-342-04-shop-ore-flight-capture
description: "Cpp: -game frame capture of a purchase."
parent: task-342-shop-ore-flight
kind: task
status: done
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
- model: opus
- tokens: input 26, cache write 58446, cache read 523324, output 7041
- usage: turns 13, context start 11k peak 58k, resumes 0, 3 min, units 0.16 M
- runs: 1
- milestone: M2
- started: 2026-10-28T06:51:49Z
- waiting_for: none
- result: -game capture, 18 frames 0.1 s apart (rr.Shop.Buy 0, price 25, 3 icons): icons leave the balance, arc up and land on the Buy button top edge, balance 150 to 134 to 125, then sold look at +0.67 s. Sequence 0.67 s (task-343/344 effects not built yet). Not seen: input block, real click, step-by-step count down. Saved/ShopOreFlight/contact_sheet.png, crops_00_07.png

## Relations
- [[task-342-shop-ore-flight]] — parent global task.
