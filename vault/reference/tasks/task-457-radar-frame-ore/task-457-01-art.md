---
id: task-457-01-art
description: "Art: Cut the radar frame sprite of the retained mockup."
parent: task-457-radar-frame-ore
kind: task
status: done
---
# Radar frame holding the ore count: art

- goal: Cut the radar frame sprite of the retained mockup.
- agent: art
- depends_on: none
- inputs: Art/Source/hud-unified/, Art/Source/hud-unified/reference/external-example-radar-panel-2026-11-05.png (external idea only, not to copy), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-prompts-shop-screen-mockup (vault/reference/gdd/art-audio/art-prompts-shop-screen-mockup.md)
- expected_output: see done_when
- done_when: Sprite retained by the owner, keyed, committed and listed in the mockup art-prompts node.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 8, cache write 22820, cache read 55921, output 4084
- usage: turns 4, context start 9k peak 27k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M2
- started: 2026-11-06T00:33:47Z
- waiting_for: none
- result: Commits b8842c42, 28c9e3ef. 2 candidates (Kie.ai GPT Image 2 i2i, refs mockup crop + mockup, 12 credits), critic: both pass A1-A18, none copies the external example. Owner 2026-11-05: "Candidat 1". Kept Art/Source/hud-unified/20261105-155448-hud-unified-1.png + log; candidate 2 moved to Saved/Scratch/hud-unified/. Keyed cut-out Art/UI/HUD/T_HUD_RadarFrame.png (1024 x 580, ui_icon_prepare --check clean; hole centre 304,288 diameter ~383; plate inside x 578..928 y 122..279), listed in art-prompts-hud-unified-mockup.

## Relations
- [[task-457-radar-frame-ore]] — parent global task.
