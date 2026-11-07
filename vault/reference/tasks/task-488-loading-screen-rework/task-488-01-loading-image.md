---
id: task-488-01-loading-image
description: "Art: One full-screen loading illustration made and kept as intermission says (subject Q2), prompt listed in a new art-prompts node."
parent: task-488-loading-screen-rework
kind: task
status: in-progress
---
# Richer loading screen (illustration and framed texts): art

- goal: One full-screen loading illustration made and kept as intermission says (subject Q2), prompt listed in a new art-prompts node.
- agent: art
- depends_on: none
- inputs: intermission (vault/reference/gdd/run/intermission.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-main-menu-background (vault/reference/gdd/art-audio/art-prompts-main-menu-background.md), vault/assets/keyart.jpg, vault/assets/rigs-lineup.jpg, vault/assets/main-menu-background.png
- expected_output: see done_when
- done_when: A 16:9 image in key art style, no text or UI, subject as intermission says, saved in Art/Source and vault/assets and imported for UI use; a new art-prompts node under art-audio-index holds the verbatim prompt, model and inputs; any rejected variant is in Saved/Scratch labelled temporary.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-07T04:21:16Z
- waiting_for: none
- result: open

## Relations
- [[task-488-loading-screen-rework]] — parent global task.
