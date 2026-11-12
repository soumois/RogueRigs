---
id: task-506-02-options-gameplay-tab
description: "Bp: Gameplay tab in the options screen."
parent: task-506-options-gameplay-and-pause
kind: task
status: done
---
# Options screen: Gameplay tab and access from the pause menu: bp

- goal: Gameplay tab in the options screen.
- agent: bp
- depends_on: task-506-01-settings-gameplay
- inputs: options-menu (vault/reference/gdd/run/options-menu.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: A2 holds and each toggle changes its setting through C++.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-506
- usage: included in the executor run of task-506
- runs: 1
- milestone: M2
- started: 2026-11-12T04:59:14Z
- waiting_for: none
- result: GameplayList (VerticalBox, SoundList anchors) added to WBP_Options and GameplayTabButton enabled (d2d0040e). -game 1920x1080 (Saved/Captures/t506_02a): Gameplay tab shows the four toggles (Damage numbers, Aim indicator, Screen shake, Pause when the window loses focus) in the VSync row look; rr.Options step changed DamageNumbers and AimIndicator to Off in the settings dump; Reset on the Gameplay tab set them back to On, other tabs unchanged.

## Relations
- [[task-506-options-gameplay-and-pause]] — parent global task.
