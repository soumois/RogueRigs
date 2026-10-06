// Rogue Rigs - damage number world subsystem: every hit number of the world, merged per target over a short window, pushed in batches to one Niagara system.

#include "Damage/RRDamageNumberSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/App.h"
#include "Misc/PackageName.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraEmitter.h"
#include "NiagaraEmitterHandle.h"
#include "NiagaraEmitterInstance.h"
#include "NiagaraScript.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraSystem.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDamageNumberSubsystem)

const FName URRDamageNumberSubsystem::DigitPositionsParameter(TEXT("User.DigitPositions"));
const FName URRDamageNumberSubsystem::DigitIndicesParameter(TEXT("User.DigitIndices"));
const FName URRDamageNumberSubsystem::DigitSizesParameter(TEXT("User.DigitSizes"));
const FName URRDamageNumberSubsystem::DigitColorsParameter(TEXT("User.DigitColors"));
const FName URRDamageNumberSubsystem::DigitCriticalsParameter(TEXT("User.DigitCriticals"));
const FName URRDamageNumberSubsystem::DigitSpawnTimesParameter(TEXT("User.DigitSpawnTimes"));
const FName URRDamageNumberSubsystem::DigitCountParameter(TEXT("User.DigitCount"));
const FName URRDamageNumberSubsystem::DigitBatchIdParameter(TEXT("User.DigitBatchId"));

namespace RRDamageNumbers
{
	/** Tolerance on the window end, so a window of N fixed frames closes on frame N despite float accumulation. */
	constexpr double WindowTolerance = 1e-3;
}

URRDamageNumberSubsystem::URRDamageNumberSubsystem()
{
	NumberSystem = TSoftObjectPtr<UNiagaraSystem>(FSoftObjectPath(TEXT("/Game/RogueRigs/FX/NS_DamageNumbers.NS_DamageNumbers")));
}

bool URRDamageNumberSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void URRDamageNumberSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	if (InWorld.GetNetMode() == NM_DedicatedServer || NumberSystem.IsNull())
	{
		return;
	}
	// Package check first: loading a missing asset logs a LogUObjectGlobals warning in every world.
	UNiagaraSystem* System = FPackageName::DoesPackageExist(NumberSystem.ToSoftObjectPath().GetLongPackageName()) ? NumberSystem.LoadSynchronous() : nullptr;
	if (!System)
	{
		// Not an error before the Niagara asset exists (task-025-02): numbers still merge, nothing renders.
		UE_LOG(LogRogueRigs, Log, TEXT("Damage numbers: Niagara system %s not found; numbers are not rendered."), *NumberSystem.ToString());
		return;
	}
	// Same set-up as UNiagaraFunctionLibrary::SpawnSystemAtLocation (owner: world settings, absolute transform at the
	// origin, no auto destroy), built by hand so the component and its user parameters also exist in a process that
	// cannot render (headless automation tests); Niagara itself only simulates where it can render.
	NiagaraComponent = NewObject<UNiagaraComponent>(InWorld.GetWorldSettings(), NAME_None, RF_Transient);
	NiagaraComponent->SetAsset(System);
	NiagaraComponent->bAutoActivate = false;
	NiagaraComponent->SetAutoDestroy(false);
	NiagaraComponent->SetVisibleInRayTracing(false);
	NiagaraComponent->RegisterComponentWithWorld(&InWorld);
	NiagaraComponent->SetAbsolute(true, true, true);
	NiagaraComponent->SetWorldLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	CheckUserParameters(*System);
	float AssetLifetime = DisplayLifetime;
	FVector AssetVelocity = RiseVelocity;
	bDisplayReadFromAsset = ReadDisplayFromSystem(*System, AssetLifetime, AssetVelocity) && AssetLifetime > 0.f;
	if (bDisplayReadFromAsset)
	{
		DisplayLifetime = AssetLifetime;
		RiseVelocity = AssetVelocity;
	}
	UE_LOG(LogRogueRigs, Log, TEXT("Damage numbers: display lifetime %.3f s, rise velocity %s (%s)."), DisplayLifetime, *RiseVelocity.ToString(),
		bDisplayReadFromAsset ? *System->GetName() : TEXT("config"));
	if (HasParameter(DigitCountParameter))
	{
		NiagaraComponent->SetVariableInt(DigitCountParameter, 0);
	}
	if (HasParameter(DigitBatchIdParameter))
	{
		NiagaraComponent->SetVariableInt(DigitBatchIdParameter, DigitBatchId);
	}
	if (FApp::CanEverRender())
	{
		NiagaraComponent->Activate(/*bReset*/ true);
	}
}

