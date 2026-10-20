---
id: task-246-card-pick-destination
description: "Global task: The chosen card icon flies to the HUD slot of its weapon, splits to every owned slot for an all-weapons upgrade, flies to the XP bar for a player stat; the other cards dissolve."
parent: tasks-done-index
kind: task
status: done
---
# Card pick destination

- request: Point 4 of the card juice: destination of the pick; targeted upgrade goes to that weapon slot, global splits to each owned slot, player stats to the XP bar, new weapon to its new slot, dissolve of the unchosen cards (owner 2026-10-19) (owner, 2026-10-19)
- goal: The chosen card icon flies to the HUD slot of its weapon, splits to every owned slot for an all-weapons upgrade, flies to the XP bar for a player stat; the other cards dissolve.
- acceptance: every subtask done_when passed and its frames or images shown to the owner; committed.
- gdd_check: xp-level-up, upgrades, rarities, readability-rules read (2026-10-19); no conflict, no open value; the GDD section on the card animation is added after the last card task (owner default, provisional).
- confirmed_by_owner: yes (2026-10-19)
- milestone: M2
- next_step: none
- result: Pick of a card: 0.55 s animation in the paused game, icon flight to the weapon slot, split to every owned slot, XP bar for a player stat, first free slot for a new weapon, dissolve of the discarded cards in the tier colour, then the pick is applied. Fire edge pale yellow (owner to judge); see 246-03.

## Subtasks
1. task-246-01-card-pick-destination — bp
2. task-246-02-card-pick-destination — bp
3. task-246-03-card-pick-destination — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-246-01-card-pick-destination]] — step 1, bp.
- [[task-246-02-card-pick-destination]] — step 2, bp.
- [[task-246-03-card-pick-destination]] — step 3, cpp.
