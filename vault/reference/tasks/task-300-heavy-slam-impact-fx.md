---
id: task-300-heavy-slam-impact-fx
description: "Global task: A dust ring and rock chips burst from the ground when the heavy slam lands, sized to the slam radius."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heavy slam ground impact effect

- request: "Prévoir une particule lorsque le gros robot frappe le sol" (owner, 2026-10-23)
- goal: NS_HeavySlamImpact (dust ring expanding to the slam radius plus rock chips near the fists), spawned by BP_Heavy OnSlam at Location with User.Radius = Radius (provisional, owner 2026-10-23).
- acceptance: effect plays once per slam at the heavy feet, ring reaching the slam radius; -game frames of a slam; committed.
- gdd_check: enemy-heavy read (2026-10-23): slam impact line added (provisional, owner 2026-10-23). Default: dust in neutral rock grey-brown, chips reuse the NS_DigImpact chip look; no gameplay effect.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: task-300-02-bp-slam-impact-wire
- result: open

## Subtasks
1. task-300-01-bp-slam-impact-ns — bp: NS_HeavySlamImpact
2. task-300-02-bp-slam-impact-wire — bp: BP_Heavy plays the slam impact
3. task-300-03-capture-slam-impact — cpp: Capture the slam impact

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-300-01-bp-slam-impact-ns]] — step 01, bp.
- [[task-300-02-bp-slam-impact-wire]] — step 02, bp.
- [[task-300-03-capture-slam-impact]] — step 03, cpp.
