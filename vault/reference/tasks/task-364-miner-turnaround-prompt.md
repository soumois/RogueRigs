---
id: task-364-miner-turnaround-prompt
description: "Global task: Write the ChatGPT image prompt of the miner turnaround sheet (front, side, back, A-pose) that the owner turns into the 3D mesh himself."
parent: tasks-done-index
kind: task
status: done
---
# Miner turnaround prompt for ChatGPT

- request: Owner, 2026-10-29, verbatim: "Il me faudrait le prompt pour chatgpt pour le personnage puis je créerais la version 3D. Tu as déjà une idée de comment de l'asset 3D, tu arrives à l'animer ? Comme faire pour que le joueur tienne l'arme ?" Meaning: point 1 (this task) = a ChatGPT image prompt for the player character (the miner); the owner generates the image and makes the 3D version himself. Points 2 (how the 3D asset will be rigged and animated) and 3 (how the miner holds the weapon) are questions answered in the planner report from the vault; no task for them unless the owner asks (Q4). Draft prompt proposed to the owner (paragraph markers P1 to P5 are not part of the prompt): P1: Character turnaround sheet for 3D modeling of a stylized space miner, the playable hero of a top-down game, same character design as the attached validated miner: front view, side view (exact 90 degrees) and back view, all three at the same size, same angle and same framing, full body, subject centred with a margin, standing in a neutral A-pose (arms straight, about 45 degrees down from the shoulders, palms facing down, fingers together, thumbs slightly apart), legs straight and slightly apart, feet flat, no stand, no ground. P2: Style: stylized flat-shaded low-poly game asset, chunky blocky volumes with large flat facets, facets clearly visible, no smoothing, no textures, clean hard-surface look. P3: Design: a sturdy human miner in a full suit of rigid plate armor, slightly chunky proportions with a slightly large head so it reads from a steep top-down camera. A closed helmet with a wide amber visor. Rigid plates on the chest, shoulders, upper arms, forearms, thighs and shins, with clearly separated joints at the neck, shoulders, elbows, wrists, hips, knees and ankles (dark segmented joint covers) so the body can be rigged and bent. Big armored gloves with separated fingers, able to grip a rifle. A compact salvaged backpack bolted to the back and a small pouch on the belt. A clearly readable front and back. P4: Colors and wear: patched, mismatched white and teal plates with rust chips, bolts, rivets and welded repairs, one plate visibly replaced by a different teal plate, a repaired and improvised salvaged look. The amber visor is the only warm light; no red, no yellow, no green, no blue, no violet. P5: Background and light: plain white background, even neutral lighting, no shadows, no weapon, nothing in the hands, no effects, no text, no UI, no logo. Owner answer 2026-10-29 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3, Q4 accepted.
- goal: A prompt node art-prompts-miner, child of art-prompts, holds the verbatim ChatGPT prompt of the miner turnaround sheet for 3D (the draft in request as amended by the owner answers), with the reference images to attach, so the owner pastes it into ChatGPT.
- acceptance: vault/reference/gdd/art-audio/art-prompts-miner.md exists with the prompt verbatim in a fenced block (one paragraph per P1..P5 marker, markers removed), the images to attach, the decided pose and the note that the owner makes the 3D himself; art-prompts lists it in its Relations; no image generated and no Kie.ai credit spent; lint clean; committed
- gdd_check: Read miner (spec approved), asset-pipeline, art-style-guide, art-prompts, art-prompts-rigs, art-prompts-drone, art-3d-lessons, bp-miner, bp-burst-rifle, sys-player, sys-weapons (2026-10-29). No conflict: A6 (white, teal, rust, amber visor, backpack), A2 (rigid parts only, so joint covers are segmented, not cloth), A16-A17 and asset-pipeline step 1 (front, side, back, plain white, no weapon, no effects, even light; T-pose or A-pose allowed); the drone turnaround prompt is the format model (exact 90 degree side view, same size and framing, the side view of miner-turnaround.jpg was inconsistent). The existing T-pose prompt (art-prompts-rigs section 4) is kept as history. Change: miner.md said "Asset: Tripo (auto-rig)"; the owner now makes the 3D himself (edit in the defaults). Open values reported, not decided by this task: rig and animation route of the miner (asset-pipeline: Tripo deferred, v3 API unverified; rigid binding allowed; parts and bones list open); which of up to 4 equipped weapons the miner holds visibly (MaxWeapons 4, nothing in the GDD); weapon meshes are placeholder cubes; miner hand socket not set (WeaponAttachSocketName None, capsule offset since task-008-03). In milestone scope (M2 art).
- confirmed_by_owner: yes (2026-10-29, "ok")
- milestone: M2
- next_step: none
- started: 2026-10-29T17:56:21Z
- waiting_for: none
- executor_tokens: input 14, cache write 35888, cache read 144038, output 2919
- executor_usage: runs 1, turns 6, context start 18k peak 36k, resumes 0, 1 min, units 0.07 M
- planning_tokens: none (no planner run attributed)
- planning_usage: none
- result: Done. art-prompts-miner written (commit 76eed24e), linked from art-prompts, no image generated

## Questions
Decided 2026-10-29 (owner "ok"): every default below is accepted; values provisional, owner 2026-10-29.
- Q1 Pose of the sheet: A-pose (arms about 45 degrees down) or T-pose as the existing miner-turnaround.jpg. Default: A-pose (better shoulders for an auto-rig; asset-pipeline allows both).
- Q2 Reference: attach vault/assets/miner-turnaround.jpg and vault/assets/rigs-lineup.jpg in ChatGPT and keep the same design, or redesign freely. Default: attach both and keep the design.
- Q3 Prompt text: the draft in request (P1 to P5) as is, or with changes he dictates. Default: as is.
- Q4 Animation and weapon holding: answered only in the report now, a separate task planned once the mesh exists; or plan it now. Default: no task now.

## Subtasks
1. task-364-01-prompt-node (vault/reference/tasks/task-364-miner-turnaround-prompt/task-364-01-prompt-node.md) — art: write art-prompts-miner with the verbatim miner turnaround prompt; no generation

## Relations
- [[tasks-done-index]] — parent index.
- [[task-364-01-prompt-node]] — step 1, art.
- [[miner]] — the player character spec this prompt serves.
- [[art-prompts]] — parent of the prompt node to create.
