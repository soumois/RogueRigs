---
id: task-356-03-shop-anchors-capture
description: "Cpp: -game captures of the anchored shop screen at four resolutions (1080p, 720p, 16:10, 21:9), compared with the owner's version."
parent: task-356-shop-screen-anchors
kind: task
status: done
---
# Shop screen: Canvas and anchors: capture

- goal: -game, same procedure as task-348-08, windowed at each resolution of global Q4 (1920 x 1080, 1280 x 720, 1920 x 1200, 2560 x 1080): one shop frame per resolution, plus frames of one purchase (rr.Shop.Buy) at 1920 x 1080; look at every image; one comparison sheet with the four resolutions side by side and the look before from `Saved/ShopFix/task-353/` (before the owner's hand edits; the owner's own version is not rendered, he has seen it in the editor). Sent to the owner with the sheet. No code change unless a capture step needs it; then report it.
- agent: cpp
- depends_on: task-356-02-shop-card-fit
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), sys-run-shop (vault/reference/tech/systems/world/sys-run/sys-run-shop.md), wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), task-348-08-shop-screen-capture (vault/reference/tasks/task-348-shop-screen-redesign/task-348-08-shop-screen-capture.md) (capture procedure), global task-356-shop-screen-anchors `acceptance`
- expected_output: frames and comparison sheet in `Saved/ShopFix/task-356/` (not committed); report of what is seen per resolution and what is unseen.
- done_when: at each of the four resolutions the sheet shows title, subtitle, resource indicator, panel with 5 readable cards, Reroll and Continue fully on screen, no overlap, nothing cut; purchase effects play at 1920 x 1080 (flash, sparks, ore flight from the indicator, count-down, burn, SOLD); or the report lists what differs; unseen items listed.
- editor_state: any (no build)
- token_ceiling: 1.5 M units
- model: opus
- tokens: input 64, cache write 73452, cache read 1281000, output 15655
- usage: turns 28, context start 12k peak 73k, resumes 4 (message x4), 8 min, units 0.30 M
- runs: 1
- milestone: M2
- started: 2026-10-28T21:27:34Z
- waiting_for: none
- result: Captures at 4 resolutions seen: all fit, no overlap (indicator-panel gap 8 px at 720p, 11-17 elsewhere); 2560x1080 cards spaced ~150 px apart (wide empty panel); purchase effects play; capture.js --force-res added c1a0124b (7 launches vs 5 allowed)

## Relations
- [[task-356-shop-screen-anchors]] — parent global task.
