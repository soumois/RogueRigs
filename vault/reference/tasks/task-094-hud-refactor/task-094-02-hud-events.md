---
id: task-094-02-hud-events
description: "The C++ events the HUD sub-widgets bind to exist, with a panel base class so code initialises only WBP_HUD."
parent: task-094-hud-refactor
kind: task
status: in-progress
---
# Missing gameplay events and the panel base class

- goal: The C++ events the HUD sub-widgets bind to exist, with a panel base class so code initialises only WBP_HUD.
- agent: cpp
- depends_on: task-094-01-hud-refactor
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-missing-events (vault/reference/tech/blueprints/hud-refactor-missing-events.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), sys-weapons (vault/reference/tech/systems/sys-weapons.md), sys-level-up (vault/reference/tech/systems/sys-level-up.md), sys-player (vault/reference/tech/systems/sys-player.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md), sys-subtitles (vault/reference/tech/systems/sys-subtitles.md)
- expected_output: implement the list of missing events of the audit note hud-refactor-missing-events (dynamic multicast, BlueprintAssignable): ARRMiner::OnHealthChanged(Current, Max), ARRMiner::OnDamaged(Amount, HealthFraction), ARRWeapon::OnAmmoChanged(Weapon, Rounds, MagazineSize), ARRMiner::OnWeaponSlotChanged(SlotIndex, FRRWeaponSlotHUD), URRRunFlowSubsystem::OnAlertBarEventsChanged(Events), OnHordeIncoming relay, OnLocatorTargetChanged, optional OnXPGained: fire them from the systems that own the values (no polling), keep the existing events working. Add the C++ base class URRHUDPanelWidget (UUserWidget, Abstract) with virtual BindToMiner / BindToRunFlow / BindToSpawner hooks (BlueprintNativeEvent) that URRHUDWidget calls once on each child panel it contains, so code initialises only WBP_HUD (children are found in the widget tree). Do not change gameplay or the existing Blueprints. Tests: each event fires once per change with the right payload, no event when nothing changes, the panel base binds children once. Update sys-hud and sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 90000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-094-hud-refactor]] — parent global task.
