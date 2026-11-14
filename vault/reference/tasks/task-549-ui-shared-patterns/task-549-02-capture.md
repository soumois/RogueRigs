---
id: task-549-02-capture
description: "Capture: Before/after captures of every game screen at 1920 x 1080"
parent: task-549-ui-shared-patterns
kind: task
status: done
---
# UI: generalise the patterns and scripts used again and again: capture

- goal: Before/after captures of every game screen at 1920 x 1080
- agent: capture
- depends_on: task-549-01-factor
- inputs: umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: C1 to C7: main menu, pause, stats, shop, death, options and run-won screens each match their before frame.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-549
- usage: included in the executor run of task-549
- runs: 1
- milestone: M2
- started: 2026-11-14T04:55:13Z
- waiting_for: none
- result: 10 launches with Tools/capture/ui-screens.js at 1920x1080 (5 before, 5 after the 01 build): Saved/Captures/t549_before/ and t549_after/ (<screen>.png, compare-<screen>.png, PSNR). Before sheet t549_before/sheet.png; shop and runwon pairs t549_after/cmp2.png. C1 main menu seen, identical pixels (PSNR inf). C2 pause seen, 71.8 dB. C3 stats seen, 51.0 dB (same frame, title, bolted dividers, NEXT LEVEL). C4 shop seen, 24.6 dB: only the random card offers differ (frame, rivets, clamps, title bars, counter, SOLD slot, Reroll, Continue the same). C5 death seen, 49.0 dB. C6 options seen, 71.7 dB. C7 run-won seen, 33.1 dB: the UI is the same, only the random level behind it differs. Pre-existing, out of scope: the level 4 subtitle still crosses its thin lines (task-381 defect). Not verified: other resolutions (no layout changed).

## Relations
- [[task-549-ui-shared-patterns]] — parent global task.
