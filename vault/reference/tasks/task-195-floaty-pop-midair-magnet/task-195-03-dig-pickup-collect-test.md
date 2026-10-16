---
id: task-195-03-dig-pickup-collect-test
description: "C++ test fix: RogueRigs.Dig.PickupCollect expectations follow the mid-air magnet and the 300 uu pop of task-195."
parent: task-195-floaty-pop-midair-magnet
kind: task
status: done
---
# Dig pickup collect test: floaty pop

- goal: update the expectations of RogueRigs.Dig.PickupCollect (Source/RogueRigs/Tests/RRDigTests.cpp) broken by task-195-02 on BP_PickupHeal: earliest collect = PopAttractDelay from spawn when bAttractDuringPop is on (else PopDuration + AttractDelay, today), the far pickup placed outside reach of a 300 uu pop, enough frames for a 1.6 s pop; values read by reflection like the existing ones; no new test.
- agent: cpp
- depends_on: task-195-01-floaty-pop-midair-magnet
- inputs: sys-pickups-lifecycle (vault/reference/tech/systems/world/sys-pickups/sys-pickups-lifecycle.md), sys-tests-dig-ore (vault/reference/tech/systems/tools/sys-automation-tests/sys-tests-dig-ore.md), agent-cpp (vault/reference/agents/agent-cpp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: the test updated; sys-tests-dig-ore updated.
- done_when: build clean; RogueRigs.Pickup, RogueRigs.ActorPool and RogueRigs.Dig tests all pass; docs updated; lint clean; committed by path.
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 20, cache write 35580, cache read 237317, output 270
- runs: 1
- milestone: M1
- result: RogueRigs.Dig.PickupCollect expectations follow the mid-air magnet (earliest collect min(PopAttractDelay, PopDuration + AttractDelay)), far pickup at radius + PopMaxDistance + 100, frames from the pop timing; Pickup, ActorPool and Dig tests 31/31 pass; commits 8bfff950, 49975428

## Relations
- [[task-195-floaty-pop-midair-magnet]] — parent global task.
