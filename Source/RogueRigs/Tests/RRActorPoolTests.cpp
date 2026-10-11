// Rogue Rigs - automation tests of the generic actor pool (RogueRigs.ActorPool.*): reuse, reset hooks, creation spread under MaxFrames, cancellation, counters, teardown.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestPooledActor.h"

namespace RRActorPoolTests
{
	using namespace RRTestHarness;

	/** The pool of the test world, failing the test when missing. */
	URRActorPoolSubsystem* GetPool(FAutomationTestBase& Test, const FTestWorld& TestWorld)
	{
		URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
		Test.TestNotNull(TEXT("actor pool subsystem exists in a game world"), Pool);
		return Pool;
	}

	/** Acquire params whose OnReady appends the delivered actor to Delivered. */
	FRRPoolAcquireParams CollectInto(TArray<ARRTestPooledActor*>& Delivered, int32 MaxFrames = 1)
	{
		FRRPoolAcquireParams Params;
		Params.MaxFrames = MaxFrames;
		Params.OnReady = [&Delivered](AActor* Actor) { Delivered.Add(Cast<ARRTestPooledActor>(Actor)); };
		return Params;
	}

	/** True when Actor is parked: hidden, no collision, actor tick off. */
	bool IsParked(const AActor* Actor)
	{
		return Actor->IsHidden() && !Actor->GetActorEnableCollision() && !Actor->IsActorTickEnabled();
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolReuseTest, "RogueRigs.ActorPool.ReuseReleased",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolReuseTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
	if (!Pool)
	{
		return false;
	}
	TArray<ARRTestPooledActor*> Delivered;
	const FRRPoolRequestHandle First = Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform(FVector(100.f, 0.f, 0.f)), CollectInto(Delivered));
	TestTrue(TEXT("empty pool: the request waits for a creation"), Pool->IsRequestPending(First));
	TestEqual(TEXT("nothing delivered before the tick"), Delivered.Num(), 0);
	TestWorld.Tick();
	if (!TestEqual(TEXT("created and delivered on the first tick"), Delivered.Num(), 1) || !Delivered[0])
	{
		return false;
	}
	ARRTestPooledActor* Actor = Delivered[0];
	TestFalse(TEXT("request no longer pending"), Pool->IsRequestPending(First));
	TestTrue(TEXT("delivered at the requested location"), Actor->GetActorLocation().Equals(FVector(100.f, 0.f, 0.f)));
	TestTrue(TEXT("pool member, not idle"), Pool->IsPoolMember(Actor) && !Pool->IsIdle(Actor));
	TestFalse(TEXT("acquired actor is not pooled idle"), URRActorPoolSubsystem::IsPooledIdle(Actor));

