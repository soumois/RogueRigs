---
id: task-300-heavy-slam-impact-fx
description: "Global task: A dust ring and rock chips burst from the ground when the heavy slam lands, sized to the slam radius."
parent: tasks-done-index
kind: task
status: done
---
# Heavy slam ground impact effect

- request: "Prévoir une particule lorsque le gros robot frappe le sol" (owner, 2026-10-23)
- goal: NS_HeavySlamImpact (dust ring expanding to the slam radius plus rock chips near the fists), spawned by BP_Heavy OnSlam at Location with User.Radius = Radius (provisional, owner 2026-10-23).
- acceptance: effect plays once per slam at the heavy feet, ring reaching the slam radius; -game frames of a slam; committed.
- gdd_check: enemy-heavy read (2026-10-23): slam impact line added (provisional, owner 2026-10-23). Default: dust in neutral rock grey-brown, chips reuse the NS_DigImpact chip look; no gameplay effect.
- confirmed_by_owner: yes (2026-10-23, "Le reste est bon"; planned in a planning pass, each task run later by its own orchestrator)
- milestone: M2
- next_step: none
- result: Done. Run 1 (Saved/RobotMeshes/slam-impact/): ring ~0.4 R, raised, whitish, chips specks -> fix task-300-04. Run 2 (Saved/RobotMeshes/slam-impact-v2/slam_sheet.png, frames 08-17, slomo 0.1): flat ring centred at the feet, ~0.86 R at frame 14, ~0.98 R at frame 15 (~+0.3 s), gone at +0.35 s; light beige-tan semi-transparent (lighter than intended grey-brown); ~5 light-tan chips visible near the legs, short throw. Pixel estimates from the pitched camera.
- owner_decision: "On garde l'impact au sol comme ça" (owner 2026-10-23): the light beige-tan dust, faded ring edge and small chips are accepted, no rework.

## Subtasks
1. task-300-01-bp-slam-impact-ns — bp: NS_HeavySlamImpact
2. task-300-02-bp-slam-impact-wire — bp: BP_Heavy plays the slam impact
3. task-300-03-capture-slam-impact — cpp: Capture the slam impact (run 1 found defects; rerun after 04)
4. task-300-04-bp-slam-impact-fix — bp: fix NS_HeavySlamImpact from the capture

## Relations
- [[tasks-done-index]] — parent index.
- [[task-300-01-bp-slam-impact-ns]] — step 01, bp.
- [[task-300-02-bp-slam-impact-wire]] — step 02, bp.
- [[task-300-03-capture-slam-impact]] — step 03, cpp.
- [[task-300-04-bp-slam-impact-fix]] — step 04, bp fix after the first capture.
