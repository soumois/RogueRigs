---
id: task-548-02-shop-layout
description: "Bp: Apply the new frame and the image 8 layout to the shop screen"
parent: task-548-shop-frame-layout
kind: task
status: done
---
# Spare Parts shop: own panel frame, layout as owner image 8: bp

- goal: Apply the new frame and the image 8 layout to the shop screen
- agent: bp
- depends_on: task-548-01-frame-art
- inputs: run-shop (vault/reference/gdd/run/run-shop.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Saved/Scratch/owner-ui-batch-2026-11-13/8.webp
- expected_output: see done_when
- done_when: A1 to A3 hold.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-548
- usage: included in the executor run of task-548
- runs: 1
- milestone: M2
- started: 2026-11-14T04:27:39Z
- waiting_for: none
- result: cbb5f55b. WBP_ShopScreen: ShopPanel brush T_Shop_PanelFrame (Box, margins 0.4339, ImageSize 384; drawn bars about 35-39 px at 1080p) instead of T_Shop_Panel; anchors (0.024,0.136)-(0.976,0.776); padding 72/66/72/76 (Border and BorderSlot); card slots padding L/R 14. New Root child PanelDeco (CanvasPanel, index 2, same anchors, HitTestInvisible): RivetT0..6 / RivetB0..6 (T_Shop_PanelRivet 25.4, point anchors x 0.2..0.8, y +28.6 / -31.7), RivetL0..1 / RivetR0..1 (y 0.3 / 0.7, x +32.5 / -33.7), ClampL / ClampR (T_Shop_PanelClamp 65.1 x 62.7 at (0,0.5) / (1,0.5), right one render scale X -1). Textures imported to /Game/RogueRigs/UI/Shop (UI group, NoMipmaps, TC_EditorIcon, NeverStream, sRGB). WBP_ShopCard, texts, graphs, Reroll and Continue untouched; no URRScreenEntrance (Q13). Seen at 1920x1080 (Saved/Captures/task548_b1080/shop_00.png): frame corners and clamps round, rivets on the bars, uniform dark interior, cards 278 x 400 with margin to the frame, SOLD and NEED 15 MORE (task548_a1080) unchanged. Note: changing the brush ImageSize 426 to 384 did not change the drawn frame (same pixels), cause not investigated. Cards are a little smaller and further apart than image 8 (card ratio kept, no stretch).

## Relations
- [[task-548-shop-frame-layout]] — parent global task.
