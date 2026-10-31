---
id: bp-burst-rifle
description: "BP_BurstRifle, Blueprint of ARRWeapon for the burst rifle: SM_BurstRifle mesh, muzzle position, hand socket, projectile class. Open to see its defaults."
parent: blueprints-player-index
kind: doc
---
# BP_BurstRifle

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle
- parent_class: ARRWeapon (/Script/RogueRigs.RRWeapon)
- purpose: the burst rifle weapon actor; visuals and references only
- components: inherited Root, Muzzle (relative location 41.8, 0, 0; was 40), auto-fire, ASC; added RifleMesh under the root: StaticMesh SM_BurstRifle ([[burst-rifle-mesh]]), relative location 0, scale 1, collision profile NoCollision (was BlockAllDynamic; CastShadow unchanged, true). Mesh 60 x 7.3 x 20.7 cm, pivot at the grip, muzzle +X
- exposed_defaults: StatsRowName = BurstRifle; ProjectileClass = BP_Projectile_C; HandSocketName = WeaponSocket_R (task-400, socket on the miner mesh, [[bp-miner]]); other C++ defaults kept
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Weapons/BurstRifle/ SM_BurstRifle, M_BurstRifle (4 texture samples to BaseColor, Normal, Metallic, Roughness), T_BurstRifle_BaseColor (sRGB), _Normal (normal map), _Metallic and _Roughness (sRGB off, TC_Masks); MI_GB_Shot
- owner_agent: agent-bp
- verified: compiled and saved. Headless (task-007-05): timing, magazine, projectile stop on rock and dummy damage pass when spawned alone. Superseded by task-008-03: BP_Miner now uses no socket (capsule offset). Before, at `hand_r` the socket was pitched about 84°, so the rifle +X and the Muzzle offset point up: muzzle 130 uu above the floor, above the 100 uu blocks; shots would fly over rock (fix open: socket, attach rotation or Muzzle offset).

- task-400 PIE: parent = BP_Miner_C CharacterMesh0, rifle world scale 1, bounds 60 x 7.3 x 20.7, projectiles spawn at z 50 (shot origin unchanged, [[sys-weapons]]). Open observation (capture RifleInHand2, not a decided value): rifle shouldered beside the head and follows the miner, no grey cube; it renders flat cream-white, so colours were not readable (possible material or lighting issue, owner to judge); two other capture checks (C4, C5) not seen or ambiguous. Material compile success and collision shape read-back unverified. Left-hand IK is task-401, not done.

## Relations
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from.
- [[bp-miner]] — owns WeaponSocket_R.
- [[burst-rifle-mesh]] — source and game mesh of SM_BurstRifle.
- [[task-400-burst-rifle-in-hand]] — task that put the mesh in the hand.
- [[sys-automation-tests]] — RogueRigs.Weapons.* tests using it, incl. the failing muzzle height check.
