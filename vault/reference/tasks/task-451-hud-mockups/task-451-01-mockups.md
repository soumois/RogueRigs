---
id: task-451-01-mockups
description: "Art: Generate the unified HUD proposals from the current screenshot, show them, record the owner's choice."
parent: task-451-hud-mockups
kind: task
status: done
---
# Unified in-game HUD: visual proposals: art

- goal: Generate the unified HUD proposals from the current screenshot, show them, record the owner's choice.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/reference/owner-hud-current-2026-11-05.webp, Art/Source/hud-unified/reference/external-example-radar-panel-2026-11-05.png (external example of the radar frame idea, not to copy; it is not a generation reference unless the Executor states why), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), agent-art (vault/reference/agents/agent-art.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Proposals shown to the owner; his choice recorded in a new art-prompts child node (prompts, runs, credits, his words, what each of tasks 452-458 takes); retained image, its log and prompts committed; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 16, cache write 48947, cache read 113247, output 10637
- usage: turns 8, context start 10k peak 32k, resumes 0, 2 min, units 0.13 M
- runs: 1
- milestone: M2
- started: 2026-11-05T23:36:53Z
- waiting_for: none
- result: 9 mockups in 3 directions (Kie.ai GPT Image 2, 54 credits: 36 first round + 18 one correction round), style-guide reviewed twice (no A-rule fail; remaining brief gap: XP tick spacing, set in UMG). Owner 2026-11-05: "A, plaques rivetées"; retained Art/Source/hud-unified/20261105-152627-hud-unified-1.png with its log; others temporary in Saved/Scratch/hud-unified; art-prompts-hud-unified-mockup records prompts, runs, credits, choice and what 452-458 take.

## Relations
- [[task-451-hud-mockups]] — parent global task.
