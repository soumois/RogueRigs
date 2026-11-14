---
id: wbp-ui-bolted-bar
description: "WBP_UI_BoltedBarV and WBP_UI_BoltedBarH, shared divider bars with a round bolt at each end, and their textures; open to place a divider on a framed screen."
parent: blueprints-ui-index
kind: doc
---
# WBP_UI_BoltedBarV / WBP_UI_BoltedBarH

- asset_path: /Game/RogueRigs/UI/Shared/WBP_UI_BoltedBarV, /Game/RogueRigs/UI/Shared/WBP_UI_BoltedBarH (task-545, 8daaaa31)
- parent_class: UserWidget (no C++ base)
- purpose: shared divider piece for the framed screens (pause, level cleared, shop): a bar stretched along its slot with unstretched ends and a round bolt on each end, drawn on the frame bars or on another divider ([[pause-menu]] "Grid and bolts").
- components (Root CanvasPanel; all images HitTestInvisible):

| Widget | V variant | H variant |
|---|---|---|
| Bar (Image) | anchors (0.5,0)-(0.5,1), align (0.5,0), width 16 | anchors (0,0.5)-(1,0.5), align (0,0.5), height 16 |
| Bar brush | T_UI_BarV, Box, image 16x292, margins T/B 0.05 | T_UI_BarH, Box, image 292x16, margins L/R 0.05 |
| BoltStart (Image) | point (0.5,0), align (0.5,0.5), 34x34 | point (0,0.5), align (0.5,0.5), 34x34 |
| BoltEnd (Image) | point (0.5,1), align (0.5,0.5), 34x34 | point (1,0.5), align (0.5,0.5), 34x34 |

- placing it: give its slot the span from bolt centre to bolt centre (V: x point anchor, y from frame top to frame bottom with slate insets that land on the frame bar centres; H: the mirror) and a cross size of 34. A junction is two bars meeting at one point: their bolts overlap exactly. Bolts are a separate image, never stretched; sizes are slate units, like the frame Box margins.
- exposed_defaults: none.
- events: none.
- logic_in_bp: none.
- assets (keep, Art/Source/ui-frame-pieces/): T_UI_Bolt.png 64x64, the octagonal teal nut cut from the level-end divider sprite (mirror-masked); T_UI_BarH.png 768x42 and T_UI_BarV.png 42x768, the divider bars recoloured olive teal-steel with edge rust streaks by `scripts/make_bars.py` (BarV is BarH turned 90 degrees). Imported to /Game/RogueRigs/UI/Shared/ (TEXTUREGROUP_UI, no mips). No Kie.ai credits. Style-guide: bolt pass; bars reviewed in 5 rounds, the last correction not re-reviewed (task-545-01 result).
- used_by: [[wbp-pause-menu]]; [[wbp-intermission]] (task-546: DividerH frame side to side, DividerV from the mid junction to the bottom bar); [[wbp-options]] (task-551: GraphicsDividerV, frame top to bottom, pause offsets).
- owner_agent: agent-bp

## Relations
- [[blueprints-ui-index]] — parent index.
- [[wbp-pause-menu]] — first screen that places it.
- [[wbp-intermission]] — the level cleared screen that places it.
- [[wbp-options]] — the options Graphics tab divider.
- [[pause-menu]] — the grid and bolts design it builds.
