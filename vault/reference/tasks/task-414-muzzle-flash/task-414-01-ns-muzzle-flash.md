---
id: task-414-01-ns-muzzle-flash
description: "Bp: create NS_MuzzleFlash (one-shot white-gold core and forward sparks, and its sprite material if needed) in Content/RogueRigs/FX/"
parent: task-414-muzzle-flash
kind: task
status: done
---
# Muzzle flash on every miner shot: bp

- goal: Per the global Questions as decided (Q4 look). Create the Niagara system /Game/RogueRigs/FX/NS_MuzzleFlash (and a sprite material or instance only if no existing one fits, in the same folder): one-shot burst, local space, oriented so +X of the attach point is the shot direction; a bright white-gold core sprite about 25 uu wide lasting about 0.06 s and a few short sparks thrown forward along +X; no light, no loop; system finishes and can auto-destroy; low particle count (fires about 4 to 5 times per second). Compile and save; touch no other asset (never BP_BurstRifle or ABP_Miner).
- agent: bp
- depends_on: none
- inputs: task-414-muzzle-flash (vault/reference/tasks/task-414-muzzle-flash.md), art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), bp-shooter-charge (vault/reference/tech/blueprints/fx/bp-shooter-charge.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: NS_MuzzleFlash.uasset (and its material if created) saved; one commit `task-414-01-ns-muzzle-flash: <summary>` by path; a change summary for the cpp and doc steps (asset paths, emitters, durations, sizes, colours, user parameters if any, commit hash).
- done_when: NS_MuzzleFlash compiles without error and is saved in Content/RogueRigs/FX/; read-back shows one-shot (no loop), white-gold colour, core about 25 uu and about 0.06 s, forward sparks along +X, no light renderer; only the new assets staged; committed by path.
- editor_state: open
- token_ceiling: 1.0 M units
- model: sonnet
- tokens: input 50, cache write 65833, cache read 1020905, output 17684
- usage: turns 25, context start 11k peak 69k, resumes 0, 3 min, units 0.27 M
- runs: 1
- milestone: M2
- started: 2026-10-31T22:17:02Z
- waiting_for: none
- result: NS_MuzzleFlash created, compiled, saved (2dcc85fe); core 25uu 0.06s, 4 forward sparks, no light; look unverified

## Relations
- [[task-414-muzzle-flash]] — parent global task.
