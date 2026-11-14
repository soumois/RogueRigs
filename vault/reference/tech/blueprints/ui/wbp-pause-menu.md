---
id: wbp-pause-menu
description: "WBP_PauseMenu, the pause screen: miner stats, bonuses, up to four weapon blocks, Resume and Exit; anchors table and the OnStatsRefreshed graph."
parent: blueprints-ui-index
kind: doc
---
# WBP_PauseMenu

- asset_path: /Game/RogueRigs/UI/WBP_PauseMenu
- parent_class: URRPauseMenuWidget (/Script/RogueRigs.RRPauseMenuWidget, [[sys-pause-menu]])
- purpose: shows the miner stats, global weapon bonuses and owned weapons while the game is paused; created by the player controller at Z 40. Mockup 1672x941; every top-level widget is a direct child of Root (CanvasPanel); anchors are fractions.
- components (anchors (minX,minY)-(maxX,maxY); textures: T_Shop_TitleBar, T_Shop_ButtonContinue, T_LevelEnd_Frame; dividers are the shared [[wbp-ui-bolted-bar]] pieces since task-545):

| Widget | Anchors | Notes |
|---|---|---|
| Backdrop (Border) | (0,0)-(1,1) | black alpha 0.94 |
| TitleBarL (Image) | point (0.5,0.112), align (1,0.5) | offsets (-162,0,475,38); brush Box 276x38, margins L 0.14, R 0.18 (tip and bolt unstretched; task-545, owner image 10) |
| TitleBarR (Image) | point (0.5,0.112), align (0,0.5) | offsets (162,0,475,38), same brush, render scale X -1 |
| Title | point (0.5,0.112), align (0.5,0.5) | "PAUSED", auto size, Roboto Bold 56, outline 3 |
| Frame (Border) | (0.1585,0.1753)-(0.8421,0.7992) | T_LevelEnd_Frame, Box 942x458, margins L/R 0.0955, T/B 0.1878 |
| MinerDividerV (WBP_UI_BoltedBarV) | x point 0.3966, y 0.1753-0.7992 | offsets (0,20,34,25), align (0.5,0): bolts on the top and bottom frame bars; always visible |
| WeaponDividerV (WBP_UI_BoltedBarV) | x point 0.6065, y 0.1753-0.7992 | same offsets; visible when more than 1 weapon |
| WeaponDividerH (CanvasPanel) | (0,0)-(1,1) | SelfHitTestInvisible group, visible when more than 2 weapons; holds the two below |
| WeaponDividerHLeft (WBP_UI_BoltedBarH) | x 0.3966-0.6065, y point 0.490 | offsets (0,0,0,34), align (0,0.5) |
| WeaponDividerHRight (WBP_UI_BoltedBarH) | x 0.6065-0.8421, y point 0.490 | offsets (0,0,30,34): end bolt on the right frame side |
| MinerTitle | point (0.283,0.2604) | size 28 |
| MinerLine | (0.2033,0.2965)-(0.369,0.2965) | |
| MinerLabels | (0.2033,0.3103)-(0.29,0.5345) | size 22 |
| MinerValues | (0.24,0.3103)-(0.369,0.5345) | size 22, right |
| BonusTitle | point (0.2033,0.5664) | size 21 |
| BonusLine | (0.2033,0.592)-(0.369,0.592) | |
| BonusLabels | (0.2033,0.6015)-(0.29,0.757) | size 18 |
| BonusValues | (0.24,0.6015)-(0.369,0.757) | size 18, right |
| WeaponBlock1 (CanvasPanel) | (0.4132,0.2125)-(0.5868,0.4782) | holds the four widgets below |
| WeaponBlock2 | (0.6277,0.2125)-(0.8011,0.4782) | same |
| WeaponBlock3 | (0.4132,0.4952)-(0.5868,0.7609) | same |
| WeaponBlock4 | (0.6277,0.4952)-(0.8011,0.7609) | same |
| WeaponName<i> | point (0.5,0.112) | size 20 |
| WeaponLine<i> | (0.017,0.192)-(0.99,0.192) | |
| WeaponLabels<i> | (0.048,0.224)-(0.60,0.97) | size 17 |
| WeaponValues<i> | (0.40,0.224)-(0.945,0.97) | size 17, right |
| ResumeButton | point (0.3013,0.8619), align (0.5,0.5) | size 380x100 (task-510-09; was 440x108 at 0.3756); T_Shop_ButtonContinue Box margin L/R 0.155, image 640x163, tints 1/1.3/0.7/0.35 |
| OptionsButton | point (0.5003,0.8619), align (0.5,0.5) | task-510-09: same style, label OptionsText "Options" (ResumeText font); opens [[wbp-options]] |
| ExitButton | point (0.6993,0.8619), align (0.5,0.5) | same style (was 0.6274) |

- button labels: ResumeText and ExitText, Roboto Bold 36, colour (0.05,0.04,0.03).
- exposed_defaults: none on the widget. [[bp-rr-player-controller]] `PauseMenuWidgetClass` = /Game/RogueRigs/UI/WBP_PauseMenu.WBP_PauseMenu_C.
- events: `OnStatsRefreshed(NewStats)` only. It breaks the struct, sets MinerValues and BonusValues (newline-joined `*Text`), and for each block i = 1..4 sets visibility (SelfHitTestInvisible if Length(Weapons) > i-1, else Collapsed) and fills the name and the seven value texts; it also sets the two weapon dividers' visibility. Resume and Exit are bound by the C++ base.
- logic_in_bp: display only.
- assets: the textures above; font Roboto Bold (no stencil font asset exists, so the title is plain Roboto Bold).
- verified (task-424-05): build clean, -game captures `Saved/Captures/pmfix_*/pm_00.png` at four resolutions with one weapon. Not verified: two and four weapon layouts.
- owner_agent: agent-bp

- grid_and_bolts (task-545, 8daaaa31): the three Image dividers were replaced by the shared bolted bars above, same names; the graph's two divider SetVisibility calls were reconnected to new GetWeaponDividerV/H nodes after the swap. Verified -game at the four resolutions with 3 weapons and at 1920x1080 with 2 weapons (Saved/Captures/t545_c_sheet.png, t545_c_bolt_crops.png). Owner target: Art/Source/pause-menu/reference/owner-pause-target-2026-11-13.webp.
- options (task-510-09, d4f9f754): OptionsButton bound by the C++ base (click = URROptionsWidget::OpenOptions over the paused arena, plate hover). Verified -game at four resolutions (Saved/Captures/t500_c_*/pm_00.png).
- button_hover (task-485-01, 01a90440): ResumeButton and ExitButton attached in URRPauseMenuWidget::NativeOnInitialized: at runtime the Normal brush draws through M_UI_ButtonPlate and the Hovered brush becomes the Normal one (the 1.3 hovered tint above is no longer drawn); the plate brightens and grows on mouse hover. Asset unchanged. Verified: -game at the four resolutions (Saved/Captures/task485_slice/crops.png, task485_resA/sheet_pm.png, task485_resA1200). See [[sys-button-hover]]; values of [[ui-button-hover]] (scale 1.05, about +25 %, 0.1 s real time).

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-pause-menu]] — the C++ base, stats struct and text formats.
- [[bp-rr-player-controller]] — creates it through PauseMenuWidgetClass.
- [[pause-menu]] — the design.
- [[sys-button-hover]] — shared mouse hover of its buttons.
- [[wbp-options]] — the options screen its Options button opens.
- [[wbp-ui-bolted-bar]] — the shared divider pieces of its grid.
