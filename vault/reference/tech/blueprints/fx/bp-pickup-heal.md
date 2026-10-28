---
id: bp-pickup-heal
description: "BP_PickupHeal, Blueprint of ARRPickup for the heal pickup: three red crystal chunk meshes drawn at random, their materials, convex collision, no shadow."
parent: blueprints-fx-index
kind: doc
---
# BP_PickupHeal

- asset_path: /Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal
- parent_class: ARRPickup (/Script/RogueRigs.RRPickup)
- purpose: the visible heal pickup the grid drops from digging; visuals only, behaviour in C++ ([[sys-pickups]]).
- components: inherited root `PickupMesh` only; none added.
- exposed_defaults: PickupType = Heal; PickupMesh StaticMesh = /Game/RogueRigs/Art/HealChunk/SM_HealChunk_01, RelativeScale3D (1, 1, 1) (was the engine cube at 0.3), OverrideMaterials empty (MI_GB_PickupHeal cleared, asset kept; each variant uses its slot defaults), CastShadow false, MeshVariants = [SM_HealChunk_01, _02, _03] (task-339-04); no trail;  PopMinDistance = 25 uu and PopMaxDistance = 67 uu (task-027-02: the chunk lands within about one 67 uu cell of the broken block); other C++ defaults kept.
- events: OnCollected(Location) spawns BP_HealRing at Location, the miner position ([[bp-heal-ring]]).
- logic_in_bp: display only (one event).
- assets (task-339): meshes /Game/RogueRigs/Art/HealChunk/SM_HealChunk_01, _02, _03 (Blender, source `Tools/art/blender/heal_chunk.py`, FBX in Art/Meshes/HealChunk/; 180, 172 and 180 tris; 26.61 x 25.76 x 30, 26.76 x 25.91 x 30 and 26.55 x 25.71 x 30 cm; centred pivot; slots crystal_bright then crystal_dark; UVMap and LightMap; Nanite off; fidelity checks C1-C8 passed after 4 returns, owner accepted the preview). Materials MI_HealChunk_Bright and MI_HealChunk_Dark in the same folder, duplicated from MI_HealCrystal_Bright/Dark, parent /Game/RogueRigs/Art/XPNut/M_XPNutGlow: Bright Color and GlowColor linear (1, 0, 0.05), GlowAmount 8; Dark Color and GlowColor (0.5, 0, 0.025), GlowAmount 4; PulseAmount 0.4, PulseFrequency 0.8333, Roughness 0.6. Slot defaults on the meshes: crystal_bright = Bright, crystal_dark = Dark. MI_HealCrystal_* unchanged. MI_GB_PickupHeal (/Game/RogueRigs/Materials/GreyBox/, M_GreyBox red) is no longer used by this Blueprint.
- rest_orientation: `RestOrientation` = Full (task-192-02, owner 2026-10-15; [[sys-pickups-lifecycle]]), kept in task-339-04; `MeshVariants` was empty, now the three chunks (drawn once per life). Read back on the class default object after compile and save.
- floaty_pop (task-195-02, provisional, owner 2026-10-15): class defaults PopDuration 1.6 s, PopArcHeight 250, PopMinDistance 80, PopMaxDistance 300 (were 0.35 s, 80, 25, 67), PopFallTimeRatio 2, bAttractDuringPop true, PopAttractDelay 1.2 s; PopBounceFraction 0.25 unchanged; read back on the class default object after compile and save. Landing on unbroken rock accepted for now. task-256-02 (provisional, owner 2026-10-20): PopMaxDistance = 150 (was 300), PopMinDistance 80 kept, other defaults unchanged (same change as [[bp-pickup-ore]]); read back on the class default object after compile and save.
- simple_collision (task-339-03, tool `Tools/unreal/pickup_simple_collision.py`, best mode): one convex hull of 48 triangles per variant, centred, bounds equal to the bodies; box/hull volume ratios 2.16, 2.25, 2.10 (box rejected); hull volumes about 9509, 9193 and 9719 cm3. History: before task-339 the engine cube box (task-229-01) was used.
- heal_deposit_crystal (task-262-01): the crystal slot of SM_HealDeposit_01..03 now uses MI_HealDepositCrystal (/Game/RogueRigs/Art/OreDeposit/, parent M_DepositCrystalGlow, Color (0.5, 0, 0.025), GlowColor (1, 0, 0.05), GlowAmount 1.5, PulseAmount 0.5) instead of MI_HealCrystal_Bright, which stays as it was for the grid rock crystals; see [[bp-deposit-crystal-material]].
- owner_agent: agent-bp
- verified (task-339-04): BP_PickupHeal compile result read as null (no error text); BP_ArenaGrid `HealPickupClass` still BP_PickupHeal_C. Tests RogueRigs.Pickups.HealFullHealthWaits and HealPartialClamp pass (task-339-05). -game capture Saved/Captures/HealChunk/contact_sheet.png (8 chunks via rr.Debug.SpawnNear near a full-health miner, not a real vein break): chunks read pink-red with glow rather than pure red (cause unknown, material or bloom, not chased), pointed-prism clusters about a quarter of miner height, random orientations. Not verified (open): size against the ore chunk, rest on a face, that all three variants appear. Earlier: defaults read back through ObjectTools on the class default object; referenced by BP_ArenaGrid ([[bp-arena-grid]]) as `HealPickupClass`. PIE collection not driven through MCP (see [[task-006-04-bp-dig]]).

## Relations
- [[blueprints-index]] — parent index.
- [[sys-pickups]] — the C++ class it derives from.
- [[sys-pickups-lifecycle]] — the rest orientation rule.
- [[bp-arena-grid]] — the grid Blueprint that spawns it.
- [[bp-pickup-ore]] — sibling Blueprint with the same pop distance, and the chunk model this copies.
- [[color-code]] — colour convention.
- [[bp-deposit-crystal-material]] — the deposit crystal material.
- [[bp-heal-ring]] — the ring it spawns on collection.
