---
id: wbp-run-lost
description: "WBP_RunLost, the run-lost screen (death or missed shuttle): anchors table for owner retouches, appearance graph with skip, Retry and Main Menu."
parent: blueprints-ui-index
kind: doc
---
# WBP_RunLost

- asset_path: /Game/RogueRigs/UI/WBP_RunLost (task-484-04, commit 0ec081cd)
- parent_class: URRRunLostWidget (/Script/RogueRigs.RRRunLostWidget, [[sys-run-lost]])
- purpose: the screen of [[death-screen]]; created by [[bp-rr-player-controller]] (RunLostWidgetClass, Z 45) on OnRunLost, game paused, HUD hidden. Duplicated from [[wbp-pause-menu]] and reparented; layout from the validated mockup of [[art-prompts-death-screen-mockup]] (1672x941). Every top-level widget is a direct child of Root (CanvasPanel); anchors are fractions; sizes in slate units.
- components (textures reused: T_Shop_TitleBar, T_LevelEnd_Frame, T_Shop_ButtonContinue; no new sprite):

| Widget | Anchors | Notes |
|---|---|---|
| Backdrop (Border) | (0,0)-(1,1) | black alpha 0.75 |
| TitleBarL / TitleBarR (Image) | point (0.5,0.202), align (1,0.5) / (0,0.5) | 210 x 29; X set at show to -/+ (title width / 2 + 24); R mirrored |
| Title | point (0.5,0.202), align (0.5,0.5) | auto size, Roboto Bold 60, outline 3, rust red (0.6,0.055,0.035) |
| Subtitle | point (0.5,0.278), align (0.5,0.5) | auto size, 24, outline 2, grey 0.85 |
| SubLineL / SubLineR (Image) | point (0.5,0.278), align (1,0.5) / (0,0.5) | 130 x 2, grey alpha 0.6; X set at show to -/+ (subtitle width / 2 + 16) |
| RecapBlock (CanvasPanel) | (0.25,0.343)-(0.749,0.667) | holds the widgets below, anchors relative to it |
| Frame (Border) | (0,0)-(1,1) | T_LevelEnd_Frame Box, margins L/R 0.0955, T/B 0.1878 |
| RowLine1 / RowLine2 | (0.095,0.384)-(0.907,0.384) / y 0.616 | height 2 |
| LevelLabel, RobotsLabel, DuskiteLabel | point (0.126, 0.266 / 0.505 / 0.738), align (0,0.5) | 30, outline 2: "Level reached", "Robots destroyed", "Duskite lost" |
| LevelValue, RobotsValue, DuskiteValue | point (0.874, same y), align (1,0.5) | 30, right |
| RetryButton | point (0.367,0.765), align (0.5,0.5) | 440 x 108, T_Shop_ButtonContinue as in WBP_PauseMenu; RetryText "Retry" 30 |
| MainMenuButton | point (0.634,0.765), align (0.5,0.5) | same; MainMenuText "Main Menu" 30 |
| SkipButton | (0,0)-(1,1) | invisible (NoDrawType, alpha 0), not focusable, on top during the appearance |

- exposed_defaults: none on the widget. Variables Elapsed (float), Animating (bool).
- events: OnRunLostShown(NewRecap): sets Title, Subtitle and the three values from the recap texts, ForceLayoutPrepass, places the rails and lines from the desired widths, SkipButton Visible, Elapsed 0, Animating true, ApplyAppearance(0). Tick: while Animating, Elapsed += delta; at 1.4 s or on WasInputKeyJustPressed(AnyKey) of the owning player calls RequestSkipAppearance, else ApplyAppearance(Elapsed). OnClicked(SkipButton) calls RequestSkipAppearance. OnSkipAppearanceRequested: Animating false, ApplyAppearance(1.4), SkipButton Collapsed, FinishAppearance (focus on RetryButton). Construct: SetShouldPerformFullTickWhenPaused(true) on the owning controller, so the paused camera keeps applying the desaturation. Retry and Main Menu clicks are bound by the C++ base.
- functions: ApplyAppearance(T), values of [[death-screen]] Appearance: Backdrop opacity and SetLevelDesaturation over 0 to 0.6 s; Title, rails, Subtitle and lines opacity over 0.6 to 0.85 s, Title scale 1.4 to 1 and a shake (sin(T x 90) x 8 px, fading to 0 by 1.0 s); RecapBlock opacity and slide up 40 px over 0.85 to 1.15 s; both buttons opacity over 1.15 to 1.35 s.
- logic_in_bp: display only.
- assets: the textures above; font Roboto Bold.
- verified (task-484-04 to 06): compiled with warnings as errors, saved; -game death and missed shuttle at 1920x1080, 1280x720, 1920x1200, 2560x1080 (Saved/Captures/rl06_owner); no script error. Not verified on screen: non-zero recap values, gamepad confirm, skip by key.
- owner_agent: agent-bp

## Relations
- [[blueprints-ui-index]] — parent index.
- [[sys-run-lost]] — the C++ base, recap and actions.
- [[bp-rr-player-controller]] — creates it through RunLostWidgetClass.
- [[death-screen]] — the design.
- [[art-prompts-death-screen-mockup]] — the validated mockup and element mapping.
