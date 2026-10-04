// Rogue Rigs - runtime CSV loader: reads stat tables from Content into DataTables and applies base values on an ASC.

#include "GAS/RRBaseStatsLoader.h"

#include "RogueRigs.h"
#include "AbilitySystemComponent.h"
#include "Engine/DataTable.h"
#include "GAS/RRHealthAttributeSet.h"
#include "GAS/RRMovementAttributeSet.h"
#include "GAS/RRPlayerAttributeSet.h"
#include "GAS/RRPlayerBaseStatsRow.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"

const TCHAR* FRRBaseStatsLoader::PlayerBaseStatsPath = TEXT("RogueRigs/Data/Tables/PlayerBaseStats.csv");

UDataTable* FRRBaseStatsLoader::LoadTableFromCSV(const FString& ContentRelativePath, UScriptStruct* RowStruct, UObject* Outer)
{
	if (RowStruct == nullptr || !RowStruct->IsChildOf(FTableRowBase::StaticStruct()))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("FRRBaseStatsLoader: invalid row struct for %s (must derive from FTableRowBase)."), *ContentRelativePath);
		return nullptr;
	}

	const FString FilePath = FPaths::Combine(FPaths::ProjectContentDir(), ContentRelativePath);
	FString CSVText;
	if (!FFileHelper::LoadFileToString(CSVText, *FilePath))
	{
		UE_LOG(LogRogueRigs, Error, TEXT("FRRBaseStatsLoader: CSV file not found or unreadable: %s"), *FilePath);
		return nullptr;
	}

	UDataTable* Table = NewObject<UDataTable>(Outer != nullptr ? Outer : GetTransientPackage(), NAME_None, RF_Transient);
	Table->RowStruct = RowStruct;

	const TArray<FString> Problems = Table->CreateTableFromCSVString(CSVText);
	for (const FString& Problem : Problems)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRBaseStatsLoader: %s: %s"), *FilePath, *Problem);
	}

	if (Table->GetRowMap().IsEmpty())
	{
		UE_LOG(LogRogueRigs, Error, TEXT("FRRBaseStatsLoader: no row read from %s"), *FilePath);
		return nullptr;
	}

	return Table;
}

namespace RRStatsCache
{
	/** One parsed CSV: the rooted table plus what it was parsed from. */
	struct FEntry
	{
		UDataTable* Table = nullptr;
		UScriptStruct* RowStruct = nullptr;
		FDateTime FileTimestamp;
	};

	/** Parsed tables by content-relative path. Raw rooted pointers: safe at static destruction. */
	static TMap<FString, FEntry>& Get()
	{
		static TMap<FString, FEntry> Entries;
		return Entries;
	}
}

const UDataTable* FRRBaseStatsLoader::GetCachedTable(const FString& ContentRelativePath, UScriptStruct* RowStruct)
{
	check(IsInGameThread());
	const FString FilePath = FPaths::Combine(FPaths::ProjectContentDir(), ContentRelativePath);
	const FDateTime Timestamp = IFileManager::Get().GetTimeStamp(*FilePath);

	RRStatsCache::FEntry& Entry = RRStatsCache::Get().FindOrAdd(ContentRelativePath);
	if (Entry.Table != nullptr && Entry.RowStruct == RowStruct && Entry.FileTimestamp == Timestamp)
	{
		return Entry.Table;
	}

	if (Entry.Table != nullptr)
	{
		Entry.Table->RemoveFromRoot();
		Entry.Table = nullptr;
	}

	UDataTable* Table = LoadTableFromCSV(ContentRelativePath, RowStruct);
	if (Table == nullptr)
	{
		return nullptr;
	}
	Table->AddToRoot();
	Entry.Table = Table;
	Entry.RowStruct = RowStruct;
	Entry.FileTimestamp = Timestamp;
	return Table;
}

const uint8* FRRBaseStatsLoader::FindRowData(const FString& ContentRelativePath, UScriptStruct* RowStruct, FName RowName, const TCHAR* Context)
{
	const UDataTable* Table = GetCachedTable(ContentRelativePath, RowStruct);
	if (Table == nullptr)
	{
		return nullptr;
	}

	const uint8* const* Row = Table->GetRowMap().Find(RowName);
	if (Row == nullptr || *Row == nullptr)
	{
		UE_LOG(LogRogueRigs, Error, TEXT("FRRBaseStatsLoader: row '%s' not found in %s (%s)"), *RowName.ToString(), *ContentRelativePath, Context);
		return nullptr;
	}
	return *Row;
}

bool FRRBaseStatsLoader::LoadPlayerBaseStats(FName RowName, FRRPlayerBaseStatsRow& OutRow)
{
	return FindRow(PlayerBaseStatsPath, RowName, OutRow, TEXT("FRRBaseStatsLoader::LoadPlayerBaseStats"));
}

bool FRRBaseStatsLoader::SetBaseIfPresent(UAbilitySystemComponent& ASC, const FGameplayAttribute& Attribute, float Value)
{
	if (!ASC.HasAttributeSetForAttribute(Attribute))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("FRRBaseStatsLoader: ASC on %s has no set for attribute %s; base value skipped."),
			*GetNameSafe(ASC.GetOwner()), *Attribute.GetName());
		return false;
	}

	ASC.SetNumericAttributeBase(Attribute, Value);
	return true;
}

bool FRRBaseStatsLoader::ApplyPlayerBaseStats(UAbilitySystemComponent& ASC, const FRRPlayerBaseStatsRow& Row)
{
	bool bAllApplied = true;

	// MaxHealth first: Health is clamped against the current MaxHealth.
	bAllApplied &= SetBaseIfPresent(ASC, URRHealthAttributeSet::GetMaxHealthAttribute(), Row.MaxHealth);
	bAllApplied &= SetBaseIfPresent(ASC, URRHealthAttributeSet::GetHealthAttribute(), Row.MaxHealth);
	bAllApplied &= SetBaseIfPresent(ASC, URRMovementAttributeSet::GetMoveSpeedAttribute(), Row.MoveSpeed);
	bAllApplied &= SetBaseIfPresent(ASC, URRPlayerAttributeSet::GetPickupRadiusAttribute(), Row.PickupRadius);
	bAllApplied &= SetBaseIfPresent(ASC, URRPlayerAttributeSet::GetXPGainAttribute(), Row.XPGain);
	bAllApplied &= SetBaseIfPresent(ASC, URRPlayerAttributeSet::GetMiningSpeedAttribute(), Row.MiningSpeed);
	bAllApplied &= SetBaseIfPresent(ASC, URRPlayerAttributeSet::GetLuckAttribute(), Row.Luck);

	return bAllApplied;
}

bool FRRBaseStatsLoader::LoadAndApplyPlayerBaseStats(UAbilitySystemComponent& ASC, FName RowName)
{
	FRRPlayerBaseStatsRow Row;
	if (!LoadPlayerBaseStats(RowName, Row))
	{
		return false;
	}
	return ApplyPlayerBaseStats(ASC, Row);
}
