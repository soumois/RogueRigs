---
id: task-521-01-tab-size-fix
description: "Bp: tabs keep the same size when clicked or selected"
parent: task-521-options-tab-button-shrinks
kind: task
status: done
---
# Options : l'onglet cliqué devient plus petit: bp

- goal: tabs keep the same size when clicked or selected
- agent: bp
- depends_on: none
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), sys-button-hover (vault/reference/tech/systems/ui/sys-button-hover.md), sys-options (vault/reference/tech/systems/ui/sys-options.md)
- expected_output: see done_when
- done_when: A1 to A3 hold; commit.
- editor_state: editor open, PIE stopped. Editor note 2026-11-13: the owner was in PIE and the unreal MCP server refused connections (ECONNREFUSED) in the main session.
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-521
- usage: included in the executor run of task-521
- runs: 1
- milestone: M2
- started: 2026-11-13T13:26:17Z
- waiting_for: none
- result: Cause: the active tab plate T_Shop_ButtonContinue (640 x 163) has a 26 px transparent border on every side and a wider aspect than T_Options_Tab (396 x 77), so the selected tab drew its cream plate about 68 % tall and narrower. Fix in C++ (commit 3548c421, the cause was in URROptionsWidget::StyleTabButton, no Blueprint change needed): every tab gets the inactive plate size (layout height x 396/77), and the active brush crops the border through its UV region (new URROptionsSettings::ActiveTabTransparentBorder = 26 px, provisional), 3-slice margin converted to the cropped region. Build only, exit 0. A1 the three tabs same size after clicking each in turn (1920x1080, 1280x720, 1920x1200, 2560x1080); A2 Reset to defaults and Back unchanged after a click; A3 tabs have no hover growth (sys-options: they brighten by tint), selected look unchanged apart from the plate filling the tab. Scope Q2: no other screen checked for this defect; the cause is specific to the Options tab textures.

## Relations
- [[task-521-options-tab-button-shrinks]] — parent global task.
