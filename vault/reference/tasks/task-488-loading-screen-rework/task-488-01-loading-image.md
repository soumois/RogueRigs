---
id: task-488-01-loading-image
description: "Art: One full-screen loading illustration made and kept as intermission says (subject Q2), prompt listed in a new art-prompts node."
parent: task-488-loading-screen-rework
kind: task
status: done
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
- model: opus
- tokens: input 18, cache write 66801, cache read 93091, output 12922
- usage: turns 9, context start 9k peak 25k, resumes 0, 2 min, units 0.16 M
- runs: 1
- milestone: M2
- started: 2026-11-07T04:21:16Z
- waiting_for: none
- result: Image made in 3 Kie.ai image-to-image runs (24 credits): scene run 20261106-202154 (2 variants), corrections 20261106-202424 and 20261106-202627. Retained Art/Source/loading-screen/20261106-202627-loading-screen-1.png (1672 x 941, 16:9, no text): the run shuttle with miner and drone at the open hatch above the arena asteroid, calm dark bottom left. Copied to vault/assets/loading-screen-background.png, imported as /Game/RogueRigs/UI/T_LoadingBackground (UI group, no mips). Prompts node art-prompts-loading-screen. style-guide: final pass; open deviation A12 (low camera, as the main menu background) for the owner. Rejected variant 2 in Saved/Scratch/task-488 (temporary). Commit 7dc4530c.

## Relations
- [[task-488-loading-screen-rework]] — parent global task.
