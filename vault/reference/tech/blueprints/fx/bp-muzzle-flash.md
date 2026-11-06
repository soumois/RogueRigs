---
id: bp-muzzle-flash
description: "NS_MuzzleFlash (green rifle), NS_MuzzleFlashTurret, NS_MuzzleFlashShotgun (white-gold) and their materials: the stylised flash per shot; open for emitters, colours, sizes, spawn rule."
parent: blueprints-fx-index
kind: doc
---
# NS_MuzzleFlash

- asset_path: /Game/RogueRigs/FX/NS_MuzzleFlash (rifle; sizes x0.7 since task-476-01, commit af6e7912), /Game/RogueRigs/FX/NS_MuzzleFlashTurret (turrets; task-476-01: copy of NS_MuzzleFlash as it was after task-464, so the turret flash keeps its size), /Game/RogueRigs/FX/NS_MuzzleFlashShotgun (copy, sizes x1.5 of the pre-476 rifle), /Game/RogueRigs/FX/M_MuzzleFlash (shotgun, white-gold), /Game/RogueRigs/FX/M_MuzzleFlashGreen (rifle and turrets; task-464-03, commit 468bd765) (task-440-01, commit c94a6a10; first version task-414-01). Niagara systems, no Blueprint.
- parent_class: UNiagaraSystem
- purpose: stylised flash at the muzzle of the miner's hand weapon on every shot and at the turret barrel on every turret shot ([[art-style-guide]] A8). Shape reference only, not shipped: `Art/Source/muzzle-flash/20261105-owner-ref-video-1328-1333-sheet.png`. Colour reference of the green flash (style example, keep): `Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp` ([[color-code]], owner 2026-11-05). Spawned by C++, see spawn.
- user_params: none. No light (sprite renderers only).
- emitters (CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous, M_MuzzleFlash, VelocityAligned, FaceCamera, no shadows, sprite rotation 0, `Scale Alpha` = `Particles.NormalizedAge`). All values provisional, the owner tunes them in game:
  - `Core` (sort 1): 3 sprites, lifetime 0.09 s, `Particles.Velocity` (10, 0, 0) to point the sprite along +X. Exec index 0 streak, 1 shape, 2 core, drawn in that order. Rifle sizes random between min and max (task-476, owner 2026-11-06 "un peu trop grand": x0.7, provisional): streak 11.2 x 210 to 252 uu, shape 119 to 140 uu, core 35 to 42 uu. Turret (NS_MuzzleFlashTurret, the pre-476 rifle values): 16 x 300 to 360, 170 to 200, 50 to 60. Shotgun: 24 x 450 to 540, 255 to 300, 75 to 90.
  - `Sparks` (sort 0): lifetime 0.06 to 0.1 s, cone +X 22 deg, drag 0.5. Rifle 3 sprites, 5.6 to 8.4 x 35 to 63 uu, speed 280 to 490 (x0.7, task-476); turret 8 to 12 x 50 to 90 uu, speed 400 to 700; shotgun 5 sprites, 12 to 18 x 75 to 135 uu, speed 400 to 700.
  - size rule (task-476-01): Niagara sprite sizes do not follow the component scale (the turret flash at relative scale 0.6 showed full-size sprites), so a size change is made in the system's `InitializeParticle` Sprite Size Min/Max and `AddVelocity` speed range, and the turret has its own copy.
