---
id: task-501-visual-preview-gate
description: "Global task: a visual or taste request first gets a mockup or one slice with a capture, then waits for the owner go."
parent: tasks-to-do-index
kind: task
status: todo
---
# Visual requests: preview and owner go before the full build

- request: Owner 2026-11-11, verbatim: "ok, prépare la tâche pour mettre ça en place." Earlier in the same conversation, on removing the specialist agent definitions and merging executor reading nodes: "Oui, c'est juste que ça rentrait en conflit avec les régles du vault, ça n'est pas un soucis ?" On ruflo: "Non, ruflo est apparu car j'ai changé de pc et j'avais un plugin activé, mais je ne veux pas ça." "ça" = the main session proposal he accepted: keep the base (planner, one fresh Opus executor per global, style-guide critic, vault as memory) but route each request by family at the entry, picked by the relay from the owner sentence or named by the owner. (1) Réglage (task-490, 491, 474): no planner, one executor on the owner words, a script writes the task node for the trace (a plan costs 0.36 M on average, a light task 0.15 to 0.3 M). (2) Comportement ou système (task-496, 497, 493): planner then executor, plus a handover to a fresh executor at a subtask boundary over 120k context, and a budget that triggers something instead of being indicative. (3) Visuel ou goût (task-499, 461/467/477, 462/466): preview first (mockup or one slice, one capture, owner go), then the full build (task-499: 6.57 M refused at the capture and reverted). (4) Gros morceau (task-484 at 7 M, task-424): the planner splits into several globals, one fresh executor each. Not wanted: specialists per trade, a verifier agent; the only delegation kept is style-guide. Prepare only, launch nothing. Owner answer 2026-11-11 (relayed by the main session, verbatim): "ok" = the plan and the defaults of Q5 accepted.
- goal: A visual or taste request never builds everything before the owner has seen and approved one preview.
- acceptance: A1 orchestration-protocol step 3: a visuel request starts with one preview subtask (a mockup, or one single slice with one capture) and every build subtask depends on it. A2 agent-executor and execution-protocol: after the preview the executor always sets wait-answers with the image path and continues only on the owner go; a refusal leaves the build subtasks unstarted, so nothing has to be reverted. A3 The existing representative_slice rule is reconciled with this gate as decided in Q5, with no contradiction left between the nodes. A4 Lint clean, committed.
- gdd_check: No gdd node touched: the request changes the dev crew (agents branch). Read: agent-planner, orchestration-protocol, orchestrator-lessons, task-spec, agent-executor, execution-protocol, verification-levels. No code read. Facts from the main session, not re-checked: rapport.md section 7 (2026-11-07) measured the 120k handover at -21 % on task-495 (one run) on branch exp/crew-tools, which no longer exists: rebuild, do not merge. Since then task-496 (standard, budget 90 turns 1.5 M) ran 128 turns, peak 339k, 3.66 M; task-499 cost 6.57 M and was cancelled. Rule nodes orchestration-protocol, execution-protocol and verification-levels (kind: rule) are edited under this owner intent once confirmed. _authoring constraint raised by the owner: no node merge; one concept per node.
- verification_level: light
- confirmed_by_owner: yes (2026-11-11, "ok")
- milestone: M2
- next_step: task-501-01-preview-rule
- started: open
- waiting_for: none
- result: open

## Questions
Decided 2026-11-11 (owner "ok"): every default below is accepted; values provisional, owner 2026-11-11.
- Q5 Preview gate: for the visuel family the owner go is always required after the preview, even when the slice passes its own checklist; the planner chooses a mockup when the look is not decided and an in-game slice otherwise. The representative_slice rule without owner checkpoint stays for uncertain integration that is not a matter of taste. Default: as written.

## Subtasks
1. task-501-01-preview-rule (vault/reference/tasks/task-501-visual-preview-gate/task-501-01-preview-rule.md) — doc: Write the preview-first rule for the planner and the owner-go wait for the executor.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-501-01-preview-rule]] — step 1, doc.
