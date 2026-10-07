---
id: task-034-05-hud-ammo-data
description: "HUD data for the weapon slots: rounds left, magazine size, reload progress per weapon; turret charges and recharge progress; slot owned or locked."
parent: task-034-fourth-test
kind: task
status: todo
---
# Ammo data for the HUD

- goal: the HUD can show ammo, reload and turret recharge per slot.
- agent: cpp
- depends_on: task-034-04-director-mix
- inputs: [[readability-rules]], [[sys-hud]], [[sys-weapons]], [[sys-turret]], [[sys-automation-tests]], [[cpp-standard]]
- expected_output: Blueprint-readable getters on URRHUDWidget (or the weapons) per slot index: owned or locked, rounds left, magazine size, reloading flag, reload progress 0..1; turret launcher charges, max charges, recharge progress 0..1; exact names listed for task-034-02; update only existing tests that break; docs sys-hud.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-034-fourth-test]] — parent global task.
