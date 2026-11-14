---
id: bp-dig-debris
description: "NS_DigDebris, the one-shot burst of block-coloured debris when the drone breaks a block; open for its User parameters, emitter values and bounds."
parent: blueprints-fx-index
kind: doc
---
# NS_DigDebris

- asset_path: /Game/RogueRigs/FX/Dig/NS_DigDebris (Niagara system, no Blueprint; task-438-02, made from NS_ShotImpactSpark); material /Game/RogueRigs/FX/Dig/M_DigChip ([[bp-dig-impact]])
- parent_class: UNiagaraSystem
- purpose: debris thrown by each block the drone breaks; they fall and are gone within 0.8 s, nothing stays on the floor ([[drone]], [[color-code]], [[art-style-guide]] A14).
- user_params (exact names, set before activation):
  - `User.ChipColor`, `User.ChipColor2`, `User.ChipColor3`: LinearColor, the three tones of the burst, defaults the rock debris tones lilac (0.12, 0.075, 0.2), mid violet (0.06, 0.015, 0.15), dark violet (0.025, 0.006, 0.06) (`RRBlockColors::RockDebrisTones`, [[color-code]] "Rock destruction debris", owner 2026-11-13, provisional; was one colour (0.35, 0.28, 0.5) that rendered near white, [[task-543-wall-debris-less-white]]). InitializeParticle Color is the HLSL `lerp(lerp(User.ChipColor, User.ChipColor2, saturate(fmod((float)Engine.ExecIndex, 3.0))), User.ChipColor3, saturate(fmod((float)Engine.ExecIndex, 3.0) - 1.0))`: one tone per particle in turn, 4 of each in a burst of 12. The drone sets the three rock tones for rock and the block colour three times for ore, heal ore and barrel, so those look as before. Unlit and below 1, so no bloom.
  - `User.ThrowDirection`: Vector3f, default (1, 0, 0). Horizontal unit vector, world space; the cone axis is `normalize(User.ThrowDirection + float3(0, 0, 1.1))`, so up and that way.
- emitters: one, keeps the template name `Sparks` (no rename tool used): CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous 12 (Emitter Update), lifetime 0.55 to 0.8 s, ShapeLocation sphere radius 28, AddVelocity cone 55 deg, speed 160 to 420, gravity Z -1400, drag 0.4, non-uniform size (9, 7) to (22, 16) uu, random rotation, SpriteRotationRate `Particles.MaterialRandom * 1400 - 700` deg/s, ScaleSpriteSize curve 1 until 0.7 of the life then 0. Renderer: sprite, Automatic alignment and facing, M_DigChip (opaque unlit, a sprite passing under the floor is hidden), no shadows. All values provisional: the owner tunes them in game.
- spawn: `ARRDrone::DigDebrisSystem`, through the Niagara pool ([[sys-drone-dig]]). Spawn rotation zero.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: M_DigChip (no new material).
- reuse: any one-shot debris of a block colour in a given direction: set the two parameters at spawn.
- verified: compile UpToDate, 0 errors, 0 warnings, 12 and 0.8 s read back; `RogueRigs.Dig.DigDebrisAsset` under `-RenderOffScreen`: six broken blocks play six bursts, back in the pool after 48 frames of 1/60 s (0.8 s), no component created at the break.
- verified_task_543: `RogueRigs.Dig.DigDebrisAsset` reads the particle colours from the simulation (4 lilac, 4 mid, 4 dark violet); capture `Saved/Captures/task543-after3/sheet_dig.png`: lilac and violet chips at the broken block, none near white, no halo, amount and size as before. MCP pitfall met: the HLSL edit stayed out of the compiled script until one more stack edit (Lifetime Min) followed it ([[unreal-mcp-quirks]]).
- unverified: cost in play (bounded by construction, no stress run).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-drone-dig]] — the C++ trigger, pool cap and spawn point.
- [[bp-dig-impact]] — the looping laser impact effect, unchanged, and the chip material.
- [[bp-shot-impact-spark]] — the template it was made from.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[task-438-dig-debris]] — the task that made it.
- [[task-543-wall-debris-less-white]] — the task that gave it the three rock tones.
