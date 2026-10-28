---
id: task-336-01-heal-crystal-variants
description: "Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same."
parent: task-336-heal-crystal-variants
kind: task
status: in-progress
---
# Heal crystal variants: images

- goal: Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same.
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), bp-pickup-heal (vault/reference/tech/blueprints/fx/bp-pickup-heal.md), bp-pickup-ore (vault/reference/tech/blueprints/fx/bp-pickup-ore.md), art-prompts-ore-deposit (vault/reference/gdd/art-audio/art-prompts-ore-deposit.md)
- owner_pick: 2026-10-28, owner "ok" to Q1 of [[task-339-heal-chunk-mesh]] (relayed by the main session): the pass 2 sheet `Art/Source/heal-pickup/20261027-165219-heal-pickup-1.png` is validated despite the style-guide A1 fail; copy it to `vault/assets/heal-pickup-sheet.png`, commit it with `Art/Source/heal-pickup/20261027-165219-heal-pickup.review.json`, then close this task. Earlier sheets stay untracked.
- expected_output: (owner "ok" 2026-10-28 to Q1 to Q5 of the global task; read its gdd_check for the decided look) one 3-up sheet PNG (1K) of 3 isolated red heal pickup crystal variants (Q4 look, Q5 framing) generated image-to-image from vault/assets/heal-deposit-sheet.png, at most 2 correction passes; run logs and prompt files in Art/Source/heal-pickup/; a prompt node art-prompts-heal-pickup, child of art-prompts, with the verbatim prompts and the self-review.
- done_when: images sent to the owner; style-guide review done; the owner picks or rejects; the chosen image is copied to vault/assets/ only on his word; prompt node written
- editor_state: any
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-10-28T01:25:15Z
- waiting_for: none
- result: open

## Relations
- [[task-336-heal-crystal-variants]] — parent global task.
