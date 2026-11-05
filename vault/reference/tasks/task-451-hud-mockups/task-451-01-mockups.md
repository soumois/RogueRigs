---
id: task-451-01-mockups
description: "Art: Generate the unified HUD proposals from the current screenshot, show them, record the owner's choice."
parent: task-451-hud-mockups
kind: task
status: todo
---
# Unified in-game HUD: visual proposals: art

- goal: Generate the unified HUD proposals from the current screenshot, show them, record the owner's choice.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp, kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), agent-art (vault/reference/agents/agent-art.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Proposals shown to the owner; his choice recorded in a new art-prompts child node (prompts, runs, credits, his words, what each of tasks 452-458 takes); retained image, its log and prompts committed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-451-hud-mockups]] — parent global task.
