---
id: task-343-02-shop-buy-juice-capture
description: "Cpp: -game frame capture of the Buy button reaction."
parent: task-343-shop-buy-juice
kind: task
status: done
---
# Shop purchase: juicy Buy button click: -game frame capture of the Buy button reaction

- goal: -game frames of a purchase through the first 0.4 s, contact sheet sent to the owner.
- agent: cpp
- depends_on: task-343-01-shop-buy-juice
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md)
- expected_output: frames and contact sheet in `Saved/ShopBuyJuice/` (not committed), report.
- done_when: contact sheet shows the punch, flash and sparks; unseen items listed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: opus
- tokens: input 50, cache write 52791, cache read 761103, output 10500
- usage: turns 23, context start 11k peak 53k, resumes 2 (message x2), 4 min, units 0.19 M
- runs: 1
- milestone: M2
- started: 2026-10-28T07:18:53Z
- waiting_for: none
- result: Two -game runs via rr.Shop.Buy; frames ~0.1 s apart (screenshot stalls): flash, 8 tier sparks and overshoot visible; 0.88 low point and 1.12 peak not resolved; greyed button and real click not tested; Saved/ShopBuyJuice/contact_sheet.png

## Relations
- [[task-343-shop-buy-juice]] — parent global task.
