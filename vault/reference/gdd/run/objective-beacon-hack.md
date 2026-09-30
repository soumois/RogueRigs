---
id: objective-beacon-hack
description: "Default level objective: the drone hacks the claim beacon only while the miner stays in the zone; interruption drains progress slowly."
parent: run-index
kind: spec
status: approved
---
# Objective: beacon hack

- The drone hacks the claim beacon only while the miner stays inside the objective zone.
- If the miner leaves the zone, the drone follows him and hacking stops.
- During an interruption, hack progress drains slowly.
- The beacon shows its gauge, so the player sees why progress stops.
- The drone cannot dig and hack at the same time: every block dug costs objective time. This is the central trade-off of a level.
- Completing the hack triggers [[extraction]].
- Values: hack duration, drain rate, zone radius → open.
- Milestone: M1.

## Relations
- [[run-index]] — parent index.
- [[drone]] — the drone's hack state.
- [[objective-variants]] — the other objectives.
