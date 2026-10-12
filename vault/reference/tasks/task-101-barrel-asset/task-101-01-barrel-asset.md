---
id: task-101-01-barrel-asset
description: "The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh."
parent: task-101-barrel-asset
kind: task
status: done
---
# Barrel source image

- goal: The explosive barrel has an owner-validated source image, then a Blender-built export-ready mesh.
- agent: art
- depends_on: none
- inputs: barrel (vault/reference/gdd/terrain/barrel.md), agent-art (vault/reference/agents/agent-art.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: Prompt already written by the owner in Art/Source/barrel/prompts/01-barrel.txt: use it verbatim, do not rewrite it. Run node Tools/art/kie_image.js --asset barrel --prompt-file Art/Source/barrel/prompts/01-barrel.txt --ref vault/assets/keyart.jpg --aspect 1:1 --variants 2 (key in KIE_API_KEY). keyart.jpg is a style reference only. No prompt node, nothing copied to vault/assets/. Review each PNG against art-style-guide A rules.
- done_when: 2 PNG and their run log exist in Art/Source/barrel/; a pass or exact gap is written per image against the A rules of art-style-guide; nothing in vault/assets/; no more than 2 variants generated; committed (Art/Source/barrel by name).
- editor_state: any
- token_ceiling: 20000
- model: sonnet
- tokens: input 40, cache write 28770, cache read 271500, output 1234
- runs: 4
- milestone: M2
- result: 2 candidates generated (12 credits; two fixes needed: env var not seen by the app, Kie upload host); owner validated shape 2 (Art/Source/barrel/20261011-162640-barrel-2.png); image 1 kept untracked. Commits 6bc6917, 937f4cf.

## Relations
- [[task-101-barrel-asset]] — parent global task.
