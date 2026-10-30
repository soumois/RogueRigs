---
id: task-387-01-prompt-and-refs
description: "Art: crop the rifle from rigs-lineup.jpg, write the burst rifle turnaround prompt and the node art-prompts-burst-rifle; no generation"
parent: task-387-burst-rifle-ref-prompt
kind: task
status: todo
---
# Burst rifle turnaround prompt and reference images: art

- goal: Following the decisions of the global request and Questions (Q1 to Q5): (1) cut the rifle held by the miner out of vault/assets/rigs-lineup.jpg (1600 x 900; the rifle lies roughly between x 200 and 560, y 290 and 420; keep the whole rifle from stock to muzzle brake, leave out as much of the muzzle flash as possible, no other change than an optional integer upscale) into Art/Source/burst-rifle/reference/rifle-crop-rigs-lineup.png with ffmpeg (WinGet) or a Node script, and look at the crop to check it; (2) write Art/Source/burst-rifle/prompts/chatgpt-burst-rifle-turnaround.md with the prompt verbatim (P1 to P5 as decided, one paragraph each, markers removed) and the attachments in order; (3) write the node art-prompts-burst-rifle (child of art-prompts, format model art-prompts-miner: who generates, decisions with the owner words, attachments with paths, prompt in a fenced block, open values: name, size, budget, hand socket) and add its line to the Relations of art-prompts. Do not touch burst-rifle.md (the Planner edit covers it) nor the Validated images list.
- agent: art
- depends_on: none
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-miner (vault/reference/gdd/art-audio/art-prompts-miner.md) (format model of a prompt node), art-prompts-rigs (vault/reference/gdd/art-audio/art-prompts-rigs.md) (source prompt of rigs-lineup.jpg), art-prompts-console (vault/reference/gdd/art-audio/art-prompts-console.md) (four-view turnaround model), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md) (Meshy multi-view rule), vault/assets/rigs-lineup.jpg (crop source and attachment)
- expected_output: Art/Source/burst-rifle/reference/rifle-crop-rigs-lineup.png; Art/Source/burst-rifle/prompts/chatgpt-burst-rifle-turnaround.md; vault/reference/gdd/art-audio/art-prompts-burst-rifle.md; art-prompts Relations line; one commit task-387-01-prompt-and-refs: <summary>; report with the crop size and the path of the crop, to be shown to the owner.
- done_when: The crop exists, shows the whole rifle (stock to muzzle brake) with no other subject cut in; the prompt file and the node exist and are committed; the prompt follows Q1 to Q5 as decided and lists the attachments with paths; art-prompts links the new node; no kie_image.js and no meshy_3d.js run (unless Q1 option b); node vault/scripts/lint.js --write clean.
- editor_state: not needed
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
- [[task-387-burst-rifle-ref-prompt]] — parent global task.
