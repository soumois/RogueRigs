---
id: task-102-01-company-logo
description: "Four distinct logo proposals for the Kessler-Voss Mining Consortium exist as candidate images; the owner keeps one."
parent: task-102-company-logo
kind: task
status: in-progress
---
# Kessler-Voss logo proposals

- goal: Four distinct logo proposals for the Kessler-Voss Mining Consortium exist as candidate images; the owner keeps one.
- agent: art
- depends_on: none
- inputs: lore (vault/reference/gdd/lore.md), company-voice (vault/reference/gdd/company-voice.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), agent-art (vault/reference/agents/agent-art.md)
- expected_output: Four distinct logo concepts written by the art agent from the lore (company: Kessler-Voss Mining Consortium; corporate voice: propaganda, upbeat, glossy; robots carry a small yellow logo plate), one run per concept: node Tools/art/kie_image.js --asset kessler-voss-logo --prompt-file Art/Source/kessler-voss-logo/prompts/<NN>-<concept>.txt --aspect 1:1 --variants 1 (text-to-image, 1K). Flat colour logo, readable at small size, must still read without its name (the name may or may not appear: text allowed by A14 as amended 2026-10-11, owner wants to see whether the name shows on small logos). No invented lore: no new company name, no slogan other than those in company-voice. Colours: company yellow is allowed; never enemy-only acid green or steel blue, never orange (reserved for explosions). Save each prompt under Art/Source/kessler-voss-logo/prompts/. No prompt node, nothing copied to vault/assets/. Commit only the image the owner keeps, later (owner 2026-10-11): leave the others uncommitted.
- done_when: 4 PNG (one per distinct concept) and their run logs exist in Art/Source/kessler-voss-logo/; a pass or exact gap per image against the amended A rules, readability at 64 px described, nothing in vault/assets/; nothing committed except by the Orchestrator once the owner keeps one.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-102-company-logo]] — parent global task.
