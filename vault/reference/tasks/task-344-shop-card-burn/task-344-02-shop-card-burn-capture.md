---
id: task-344-02-shop-card-burn-capture
description: "Cpp: -game frame capture of the bought card burning."
parent: task-344-shop-card-burn
kind: task
status: done
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
- model: opus
- tokens: input 36, cache write 54710, cache read 546186, output 7581
- usage: turns 16, context start 11k peak 55k, resumes 2 (message x2), 3 min, units 0.16 M
- runs: 1
- milestone: M2
- started: 2026-10-28T15:35:23Z
- waiting_for: none
- result: -game run (rr.Shop.Buy 0, 32 frames): only the bought card burns (~0.4-0.5 s, face fades, overlay fits frame), then Sold. No refunds.; others untouched; no icon flight; no runtime errors. Flames are the material's fixed yellow (Tint multiplies Tex before the fire lerp, same as level-up pick), tier shows only in the tinted frame. Unseen: burn start gap 0.03-0.64 s, other tiers, real click, side-by-side with level-up pick. Saved/ShopCardBurn/contact_sheet.png

## Relations
- [[task-344-shop-card-burn]] — parent global task.
