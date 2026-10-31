---
id: task-405-right-elbow-raise
description: "Global task: raise the miner right elbow a little away from the body in every clip, hand and rifle unchanged, owner-tunable offset."
parent: tasks-done-index
kind: task
status: cancelled
---
# Miner right elbow raised a little, owner tunable

- request: Owner, 2026-10-31, verbatim (French, relayed by the main session): "Pour l'arme je préferais cette position (X=9.898091,Y=-1.129037,Z=-13.000000) et un scale de 1.25. Par contre pour la main gauche, je peux mettre ce que je veux dans Joint Target location, ça ne change rien". Later owner words 2026-10-31 (relayed by the main session, verbatim): "Là c'est n'est pas le coude, c'est la main gauche, c'était déjà là, je voulais l'ajuster moi même" (he had typed values in the ABP_Miner Two Bone IK Joint Target Location and saw no change); "Sauf que c'est le coude droit que je voudrais relever un peu dans l'animation ( il est un peu tro collé au corps )"; "Sinon c'est bizzare, quand j'ai déplace gun, la main n'a pas suivi". This task is the point "c'est le coude droit que je voudrais relever un peu dans l'animation ( il est un peu tro collé au corps )", which replaces the first reading of the Joint Target sentence (left elbow): the owner wants the RIGHT elbow raised a little. The left-hand points (Joint Target is the left elbow pole, the hand did not follow the moved rifle) are handled in [[task-404-rifle-mesh-owner-offset]] (Q5). Runs after task-404.
- goal: In every clip (idle, walk and run directions) the miner right elbow sits a little higher and further from the body than in the Mixamo clips, while the right hand, the rifle on WeaponSocket_R and the left hand on LeftHandGrip stay where they are; the owner tunes the amount himself from one exposed value.
- acceptance: ABP_Miner has a right arm elbow adjustment (method Q4) applied after locomotion and before the left hand IK; one exposed vector variable `RightElbowOffset` (cm) drives it, default per Q4; changing it visibly moves the right elbow, live in the ABP editor preview (no weapon needed) and in PIE; PIE standing and in the four forced directions: right hand bone moves less than 0.5 uu versus the adjustment off (Q4 A), left hand within 3 uu of LeftHandGrip, right elbow (RightForeArm) further from the spine than with the adjustment off; BS_Miner_Locomotion and the clips untouched; doc node updated by the doc step; lint clean; committed by path.
- gdd_check: Read 2026-10-31: [[miner]] (approved; animation = Mixamo clips played by ABP_Miner, weapon socket WeaponSocket_R, left hand by IK; nothing on the elbow pose), [[bp-miner]] (ABP_Miner: Locomotion -> LocalToComponent -> Two Bone IK on LeftHand -> ComponentToLocal; WeaponSocket_R on RightHand; mesh forward = component +Y, left = +X; the right arm has no IK and follows the clips; the Mixamo rifle stance puts the right hand at the cheek/shoulder), [[sys-player-classes]] (LeftHandIKTarget is in RightHand bone space, so it stays right as long as the right hand does not move), [[task-386-miner-mixamo-animation]] (Mixamo clips; owner fix 5e8a0735 on BS_Miner_Locomotion, do not change). Conflicts: none (rendering only, no game rule changes). Open values: method and default amount (Q4). Milestone M2 (art).
- confirmed_by_owner: no (plan and questions handed back 2026-10-31)
- milestone: M2
- next_step: none
- started: open
- waiting_for: none
- result: Cancelled before start by the owner 2026-10-31: "Laisse tomber le coude et execue le reste".

## Questions
Asked 2026-10-31, each with a default; open until the owner answers.
- Q4 Right elbow method: Default A: in ABP_Miner, a Two Bone IK on RightHand (chain RightHand, RightForeArm, RightArm) whose effector is the right hand itself (Bone Space RightHand, location 0), so the hand and the rifle do not move; only the elbow pole moves, = the clip elbow plus `RightElbowOffset`, an ABP variable you edit in the class defaults (live in the ABP preview, no weapon needed); default about 10 cm up and out from the body, provisional, you tune it. B: a Transform (Modify) Bone rotation on RightArm (simpler, but it also moves the hand, so the rifle and its aim move with it). C: edit the arm in the nine Mixamo clips in Blender (heavier, per clip, not tunable live).
- Q3 of [[task-404-rifle-mesh-owner-offset]] (visual check) applies here.

## Subtasks
1. task-405-01-abp-right-elbow-ik (vault/reference/tasks/task-405-right-elbow-raise/task-405-01-abp-right-elbow-ik.md) — bp: ABP_Miner: right elbow raised by an owner offset, hand kept in place
2. task-405-02-doc-right-elbow-raise (vault/reference/tasks/task-405-right-elbow-raise/task-405-02-doc-right-elbow-raise.md) — doc: update bp-miner with the right elbow adjustment and how to tune it

## Relations
- [[tasks-done-index]] — parent index.
- [[task-405-01-abp-right-elbow-ik]] — step 1, bp.
- [[task-405-02-doc-right-elbow-raise]] — step 2, doc.
- [[task-404-rifle-mesh-owner-offset]] — point 1, runs first; holds Q1 to Q3 and Q5 (left hand follow).
- [[task-401-miner-hands-fit-rifle]] — added the left hand Two Bone IK this adjustment sits before.
- [[bp-miner]] — documents ABP_Miner.
