---
id: task-169-weapon-card-design
description: "Global task: new-weapon cards with their own layout (New weapon, icon, name, explanation, lore) and a unique frame."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# New-weapon cards: own design

- request: Les cartes armes seront un peu différentes: il n'y aura que l'icone d'arme, sur le top de la carte "New weapon", en dessous l'explication de l'arme et puis le lore et il faudra un encadré unique. Defaults accepted by the owner: unique = a special salvage frame distinct from rarity borders, made by the art agent; weapon name under the icon; explanation = current catalogue description; one lore line per offerable weapon (shotgun, turret launcher) (owner, 2026-10-14)
- goal: Give the new-weapon cards their own design: "New weapon" on top, the weapon icon only, the weapon name, its explanation, a lore line and a unique frame distinct from the rarity frames.
- acceptance: A new-weapon card has the stat card size and shows "New weapon" on top, the weapon icon only, the weapon name, its explanation and a lore line, inside the owner-chosen unique frame distinct from the rarity frames; every subtask done_when passed; docs updated; lint clean; committed. Visual result not seen by the crew.
- gdd_check: upgrades, sys-upgrades, wbp-upgrade-choice, sys-text-content read (2026-10-14). No conflict. Only the shotgun and the turret launcher are offered (burst rifle is the start weapon). A cpp subtask is needed to load the weapon lore (consequence of the plan, reported to the owner). Owner defaults accepted 2026-10-14.
- confirmed_by_owner: yes (2026-10-14)
- milestone: M2
- next_step: task-169-02-weapon-card-frame
- result: open

## Subtasks
1. task-169-01-weapon-lore-lines — text
2. task-169-02-weapon-card-frame — art
3. task-169-03-weapon-card-lore — cpp
4. task-169-04-weapon-card-layout — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-169-01-weapon-lore-lines]] — step 1, text.
- [[task-169-02-weapon-card-frame]] — step 2, art.
- [[task-169-03-weapon-card-lore]] — step 3, cpp.
- [[task-169-04-weapon-card-layout]] — step 4, bp.
