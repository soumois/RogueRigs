---
id: task-337-01-main-menu-background
description: "Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner."
parent: task-337-main-menu-background
kind: task
status: todo
---
# Main menu background image

- goal: Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner.
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), art-prompts-arena-final (vault/reference/gdd/art-audio/art-prompts-arena-final.md), art-prompts-rigs (vault/reference/gdd/art-audio/art-prompts-rigs.md), art-prompts-robots (vault/reference/gdd/art-audio/art-prompts-robots.md), main-menu (vault/reference/gdd/run/main-menu.md, decided look of the background); reference images vault/assets/keyart.jpg, vault/assets/rigs-lineup.jpg, vault/assets/enemies-lineup.jpg
- expected_output: (owner "ok" 2026-10-28 to Q1 to Q11; decided look in main-menu and the global gdd_check Q2 to Q5) one Kie.ai run, image-to-image from keyart.jpg, rigs-lineup.jpg and enemies-lineup.jpg, aspect 16:9, 1K, 3 variants, then at most 2 correction passes of one image ("Keep this exact image ... Only change ..."), about 30 credits at most; no text and no UI in the image; run logs and prompt files in Art/Source/main-menu-background/; a prompt node art-prompts-main-menu-background, child of art-prompts, with the verbatim prompts and the self-review.
- done_when: images sent to the owner; style-guide review done; the owner picks or rejects; the chosen image is copied to vault/assets/ only on his word; prompt node written
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-337-main-menu-background]] — parent global task.
