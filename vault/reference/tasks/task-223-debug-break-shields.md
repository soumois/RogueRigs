---
id: task-223-debug-break-shields
description: "Debug console command that empties only the shield of living shield chasers (health untouched), firing the real break, for captures."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Debug command: break shields only

- request: "Oui, ajoute une commande de debug qui vide seulement le bouclier d'un chasseur, puis refais la capture après casse." (owner 2026-10-17)
- goal: `rr.Debug.BreakShields` makes shield chasers lose their shield exactly as a real break (OnShieldBroken fires: material swap and NS_ShieldDebris), with health unchanged, so a shieldless shield chaser can be captured.
- acceptance: the command exists, builds, and in a `-game` run a shield chaser keeps walking without its shield after it; then the missing frame of task-220-04 (`vault/assets/shield-chaser-ingame-broken.png`) is captured.
- gdd_check: debug only, no gameplay change ([[sys-cheats]]); [[sys-enemies-shield-chaser]] (Shield attribute, OnShieldBroken from the health set). Default (Orchestrator): optional argument = radius in uu around the miner, none = every living shield chaser.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-223-01-debug-break-shields-command
- result: open

## Subtasks
- [[task-223-01-debug-break-shields-command]] — cpp: the command, build, doc.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[sys-cheats]] — debug tooling.
- [[task-220-shield-chaser-integration]] — the capture it unblocks.
