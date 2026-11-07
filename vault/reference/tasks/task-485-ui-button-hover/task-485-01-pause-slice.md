---
id: task-485-01-pause-slice
description: "Bp: Representative slice: hover effect of ui-button-hover on WBP_PauseMenu Resume and Exit, built so the other screens can reuse the same mechanism."
parent: task-485-ui-button-hover
kind: task
status: done
---
# Hover effect on every button of every screen: bp

- goal: Representative slice: hover effect of ui-button-hover on WBP_PauseMenu Resume and Exit, built so the other screens can reuse the same mechanism.
- agent: bp
- depends_on: none
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), wbp-pause-menu (vault/reference/tech/blueprints/ui/wbp-pause-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: In -game with the pause open, hovering Resume or Exit shows the plate brighter and grown to 1.05 with a 0.1 s ease while paused, back to rest on leave; clicks unchanged; no layout change; the mechanism is reusable by the other screens without copying graphs per button; compiled with warnings as errors, saved, no script error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-485
- usage: included in the executor run of task-485
- runs: 1
- milestone: M2
- started: 2026-11-07T02:19:47Z
- waiting_for: none
- result: Done (01a90440). Shared mechanism in C++: URRButtonHoverSubsystem (GameInstance subsystem, core ticker so it runs while paused) polls each attached button; hovered = Slate IsHovered and button plus parents enabled (focus alone and disabled buttons show nothing). Plate brightens through UI material M_UI_ButtonPlate (PlateTexture x vertex colour x Brightness) put on the Normal and Hovered brushes (tint >1 is clamped by Slate vertex colours, which is why the old 1.3 hover tint never showed); render scale 1.05, 0.1 s smoothstep ease; Pressed/Disabled brushes untouched. Values in URRButtonHoverSettings (DefaultGame.ini). Any widget opts in with URRButtonHoverSubsystem::Attach/AttachNamed, one call, no graph per button. Pause: Attach on ResumeButton and ExitButton in NativeOnInitialized; WBP_PauseMenu asset unchanged. Debug command rr.Debug.Cursor fx fy for hover captures. Build succeeded; material compiled and saved. Seen in -game 1920x1080 (Saved/Captures/task485_slice/crops.png): hovered plate brighter and larger, other at rest, nothing moved. Click not exercised (OnClicked bindings untouched).

## Relations
- [[task-485-ui-button-hover]] — parent global task.
