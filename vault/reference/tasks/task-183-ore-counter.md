---
id: task-183-ore-counter
description: "Global task: carried ore counted, secured on extraction, lost on a miss or death, shown with an icon next to the radar."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Ore counter

- request: Code Architect run 1 (`/architect`, assignments/A5-goal-oriented-coding-agent/blackboard.md) recommended the ore counter; owner answer: "Ok, juste pour l'info, tu peux la mettre à coté du radar. Pour cela, il te faudra aussi l'icone et demande au passage un rendu visuel proposé par l'agent Art in game ( il en aura besoin pour créer l'icone HUD )" (owner, 2026-10-14)
- goal: ore chunks collected during the level add to a carried count; a successful extraction moves it to a secured total kept in the game instance for the session; a missed shuttle or the miner's death empties the carried count; a HUD panel next to the radar shows the ore icon and the carried count; the success screen shows the secured ore; cheat `rr.Cheat.GrantOre <N>`.
- acceptance: `rr.Cheat.GrantOre 10` shows 10 next to the radar; boarding the shuttle shows 10 secured on the success screen; letting the countdown expire (`ShuttleCountdownOverride`) or dying sets the carried count to 0; build clean; existing tests pass; docs updated; committed.
- gdd_check: [[ore-economy]] and [[extraction]] (approved, M1) read 2026-10-14. No conflict. `open` and not built: share of secured ore lost on a failed run. Owner defaults accepted with "ok": secured ore in the game instance, death loses carried ore, one dedicated widget (placed next to the radar). Colour: Duskite violet ([[color-code]]).
- confirmed_by_owner: yes (2026-10-14: "Oui tu peux lancer toutes les tâches")
- milestone: M1
- next_step: task-183-04-ore-counter-widget
- result: open

## Subtasks
1. task-183-01-ore-counter-logic — cpp
2. task-183-02-ore-hud-render — art
3. task-183-03-ore-hud-icon — art
4. task-183-04-ore-counter-widget — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-183-01-ore-counter-logic]] — step 1, cpp.
- [[task-183-02-ore-hud-render]] — step 2, art.
- [[task-183-03-ore-hud-icon]] — step 3, art.
- [[task-183-04-ore-counter-widget]] — step 4, bp.
- [[ore-economy]] — the rule it implements.
