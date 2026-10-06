---
id: task-026-pressure
description: "Global task: wave director with calm start, rising multi-side flow, lull, announced hordes; damage vignette; event icons on the alert bar; light halo test."
parent: tasks-index
kind: task
status: done
---
# Pressure and rhythm

- request: Owner answers to the DRGS improvement plan (2026-10-05): "1. oui 2. Oui tout à fait, même si pour le proto, nous aurons que 3 armes maxi pour l'instant. 3. Le bouton, c'est trés important ça ! C'était une frustration pour moi 4. On peut garder le coup au sol, il faut juste que la zone soit grande alors 5. Oui, je pense que c'est important de garder un rythme soutenu de level up au départ. Aussi, essaye de comprendre la maniére dont l'xp et les minerais sont ejectés et puis \"aspirer\". [...] Je te laisse lancer toutes les tasks, mais lorsque tu as fini le stress test, je veux bien directement un rapport."
- goal: the level feels oppressive like DRGS: calm 10 s, isolated robots, a first pack at about 22 s, a rising flow from several sides, a lull then "HORDE EN APPROCHE !" and a horde front 4 s later; red vignette on hits; alert bar shows upcoming events.
- acceptance: (1) build exits 0; tests pass; (2) director phases and timings, homogeneous packs with tough robots in front, horde from one side (tests); (3) vignette and banner events (tests); (4) PIE feel check; (5) docs; lint clean; committed.
- gdd_check: Read [[difficulty]], [[level-flow]], [[enemies-index]], [[sys-spawner]], [[sys-difficulty]]. Values provisional (owner 2026-10-05): see subtasks; alive cap 100 → 150 (perf ceiling about 1500). Analysis in [[drgs-analysis]]. The owner launched all lots and delegated provisional values (2026-10-05); decisions written into the GDD before planning; the grid-size lot waits for the owner after the stress report.
- confirmed_by_owner: yes (2026-10-05, all lots launched by the owner)
- milestone: M2
- next_step: none
- result: Done. Landing clearance (console 2 × 2, shuttle 4 × 3). Wave director: calm 10 s, lone chasers, first pack 22 s, 2–3 packs from distinct sides 6 → 3 s, hordes at 95 s and 175 s (run-time trigger, provisional) with lull, 4 s banner, 40–60 robot front, cap 150. HUD: red vignette (hit flash, low-health base), HORDE EN APPROCHE ! banner, alert icons. Light halo cheat rr.Cheat.LightHalo (off by default). Build 0, 170/170. Open: first-hit vignette missing in -game while the shader compiled (recheck in a cooked build); ObjectiveText overlaps the C/N icons (fix in task-027-02); PIE feel not judged by a human.

## Subtasks
0. task-026-00-landing-clearance — cpp: console and shuttle land on a fully open footprint (owner, after handover-2026-10-06)
1. task-026-01-wave-director — cpp: wave director phases, homogeneous packs, hordes, tests
2. task-026-02-feedback-events — cpp: vignette, horde banner and alert-bar event events, tests
3. task-026-03-bp-pressure — bp: banner, vignette, alert icons, light halo test, PIE
4. task-026-04-tests-integration — cpp: integration tests with the final Blueprints

## Relations
- [[tasks-index]] — parent index.
- [[task-026-00-landing-clearance]] — step 0, cpp, added after task-029.
- [[task-026-01-wave-director]] — step 1, cpp.
- [[task-026-02-feedback-events]] — step 2, cpp.
- [[task-026-03-bp-pressure]] — step 3, bp.
- [[task-026-04-tests-integration]] — step 4, cpp.
