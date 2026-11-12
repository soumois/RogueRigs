---
id: task-505-05-options-doc
description: "Doc: Document the settings system and the options widget."
parent: task-505-options-screen-sound-graphics
kind: task
status: done
---
# Options screen with Sound and Graphics, opened from the main menu: doc

- goal: Document the settings system and the options widget.
- agent: doc
- depends_on: task-505-04-options-capture
- inputs: blueprints-index (vault/reference/tech/blueprints/blueprints-index.md), wbp-main-menu (vault/reference/tech/blueprints/ui/wbp-main-menu.md), options-menu (vault/reference/gdd/run/options-menu.md), vault/reference/tech/systems/
- expected_output: see done_when
- done_when: A system node for the settings and a bp node for the options widget (anchors per umg-layout) exist and are linked; wbp-main-menu updated; options-menu first-launch values filled; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-505
- usage: included in the executor run of task-505
- runs: 1
- milestone: M2
- started: 2026-11-12T04:38:11Z
- waiting_for: none
- result: Done (03956797). Created sys-game-settings (URRGameUserSettings, URROptionsSettings, volume assets, keep-or-revert, rr.Settings), sys-options-screen (URROptionsWidget rows, tabs, Reset, Back, Escape, main menu OpenOptions, rr.Options), wbp-options (anchors table per umg-layout, read from the editor). Updated wbp-main-menu and sys-main-menu (Options active, OptionsFocus, hover focus in C++), indexes, options-menu first-launch values (Windowed fullscreen at desktop size, VSync Off, Unlimited, Epic, 100 %). Lint clean on touched nodes.

## Relations
- [[task-505-options-screen-sound-graphics]] — parent global task.
