---
id: bp-muzzle-flash
description: "NS_MuzzleFlash, the one-shot white-gold core and four forward sparks at the hand weapon Muzzle per miner shot; open for emitters, values, spawn rule."
parent: blueprints-fx-index
kind: doc
---
# NS_MuzzleFlash

- asset_path: /Game/RogueRigs/FX/NS_MuzzleFlash (Niagara system, no Blueprint; task-414-01, commit 2dcc85fe)
- parent_class: UNiagaraSystem
- purpose: short flash at the muzzle of the miner's hand weapon on every shot; spawned by C++ (task-414-02, commit 743a05e2), see Spawn below and [[sys-weapons]].
- user_params: none. No light.
- emitters (both from the engine Fountain template, CPU sim, local space, Loop Once 0.1 s, SpawnBurst_Instantaneous):
  - `Core`: 1 particle, lifetime 0.06 s, size 25 uu, HDR white-gold (8, 6.5, 2.4).
  - `Sparks`: 4 particles, lifetime 0.07 to 0.12 s, AddVelocity cone axis +X angle 12 deg, speed 400 to 700, gravity 0, drag 0.5, HDR (6, 4.6, 1.6), non-uniform size 1.2 to 1.8 x 8 to 12 uu, VelocityAligned.
- spawn: `ARRWeapon::MuzzleFlashSystem` (`TObjectPtr<UNiagaraSystem>`, EditDefaultsOnly, BlueprintReadOnly, category `Weapon|FX`), loaded in the `ARRWeapon` constructor by path (`ConstructorHelpers`, as `ARRDrone`), so [[bp-burst-rifle]] is unchanged. Protected `ARRWeapon::SpawnMuzzleFlash(const FRotator&) const` calls `UNiagaraFunctionLibrary::SpawnSystemAttached` on the hand weapon `Muzzle`, KeepWorldPosition, world rotation = shot direction (+X along the shot), auto-destroy, no pool. Rifle: one per bullet (3 per burst). Shotgun: one per volley, not per pellet ([[sys-weapons-shotgun]]). Turret: `MuzzleFlashSystem` = nullptr, none; robots none. Null system or `!FApp::CanEverRender()` (-nullrhi) returns nullptr.
- components: none (Niagara asset). No collision, no events.
- logic_in_bp: none
- assets: engine DefaultSpriteMaterial (no new material).
- verified: build ok; 26 tests (Weapons, Shotgun, Turret) pass headless.
- unverified: render look (owner checks in PIE, Q6 of [[task-414-muzzle-flash]]); a flash playing while the world is paused at level end (up to about 0.1 s frozen).
- owner_agent: agent-bp

## Relations
- [[blueprints-fx-index]] — parent index.
- [[sys-weapons]] — the C++ property and spawn rule.
- [[sys-weapons-shotgun]] — one flash per volley.
- [[bp-burst-rifle]] — its Muzzle hosts the flash.
- [[task-414-muzzle-flash]] — the task that made it.
