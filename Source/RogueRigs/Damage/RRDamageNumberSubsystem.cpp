// Rogue Rigs - damage number world subsystem: every hit number of the world, merged per target over a short window, pushed in batches to one Niagara system.

#include "Damage/RRDamageNumberSubsystem.h"

#include "Engine/World.h"
#include "Misc/PackageName.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "RogueRigs.h"
#include "Settings/RRGameUserSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRDamageNumberSubsystem)

const FName URRDamageNumberSubsystem::PositionsParameter(TEXT("User.DamageNumberPositions"));
const FName URRDamageNumberSubsystem::ValuesParameter(TEXT("User.DamageNumberValues"));
const FName URRDamageNumberSubsystem::SizesParameter(TEXT("User.DamageNumberSizes"));
const FName URRDamageNumberSubsystem::ColorsParameter(TEXT("User.DamageNumberColors"));
const FName URRDamageNumberSubsystem::CriticalsParameter(TEXT("User.DamageNumberCriticals"));
const FName URRDamageNumberSubsystem::SpawnTimesParameter(TEXT("User.DamageNumberSpawnTimes"));
const FName URRDamageNumberSubsystem::NewCountParameter(TEXT("User.DamageNumberNewCount"));

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
	NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(&InWorld, System, FVector::ZeroVector, FRotator::ZeroRotator,
		FVector(1.f), /*bAutoDestroy*/ false, /*bAutoActivate*/ true, ENCPoolMethod::None, /*bPreCullCheck*/ false);
	if (NiagaraComponent)
	{
		NiagaraComponent->SetVariableInt(NewCountParameter, 0);
	}
}

void URRDamageNumberSubsystem::Deinitialize()
{
	Pending.Reset();
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

	PushToNiagara(Numbers);
	if (Numbers.Num() > 0)
	{
		OnNumbersPushed.Broadcast(Numbers);
	}
}

void URRDamageNumberSubsystem::PushToNiagara(const TArray<FRRDamageNumber>& Numbers)
{
	if (!IsValid(NiagaraComponent))
	{
		return;
	}
	if (Numbers.Num() == 0)
	{
		if (LastPushedCount != 0)
		{
			NiagaraComponent->SetVariableInt(NewCountParameter, 0);
			LastPushedCount = 0;
		}
		return;
	}

	TArray<FVector> Positions;
	TArray<float> Values;
	TArray<float> Sizes;
	TArray<FLinearColor> Colors;
	TArray<bool> Criticals;
	TArray<float> SpawnTimes;
	Positions.Reserve(Numbers.Num());
	Values.Reserve(Numbers.Num());
	Sizes.Reserve(Numbers.Num());
	Colors.Reserve(Numbers.Num());
	Criticals.Reserve(Numbers.Num());
	SpawnTimes.Reserve(Numbers.Num());
	for (const FRRDamageNumber& Number : Numbers)
	{
		Positions.Add(Number.Location);
		Values.Add(Number.Value);
		Sizes.Add(Number.Size);
		Colors.Add(Number.Color);
		Criticals.Add(Number.bCritical);
		SpawnTimes.Add(Number.SpawnTime);
	}
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(NiagaraComponent, PositionsParameter, Positions);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(NiagaraComponent, ValuesParameter, Values);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(NiagaraComponent, SizesParameter, Sizes);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayColor(NiagaraComponent, ColorsParameter, Colors);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayBool(NiagaraComponent, CriticalsParameter, Criticals);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(NiagaraComponent, SpawnTimesParameter, SpawnTimes);
	NiagaraComponent->SetVariableInt(NewCountParameter, Numbers.Num());
	LastPushedCount = Numbers.Num();
}

TStatId URRDamageNumberSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(URRDamageNumberSubsystem, STATGROUP_Tickables);
}
