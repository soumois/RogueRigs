---
id: task-303-02-bp-hack-complete-fx
description: "NS_ConsoleHackComplete and its wiring (bp): console hack reward: xp burst and big particle, step 02."
parent: task-303-console-hack-reward
kind: task
status: done
---
# NS_ConsoleHackComplete and its wiring

- goal: One-shot Niagara NS_ConsoleHackComplete: a green ring expanding past the console then fading, plus white-gold sparks, sized to the task-311 console (3 x 3 cells of 67 uu, about 201 x 172 x 147 uu at scale 1.361): ring from about the footprint edge (radius ~110 uu) out to about 3 times that, inside the 7-cell hack zone (469 uu), sparks rising above the 147 uu console top; spawned once by BP_ObjectiveConsole on hack complete; doc node bp-console-hack-complete under blueprints-fx-index and bp-objective-console updated.
- agent: bp
- depends_on: task-303-01-console-xp-burst
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-objective-console (vault/reference/tech/blueprints/world/bp-objective-console.md), color-code (vault/reference/gdd/art-audio/color-code.md), objective-console (vault/reference/gdd/run/objective-console.md); files: none
- expected_output: /Game/RogueRigs/FX/NS_ConsoleHackComplete, BP_ObjectiveConsole wiring, doc nodes
- done_when: compiles without error; values read back; wired event read back; doc nodes and index relation; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: sonnet
- tokens: input 60, cache write 88999, cache read 1645651, output 20642
- usage: turns 30, context start 10k peak 89k, resumes 0, 8 min, units 0.38 M
- runs: 1
- milestone: M2
- result: NS_ConsoleHackComplete (CPU, one burst): green ring 64 sprites at 600 uu/s, 0.55-0.6 s, colour (0.2,1.6,0.45), radius ~110 -> ~330 uu; 40 white-gold sparks (2.0,1.6,0.6) cone 25 deg up 250-450 uu/s, rise ~150-350 uu; BP_ObjectiveConsole OnHackCompleteBurst -> SpawnSystemAtLocation; compiled, read back; doc bp-console-hack-complete (32a6b03c, 7cc67160). Editor close blocked by a Save Content dialog (unknown dirty package, git clean): owner 2026-10-24 "Ferme l'éditeur sans sauvegarder", editor force-stopped without saving.

## Relations
- [[task-303-console-hack-reward]] — parent global task.
