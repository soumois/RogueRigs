// Rogue Rigs - XP and level of the miner: accumulates XP scaled by XPGain, levels up along the CSV XP curve with carry-over.

#include "Progression/RRProgressionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/DataTable.h"
#include "GAS/RRBaseStatsLoader.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "Progression/RRXPCurveRow.h"
#include "RogueRigs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRProgressionComponent)

const TCHAR* URRProgressionComponent::XPCurvePath = TEXT("RogueRigs/Data/Tables/XPCurve.csv");

URRProgressionComponent::URRProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void URRProgressionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	LoadCurve();
}

void URRProgressionComponent::LoadCurve()
{
	XPToNextLevelByLevel.Reset();

	const UDataTable* Table = FRRBaseStatsLoader::GetCachedTable(XPCurvePath, FRRXPCurveRow::StaticStruct());
	if (Table == nullptr)
	{
		return;
	}

	// Row name = level. Levels must run 1..N without gap, each with a positive need.
	TMap<int32, float> ByLevel;
	int32 MaxLevel = 0;
	for (const TPair<FName, uint8*>& Pair : Table->GetRowMap())
	{
		const int32 RowLevel = FCString::Atoi(*Pair.Key.ToString());
		const FRRXPCurveRow* Row = reinterpret_cast<const FRRXPCurveRow*>(Pair.Value);
		if (RowLevel < 1 || Row == nullptr || Row->XPToNextLevel <= 0.f)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("URRProgressionComponent: invalid row '%s' in %s (level >= 1 and XPToNextLevel > 0 required)."),
				*Pair.Key.ToString(), XPCurvePath);
			return;
		}
		ByLevel.Add(RowLevel, Row->XPToNextLevel);
		MaxLevel = FMath::Max(MaxLevel, RowLevel);
	}

	TArray<float> Curve;
	Curve.Reserve(MaxLevel);
	for (int32 CurveLevel = 1; CurveLevel <= MaxLevel; ++CurveLevel)
	{
		const float* Need = ByLevel.Find(CurveLevel);
		if (Need == nullptr)
		{
			UE_LOG(LogRogueRigs, Error, TEXT("URRProgressionComponent: level %d missing in %s."), CurveLevel, XPCurvePath);
			return;
		}
		Curve.Add(*Need);
	}
	XPToNextLevelByLevel = MoveTemp(Curve);
}

float URRProgressionComponent::GetXPToNextLevelAt(int32 InLevel) const
{
	if (XPToNextLevelByLevel.IsEmpty())
	{
		return 0.f;
	}
	const int32 Index = FMath::Clamp(InLevel - 1, 0, XPToNextLevelByLevel.Num() - 1);
	return XPToNextLevelByLevel[Index];
}

float URRProgressionComponent::GetXPGain() const
{
	const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (ASC == nullptr || !ASC->HasAttributeSetForAttribute(URRPlayerAttributeSet::GetXPGainAttribute()))
	{
		return 1.f;
	}
	return ASC->GetNumericAttribute(URRPlayerAttributeSet::GetXPGainAttribute());
}

void URRProgressionComponent::AddXP(float Raw)
{
	if (Raw <= 0.f)
	{
		return;
	}

	XP += Raw * GetXPGain();

	float Needed = GetXPToNextLevel();
	if (Needed <= 0.f)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("URRProgressionComponent: no XP curve on %s; XP accumulates without level-up."), *GetNameSafe(GetOwner()));
	}
	while (Needed > 0.f && XP >= Needed)
	{
		XP -= Needed;
		++Level;
		UE_LOG(LogRogueRigs, Log, TEXT("URRProgressionComponent: %s reached level %d."), *GetNameSafe(GetOwner()), Level);
		OnLevelUp.Broadcast(Level);
		Needed = GetXPToNextLevel();
	}

	OnXPChanged.Broadcast(XP, Needed, Level);
}
