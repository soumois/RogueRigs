---
id: task-182-debris-pool-80-test
description: "Global task: test 80 debris systems with debris living 4.5 s plus a 0.4 s sink, and measure what raising the cap costs."
parent: tasks-to-do-index
kind: task
status: todo
---
# Debris pool 80 and 4.5 s life: test and cost

- request: Prépare une tâche test avec 80 et passer le temps des débris à 4.5 sec, mais ne lance rien. Aussi retiens un peu ce que ça coûte d'augmenter le nombre de particules débris possible. (owner, 2026-10-14); then: non, 4.5 et 0.4 d'enfoncement (owner, 2026-10-14)
- goal: Debris stay 4.5 s at full size then sink in 0.4 s (4.9 s), the debris pool holds 80 systems, and the cost of the larger cap is measured and recorded.
- acceptance: lifetime 4.9 s and sink from 4.5 s read back; DebrisPoolSize 80 on both chaser Blueprints; game-mode 200/300 and S7 200 measured against task-181; cost per extra debris system recorded in perf-stress-chaser-meshy; enemy-death updated; committed.
- gdd_check: enemy-death (pools of 40, debris 3 s + 0.4 s) and perf-stress-chaser-meshy read (2026-10-14); the change is a test the owner asked for; enemy-death to be updated (provisional) when it lands.
- confirmed_by_owner: yes (2026-10-15); measure last
- milestone: M3
- next_step: task-182-01-debris-lifetime-4-5s
- result: open

## Subtasks
1. task-182-01-debris-lifetime-4-5s — bp
2. task-182-02-debris-pool-80 — bp
3. task-182-03-debris-pool-cost — cpp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-182-01-debris-lifetime-4-5s]] — step 1, bp.
- [[task-182-02-debris-pool-80]] — step 2, bp.
- [[task-182-03-debris-pool-cost]] — step 3, cpp.
