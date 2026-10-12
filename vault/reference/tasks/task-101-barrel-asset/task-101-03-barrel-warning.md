---
id: task-101-03-barrel-warning
description: "Barrel with an explosive warning sign: one correction of the sticker image (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: done
---
# Barrel with an explosive warning sign

- goal: Barrel with an explosive warning sign: one correction of the sticker image (owner 2026-10-11).
- agent: art
- depends_on: task-101-02-barrel-sticker
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), agent-art (vault/reference/agents/agent-art.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md); image: Art/Source/barrel/20261011-220936-barrel-1.png (owner retained 2026-10-11)
- expected_output: Correction prompt Art/Source/barrel/prompts/03-warning.txt, opening "Keep this exact image: same ..." and changing ONE thing: a flat explosion or flame pictogram, white or dark grey, no yellow and black stripes, on the facet next to the sticker; everything else identical. Run node Tools/art/kie_image.js --asset barrel --prompt-file Art/Source/barrel/prompts/03-warning.txt --ref Art/Source/barrel/20261011-220936-barrel-1.png --aspect 1:1 --variants 2. Nothing in vault/assets/, nothing committed.
- done_when: 2 PNG and their run log exist in Art/Source/barrel/; each reviewed against the A rules and readability-rules (orange means explodes; no enemy-only colours, no yellow and black stripes); only the pictogram changed; no more than 2 variants; nothing committed.
- editor_state: any
- token_ceiling: 25000
- model: sonnet
- tokens: input 12, cache write 23863, cache read 77348, output 433
- runs: 1
- milestone: M2
- result: Explosion pictogram added (12 credits); owner retained Art/Source/barrel/20261011-221704-barrel-2.png, copied to vault/assets/barrel.png (validated image).

## Relations
- [[task-101-barrel-asset]] — parent global task.
