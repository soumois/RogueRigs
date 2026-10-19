---
id: task-234-06-heal-deposit-prompts
description: "New ChatGPT prompts for 3 heal deposit variants that read apart from Duskite: red crystals embedded in rock."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Heal deposit prompts, version 2

- goal: 3 prompts (plus a 3 x 1 sheet prompt) for heal deposits whose silhouette differs clearly from the Duskite formations (owner 2026-10-19: \"les cristaux de soin, sont trop ressemblant au Duskite, il peut avoir des cristaux intégrer à la roche aussi\"): red crystals embedded in rock (inlaid, half-buried, breaking out of a rock mound or of the slab), low and wide rather than tall free spires; the owner generates the images with ChatGPT. Prompts only, no image API.
- agent: art
- depends_on: task-234-01-ore-deposit-look
- inputs: vault/reference/gdd/art-audio/art-prompts-ore-deposit.md, vault/assets/duskite-deposit-sheet.png (what heal must NOT look like), Art/Source/rock-look/20261017-131704-rock-look-2.png, vault/reference/gdd/art-audio/art-style-guide.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/gdd/art-audio/readability-rules.md, vault/reference/agents/agent-art.md
- expected_output: a \"Heal prompts, version 2\" section in art-prompts-ore-deposit (version 1 kept, marked rejected: too close to Duskite), self-review including a distinctness check against the Duskite sheet.
- done_when: node updated and linted; prompts sent to the owner; committed.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
