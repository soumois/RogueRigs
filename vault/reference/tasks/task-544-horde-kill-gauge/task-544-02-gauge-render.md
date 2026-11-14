---
id: task-544-02-gauge-render
description: "Art: render of the horde gauge in the HUD with GPT Image 2 (the ChatGPT image model) through the project image API, then its elements."
parent: task-544-horde-kill-gauge
kind: task
status: in-progress
---
# Hordes: one "!" at half of C, then a kill gauge under the alert bar triggers each horde: art

- goal: The owner asked for a ChatGPT render of the gauge ("J'attends un rendu avec chatgpt, utilise l'API", Q9). Make one render of the HUD top with the horde gauge under the alert bar, with GPT Image 2 (the ChatGPT image model) through the project image API `Tools/art/kie_image.js` (key `KIE_API_KEY` from the environment, never written in the repo), image-to-image from owner image 4; brief = the Q9 look: thin red fill (the "!" red) on a dark track, about a third of the alert bar width, centred just under it, a small "!" at its left end, pulsing while full (show it partly filled), in the riveted HUD style of the alert bar. Then cut or regenerate the gauge elements (track, fill, "!") the widget needs.
- agent: art
- depends_on: task-544-01-gauge-logic
- inputs: kie-image-workflow (vault/reference/tech/pipelines/kie-image-workflow.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), level-flow (vault/reference/gdd/run/level-flow.md), Saved/Scratch/owner-ui-batch-2026-11-13/4.webp
- expected_output: render and elements in Art/Source/<asset>/ labelled keep with run log and prompt; the render path written in result so the owner sees it on return.
- done_when: 1 variant per round, 2 rounds at most, credits reported; style-guide verdict pass on the render and elements (a remaining fail is recorded, not waited on); no owner go (owner away, batch rule). If the key or tool is missing or a run fails twice, no image is made: the gap goes in result and task-544-03 builds the brief look directly; never wait.
- editor_state: closed
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-14T00:28:41Z
- waiting_for: none
- result: open

## Relations
- [[task-544-horde-kill-gauge]] — parent global task.
