---
id: task-429-pickup-magnet-arc-path
description: "Global task: a pickup taken by the magnet flies to the miner along a curved arc instead of the straight 3D line."
parent: tasks-to-do-index
kind: task
status: wait-answers
---
# Pickup magnet flight along an arc

- request: Owner 2026-11-04 (verbatim): "J'aimerais modifié le chemin que prends les pick up lorsqu'ils rentrent dans le radius d'attraction. Il faudrait que ça ne soit pas une simple ligne droite, mais plutot une sorte d'arc". Current logic (sys-pickups-lifecycle, Magnet): once latched (from Rest, or mid-air with bAttractDuringPop), ARRPickup::FlyToMiner moves the pickup along the straight 3D line to the miner, speed MagnetStartSpeed + MagnetAcceleration up to MagnetMaxSpeed, collected within CollectDistance. The owner asked for no capture, bot run or screenshot session.
- goal: In ARRPickup Magnet phase, the pickup no longer flies the straight line: it leaves at an angle off the direct line to the miner and curves into him (shape per Q1, side per Q2, bend per Q3), the deviation fading to 0 as it closes in, so it always reaches the miner even when he moves and never orbits him. Suggested method (cpp may choose an equivalent): at latch, draw once per magnet flight a side sign and a start deviation angle; each FlyToMiner step, steer the move direction = direction to the current miner location rotated by the deviation x (remaining 2D distance / latch distance) in the plane per Q1; speed profile, CollectDistance, heal refusal (ReturnToRest), mid-air latch and stream latching unchanged. New EditDefaultsOnly tunables in category Pickup|Collect, provisional owner 2026-11-04: bArcMagnet (switch, per Q4), arc deviation min and max (degrees, per Q3), side mode (per Q2). Random draws reset per life / per latch (pool reuse safe). No Blueprint asset change: the BP class defaults inherit the C++ defaults. Rules preserved: game rules unchanged (radius, speeds, collection, XP, heal); rendering only.
- acceptance: A1 a pickup entering the pickup radius flies to the miner on a visibly curved path, not a straight line. A2 shape, side and bend match Q1 to Q3. A3 it always reaches and is collected by the miner, standing or moving, without circling him; the arrival time stays close to today (path at most about 30 % longer). A4 the applicable pickup types per Q4 behave the same; unticking bArcMagnet in a Blueprint class default restores the straight line. A5 the C++ builds and the existing pickup automation tests pass (only expectations this change breaks are adjusted; no new test).
- gdd_check: xp-level-up (approved spec) says pickups inside the radius accelerate toward the miner and form a stream; no rule fixes a straight path, so no conflict; with random sides (Q2 default) the stream comes in from both sides. The values the work needs are not in the GDD (shape, side, bend, pickup types): asked as Q1-Q4, the xp-level-up sentence is added on confirmation (defaults-file edit). Rules versus rendering: magnet speeds, radius, collection and values unchanged. Milestone M2. Tech: sys-pickups and sys-pickups-lifecycle describe the straight-line magnet and are updated by the doc subtask; BP_PickupXP and BP_PickupXPBig turn to face the miner in OnMagnetStart (bp-pickup-xp), kept as is. Capture: the lesson "a change on screen ends with a capture" is asked as Q5 (the owner declined one on task-428).
- confirmed_by_owner: no (plan and questions handed back 2026-11-04)
- milestone: M2
- next_step: task-429-01-magnet-arc-cpp
- started: open
- waiting_for: owner answers to Q1-Q5 (arc shape, side, bend, pickups, visual check)
- result: open

## Questions
Asked 2026-11-04, each with a default; open until the owner answers.
- Q1 Shape of the arc: Default: a sideways curve seen from the camera (on the ground plane): the pickup sets off at an angle and bends into the miner; its height goes to the miner as today. Other choice: a vertical hop (it rises then dives onto the miner), or both combined.
- Q2 Side: Default: left or right drawn at random per pickup, so a burst comes in from both sides. Other choice: every pickup turns the same way (a swirl, always clockwise).
- Q3 Bend: Default: moderate, the pickup leaves about 40 degrees off the direct line (random per pickup between 25 and 50 degrees), straightening as it closes in; tunable in the class defaults. Other choice: stronger (about 70 degrees, wide loop) or a fixed angle without randomness.
- Q4 Which pickups: Default: all of them (XP nut, big XP chip, ore, heal), with a checkbox bArcMagnet ticked by default so a Blueprint can go back to the straight line by unticking it. Other choice: XP pickups only.
- Q5 Visual check: Default: no capture run, you judge the arc in game yourself (as on task-428). Other choice: a capture agent run in game (-game) that sends you frames of the flight.

## Subtasks
1. task-429-01-magnet-arc-cpp (vault/reference/tasks/task-429-pickup-magnet-arc-path/task-429-01-magnet-arc-cpp.md) — cpp: ARRPickup magnet flight along an arc (bArcMagnet and arc tunables)
2. task-429-02-magnet-arc-doc (vault/reference/tasks/task-429-pickup-magnet-arc-path/task-429-02-magnet-arc-doc.md) — doc: document the arc magnet in sys-pickups and sys-pickups-lifecycle

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-429-01-magnet-arc-cpp]] — step 1, cpp.
- [[task-429-02-magnet-arc-doc]] — step 2, doc.
- [[xp-level-up]] — gdd XP node; the magnet sentence gets the provisional arc rule
- [[sys-pickups]] — C++ pickup system hub
- [[sys-pickups-lifecycle]] — magnet phase as built, the straight line being replaced
