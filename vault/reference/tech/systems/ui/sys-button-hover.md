---
id: sys-button-hover
description: "Shared mouse hover of screen buttons as built: URRButtonHoverSubsystem, M_UI_ButtonPlate, settings, attach calls per widget, debug commands; open for button work."
parent: systems-ui-index
kind: doc
---
# Button hover

- responsibility: the mouse hover look of [[ui-button-hover]] on every screen button, one mechanism for all widgets (task-485, commits 01a90440, 0accb545). A widget opts a button in with one C++ call or one class default; no Blueprint graph per button.
- main_classes: `UI/RRButtonHover.h/.cpp`: `URRButtonHoverSubsystem` (GameInstance subsystem) and `URRButtonHoverSettings` (plain config object). Material `/Game/RogueRigs/UI/M_UI_ButtonPlate` (domain User Interface, Translucent; Final Color = parameter `PlateTexture` x vertex colour x scalar `Brightness` (default 1), Opacity = texture alpha x vertex alpha).
- entry_points: `URRButtonHoverSubsystem::Attach(Button, bBrightenPlate = true, ScaleWidget = null)` and `AttachNamed(Owner, Names, bBrightenPlate)`, idempotent, ignored at design time. `URRHUDPanelWidget::HoverButtonNames` (EditDefaultsOnly) attaches named buttons of a HUD panel in `NativeOnInitialized`.
- behaviour: a core ticker (real time, runs while paused) polls each button: target 1 when Slate `IsHovered` and the button and every parent (nested user widgets included) are enabled, else 0. The amount moves at 1 / EaseSeconds per second, smoothstep eased. Growth: render scale of `ScaleWidget` (default the button) = rest scale x lerp(1, HoverScale), so layout never moves. Brightening: textured plate = a material instance on the Normal brush (same margins and tint), `Brightness` = lerp(1, HoverBrightness)^2.2 (display factor); untextured plate (engine grey rounded box) = Normal tint x lerp(1, HoverBrightness) in linear, capped at 1, so a white label stays readable; since task-486 no attached button uses this mode (the level-up Reroll and run-won Main Menu became cream plates), the code path stays as a fallback. In both cases Hovered brush := Normal brush, so the instant Slate swap is gone (a tint above 1 was clamped by Slate vertex colours, which is why the old 1.3 hover tints never showed). Pressed and Disabled brushes untouched. Keyboard or gamepad focus alone never triggers it.
- attached: pause Resume, Exit and run-lost Retry, Main Menu (bound pointers, `NativeOnInitialized`); stats ContinueButton ([[sys-run-transition]] widget, `NativeOnInitialized`); shop RerollButton, ContinueButton (`NativeOnInitialized`) and each ShopCard0..4 BuyButton with BuyPlateBox as the growing widget (`URRShopScreenWidget::NativeConstruct`; the Buy juice animates the button's own scale); main menu NewRunButton, ExitButton with bBrightenPlate false (growth only, orange = the Blueprint focus colour); WBP_UpgradeChoice RerollButton and WBP_AlertObjectives RestartButton through HoverButtonNames.
- data: Config/DefaultGame.ini section `[/Script/RogueRigs.RRButtonHoverSettings]` (none written; code defaults): HoverScale 1.05, HoverBrightness 1.25, EaseSeconds 0.1, PlateMaterial M_UI_ButtonPlate (values provisional, owner 2026-11-07 via [[ui-button-hover]]).
- debug: not shipping, `UI/RRButtonHoverDebugCommands.cpp`: `rr.Debug.Cursor <fx> <fy>` (real cursor at a viewport fraction), `rr.Debug.CursorOn <ButtonName> [Index]` (cursor at the centre of the visible button of that name, Index from the left), `rr.Debug.ButtonHover` (logs owner, name, hovered, enabled, amount, plate mode). The module links SlateCore since this task.
- depends_on: [[sys-hud-events]] (HUD panel base), [[sys-pause-menu]], [[sys-run-lost]], [[sys-main-menu]].
- implements: [[ui-button-hover]]
- gotchas: `r.SetRes` cannot make the window taller than the desktop: capture 1920 x 1200 with `--res 1920x1200 --force-res`. A widget that replaces a button style at runtime would drop the plate material; none does today.

## Relations
- [[systems-ui-index]] — parent index.
- [[ui-button-hover]] — the design it implements.
- [[sys-hud-events]] — URRHUDPanelWidget, where HoverButtonNames lives.
- [[sys-pause-menu]] — first widget attached (representative slice).
- [[sys-run-lost]] — run-lost buttons attached.
- [[sys-main-menu]] — text buttons, growth only.
- [[sys-run-transition]] — stats and shop screen widget bases.
- [[tool-capture]] — capture tooling used with rr.Debug.CursorOn.
- [[task-485-ui-button-hover]] — the task that built it.
