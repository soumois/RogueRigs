---
id: request-families
description: "Entry routing of owner requests into four families (réglage, comportement, visuel, gros morceau): how the relay picks one, each path, wrong-family stop."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Request families

Every owner request is routed at the entry to one family, which sets its path (owner 2026-11-11, [[task-500-request-families]]; values provisional). The base stays: [[agent-planner]], one fresh Opus [[agent-executor]] per global, [[agent-style-guide]] as the only delegation, the vault as memory. No specialist per trade, no verifier agent.

## Picking the family
The relay (main session) picks it from the owner sentence and states it in one line before launching anything: `Famille : <family> (<reason in a few words>)`. One owner word forces it: `réglage` (or `reglage`), `comportement`, `visuel`, `gros`. The family is written `- family:` on the global (`reglage`, `comportement`, `visuel`, `gros`).

| Family | Criteria | Path |
|---|---|---|
| réglage | A setting on an existing system: a value, a scale, a timing, a parameter; no new asset, no new system, no design choice | No planner. `node vault/scripts/task-reglage.js "<owner words>"` writes the global (level light, one subtask) before the run; one fresh executor runs it |
| comportement | New or changed behaviour or system | Planner, then one executor per global |
| visuel | A look or taste to find | Planner, then executor; preview first, then owner go before the build ([[orchestration-protocol]] step 3, [[execution-protocol]] step 5.7) |
| gros morceau | Larger than one standard budget | Planner, then one executor; planned as one global with its subtasks, not split into several globals (owner 2026-11-12, [[task-508-workflow-back-to-old]]) |

Examples: réglage tasks 490, 491, 474; comportement 496, 497, 493; visuel 499, 461, 462; gros morceau 484, 424.

## Réglage node
`task-reglage.js` takes the next free task number, writes the global and its subtask through `task-create.js` (owner words verbatim in `request`, `confirmed_by_owner: yes` from the request itself, `verification_level: light`, `family: reglage`), lints and commits by path, and prints `id:` and `next:`. The relay launches the executor with that id first in its prompt, so cost recording, resume and the closing gate work as for a planned task. `--no-commit` is for sample runs.

## Wrong family
A réglage executor that meets a new asset or system, a value marked `open` or a design question stops before editing, cancels its global with `task-status.js <global>.md status=cancelled "result=wrong family: <reason>"` (commit by path) and reports the reason. The relay relaunches the request through the planner (family comportement or visuel), quoting the owner words and that report.

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — planner path of comportement, visuel and gros morceau.
- [[agent-planner]] — plans comportement, visuel and gros morceau.
- [[agent-executor]] — runs every family; wrong-family stop.
- [[execution-protocol]] — step 0 for a réglage global.
- [[verification-levels]] — light level of a réglage.
- [[task-spec]] — `family` field of a plan.
- [[task-500-request-families]] — owner decision behind this rule.
- [[task-501-visual-preview-gate]] — preview of the visuel family.
- [[task-508-workflow-back-to-old]] — owner decision: no split of a gros morceau.