void URRDamageNumberSubsystem::Deinitialize()
{
	Pending.Reset();
	Displayed.Reset();
	LastDigits.Reset();
	MissingParameters.Reset();
	if (IsValid(NiagaraComponent))
	{
		NiagaraComponent->DestroyComponent();
	}
	NiagaraComponent = nullptr;
	Super::Deinitialize();
}

bool URRDamageNumberSubsystem::AreNumbersShown() const
{
	const URRGameUserSettings* Settings = URRGameUserSettings::GetRRGameUserSettings();
	return Settings == nullptr || Settings->GetShowDamageNumbers();
}

void URRDamageNumberSubsystem::AddHit(AActor* Target, float Value, bool bCritical, FVector Location)
{
	const UWorld* World = GetWorld();
	if (!World || Value <= 0.f || !AreNumbersShown())
	{
		return;
	}
	const double Now = World->GetTimeSeconds();

	if (Target)
	{
		const FObjectKey Key(Target);
		// Newest first: an open entry of this target is near the end.
		for (int32 Index = Pending.Num() - 1; Index >= 0; --Index)
		{
			FPendingNumber& Entry = Pending[Index];
			if (Entry.Target == Key && Now - Entry.FirstHitTime < MergeWindow - RRDamageNumbers::WindowTolerance)
			{
				Entry.Value += Value;
				Entry.bCritical |= bCritical;
				Entry.HitLocation = Location;
				return;
			}
		}
	}

	FPendingNumber& Entry = Pending.AddDefaulted_GetRef();
	if (Target)
	{
		Entry.Target = FObjectKey(Target);
	}
	Entry.HitLocation = Location;
	Entry.Jitter = FVector(FMath::FRandRange(-Jitter, Jitter), FMath::FRandRange(-Jitter, Jitter), 0.f);
	// The new entry is already the last one of Pending; the scan skips it.
	Entry.Stack = ComputeStackOffset(Location + Entry.Jitter, Value, bCritical, Now);
	Entry.Value = Value;
	Entry.bCritical = bCritical;
	Entry.FirstHitTime = Now;
}

void URRDamageNumberSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	if (!AreNumbersShown())
	{
		Pending.Reset();
	}

	const double Now = World->GetTimeSeconds();
	// Displayed numbers are appended in push order: the expired ones form a prefix.
	int32 Expired = 0;
	while (Expired < Displayed.Num() && Now - Displayed[Expired].SpawnTime >= DisplayLifetime)
	{
		++Expired;
	}
	if (Expired > 0)
	{
		Displayed.RemoveAt(0, Expired, EAllowShrinking::No);
	}
	TArray<FRRDamageNumber> Numbers;
	// Entries are appended in time order: the due ones form a prefix of Pending.
	int32 DueCount = 0;
	while (DueCount < Pending.Num() && DueCount < MaxNumbersPerTick
		&& Now - Pending[DueCount].FirstHitTime >= MergeWindow - RRDamageNumbers::WindowTolerance)
	{
		const FPendingNumber& Entry = Pending[DueCount];
		FRRDamageNumber& Number = Numbers.AddDefaulted_GetRef();
		Number.Location = Entry.HitLocation + Entry.Jitter + Entry.Stack;
		Number.Value = Entry.Value;
		Number.bCritical = Entry.bCritical;
		Number.Size = Entry.bCritical ? NumberSize * CriticalScale : NumberSize;
		Number.Color = Entry.bCritical ? CriticalColor : NormalColor;
		Number.SpawnTime = static_cast<float>(Now);
		if (MaxStackSteps > 0)
		{
			FDisplayedNumber& Shown = Displayed.AddDefaulted_GetRef();
			Shown.Location = Number.Location;
			Shown.Footprint = GetFootprint(Entry.Value, Entry.bCritical);
			Shown.SpawnTime = Now;
		}
		++DueCount;
	}
	if (DueCount > 0)
	{
		Pending.RemoveAt(0, DueCount, EAllowShrinking::No);
	}

	if (Numbers.Num() == 0)
	{
		return;
	}
	LastDigits.Reset();
	for (const FRRDamageNumber& Number : Numbers)
	{
		AppendDigits(Number, LastDigits);
	}
	++DigitBatchId;
	PushToNiagara();
	TotalPushedCount += Numbers.Num();
	OnNumbersPushed.Broadcast(Numbers);
}

