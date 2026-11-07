---
id: task-485-10-capture
description: "Capture: -game capture of every screen with its buttons at rest and hovered, at the four resolutions."
parent: task-485-ui-button-hover
kind: task
status: in-progress
---
# Hover effect on every button of every screen: capture

- goal: -game capture of every screen with its buttons at rest and hovered, at the four resolutions.
- agent: capture
- depends_on: task-485-09-main-menu
- inputs: ui-button-hover (vault/reference/gdd/art-audio/ui-button-hover.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Contact sheet sent to the owner, each point a pair of frames at rest and hovered: C1 main menu New Run and Exit; C2 pause Resume and Exit; C3 run-lost Retry and Main Menu; C4 stats Continue; C5 shop Reroll, Continue and one Buy; C6 a disabled Buy or Reroll hovered shows no change; C7 level-up Reroll, and a card hover unchanged; C8 run-won end screen button; C9 at 1920 x 1080, 1280 x 720, 1920 x 1200 and 2560 x 1080 nothing moved, overlapped or clipped on any screen. The owner's "je valide" on screen replaces this capture.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T03:03:24Z
- waiting_for: none
- result: open

## Relations
- [[task-485-ui-button-hover]] — parent global task.
