---
id: verification-levels
description: "Verification level of a global task: criteria, verification size, indicative executor budget, shortened light acceptance, known limits. Open to plan or run a task."
parent: agents-index
kind: rule
applies_to: orchestrator
---
# Verification levels

Each global task carries `verification_level`, so that verification follows the size of the change (owner 2026-11-06, [[task-482-verification-levels]]). The Planner sets it, the Executor applies it. An older node without the field counts as standard.

| Level | Owner word | Criteria | Verification size | Budget |
|---|---|---|---|---|
| light | leger | A setting on an existing system; no new asset, no new system | 3-4 acceptance points; if visible, 1 launch after and 1 sheet | 40 turns, 0.6 M units |
| standard | standard | New behaviour or new asset | As before this rule | 90 turns, 1.5 M units |
| exploratory | exploratoire | A look to find; several loops likely | Capture loop with a fixed number of rounds | 150 turns, 3 M units |

The figures are starting values (provisional, owner 2026-11-06). The budget is indicative for the Executor's whole run and never blocks: exceeding it is not a failure ([[agent-executor]] `budget`).

## Limits of each level
- Light without a before image: add one launch before the change, so 2 launches and 1 sheet. The comparison of [[orchestrator-lessons]] (owner 2026-10-16) stays; only its size changes.
- A change of screen or widget layout is never light: the four resolutions of [[umg-layout]] stay the default, and the owner can ask for fewer in the plan.
- Exploratory: 3 rounds unless the plan sets another number. A round is one capture and the correction it leads to. At the limit the Executor shows the owner what it has and waits for him (`wait-answers`).

## Shortening a light acceptance
Example, [[task-474-miner-scale-down]], one Blueprint value and one C++ line:
- Scale read back in the Blueprint, not height measured in Blender at 1 %.
- Rifle in hand and laser at the muzzle seen on one frame, not the full list of attachments. Keep this kind of check on purpose: the scale change really did affect the rifle attach.
- One line "gameplay unchanged" through the capsule read-back and targeted tests, not camera, drone, light and animations one by one.
- 1 launch and 1 sheet with the owner reference as before, not 2 launches, 3 angles and 4 sheets.

## Known limits
- Modest expected gain: about 2.4 M of 37.5 M units (6 %) on tasks 474, 475, 476 at about 0.8 M each, an estimate.
- Risk: a defect reaches owner validation instead of being caught by the Executor.
- A comparison on about ten tasks is not planned unless the owner asks.

## Relations
- [[agents-index]] — parent index.
- [[orchestration-protocol]] — step 3, where the Planner sets the level.
- [[execution-protocol]] — where the Executor applies it.
- [[agent-executor]] — budget rule.
- [[orchestrator-lessons]] — owner rule of 2026-10-16 whose size varies here.
- [[umg-layout]] — the four resolutions a layout keeps.
- [[task-474-miner-scale-down]] — the shortening example.
- [[task-482-verification-levels]] — the owner's decision and the cost analysis behind it.