FVector2D URRDamageNumberSubsystem::GetFootprint(float Value, bool bCritical) const
{
	const float Size = bCritical ? NumberSize * CriticalScale : NumberSize;
	const int32 DigitLimit = FMath::Clamp(MaxDigits, 1, 9);
	int32 Digits = 1;
	for (int32 Rest = FMath::Max(FMath::RoundToInt(Value), 1) / 10; Rest > 0 && Digits < DigitLimit; Rest /= 10)
	{
		++Digits;
	}
	// The glyph cell is DigitAdvance x Size wide per digit and Size tall.
	return FVector2D(DigitAdvance * Size * static_cast<float>(Digits), Size);
}

FVector URRDamageNumberSubsystem::ComputeStackOffset(const FVector& Location, float Value, bool bCritical, double Now) const
{
	if (MaxStackSteps <= 0)
	{
		return FVector::ZeroVector;
	}
	const FVector Right = DigitDirection.GetSafeNormal(UE_SMALL_NUMBER, FVector::YAxisVector);
	const FVector Up = StackDirection.GetSafeNormal(UE_SMALL_NUMBER, FVector::ZAxisVector);
	const FVector2D Footprint = GetFootprint(Value, bCritical);
	// The new number appears when its window closes; every other number is compared where it will be at that moment.
	const double PushTime = Now + MergeWindow;

	// Obstacles: the other open entries (pushed when their own window closes) and the numbers still on screen at PushTime.
	struct FObstacle
	{
		FVector Location;
		FVector2D Footprint;
	};
	TArray<FObstacle, TInlineAllocator<32>> Obstacles;
	// The new entry is the last one of Pending: skip it.
	for (int32 Index = 0; Index < Pending.Num() - 1; ++Index)
	{
		const FPendingNumber& Other = Pending[Index];
		const double Risen = FMath::Max(PushTime - (Other.FirstHitTime + MergeWindow), 0.0);
		Obstacles.Add({ Other.HitLocation + Other.Jitter + Other.Stack + RiseVelocity * Risen, GetFootprint(Other.Value, Other.bCritical) });
	}
	for (const FDisplayedNumber& Shown : Displayed)
	{
		const double Age = PushTime - Shown.SpawnTime;
		if (Age < DisplayLifetime)
		{
			Obstacles.Add({ Shown.Location + RiseVelocity * Age, Shown.Footprint });
		}
	}

	FVector Stack = FVector::ZeroVector;
	for (int32 Step = 0; Step < MaxStackSteps; ++Step)
	{
		bool bMoved = false;
		for (const FObstacle& Other : Obstacles)
		{
			const FVector Delta = (Location + Stack) - Other.Location;
			const float HalfWidth = 0.5f * static_cast<float>(Footprint.X + Other.Footprint.X);
			const float HalfHeight = 0.5f * static_cast<float>(Footprint.Y + Other.Footprint.Y);
			const float AlongUp = static_cast<float>(FVector::DotProduct(Delta, Up));
			if (FMath::Abs(static_cast<float>(FVector::DotProduct(Delta, Right))) < HalfWidth && FMath::Abs(AlongUp) < HalfHeight)
			{
				// Lift it just above the other number.
				Stack += Up * (HalfHeight + StackGap - AlongUp);
				bMoved = true;
				break;
			}
		}
		if (!bMoved)
		{
			break;
		}
	}
	return Stack;
}

