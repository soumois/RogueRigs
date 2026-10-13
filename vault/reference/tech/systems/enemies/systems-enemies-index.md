---
id: systems-enemies-index
description: "Robots, AI, spawner and wave director: everything about the enemies."
parent: systems-index
kind: note
---
# C++ systems — enemies

Robots, AI, spawner and wave director: everything about the enemies.

## Relations
- [[systems-index]] — parent index.
- [[sys-enemies]] — robots index: base and shared state-machine helpers, light movement, chaser, kamikaze, EnemyStats.csv.
- [[sys-enemy-ai]] — AI framework choice, pathfinding, line of sight, stun reaction.
- [[sys-spawner]] — enemy spawner actor: spawn band outside the camera view, alive cap, recycling, tunables; its child sys-spawner-director holds the wave director (phases, packs, hordes, overtime, last-level profile).
- [[sys-spawner-director]] — system