	TestTrue(TEXT("release accepted"), Pool->Release(Actor));
	TestTrue(TEXT("released actor parked (hidden, no collision, no tick)"), IsParked(Actor));
	TestTrue(TEXT("released actor is pooled idle"), URRActorPoolSubsystem::IsPooledIdle(Actor));
	AddExpectedMessagePlain(TEXT("is already idle"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	TestFalse(TEXT("a second release is refused"), Pool->Release(Actor));

	const FRRPoolRequestHandle Second = Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform(FVector(0.f, 300.f, 0.f)), CollectInto(Delivered));
	TestFalse(TEXT("idle instance: served at once (invalid handle)"), Second.IsValid());
	if (!TestEqual(TEXT("delivered synchronously"), Delivered.Num(), 2))
	{
		return false;
	}
	TestTrue(TEXT("the released instance is reused"), Delivered[1] == Actor);
	TestTrue(TEXT("reused actor moved to the new transform"), Actor->GetActorLocation().Equals(FVector(0.f, 300.f, 0.f)));
	TestTrue(TEXT("reused actor visible, colliding and ticking again"), !Actor->IsHidden() && Actor->GetActorEnableCollision() && Actor->IsActorTickEnabled());
	TestEqual(TEXT("BeginPlay ran once"), Actor->BeginPlayCount, 1);

	// A second acquire while the only instance is in use creates a new one: never the same actor twice.
	Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Delivered));
	TestWorld.Tick();
	TestTrue(TEXT("an actor in use is never handed out again"), Delivered.Num() == 3 && Delivered[2] && Delivered[2] != Actor);

	// Actors the pool did not create are refused.
	AActor* Stranger = TestWorld.Spawn<AActor>(ARRTestPooledActor::StaticClass(), FVector::ZeroVector);
	AddExpectedMessagePlain(TEXT("was not created by the pool"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
	TestFalse(TEXT("release of a foreign actor refused"), Pool->Release(Stranger));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolResetHooksTest, "RogueRigs.ActorPool.ResetHooks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolResetHooksTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
	if (!Pool)
	{
		return false;
	}
	TArray<ARRTestPooledActor*> Delivered;
	int32 AcquiredAtReady = -1;
	FRRPoolAcquireParams Params = CollectInto(Delivered);
	Params.OnInit = [](AActor* Actor) { CastChecked<ARRTestPooledActor>(Actor)->InitValue = 7; };
	Params.OnReady = [&Delivered, &AcquiredAtReady](AActor* Actor)
	{
		ARRTestPooledActor* Pooled = Cast<ARRTestPooledActor>(Actor);
		Delivered.Add(Pooled);
		AcquiredAtReady = Pooled->AcquiredCount;
	};
	Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, Params);
	TestWorld.Tick();
	if (!TestEqual(TEXT("delivered"), Delivered.Num(), 1) || !Delivered[0])
	{
		return false;
	}
	ARRTestPooledActor* Actor = Delivered[0];
	TestEqual(TEXT("creation: OnInit ran before BeginPlay"), Actor->InitValueAtBeginPlay, 7);
	TestEqual(TEXT("creation: OnPoolAcquired called once"), Actor->AcquiredCount, 1);
	TestEqual(TEXT("OnPoolAcquired runs before OnReady"), AcquiredAtReady, 1);
	TestEqual(TEXT("no release yet"), Actor->ReleasedCount, 0);

	Pool->Release(Actor);
	TestEqual(TEXT("release: OnPoolReleased called once"), Actor->ReleasedCount, 1);
	TestEqual(TEXT("release: no extra OnPoolAcquired"), Actor->AcquiredCount, 1);

	FRRPoolAcquireParams Reuse = Params;
	Reuse.OnInit = [](AActor* Pooled) { CastChecked<ARRTestPooledActor>(Pooled)->InitValue = 9; };
	Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, Reuse);
	TestTrue(TEXT("reused"), Delivered.Num() == 2 && Delivered[1] == Actor);
	TestEqual(TEXT("reuse: OnPoolAcquired called again"), Actor->AcquiredCount, 2);
	TestEqual(TEXT("reuse: OnInit ran before OnPoolAcquired"), Actor->InitValueAtAcquire, 9);
	TestEqual(TEXT("reuse: BeginPlay not run again"), Actor->BeginPlayCount, 1);
	TestEqual(TEXT("reuse: no extra OnPoolReleased"), Actor->ReleasedCount, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolSpreadTest, "RogueRigs.ActorPool.SpreadRespectsMaxFrames",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolSpreadTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
	if (!Pool)
	{
		return false;
	}
	constexpr int32 Missing = 30;
	constexpr int32 MaxFrames = 3;
	const int32 PerFrameCap = FMath::DivideAndRoundUp(Missing, MaxFrames);
	TArray<ARRTestPooledActor*> Delivered;
	for (int32 Index = 0; Index < Missing; ++Index)
	{
		Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform(FVector(Index * 10.f, 0.f, 0.f)), CollectInto(Delivered, MaxFrames));
	}
	TestEqual(TEXT("30 pending before the first tick"), Pool->GetStats().Pending, Missing);
	for (int32 Frame = 1; Frame <= MaxFrames; ++Frame)
	{
		const int32 Before = Delivered.Num();
		TestWorld.Tick();
		const int32 Created = Delivered.Num() - Before;
		TestTrue(FString::Printf(TEXT("frame %d: %d created, at most %d"), Frame, Created, PerFrameCap), Created <= PerFrameCap);
		TestEqual(FString::Printf(TEXT("frame %d: stats CreatedThisFrame"), Frame), Pool->GetStats().CreatedThisFrame, Created);
	}
	TestEqual(TEXT("all 30 delivered within 3 frames"), Delivered.Num(), Missing);
	TestEqual(TEXT("nothing pending"), Pool->GetStats().Pending, 0);
	TestTrue(TEXT("longest wait within MaxFrames"), Pool->GetStats().LongestWaitFrames <= MaxFrames);
	TSet<ARRTestPooledActor*> Unique(Delivered);
	TestEqual(TEXT("30 distinct actors"), Unique.Num(), Missing);

	// Mixed deadlines: 4 urgent (MaxFrames 1) on top of 20 relaxed (MaxFrames 20) get through on the next tick.
	Delivered.Reset();
	for (int32 Index = 0; Index < 20; ++Index)
	{
		Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Delivered, 20));
	}
	TArray<ARRTestPooledActor*> Urgent;
	for (int32 Index = 0; Index < 4; ++Index)
	{
		Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Urgent, 1));
	}
	TestWorld.Tick();
	TestEqual(TEXT("urgent requests served on the next tick"), Urgent.Num(), 4);
	TestEqual(TEXT("urgent tick creates only the urgent requests"), Delivered.Num(), 0);
	for (int32 Frame = 0; Frame < 19; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("relaxed requests delivered within their 20 frames"), Delivered.Num(), 20);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolCancelTest, "RogueRigs.ActorPool.CancelRequest",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolCancelTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
	if (!Pool)
	{
		return false;
	}
	TArray<ARRTestPooledActor*> Delivered;
	TArray<FRRPoolRequestHandle> Handles;
	for (int32 Index = 0; Index < 5; ++Index)
	{
		Handles.Add(Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Delivered, 5)));
	}
	TestTrue(TEXT("cancel a pending request"), Pool->CancelRequest(Handles[0]));
	TestFalse(TEXT("cancel resets the handle"), Handles[0].IsValid());
	TestTrue(TEXT("cancel another"), Pool->CancelRequest(Handles[3]));
	TestFalse(TEXT("cancelled request is not pending"), Pool->IsRequestPending(Handles[3]));

	// A request whose requester dies is dropped without callback.
	AActor* Requester = TestWorld.Spawn<AActor>(AActor::StaticClass(), FVector::ZeroVector);
	FRRPoolAcquireParams Owned = CollectInto(Delivered, 5);
	Owned.Requester = Requester;
	const FRRPoolRequestHandle OwnedHandle = Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, Owned);
	Requester->Destroy();
	TestEqual(TEXT("4 pending (5 - 2 cancelled + 1 owned)"), Pool->GetStats().Pending, 4);

	for (int32 Frame = 0; Frame < 5; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("only the 3 live requests delivered"), Delivered.Num(), 3);
	TestFalse(TEXT("dead requester's request dropped"), Pool->IsRequestPending(OwnedHandle));
	TestEqual(TEXT("nothing pending"), Pool->GetStats().Pending, 0);
	TestEqual(TEXT("only 3 actors created"), Pool->GetStats().TotalCreated, 3);
	TestFalse(TEXT("cancelling a delivered request does nothing"), Pool->CancelRequest(Handles[1]));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolCountersTest, "RogueRigs.ActorPool.Counters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolCountersTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	FTestWorld TestWorld;
	URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
	if (!Pool)
	{
		return false;
	}
	const TSubclassOf<AActor> Class = ARRTestPooledActor::StaticClass();
	TestEqual(TEXT("empty by default (no prewarm)"), Pool->GetStats().PoolSize, 0);

	Pool->Prewarm(Class, 4, 2);
	TestEqual(TEXT("prewarm queued"), Pool->GetStats().Pending, 4);
	TestWorld.Tick();
	TestEqual(TEXT("prewarm: half created on tick 1"), Pool->GetStats().Idle, 2);
	TestWorld.Tick();
	FRRActorPoolStats Stats = Pool->GetStats(Class);
	TestEqual(TEXT("prewarm done: 4 idle"), Stats.Idle, 4);
	TestEqual(TEXT("prewarm done: pool size 4"), Stats.PoolSize, 4);
	TestEqual(TEXT("prewarm done: nothing acquired"), Stats.Acquired, 0);

	TArray<ARRTestPooledActor*> Delivered;
	for (int32 Index = 0; Index < 3; ++Index)
	{
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered));
	}
	Stats = Pool->GetStats(Class);
	TestEqual(TEXT("3 served at once from the prewarm"), Delivered.Num(), 3);
	TestEqual(TEXT("acquired 3"), Stats.Acquired, 3);
	TestEqual(TEXT("idle 1"), Stats.Idle, 1);
	TestEqual(TEXT("reused 3"), Stats.TotalReused, 3);
	TestEqual(TEXT("created 4"), Stats.TotalCreated, 4);
	TestEqual(TEXT("served at once: wait 0"), Stats.LongestWaitFrames, 0);
	TestEqual(TEXT("prewarmed actors parked with OnPoolReleased"), Delivered[0]->ReleasedCount, 1);

	// An acquired member destroyed by its user leaves the pool.
	Delivered[2]->Destroy();
	Stats = Pool->GetStats(Class);
	TestEqual(TEXT("destroyed member forgotten: pool size 3"), Stats.PoolSize, 3);
	TestEqual(TEXT("destroyed member forgotten: acquired 2"), Stats.Acquired, 2);

	Pool->ResetStats();
	Stats = Pool->GetStats();
	TestTrue(TEXT("ResetStats clears totals and wait"), Stats.TotalCreated == 0 && Stats.TotalReused == 0 && Stats.LongestWaitFrames == 0);
	TestEqual(TEXT("ResetStats keeps the pool"), Stats.PoolSize, 3);
	TestEqual(TEXT("other class: empty stats"), Pool->GetStats(AActor::StaticClass()).PoolSize, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRActorPoolTeardownTest, "RogueRigs.ActorPool.Teardown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRActorPoolTeardownTest::RunTest(const FString& Parameters)
{
	using namespace RRActorPoolTests;
	TArray<ARRTestPooledActor*> Delivered;
	{
		FTestWorld TestWorld;
		URRActorPoolSubsystem* Pool = GetPool(*this, TestWorld);
		if (!Pool)
		{
			return false;
		}
		const TSubclassOf<AActor> Class = ARRTestPooledActor::StaticClass();
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered));
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered));
		TestWorld.Tick();
		if (!TestEqual(TEXT("two delivered"), Delivered.Num(), 2))
		{
			return false;
		}
		ARRTestPooledActor* IdleActor = Delivered[0];
		ARRTestPooledActor* InUse = Delivered[1];
		Pool->Release(IdleActor);
		Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Delivered, 5));
		Pool->Acquire(ARRTestPooledActor::StaticClass(), FTransform::Identity, CollectInto(Delivered, 5));
		TestEqual(TEXT("the idle actor was reused by the first new request"), Delivered.Num(), 3);
		Pool->Release(Delivered[2]);

		Pool->ClearPool(true);
		const FRRActorPoolStats Stats = Pool->GetStats();
		TestTrue(TEXT("cleared: no member, no pending"), Stats.PoolSize == 0 && Stats.Pending == 0 && Stats.Idle == 0);
		TestTrue(TEXT("idle actor destroyed"), !IsValid(IdleActor) || IdleActor->IsActorBeingDestroyed());
		TestTrue(TEXT("actor in use left to its caller"), IsValid(InUse) && !InUse->IsActorBeingDestroyed());
		TestFalse(TEXT("cleared pool forgets its members"), Pool->IsPoolMember(InUse));
		AddExpectedMessagePlain(TEXT("was not created by the pool"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
		TestFalse(TEXT("release after clear refused"), Pool->Release(InUse));
		const int32 DeliveredBefore = Delivered.Num();
		for (int32 Frame = 0; Frame < 6; ++Frame)
		{
			TestWorld.Tick();
		}
		TestEqual(TEXT("dropped requests never call back"), Delivered.Num(), DeliveredBefore);

		// World end with pending requests and idle actors: Deinitialize clears without callbacks.
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered));
		TestWorld.Tick();
		Pool->Release(Delivered.Last());
		Pool->Acquire(AActor::StaticClass(), FTransform::Identity, CollectInto(Delivered, 3));
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered, 3));
		Pool->Acquire(Class, FTransform::Identity, CollectInto(Delivered, 3));
	}
	TestTrue(TEXT("world teardown with a live pool completed"), true);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
