---
id: wbp-shop-screen-indicator
description: "IndicatorRessource of WBP_ShopScreen, the ore counter: widget tree, anchors of CounterBg, OreIcon and OreBalanceText, icon-font ratio, capture check. Open to edit the counter."
parent: blueprints-ui-index
kind: doc
---
# WBP_ShopScreen: IndicatorRessource (ore counter)

- asset_path: /Game/RogueRigs/UI/WBP_ShopScreen (widget IndicatorRessource, task-357-01, commit e6cbb941; values provisional, owner 2026-10-28, Q3 option a of [[task-357-shop-resource-indicator]])
- parent_class: n/a (part of [[wbp-shop-screen]])
- purpose: shows the ore balance on the plate at the top-left of the shop; icon and value sit on one line centred on the plate's vertical axis, anchored on the panel centre so both follow the DPI scale together.
- components: IndicatorRessource (SizeBox 340 x 152, unchanged, Root child anchor (0,0)) > CounterOverlay > CounterBg (plate image) + CounterCanvas (CanvasPanel, was CanvasPanel_408) > OreIcon + OreBalanceText. The CounterRow HorizontalBox is removed. Variables kept: OreIcon, OreBalanceText, CounterBg.
- anchors (slate units, DPI-scaled):
  - CounterBg: overlay slot padding 0, fill both.
  - CounterCanvas: overlay slot fill both.
  - OreIcon: anchors (0.5, 0.5), alignment (1, 0.5), offset x 6, y 21, auto size, brush 60 x 60 (T_Icon_Ore).
  - OreBalanceText: anchors (0.5, 0.5), alignment (0, 0.5), offset x 14, y 21, auto size; font Bold 44, outline 2.
  - gap between icon and text: 8 px (14 - 6).
- icon_size_rule: the icon size does NOT follow the font automatically. The 60 / 44 ratio is kept by hand: to change the font, set the brush size by hand to font x 1.36. Both scale together with the DPI since they share one canvas.
- logic_in_bp: unchanged. OreIcon is still the start point of the ore flight and OreBalanceText is written by RefreshShop and the count-down ([[wbp-shop-screen]]); FlyIcon0..9 and the graphs of tasks 342 to 344 untouched.
- verified: task-357-02 captures (-game windowed, Saved/ShopFix/task-357/, not committed) at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 with balances 0, 83 and 108: icon and value on one line inside the plate everywhere. The icon centre is 1.5 to 3.5 px lower than the digit centres, and both are 1 to 4 px above the plate's inner centre. At 1080p: icon centre at plate centre -38.5 px, digits start at +4.5 px, gap 21 px, right margin 18 px with 3 digits. The group is not horizontally centred (option a). The count-down was seen; flight and count-down only at 1920 x 1080.
- unseen: a 4-digit balance would likely touch the right edge (inference, not captured); the exact frame where the ore flight starts; hover states; fullscreen.

## Relations
- [[wbp-shop-screen]] — the Blueprint that hosts the indicator.
- [[blueprints-ui-index]] — parent index.
- [[task-357-shop-resource-indicator]] — the task that rebuilt the interior and decided the alignment.
