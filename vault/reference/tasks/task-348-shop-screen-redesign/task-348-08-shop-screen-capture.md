---
id: task-348-08-shop-screen-capture
description: "Cpp: -game capture of the restyled shop screen and one purchase, compared with the old screenshot and the mockup."
parent: task-348-shop-screen-redesign
kind: task
status: done
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
- model: opus
- tokens: input 34, cache write 45511, cache read 460478, output 7537
- usage: turns 16, context start 12k peak 46k, resumes 1 (message x1), 2 min, units 0.14 M
- runs: 1
- milestone: M2
- started: 2026-10-28T16:57:37Z
- waiting_for: none
- result: -game 1920x1080 capture ran with owner's editor left open (exit 0, 24 frames) in Saved/ShopRedesign/ (comparison_sheet.png, srd_00..23.png, crops_03_11.png, contact_sheet.png), not committed. Seen: counter panel with pipes top left, riveted panel with 5 cards, tier tabs, sold card burnt with wrench, Reroll/Continue plates, purchase fx play (ore flight, balance 45). DEFECTS: (1) title top clipped at y=0; (2) Continue plate cut at screen bottom (y 995-1080); (3) NEED 15 MORE overflows Buy plate (same size as BUY, mockup uses smaller text on grey plate); (4) counter overlaps panel top-left corner and first rivet; (5) minor second frame line beside slot 0, tab text cramped. Unseen: epic/uncommon/legendary tabs, hover/pressed, reroll disabled, burn at fine steps.

## Relations
- [[task-348-shop-screen-redesign]] — parent global task.
