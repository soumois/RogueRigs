---
id: task-356-01-shop-screen-anchors
description: "Bp: commit the owner's hand-edited WBP_ShopScreen as-is, then rebuild its layout on the Root Canvas with explicit anchors."
parent: task-356-shop-screen-anchors
kind: task
status: todo
---
# Shop screen: Canvas and anchors: WBP_ShopScreen

- goal: (1) FIRST, before any change: read the widget tree of WBP_ShopScreen through Unreal MCP as it is (owner's hand edits, modified and not committed), report it (hierarchy, slot types, anchors, sizes, what is in IndicatorRessource), then commit `Content/RogueRigs/UI/WBP_ShopScreen.uasset` alone, unchanged, message `task-356-01-shop-screen-anchors: owner manual edits of WBP_ShopScreen, committed as-is`. Never restore, revert or overwrite it from git. If the editor holds unsaved changes on it, do not save them before this commit; report. (2) Then rebuild the layout per the global `## Questions` Q1 (and Q5 rule node if it exists): top-level blocks as direct Root Canvas children with explicit anchors, alignment and offsets; sizes from anchors or content, no fixed pixel SizeBox / ImageSize except images at native size; keep the owner's look at 1920 x 1080 as the reference. Place IndicatorRessource top left by anchors; do NOT rebuild its inside (that is task-357). Read the DPI scale rule in Project Settings > User Interface and report it; do not change Config. Keep every name used by graphs and the 342 to 344 graphs: Title, Subtitle, TitleBarL, TitleBarR, ShopCard0..4 (SlotIndex 0..4), CounterBg, OreIcon (flight start, CenterOf), OreBalanceText, RerollButton, RerollIcon, RerollText, RerollDivider, RerollOre, RerollCost, ContinueButton, ContinueText, FlyIcon0..9, InputBlock and their variables, events and functions.
- agent: bp
- depends_on: none
- inputs: wbp-shop-screen (vault/reference/tech/blueprints/ui/wbp-shop-screen.md), wbp-shop-card (vault/reference/tech/blueprints/ui/wbp-shop-card.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), run-shop (vault/reference/gdd/run/run-shop.md), global task-356-shop-screen-anchors `request` and `## Questions`, umg-layout (vault/reference/tech/conventions/umg-layout.md) if Q5 is accepted, `Saved/ShopFix/task-353/` frames (look before, 1920 x 1080)
- expected_output: commit 1 (owner version, unchanged); WBP_ShopScreen compiled (warnings as errors) and saved; commit 2; change summary: the owner's tree as found, then each top-level widget with its anchors min/max, alignment, offsets, size mode, removed containers, DPI rule read.
- done_when: commit 1 exists and contains only WBP_ShopScreen.uasset, made before any edit; compiled and saved; anchors read back for every Root child; widget names, variables and task-342/343/344 graphs still present; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-356-shop-screen-anchors]] — parent global task.
