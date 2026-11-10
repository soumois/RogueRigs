---
id: task-498-heavy-slam-impact-fx
description: "Global task: Capture first whether the heavy slam already shows a dust ring and rock chips; build only what is missing, in the explosions cartoon style."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Heavy slam impact: dust ring and rock chips

- request: Context: read-only Code Architect scan of the GDD against the code (2026-11-10); the owner asked for questions on the top 4 gaps, answered them, then said: "lance les 4 tâches, je ne sais pas où sont génerés les json, il s'agit d'un script cpp qui s'en occupe". Point 3 (heavy slam impact FX, enemy-heavy: dust ring + rock chips), owner decision: "Vérifier d'abord par capture": first a -game capture of the slam to see whether ring and chips already exist; build only what is missing (orange cartoon dust ring on the ground + rock chips, style of the explosions tasks 434/446). Owner answer 2026-11-10 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q1, Q2, Q3 accepted.
- goal: When the heavy slam lands, an orange cartoon dust ring on the ground sized to the slam radius and rock chips burst out, in the style of the explosions; what already exists is kept.
- acceptance: A1 A -game capture before any change says whether a dust ring and rock chips appear on the slam frame; the frames go to the owner. A2 Only the missing parts are built; if both exist and match, nothing is built and the task ends with those frames. A3 Ring: orange, flat cartoon, on the ground, reaching about the slam radius (AttackRange), short, in the style of task-434 and task-446. A4 Chips: small rock chips in the rock and floor violets thrown out from the slam point. A5 On every slam, hit or not; none if the heavy dies during the telegraph; telegraph circle, camera shake and death puff unchanged; effects cleared between levels like the other robot effects. A6 Capture after, beside the frames before.
- gdd_check: Read enemy-heavy (slam impact line, owner 2026-10-23), color-code. Conflict noted, not asked: kessler-voss-logo says orange is reserved for explosions; the owner asked for an orange ring and the slam is an impact of the same family; color-code allows orange details and readability-rules says colour rules are not rigid. Default chip colours asked (Q1). The orange ground telegraph circle is drawn before the slam and gone at the slam frame, so the ring must not be read as a new telegraph (A3 short). No open value. Edit: enemy-heavy.
- verification_level: standard
- confirmed_by_owner: yes (2026-11-10, "ok")
- milestone: M2
- next_step: task-498-01-capture-before
- started: 2026-11-10T20:39:35Z
- waiting_for: none
- result: open

## Questions
Decided 2026-11-10 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-10.
- Q1 Couleurs : anneau orange cartoon comme demandé ; éclats de roche dans les violets de la roche et du sol (pas orange), pour qu'ils se lisent comme de la roche. Défaut : oui.
- Q2 Si la capture montre un anneau ou des éclats déjà là mais dans un autre style (pas orange, pas cartoon) : je m'arrête et je te montre les images avant de toucher à quoi que ce soit. Défaut : oui.
- Q3 Déclenchement : à chaque slam, que le miner soit touché ou non ; rien si le heavy meurt pendant sa préparation. Défaut : oui.

## Subtasks
1. task-498-01-capture-before (vault/reference/tasks/task-498-heavy-slam-impact-fx/task-498-01-capture-before.md) — capture: A -game capture of several heavy slams to see whether a dust ring and rock chips already appear.
2. task-498-02-slam-niagara (vault/reference/tasks/task-498-heavy-slam-impact-fx/task-498-02-slam-niagara.md) — bp: Only if C1 or C2 is missing: the Niagara effect with the missing ring and or chips, in the task-434 and task-446 style.
3. task-498-03-slam-wiring (vault/reference/tasks/task-498-heavy-slam-impact-fx/task-498-03-slam-wiring.md) — bp: Only if slam-niagara ran: the effect plays on the heavy slam frame at the heavy, sized to its slam radius.
4. task-498-04-capture-after (vault/reference/tasks/task-498-heavy-slam-impact-fx/task-498-04-capture-after.md) — capture: Only if something was built: a -game capture of the slam after, beside the frames before.
5. task-498-05-doc (vault/reference/tasks/task-498-heavy-slam-impact-fx/task-498-05-doc.md) — doc: Document the slam impact effect, or that it already existed.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-498-01-capture-before]] — step 1, capture.
- [[task-498-02-slam-niagara]] — step 2, bp.
- [[task-498-03-slam-wiring]] — step 3, bp.
- [[task-498-04-capture-after]] — step 4, capture.
- [[task-498-05-doc]] — step 5, doc.
