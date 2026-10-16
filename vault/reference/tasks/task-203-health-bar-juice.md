---
id: task-203-health-bar-juice
description: "Global task: make the HUD health bar juicy with smooth drain, damage chunk, flash, shake, punch and colour by health."
parent: tasks-done-index
kind: task
status: done
---
# Juicy health bar

- request: "Tu peux analyser cette vidéo pour rendre la barre de vie plus Juicy" (miisan tutorial video, 10 steps), then "Pour l'instant, retire les effet de 8 et 10, prépare les sous tâches ( la tâche étant de rendre la barre plus juicy ), mais ne lance rien" (owner, 2026-10-16)
- goal: the health bar of WBP_HealthStatus reacts to damage and heals with the video steps 02 (smooth drain), 03 and 04 (off-white damage chunk), 05 (shake), 06 (flash), 07 (punch) and 09 (colour by health). Steps 08 (damage numbers on the bar) and 10 (heal juice) are removed by the owner.
- acceptance: every subtask done; build exit 0; after task-203-07 the Orchestrator reruns the RogueRigs.HUD tests with the editor closed and they pass (only broken expectations updated, no new test); vignette and GAME OVER behave as before; wbp-health-status and sys-hud-health-bar match the work; nothing seen on screen is claimed (no visual check planned); committed.
- gdd_check: [[color-code]] read 2026-10-16: it does not list the HUD health bar, so the colours of 03, 04 and 06 are new values, written provisional for the owner to confirm; the current bar is red. No game rule changes (display only). Open points asked to the owner (colours and stops of 06, spring of 05, shake decay of 04, chunk delay restart): he answered "ok" (2026-10-16), all defaults accepted as provisional.
- confirmed_by_owner: yes (2026-10-16: "ok lance toutes les taches")
- milestone: M1
- next_step: none
- result: Done. WBP_HealthStatus reparented to URRHealthStatusWidget; HealthBarRoot overlay (20,232, 280 x 28) holds DamageChunkBar (behind) and HealthBar (transparent background); ShowHealth keeps only LowHealthAlpha and ApplyVignette (orphan duplicate chain removed); compiled with warnings as errors, saved; wbp-health-status and sys-hud-health-bar updated. Commit 8547f539. Orchestrator after the editor closed: RogueRigs.HUD 13/13 and RogueRigs.Integration.PressureVignetteOnHit pass. Nothing seen on screen or in PIE.

## Subtasks
1. task-203-01-drain — cpp, health bar class and smooth drain
2. task-203-02-damage-chunk — cpp, damage chunk
3. task-203-03-hit-flash — cpp, hit flash
4. task-203-04-shake — cpp, hit shake
5. task-203-05-punch — cpp, hit punch
6. task-203-06-color-states — cpp, colour by health
7. task-203-07-wbp-health-status — bp, wbp_healthstatus layout for the juicy bar

## Relations
- [[tasks-done-index]] — parent index.
- [[wbp-health-status]] — the panel this task changes.
- [[task-203-01-drain]] — step 1, cpp.
- [[task-203-02-damage-chunk]] — step 2, cpp.
- [[task-203-03-hit-flash]] — step 3, cpp.
- [[task-203-04-shake]] — step 4, cpp.
- [[task-203-05-punch]] — step 5, cpp.
- [[task-203-06-color-states]] — step 6, cpp.
- [[task-203-07-wbp-health-status]] — step 7, bp.
