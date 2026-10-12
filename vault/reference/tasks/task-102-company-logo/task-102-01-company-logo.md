---
id: task-102-01-company-logo
description: "Four variations of one Kessler-Voss Mining Consortium logo exist as candidate images; the owner keeps one."
parent: task-102-company-logo
kind: task
status: done
---
# Kessler-Voss logo proposals

- goal: Four variations of one Kessler-Voss Mining Consortium logo exist as candidate images; the owner keeps one.
- agent: art
- depends_on: none
- inputs: lore (vault/reference/gdd/lore.md), company-voice (vault/reference/gdd/company-voice.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), agent-art (vault/reference/agents/agent-art.md)
- expected_output: ONE base logo idea written by the art agent from the lore (company: Kessler-Voss Mining Consortium; corporate voice: propaganda, upbeat, glossy; robots carry a small yellow logo plate), then FOUR variations of it, each its own prompt file Art/Source/kessler-voss-logo/prompts/<NN>-<variation>.txt where only a few details change from one to the next (for example with or without the name, emblem proportions, plate or round shape, colour balance). One run per prompt: node Tools/art/kie_image.js --asset kessler-voss-logo --prompt-file <file> --aspect 1:1 --variants 1 (text-to-image, 1K), 4 images in all. Flat colour logo, readable at small size; text allowed by A14 as amended 2026-10-11; owner wants to see whether the name shows on small logos. No invented lore: no new company name, no slogan other than those in company-voice. Colours: company yellow allowed; never enemy-only acid green or steel blue, never orange (reserved for explosions). No prompt node, nothing copied to vault/assets/. Commit only the image the owner keeps, later (owner 2026-10-11). Starts only after the owner says go (app restart).
- done_when: 4 PNG (four variations of one logo) and their run logs exist in Art/Source/kessler-voss-logo/; a pass or exact gap per image against the amended A rules, readability at 64 px described, nothing in vault/assets/; nothing committed except by the Orchestrator once the owner keeps one.
- editor_state: any
- token_ceiling: 25000
- model: sonnet
- tokens: input 26, cache write 68439, cache read 174892, output 1527
- runs: 2
- milestone: M2
- result: 4 variations of one Kessler-Voss logo generated (24 credits); owner kept variation 1 (yellow bolted plate, KV asteroid emblem, KESSLER-VOSS and MINING CONSORTIUM); only that image, its log and prompt are tracked: Art/Source/kessler-voss-logo/20261011-201828-kessler-voss-logo-1.png. Commit 6d40471.

## Relations
- [[task-102-company-logo]] — parent global task.
