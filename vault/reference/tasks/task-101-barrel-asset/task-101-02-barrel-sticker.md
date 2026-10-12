---
id: task-101-02-barrel-sticker
description: "Barrel with the Kessler-Voss logo sticker: one correction of the validated barrel image (owner 2026-10-11)."
parent: task-101-barrel-asset
kind: task
status: in-progress
---
# Barrel with the Kessler-Voss logo sticker

- goal: Barrel with the Kessler-Voss logo sticker: one correction of the validated barrel image (owner 2026-10-11).
- agent: art
- depends_on: task-101-01-barrel-asset
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), agent-art (vault/reference/agents/agent-art.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md); images Art/Source/barrel/20261011-162640-barrel-2.png (validated shape) and Art/Source/kessler-voss-logo/20261011-201828-kessler-voss-logo-1.png (retained logo)
- expected_output: Correction prompt Art/Source/barrel/prompts/02-sticker.txt, opening "Keep this exact image: same ..." (Image 1 = barrel, Image 2 = logo) and changing ONE thing: a flat sticker with the Kessler-Voss plate logo (Image 2, text kept exactly: KESSLER-VOSS, MINING CONSORTIUM) on one front flat facet of the body, between the rings, following the facet; everything else identical (shape, 8-10 flat sides, rings, grey lid with one bolted cap, orange, white background, no shadow). Run node Tools/art/kie_image.js --asset barrel --prompt-file Art/Source/barrel/prompts/02-sticker.txt --ref Art/Source/barrel/20261011-162640-barrel-2.png --ref Art/Source/kessler-voss-logo/20261011-201828-kessler-voss-logo-1.png --aspect 1:1 --variants 2. Nothing in vault/assets/, nothing committed (the Orchestrator commits the retained image).
- done_when: 2 PNG and their run log exist in Art/Source/barrel/; each reviewed against the A rules (A1 and A14 as amended 2026-10-11) and for the exact spelling of the logo text; only the sticker changed; no more than 2 variants; nothing committed.
- editor_state: any
- token_ceiling: 25000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-101-barrel-asset]] — parent global task.
