---
id: task-364-01-prompt-node
description: "Art: write art-prompts-miner with the verbatim miner turnaround prompt; no generation"
parent: task-364-miner-turnaround-prompt
kind: task
status: todo
---
# Miner turnaround prompt for ChatGPT: art

- goal: Create vault/reference/gdd/art-audio/art-prompts-miner.md (kind note, parent art-prompts) holding the prompt of the global request verbatim in one fenced block, one paragraph per marker P1..P5 with the markers removed, amended only by the owner answers recorded in the global request and Questions; list the images to attach (Q2), the pose (Q1), that the owner generates the image in ChatGPT and makes the 3D himself, and that the validated sheet goes to vault/assets/ when he says so. Add the node to art-prompts Relations. Do NOT run kie_image.js and generate no image.
- agent: art
- depends_on: none
- inputs: task-364-miner-turnaround-prompt (vault/reference/tasks/task-364-miner-turnaround-prompt.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-rigs (vault/reference/gdd/art-audio/art-prompts-rigs.md), art-prompts-drone (vault/reference/gdd/art-audio/art-prompts-drone.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), miner (vault/reference/gdd/player/miner.md), asset-pipeline (vault/reference/tech/pipelines/asset-pipeline.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md)
- expected_output: vault/reference/gdd/art-audio/art-prompts-miner.md; art-prompts.md Relations line; commit by path; the final prompt text returned in the report for the owner
- done_when: the node exists with the prompt verbatim (diff against the global request shows only the marker removal and the owner amendments); art-prompts links it; no new file under Art/Source/ and no Kie.ai run log; node vault/scripts/lint.js --write clean; committed by path
- editor_state: not needed (Unreal may be open or closed)
- token_ceiling: 0.15 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-364-miner-turnaround-prompt]] — parent global task.