bool URRDamageNumberSubsystem::ReadDisplayFromSystem(const UNiagaraSystem& System, float& OutLifetime, FVector& OutVelocity)
{
	// Only the direct Lifetime constant counts: Lifetime Min / Max stay stored when the random mode is unused (NS_DamageNumbers
	// keeps a stale 1.75 s Max), and a Particles.Velocity constant only when non-zero (the rise may be done in Particle Update).
	static const FString LifetimeSuffix(TEXT(".InitializeParticle.Lifetime"));
	static const FString VelocitySuffix(TEXT(".Particles.Velocity"));
	bool bFoundLifetime = false;
	for (const FNiagaraEmitterHandle& Handle : System.GetEmitterHandles())
	{
		const FVersionedNiagaraEmitterData* Data = Handle.GetIsEnabled() ? Handle.GetEmitterData() : nullptr;
		const UNiagaraScript* Script = Data ? Data->SpawnScriptProps.Script.Get() : nullptr;
		if (!Script)
		{
			continue;
		}
		const FNiagaraParameterStore& Store = Script->RapidIterationParameters;
		for (const FNiagaraVariableWithOffset& Variable : Store.ReadParameterVariables())
		{
			const FString Name = Variable.GetName().ToString();
			if (Variable.GetType() == FNiagaraTypeDefinition::GetFloatDef() && Name.EndsWith(LifetimeSuffix))
			{
				const float Lifetime = Store.GetParameterValue<float>(Variable);
				OutLifetime = bFoundLifetime ? FMath::Max(OutLifetime, Lifetime) : Lifetime;
				bFoundLifetime = true;
			}
			else if (Name.EndsWith(VelocitySuffix) && Variable.GetType() == FNiagaraTypeDefinition::GetVec3Def())
			{
				const FVector Velocity(Store.GetParameterValue<FVector3f>(Variable));
				OutVelocity = Velocity.IsNearlyZero() ? OutVelocity : Velocity;
			}
		}
		if (bFoundLifetime)
		{
			break;
		}
	}
	return bFoundLifetime;
}

void URRDamageNumberSubsystem::AppendDigits(const FRRDamageNumber& Number, TArray<FRRDamageDigit>& OutDigits) const
{
	const int32 DigitLimit = FMath::Clamp(MaxDigits, 1, 9);
	int32 MaxValue = 1;
	for (int32 Index = 0; Index < DigitLimit; ++Index)
	{
		MaxValue *= 10;
	}
	// A positive hit never shows 0; a sum beyond the digit budget shows all nines.
	const int32 Shown = FMath::Clamp(FMath::RoundToInt(Number.Value), 1, MaxValue - 1);

	int32 DigitValues[9];
	int32 DigitTotal = 0;
	for (int32 Rest = Shown; Rest > 0; Rest /= 10)
	{
		DigitValues[DigitTotal++] = Rest % 10;
	}

	const FVector Direction = DigitDirection.GetSafeNormal(UE_SMALL_NUMBER, FVector::YAxisVector);
	const float Step = DigitAdvance * Number.Size;
	const float FirstOffset = -0.5f * Step * static_cast<float>(DigitTotal - 1);
	OutDigits.Reserve(OutDigits.Num() + DigitTotal);
	// DigitValues holds the least significant digit first: read it backwards so reading order runs along Direction.
	for (int32 Slot = 0; Slot < DigitTotal; ++Slot)
	{
		FRRDamageDigit& Digit = OutDigits.AddDefaulted_GetRef();
		Digit.Location = Number.Location + Direction * (FirstOffset + Step * static_cast<float>(Slot));
		Digit.Digit = DigitValues[DigitTotal - 1 - Slot];
		Digit.Size = Number.Size;
		Digit.Color = Number.Color;
		Digit.bCritical = Number.bCritical;
		Digit.SpawnTime = Number.SpawnTime;
	}
}

