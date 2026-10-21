---
id: task-278-chaser-eye-glow
description: "Global task: robot eyes get their own shared glowing material M_RobotEye; chaser eyes glow about twice as much, body unchanged except the neck."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Chaser eye glow stronger

- request: J'aimerais faire que l'oeil des chasseur glow plus (attention, ça ne doit pas impacter le reste de leur corps) (owner, 2026-10-21); after the leak report: "Tu ne peux pas faire un autre matériaux pour l'oeil du chasseur ? ( qui serait le même pour l'oeil du gros robot et tireur )", then "ok parfait" and "ok" to the plan.
- goal: The chaser and shield-chaser eye is its own material slot with a shared eye material M_RobotEye (also on the Heavy and Shooter greybox eyes) glowing about x2; the body materials lose the red-mask glow (neck ring no longer glows) and keep their look and animation; eye and body play the same vertex animation through one material function.
- acceptance: 02-10 done with their done_when checks; eye slot last on both meshes with PartId intact; MF_RobotAnimWPO used by M_Chaser_Meshy_Anim, M_ShieldChaser_Anim and M_RobotEye; EyeColor (1, 0.05, 0.05) x GlowAmount 23 read back; BP_Chaser, BP_ShieldChaser (including OnShieldBroken), BP_Heavy (including OnRecover), BP_Shooter assigned; docs with before, after and revert; lint clean; committed.
- gdd_check: done by the Orchestrator (2026-10-21): no conflict with color-code (red eyes on every robot), no open value; glow values provisional.
- confirmed_by_owner: yes (2026-10-21, owner "ok" to every default: chaser eye EyeColor (1, 0.05, 0.05) x GlowAmount 23 (about x2 of the old 0.46 x 25), Heavy and Shooter same values, kamikaze untouched, stop if the material function cannot be made (no WPO copy), no captures)
- milestone: M1
- next_step: task-278-04-robot-eye-material
- result: open. History: first approach (task-278-01, GlowAmount 25 -> 50 on the red-texel mask) stopped on a mask leak (red neck ring); owner chose a separate eye material slot instead (2026-10-21), so 278-01 is closed as superseded.

## Subtasks
1. task-278-01-chaser-eye-glow — bp (superseded: leak check only)
2. task-278-02-eye-slot-split — 3d
3. task-278-03-eye-slot-reimport — cpp
4. task-278-04-robot-eye-material — bp
5. task-278-05-chaser-body-material — bp
6. task-278-06-shield-chaser-body-material — bp
7. task-278-07-bp-chaser-eye — bp
8. task-278-08-bp-shield-chaser-eye — bp
9. task-278-09-bp-heavy-eye — bp
10. task-278-10-bp-shooter-eye — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-278-01-chaser-eye-glow]] — step 1, bp, leak check (superseded).
- [[task-278-02-eye-slot-split]] — step 2, 3d, eye faces to their own slot.
- [[task-278-03-eye-slot-reimport]] — step 3, cpp, headless reimport.
- [[task-278-04-robot-eye-material]] — step 4, bp, MF_RobotAnimWPO and M_RobotEye.
- [[task-278-05-chaser-body-material]] — step 5, bp, chaser body master.
- [[task-278-06-shield-chaser-body-material]] — step 6, bp, shield-chaser body master.
- [[task-278-07-bp-chaser-eye]] — step 7, bp, BP_Chaser eye slot.
- [[task-278-08-bp-shield-chaser-eye]] — step 8, bp, BP_ShieldChaser eye slot and swap.
- [[task-278-09-bp-heavy-eye]] — step 9, bp, BP_Heavy eyes.
- [[task-278-10-bp-shooter-eye]] — step 10, bp, BP_Shooter eyes.
