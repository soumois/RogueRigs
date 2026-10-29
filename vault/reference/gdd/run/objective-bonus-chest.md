---
id: objective-bonus-chest
description: "Optional objective: a chest the drone hacks; always rewards ore and one upgrade choice with a temporary luck bonus."
parent: run-index
kind: spec
status: approved
---
# Objective: bonus chest

- Triggered by an alert tier ([[level-flow]]). Optional: it does not pause the alert bar.
- Hacked by the drone with the same rules as the console ([[objective-console]]).
- Reward, every time:
  - no direct XP (owner 2026-10-23: one upgrade, not two; was 50 % of the level need);
  - 3 big XP chips of 8 nuts each, thrown in evenly spread directions, each popping in with a scale overshoot 0 to 1.3 to 1.0 in 0.25 s (owner 2026-10-23, task-309, provisional; replaces the direct XP);
  - ore;
  - an upgrade choice drawn with a temporary luck bonus, applied to that draw only ([[rarities]]).
- Values (owner 2026-10-07, provisional): one chest per level, dropped when the alert reaches 60 %, 30–45 cells (67 uu) from the miner on a free 2 × 2 floor footprint; hack 8 s with the console rules (zone radius 6 cells, progress drains when the miner leaves); no marked rocks; reward no direct XP (owner 2026-10-23: one upgrade, not two; was 50 % of the level need, owner 2026-10-08), 5 ore chunks, one upgrade choice with +3 luck for that draw only. The locator arrow keeps the console or shuttle; the chest shows as a gold icon on the alert bar.
- Opening (owner 2026-10-23, task-309, provisional): on hack complete the latch swings 90° outward and up about its top edge in 0.2 s; 0.5 s after the completion the lid opens up and backward about its hinge to 115° in 0.4 s (ease-out), the chest alone freezes 0.06 s (the rest of the game runs), then it settles to 100° in 0.15 s and stays open. At the freeze start a ground ring plays, at its end a light camera shake. At the lid opening the interior glows (point light for 3 s then fades out; the interior stays lit), rays and sparks play, and the reward is given (chips, ore, upgrade choice).

- Milestone: M2 (provisional).

- Lock light (owner 2026-10-23, task-308, provisional): glows in every state; red blinking 2 Hz while waiting; orange blinking while the miner is in the zone, faster as the hack progresses, 2 Hz at 0 % to 6 Hz at 100 %, in steps 2, 3, 4, 6 Hz over equal quarters of the progress (a continuous change makes the blink jump); back to red 2 Hz when the miner leaves; steady green once open. Colours: the console state lamps ([[color-code]]).

- Zone look (owner 2026-10-12): the same ground circles as the console zone, at the chest zone radius ([[objective-console]], [[ground-telegraph]]).

## Relations
- [[run-index]] — parent index.
- [[upgrades]] — what the choice offers.
- [[art-prompts-chest]] — the chest look: owner-validated turnaround sheet and prompt.