void URRDamageNumberSubsystem::PushToNiagara()
{
	if (!IsValid(NiagaraComponent) || LastDigits.Num() == 0)
	{
		return;
	}

	// A system whose emitters completed (one-shot loop, task-025-03 finding) deactivates the component: reactivate it so
	// the batch still spawns. Nothing is alive at that point, so the reset loses no particle.
	if (!NiagaraComponent->IsActive() && FApp::CanEverRender())
	{
		NiagaraComponent->Activate(/*bReset*/ true);
	}

	const int32 Count = LastDigits.Num();
	TArray<FVector> Positions;
	TArray<int32> Indices;
	TArray<float> Sizes;
	TArray<FLinearColor> Colors;
	TArray<int32> Criticals;
	TArray<float> SpawnTimes;
	Positions.Reserve(Count);
	Indices.Reserve(Count);
	Sizes.Reserve(Count);
	Colors.Reserve(Count);
	Criticals.Reserve(Count);
	SpawnTimes.Reserve(Count);
	for (const FRRDamageDigit& Digit : LastDigits)
	{
		Positions.Add(Digit.Location);
		Indices.Add(Digit.Digit);
		Sizes.Add(Digit.Size);
		Colors.Add(Digit.Color);
		Criticals.Add(Digit.bCritical ? 1 : 0);
		SpawnTimes.Add(Digit.SpawnTime);
	}
	// Arrays first, then the count, the batch id last: the emitter spawns on a batch id change only.
	if (HasParameter(DigitPositionsParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(NiagaraComponent, DigitPositionsParameter, Positions);
	}
	if (HasParameter(DigitIndicesParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(NiagaraComponent, DigitIndicesParameter, Indices);
	}
	if (HasParameter(DigitSizesParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(NiagaraComponent, DigitSizesParameter, Sizes);
	}
	if (HasParameter(DigitColorsParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(NiagaraComponent, DigitColorsParameter, Colors);
	}
	if (HasParameter(DigitCriticalsParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(NiagaraComponent, DigitCriticalsParameter, Criticals);
	}
	if (HasParameter(DigitSpawnTimesParameter))
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(NiagaraComponent, DigitSpawnTimesParameter, SpawnTimes);
	}
	if (HasParameter(DigitCountParameter))
	{
		NiagaraComponent->SetVariableInt(DigitCountParameter, Count);
	}
	if (HasParameter(DigitBatchIdParameter))
	{
		NiagaraComponent->SetVariableInt(DigitBatchIdParameter, DigitBatchId);
	}
	UE_LOG(LogRogueRigs, Verbose, TEXT("Damage numbers: batch %d, %d digits pushed to Niagara (first digit %d)."), DigitBatchId, Count, LastDigits[0].Digit);
}

void URRDamageNumberSubsystem::CheckUserParameters(const UNiagaraSystem& System)
{
	MissingParameters.Reset();
	const FName Parameters[] = { DigitPositionsParameter, DigitIndicesParameter, DigitSizesParameter, DigitColorsParameter,
		DigitCriticalsParameter, DigitSpawnTimesParameter, DigitCountParameter, DigitBatchIdParameter };
	const TArrayView<const FNiagaraVariableWithOffset> Exposed = System.GetExposedParameters().ReadParameterVariables();
	FString MissingList;
	for (const FName Parameter : Parameters)
	{
		const bool bExposed = Exposed.ContainsByPredicate([Parameter](const FNiagaraVariableWithOffset& Variable) { return Variable.GetName() == Parameter; });
		if (!bExposed)
		{
			MissingParameters.Add(Parameter);
			MissingList += MissingList.IsEmpty() ? Parameter.ToString() : FString(TEXT(", ")) + Parameter.ToString();
		}
	}
	if (!MissingList.IsEmpty())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("Damage numbers: %s does not expose %s; those parameters are not written."), *System.GetName(), *MissingList);
	}
}

bool URRDamageNumberSubsystem::GetParticleStats(int32& OutLiveParticles, int32& OutSpawnedParticles) const
{
	OutLiveParticles = 0;
	OutSpawnedParticles = 0;
	if (!IsValid(NiagaraComponent))
	{
		return false;
	}
	const FNiagaraSystemInstanceControllerConstPtr Controller = NiagaraComponent->GetSystemInstanceController();
	const FNiagaraSystemInstance* Instance = Controller.IsValid() ? Controller->GetSystemInstance_Unsafe() : nullptr;
	if (!Instance)
	{
		return false;
	}
	for (const FNiagaraEmitterInstanceRef& Emitter : Instance->GetEmitters())
	{
		OutLiveParticles += Emitter->GetNumParticles();
		OutSpawnedParticles += Emitter->GetTotalSpawnedParticles();
	}
	return true;
}

TStatId URRDamageNumberSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRDamageNumberSubsystem, STATGROUP_Tickables);
}
