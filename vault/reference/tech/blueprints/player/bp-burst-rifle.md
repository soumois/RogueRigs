---
id: bp-burst-rifle
description: "BP_BurstRifle, Blueprint of ARRWeapon for the burst rifle: SM_BurstRifle mesh, muzzle position, hand socket, projectile class. Open to see its defaults."
parent: blueprints-player-index
kind: doc
---
# BP_BurstRifle

- asset_path: /Game/RogueRigs/Blueprints/Weapons/BP_BurstRifle
- parent_class: ARRWeapon (/Script/RogueRigs.RRWeapon)
- purpose: the burst rifle weapon actor, the only shown player weapon; its Muzzle is the shot origin of all the miner's weapons (task-412-01, commit e1e65788); mesh, socket and Muzzle position are set here, shot logic is C++
- components: inherited Root, Muzzle (relative location (52.25, 0, 0) since task-409-04, was (62.148091, -1.129037, -13.0) from task-404, (41.8, 0, 0) before, 40 first; set on the CDO Default__BP_BurstRifle_C:Muzzle, under the root not under RifleMesh, since task-412-01 the shot origin of every player weapon: shots leave from its world location, [[sys-weapons]]), auto-fire, ASC; added RifleMesh under the root: StaticMesh SM_BurstRifle ([[burst-rifle-mesh]]), relative location (0, 0, 0) since task-409-04, rotation (0, 0, 0), scale (1.25, 1.25, 1.25) kept (owner 2026-10-31, task-404 had location (9.898091, -1.129037, -13.0); Muzzle x = 1.25 x 41.8 = 52.25), collision profile NoCollision (was BlockAllDynamic; CastShadow unchanged, true), OverrideMaterials empty (task-403; was [MI_GB_Shot]), so slot 0 uses M_BurstRifle from the mesh. Mesh 60 x 7.3 x 20.7 cm, pivot at the grip, muzzle +X
- exposed_defaults: StatsRowName = BurstRifle; ProjectileClass = BP_Projectile_C; HandSocketName = WeaponSocket_R (task-400, socket on the miner mesh, [[bp-miner]]); other C++ defaults kept
- events: none
- logic_in_bp: none
- assets: /Game/RogueRigs/Weapons/BurstRifle/ SM_BurstRifle, M_BurstRifle (4 texture samples to BaseColor, Normal, Metallic, Roughness), T_BurstRifle_BaseColor (sRGB), _Normal (normal map), _Metallic and _Roughness (sRGB off, TC_Masks). MI_GB_Shot (greybox MI, parent M_GreyBox, Color 1.0/0.85/0.45) is no longer used by this BP; no C++ reference to it
- owner_agent: agent-bp
- verified: compiled and saved. Headless (task-007-05): timing, magazine, projectile stop on rock and dummy damage pass when spawned alone. Superseded by task-008-03: BP_Miner now uses no socket (capsule offset). Before, at `hand_r` the socket was pitched about 84°, so the rifle +X and the Muzzle offset point up: muzzle 130 uu above the floor, above the 100 uu blocks; shots would fly over rock (fix open: socket, attach rotation or Muzzle offset).

- task-400 PIE: parent = BP_Miner_C CharacterMesh0, rifle world scale 1, bounds 60 x 7.3 x 20.7, projectiles spawned at z 50 then (shot origin was floor + ShotHeight; replaced by the Muzzle in task-412, [[sys-weapons]]). Open observation (capture RifleInHand2, not a decided value): rifle shouldered beside the head and follows the miner, no grey cube; it rendered flat cream-white; two other capture checks (C4, C5) not seen or ambiguous. Collision shape read-back unverified.

- task-403 (commit 988cbeec): cause of the flat cream-white = RifleMesh OverrideMaterials was [MI_GB_Shot] on the only slot, replacing M_BurstRifle (likely a leftover of the old greybox cube). Fix: OverrideMaterials = [] on the BP default object (read back []), BP compiled (returned no error) and saved; M_BurstRifle only recompiled (graph unchanged), saved. Ruled out in editor: M_BurstRifle is DefaultLit, Opaque, 4 samples wired right (BaseColor sampler Color sRGB TC_Default; Normal TC_Normalmap sRGB off; Metallic and Roughness masks sRGB off TC_Masks; Q2: masks stay sRGB off), emissive and specular unconnected, recompile without error (compile log not readable: compile rests on the recompile returning no error), SM_BurstRifle one slot M_BurstRifle with UV0. Level instances with their own override not checked. Capture (-game, `Saved/Captures/RifleColours403/`, 1920x1080, arm 350): C1 seen (teal, dark grey, rust patch, no longer flat cream-white), C2 ambiguous (rifle tiny, miner head overlaps; owner to judge), C3 seen; frames crop_cmp.png, crop_rifle-rifle.png, contact_sheet.png. Quirks: [[unreal-mcp-quirks]]. Left-hand IK is task-401, not done.

- task-404 (commit 0717cbcc): RifleMesh and Muzzle set as above, BP compiled, saved, read back. Mesh asset stays 60 cm, 75 cm in game. Unverified: projectile z measured in PIE after the change, left hand within 3 uu of LeftHandGrip (MCP cannot read bone or socket positions), visual render (owner validates in the editor, Q3).

- task-409-04 (commit 84c03a7d): RifleMesh location reset to (0, 0, 0), scale 1.25 kept, rotation 0; Muzzle (52.25, 0, 0); BP compiled with no error, saved. The rifle keeps its task-404 place because the miner animation A_Miner_UpperRifle_V2Grip moves the right hand onto the grip instead ([[bp-miner]], [[task-409-miner-right-hand-on-grip]]). Unverified: no PIE or capture (owner checks, Q3); compile logs not read; projectile spawn height and left-hand IK not checked.

- task-411 (cpp 3b80bb41, no Blueprint change): at runtime the miner anim instance overrides the relative transform of RifleMesh and Muzzle while the right hand offset is non-zero (counter-move by the inverse offset, so the rifle stays in place); the asset values stay RifleMesh (0, 0, 0) and Muzzle (52.25, 0, 0), saved as the base and restored at offset 0 or on uninitialize ([[sys-player-classes]]). Unverified: no PIE run by the crew.

## Relations
- [[task-411-right-hand-offset-rifle-stays]] — task that added the runtime counter-move.
- [[blueprints-index]] — parent index.
- [[sys-weapons]] — the C++ class it derives from.
- [[bp-miner]] — owns WeaponSocket_R and the V2Grip animation.
- [[task-409-miner-right-hand-on-grip]] — task that reset RifleMesh and Muzzle.
- [[burst-rifle-mesh]] — source and game mesh of SM_BurstRifle.
- [[task-400-burst-rifle-in-hand]] — task that put the mesh in the hand.
- [[task-403-burst-rifle-material-fix]] — task that found the MI_GB_Shot override and cleared it.
- [[task-404-rifle-mesh-owner-offset]] — task that set the owner RifleMesh transform and moved Muzzle.
- [[unreal-mcp-quirks]] — MCP quirks met reading this BP's components.
- [[sys-automation-tests]] — RogueRigs.Weapons.* tests using it, incl. MinerStartingWeapon (shot placement at the Muzzle since task-412-01).
