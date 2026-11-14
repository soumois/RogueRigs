---
id: bp-explosion-wall-debris
description: "NS_ExplosionWallDebris, the one-shot burst of at most 60 block-coloured sprites thrown away from an explosion centre; open for its User parameters and values."
parent: blueprints-fx-index
kind: doc
---
# NS_ExplosionWallDebris

- asset_path: /Game/RogueRigs/FX/NS_ExplosionWallDebris (Niagara system, no Blueprint; task-439-02, made from NS_DigDebris); material /Game/RogueRigs/FX/Dig/M_DigChip ([[bp-dig-impact]])
- parent_class: UNiagaraSystem
- purpose: debris of the blocks an explosion breaks: each particle leaves from a broken block and flies straight away from the explosion centre, then falls and is gone within 1.5 s ([[explosions]], [[color-code]], [[art-style-guide]] A14). One effect per explosion, never one per block.
- user_params (exact names, set before activation, [[sys-explosions]]):
  - `User.ExplosionCenter`: Position, world space. Direction source.
  - `User.DebrisPositions`: Array Position, one world start point per particle.
  - `User.DebrisColors`: Array Color, one colour per particle. A rock particle gets one of the three rock debris tones at random, lilac (0.12, 0.075, 0.2), mid violet (0.06, 0.015, 0.15), dark violet (0.025, 0.006, 0.06) (`RRBlockColors::DebrisToneForType`, [[color-code]], owner 2026-11-13, provisional, [[task-543-wall-debris-less-white]]; was one near-white rock colour); ore, heal ore and barrel keep their block colour. No asset change.
  - `User.DebrisCount`: Int32, particles to spawn.
- emitters: one, keeps the template name `Sparks` (no rename tool used): CPU sim, world space (`bLocalSpace` false), dynamic bounds, Loop Once 0.1 s, SpawnBurst_Instantaneous count HLSL `min(User.DebrisCount, 60)`. All values provisional: the owner tunes them in game.
- spawn: InitializeParticle (lifetime 0.9 to 1.4 s, non-uniform size (14, 10) to (34, 24) uu, random rotation), then one SetParameters module per step, because entries of one module read the old values: `Particles.DebrisIdx` = `Engine.ExecIndex`; `Particles.Position` and `Particles.Color` = SelectPositionFromArray and SelectLinearColorFromArray, Direct Set at `Particles.DebrisIdx` (the [[bp-damage-numbers]] pattern); `Particles.DebrisDir` = horizontal unit vector from `User.ExplosionCenter` to the particle; `Particles.Velocity` = direction with a sideways spread of up to tan 8.5 deg, times 550 to 1100 uu/s, plus 250 to 500 uu/s upward.
- update: as [[bp-dig-debris]]: gravity Z -1400, drag 0.4, sprite rotation rate, size curve 1 until 0.7 of the life then 0. No collision: a sprite passing under the floor is hidden by it, so nothing stays on the floor.
- renderer: sprite, Automatic alignment and facing, M_DigChip (opaque unlit), no shadows.
- spawn_by: `URRExplosionSubsystem::PlayWallDebris`, through the Niagara pool, at the explosion centre, rotation zero ([[sys-explosions]]).
- components: none (Niagara asset). No events.
- logic_in_bp: none
- assets: M_DigChip (no new material).
- verified: compile UpToDate, 0 errors, 0 warnings, 2 version info notes. `RogueRigs.Explosions.WallDebrisAsset` under `-RenderOffScreen`, particles read from the simulation: 60 particles from a 1500 uu test explosion, each near a requested point in a requested colour, flying upward, smallest cosine to the outward direction 0.990; back in the pool after 84 frames of 1/60 s (1.4 s).
- unverified: cost in play (bounded by construction, no stress run). Look (task-543 capture `Saved/Captures/task543-after3/sheet_bar.png`): no white flakes left; just after the flash the rock debris reads dark violet with low contrast on the dark floor.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-explosions]] — the C++ request, the bound and the pool cap.
- [[explosions]] — the design rule.
- [[bp-dig-debris]] — the system it was made from.
- [[bp-damage-numbers]] — the array read by index it reuses.
- [[bp-barrel-explosion]] — the burst played with it, unchanged.
- [[sys-actor-pool]] — the Niagara pool that plays it.
- [[task-439-explosion-wall-debris]] — the task that made it.
- [[task-543-wall-debris-less-white]] — the task that gave rock particles the three rock tones.
