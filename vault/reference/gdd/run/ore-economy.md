---
id: ore-economy
description: "Ore rules: carried ore secured only by extraction, lost on a missed shuttle, meta-progression deferred, failed-run penalty open."
parent: run-index
kind: spec
status: approved
---
# Ore economy

- Ore drops from ore blocks, whatever destroyed them ([[ore-blocks]]), and from bonus chests ([[objective-bonus-chest]]). Ore pickups use the `PickupRadius` stat.
- Ore carried during a level is secured only by a successful extraction. Missing the shuttle loses what the miner carries. This gives the risk window its stake.
- Ore feeds meta-progression between runs: deferred (the meta shop is cut from the capstone).
- Losing a share of secured ore when a run fails: open (balance parameter).
- Special ore is an objective resource, not currency ([[objective-variants]]).

## Relations
- [[run-index]] — parent index.
- [[extraction]] — the moment ore is secured.
- [[wishlist]] — the meta shop.