- sprite_count: rifle 6, shotgun 8 (was 5; bound 10).
- material: M_MuzzleFlash, Translucent, Unlit, two sided, used with Niagara sprites, no texture; one Custom node with inputs UV and C = ParticleColor RGBA; RGB to emissive, alpha to opacity; edges anti-aliased with fwidth. ParticleColor carries data, not a colour: R shape variant (4 equal ranges), G random seed (mirror, lengths, angle), B kind (0 shape, 0.5 core, 1 line), A progress. Colours are fixed in the code: gold (2.4, 1.45, 0.2) and white (4.5, 4.2, 3.0), so no system can tint it. M_MuzzleFlashGreen (task-464-03) is a copy with the same shapes and these colours instead: Green (0.45, 3.2, 0.12) rim, Pale (2.8, 4.4, 1.9) fill and core, Dark (0.02, 0.35, 0.01) glow outside the shapes, alpha 0.7 within 0.26 of the edge (shapes, core) or 0.08 (streak, shards), fading from progress 0.35 to 0.85; the core halo is dark green too. NS_MuzzleFlash renders it on both emitters; NS_MuzzleFlashShotgun keeps M_MuzzleFlash. All provisional, owner tunes in game. Sprite up (UV V = 0) is the velocity direction, so forward = (0.5 - V) x 2.
- shapes (variant random per shot): forward flame with barbs (white fill, gold rim), curved side wings with a short forward blade, four crossed leaf blades, star of seven uneven spikes. Blades grow from 0.72 to 1 by progress 0.2 and thin to nothing from 0.45 to 1. Core: jagged white disc with a faint halo, gone by progress 0.75. Line (streak and sparks): thin diamond in the forward half of the sprite.
- spawn: `ARRWeapon::MuzzleFlashSystem` defaults to NS_MuzzleFlash in the constructor, so [[bp-burst-rifle]] is unchanged; [[bp-shotgun]] sets NS_MuzzleFlashShotgun. `SpawnMuzzleFlash` and its rule: [[sys-weapons]]. Rifle: one per bullet (3 per burst). Shotgun: one per volley ([[sys-weapons-shotgun]]). Turret: [[bp-turret]] OnTurretFired spawns NS_MuzzleFlashTurret at its Muzzle, relative scale 0.6 (task-464-04; NS_MuzzleFlash before task-476-01). Robots: none.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- verified: compile UpToDate, 0 errors, 0 warnings, 3 version info notes on each system; values above read back. `RogueRigs.Weapons.MuzzleFlashAsset` under `-RenderOffScreen`: 6 and 8 sprites, 4 of 4 shapes over 16 shots, shape up to 199 and 295 uu, all gone within 8 frames of 1/60 s, no material compile error, BP defaults. Weapons, Shotgun, Turret: 29 tests pass under `-nullrhi`.
- unverified: the look in game (no capture, the owner judges it, task-440 A7), including the emissive strength and the sizes at the game camera. Formula preview, not a game frame: `Saved/Scratch/muzzle-flash/20261105-formula-preview-not-a-game-frame.png` (temporary). A flash playing while the world is paused at level end.
- verified_464: slow-motion -game frames (slomo 0.15, `Saved/Captures/task464_03_check/z.png`, `task464_04_check/flash.png`, not committed): green rifle and turret flashes with pale centre, bright green blades and a soft dark green halo. Not seen in a frame: the shotgun white-gold flash after the change (asset unchanged).
- verified_476: slow-motion -game frames before and after, same fight and camera (`Saved/Captures/task476_compare_crop.png`, `task476_compare_turret2.png`, not committed): rifle star smaller with the same tones and shapes, turret and shotgun flashes unchanged. Compile UpToDate on both green systems. `RogueRigs.Weapons.MuzzleFlashAsset` not rerun (asset-only change; its shotgun >= 1.3 x rifle check still holds by the values).
- open_observation: [[bp-shotgun]] still shows its older hidden sphere `MuzzleFlash` (r 20, MI_GB_Shot) for 0.06 s on OnShotFired; task-440 did not change it.
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-weapons]] — the C++ property and spawn rule.
- [[sys-weapons-shotgun]] — one flash per volley.
- [[bp-burst-rifle]] — its Muzzle hosts the flash.
- [[bp-shotgun]] — sets the larger system.
- [[art-style-guide]] — rule A8 of the look.
- [[task-414-muzzle-flash]] — the task that made the first version.
- [[task-440-muzzle-flash-rework]] — the rework.
- [[task-464-player-shot-colour]] — green rifle flash, turret flash.
- [[task-476-muzzle-flash-smaller]] — rifle flash x0.7, turret copy.
- [[bp-turret]] — spawns the green flash at its barrel.
- [[color-code]] — the green and white-gold rows.
