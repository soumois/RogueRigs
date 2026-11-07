---
id: task-484-02-elements
description: "Art: Map every element of the validated mockup to an existing sprite; make a new sprite only where none matches."
parent: task-484-death-screen
kind: task
status: done
---
# Death screen with Retry and Main Menu: art

- goal: Map every element of the validated mockup to an existing sprite; make a new sprite only where none matches.
- agent: art
- depends_on: task-484-01-mockup
- inputs: death-screen (vault/reference/gdd/run/death-screen.md), art-prompts-pause-menu-mockup (vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md)
- expected_output: see done_when
- done_when: The art-prompts node lists each element with its asset, 9-slice margins and credits; any new sprite is in Art/UI with its source in Art/Source, keyed, owner-chosen, 24 credits at most in total.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-484
- usage: included in the executor run of task-484
- runs: 1
- milestone: M2
- started: 2026-11-07T01:03:44Z
- waiting_for: none
- result: Every mockup element maps to an existing sprite (T_LevelEnd_Frame, T_Shop_TitleBar, T_Shop_ButtonContinue, UMG text, lines and overlay); no new sprite, 0 credits; table in art-prompts-death-screen-mockup Elements.

## Relations
- [[task-484-death-screen]] — parent global task.
