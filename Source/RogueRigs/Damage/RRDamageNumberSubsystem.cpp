// Rogue Rigs - damage number world subsystem: every hit number of the world, merged per target over a short window, pushed in batches to one Niagara system.

#include "Damage/RRDamageNumberSubsystem.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/App.h"
#include "Misc/PackageName.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraEmitterInstance.h"
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
	TArray<FRRDamageNumber> Numbers;
	// Entries are appended in time order: the due ones form a prefix of Pending.
	int32 DueCount = 0;
	while (DueCount < Pending.Num() && DueCount < MaxNumbersPerTick
		&& Now - Pending[DueCount].FirstHitTime >= MergeWindow - RRDamageNumbers::WindowTolerance)
	{
		const FPendingNumber& Entry = Pending[DueCount];
		FRRDamageNumber& Number = Numbers.AddDefaulted_GetRef();
		Number.Location = Entry.HitLocation + Entry.Jitter;
		Number.Value = Entry.Value;
		Number.bCritical = Entry.bCritical;
		Number.Size = Entry.bCritical ? NumberSize * CriticalScale : NumberSize;
		Number.Color = Entry.bCritical ? CriticalColor : NormalColor;
		Number.SpawnTime = static_cast<float>(Now);
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
