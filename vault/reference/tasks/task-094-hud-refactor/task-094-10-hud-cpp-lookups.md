---
id: task-094-10-hud-cpp-lookups
description: "C++ HUD code that looked widgets up by name in WBP_HUD finds them inside the sub-widgets, and the moved per-frame work leaves NativeTick."
parent: task-094-hud-refactor
kind: task
status: done
---
# HUD C++ lookups after the split

- goal: after the panels exist, URRHUDWidget C++ and the debug commands find their widgets inside the sub-widgets and no per-frame work that moved to events stays in NativeTick.
- agent: cpp
- depends_on: task-094-09-hud-root
- inputs: hud-refactor-audit (vault/reference/tech/blueprints/hud-refactor-audit.md), hud-refactor-audit-run (vault/reference/tech/blueprints/hud-refactor-audit-run.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-hud-events (vault/reference/tech/systems/sys-hud-events.md), sys-hud-pressure (vault/reference/tech/systems/sys-hud-pressure.md), sys-hud-minimap (vault/reference/tech/systems/sys-hud-minimap.md), wbp-hud (vault/reference/tech/blueprints/wbp-hud.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: the Blueprint subtasks move widgets out of WBP_HUD into URRHUDPanelWidget children, which breaks C++ lookups by name in the HUD tree: KeepVignetteWarm (it keeps the damage vignette material warm so the very first hit shows in a packaged build, task-055: this must keep working with the vignette now inside WBP_HealthStatus), the debug commands LogVignetteState / rr.Debug.DamageMiner / rr.Debug.Shots (Source/RogueRigs/Debug/RRVisualCheckCommands.cpp), RefreshAlertBarEvents, the minimap tick (Minimap->Tick), the alert bar and any other FindWidget / GetWidgetFromName on the HUD. Add one helper that finds a widget by name through the whole tree including the child panels (WidgetTree->ForEachWidget recursing into UUserWidget children), use it everywhere, and remove from NativeTick whatever the new events replaced (the audit lists it: RefreshAlertBarEvents now event-driven via OnAlertBarEventsChanged). Known breaks reported by the bp subtasks: the card widgets moved to WBP_UpgradeChoice, so GetWidgetFromName on the HUD tree no longer finds UpgradeScreen, RerollButton, CardName0..2, CardDesc0..2 in Source/RogueRigs/Tests/RRIntegrationTests.cpp (about lines 1428-1436, 1550, 1691, 1877, 1917) and RRWeaponsIntegrationTests.cpp (about lines 347-399, including Integration.WeaponCardEquipsShotgun): update those lookups through the new helper, not the expectations; the C++ URRHUDWidget still binds offer events and calls Blueprint events that are now unimplemented (harmless: remove or keep documented); URRHUDWidget::ChooseUpgrade and RequestReroll are no longer called by the Blueprint. Also from 094-07 (alerts moved to WBP_AlertObjectives): Source/RogueRigs/Tests/RRIntegrationTests.cpp looks up EndScreen, EndTitle, ShuttleText, RestartButton (about lines 1855-1947) and ObjectiveText, AlertBar, AlertLabel, HackBar (about 2008-2068) in the HUD tree; RRPressureIntegrationTests.cpp and Debug/RRVisualCheckCommands.cpp probably look up alert icons or HordeBanner the same way; URRHUDWidget.cpp (about lines 378-396 and 515-596) still runs RefreshAlertBarEvents from the tick and its handlers, duplicating the run flow list: remove it if RRHUDPressureTests can read the run flow list instead (the C++ agent decides, tests must keep their intent). Keep behaviour identical. Tests: the existing HUD tests still pass with children panels; a test that the helper finds a widget inside a nested child. Docs sys-hud, sys-hud-pressure, sys-hud-minimap, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; no lookup by name left that only searches the root tree; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: FindHUDWidget helper (nested panels), every HUD name lookup in code, tests and debug commands updated (expectations unchanged), RefreshAlertBarEvents out of NativeTick (run flow list), test harness constructs the panels headless, new test FindWidgetInPanels; 236/237 tests: PressureVignetteOnHit fails from a Blueprint bug (vignette fade step 0) fixed in task-104. Commits 42b8678, e5f705e, 45cd333.

## Relations
- [[task-094-hud-refactor]] — parent global task.
