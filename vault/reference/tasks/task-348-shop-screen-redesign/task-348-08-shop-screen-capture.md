---
id: task-348-08-shop-screen-capture
description: "Cpp: -game capture of the restyled shop screen and one purchase, compared with the old screenshot and the mockup."
parent: task-348-shop-screen-redesign
kind: task
status: in-progress
---
# Shop screen redesign: capture

- goal: -game at 1920 x 1080: one frame of the shop with affordable, short and sold cards, and frames of one purchase (rr.Shop.Buy) through the 342 to 344 sequence; a side-by-side with `Art/Source/shop-screen/reference/shop-screen-2026-10-28.png` and the mockup, sent to the owner.
- agent: cpp
- depends_on: task-348-07-shop-screen-restyle
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), `Art/Source/shop-screen/reference/chatgpt-shop-mockup-2026-10-28.webp`
- expected_output: frames and comparison sheet in `Saved/ShopRedesign/` (not committed), report with the differences seen.
- done_when: the sheet shows every element of the global acceptance or lists what differs; unseen items listed.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T16:57:37Z
- waiting_for: none
- result: open

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
