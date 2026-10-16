---
id: task-204-health-bar-dark-empty
description: "Global task: the empty part of the HUD health bar is dark grey #0B0C14 like the reference video, checked with an in-game capture."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Health bar dark empty part

- request: "Oui, passe la partie vide de la barre de vie en gris foncé, comme dans la vidéo, avec une capture en jeu à la fin." (owner, 2026-10-16, after the task-203-09 captures showed a light grey #BABABA empty part)
- goal: the empty part of the health bar (behind the fill and the damage chunk) is dark grey #0B0C14 (sampled from the miisan video frame at 12 s, centre of the empty bar), so the off-white chunk and the fill stand out.
- acceptance: an in-game capture at 1920 x 1080 after a hit shows the bar 280 x 28 at 20,232 with the empty part about #0B0C14 (sRGB), the chunk and fill unchanged; capture sent to the owner; wbp-health-status updated; committed.
- gdd_check: [[color-code]] has no HUD bar entry; the colour is the owner's choice from the video ("comme dans la vidéo"), provisional, owner 2026-10-16. Border unchanged (the video bar has a light grey #97969B border; not requested).
- confirmed_by_owner: yes (2026-10-16, his request)
- milestone: M1
- next_step: task-204-02-empty-bar-capture
- result: open

## Subtasks
1. task-204-01-empty-bar-color — bp, DamageChunkBar background tint #0B0C14
2. task-204-02-empty-bar-capture — cpp, in-game capture after a hit

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-203-health-bar-juice]] — the juicy bar this restyles.
- [[task-204-01-empty-bar-color]] — step 1, bp.
- [[task-204-02-empty-bar-capture]] — step 2, cpp.
