---
id: task-187-deposit-whole-on-explosion
description: "Global task: an explosion that destroys any block of an ore deposit destroys the whole deposit and pops all its chunks."
parent: tasks-done-index
kind: task
status: done
---
# Ore deposits: destroyed whole by explosions

- request: Il arrive lors d'explosion qu'un minerai soit partiellement détruit. Cela ne doit pas être possible, il doit se détruire entièrement dans ce cas-là (owner, 2026-10-15)
- goal: When an explosion destroys any block of an ore deposit, the whole deposit is destroyed and pops all its chunks: no partly destroyed deposit.
- acceptance: Build passes with the editor closed; existing grid, explosion, dig and ore tests pass (update only an expectation this change breaks, e.g. a test that expects a partial deposit); a -game run with an explosion over part of a deposit logs the whole deposit destroyed and its 4 chunks (values, no screenshots; no new test); docs updated; committed by path; committed.
- gdd_check: ore-blocks, explosions read (2026-10-15): explosions destroy every block in the radius and drone mining breaks a 2 x 2 deposit whole, but nothing says a deposit cut by an explosion breaks whole: new rule, ore-blocks to update; open: heal veins included (default yes).
- confirmed_by_owner: yes (2026-10-15); heal veins included
- milestone: M2
- next_step: none
- result: Done. DestroyCell with Barrel/Kamikaze source on a deposit block destroys the deposit's other standing blocks (DestroySingleCell, no recursion, queued/destroyed skipped, drops and console counts per block); DestroyCellsInRadius returns real count; run: deposit 161 with 2 of 4 cells in radius broke whole, 4 chunks; 97/97 tests after 2 expectations updated; start capsule path and heal veins not exercised; commits 1531c7fd, 60b94290, 0f49dcae

## Subtasks
1. task-187-01-deposit-whole-on-explosion — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-187-01-deposit-whole-on-explosion]] — step 1, cpp.
