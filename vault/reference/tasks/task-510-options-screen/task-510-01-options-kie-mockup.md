---
id: task-510-01-options-kie-mockup
description: "Art: one Kie.ai gpt-image-2 image-to-image mockup of the options screen from the pause menu and level-end references, credits reported, owner validation gate"
parent: task-510-options-screen
kind: task
status: done
---
# Options screen: Sound, Graphics and Gameplay settings, look from a Kie.ai mockup: art

- goal: one Kie.ai gpt-image-2 image-to-image mockup of the options screen, Graphics tab shown, from the pause menu and level-end references, credits reported, validation gate
- agent: art
- depends_on: none
- inputs: options-screen (vault/reference/gdd/run/options-screen.md), kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), task-424-01-pause-menu-kie-mockup (vault/reference/tasks/task-424-pause-menu-stats/task-424-01-pause-menu-kie-mockup.md), art-prompts-level-end-screen-mockup (vault/reference/gdd/art-audio/art-prompts-level-end-screen-mockup.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md)
- expected_output: see done_when
- done_when: Mockup image (16:9, 1K, Graphics tab with the Sound and Gameplay tabs visible) made with kie_image.js image-to-image, prompt and log kept, credits reported, file sent; the Executor stops and reports the image; the go is written in the result: owner "je valide", or the main session under the owner sentence of 2026-11-11 in the global request; each change round written too.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: input 6, cache write 26850, cache read 29416, output 3484
- usage: turns 3, context start 7k peak 27k, resumes 0, 1 min, units 0.05 M
- runs: 1
- milestone: M3
- started: 2026-11-12T05:30:29Z
- waiting_for: none
- result: Mockup round 1 by kie_image.js gpt-image-2 image-to-image, refs pause menu mockup (Image 1) and level-end mockup (Image 2): Art/Source/options-screen/20261112-062655-options-screen-1.png, log 20261112-062655-options-screen.json, 6 credits (total 6), file sent. style-guide pass, 0 fails (notes: slider knob mid-track for 100 %, button widths, VSync pill toggle). Go given by the main session under the owner sentence of 2026-11-11 ('Si la maquette est proche et que je n'ai pas fait de commentaire, tu pourras valider ( ou si mon commentaire est valable pour la maquette aussi'), not the owner's own je valide; noted for him: two columns Display/Quality, slider knob kept, no new round. Prompt node art-prompts-options-screen-mockup.

## Relations
- [[task-510-options-screen]] — parent global task.
