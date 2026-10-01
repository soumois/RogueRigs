---
id: objective-console
description: "Mandatory objective: a command console dropped near the miner, marked rocks to clear, then a drone hack that unpauses the alert bar."
parent: run-index
kind: spec
status: approved
---
# Objective: command console

- Triggered by an alert tier; at least one per level ([[level-flow]]).
- The console is dropped outside the miner's view radius, but not at the far end of the level.
- The rocks around it are marked and must be destroyed before hacking. Only the drone and explosions break rock ([[destruction-rules]]), so clearing them costs digging time.
- Hack rules:
  - the drone hacks only while the miner stays inside the console zone;
  - if the miner leaves, the drone follows him and hacking stops; progress drains slowly;
  - the console shows its gauge, so the player sees why progress stops.
- The drone hacks one target at a time and cannot dig and hack at once.
- While the console is not hacked, the alert bar is paused.
- Open: drop distance range, marked-rock radius, marker look (must follow [[color-code]]), hack duration, drain rate, zone radius.
- Lore (provisional): the Rigs' ship detects an entry point into the claim's network and drops the console there as an access point ([[lore]]).
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[drone]] — the drone's hack state.
- [[objective-bonus-chest]] — the optional hack objective.
