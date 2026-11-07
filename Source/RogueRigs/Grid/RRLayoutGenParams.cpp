// Rogue Rigs - readers of the layout generator params and brief JSON files.

#include "Grid/RRLayoutGenParams.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	/** Reads named fields of one JSON object and collects the missing ones. */
	struct FParamReader
	{
		TArray<FString>& Missing;
		TSharedPtr<FJsonObject> Object;
		FString Prefix;

		FParamReader(TArray<FString>& InMissing, const TSharedPtr<FJsonObject>& InObject, const FString& InPrefix)
			: Missing(InMissing), Object(InObject), Prefix(InPrefix)
		{
		}

		void Num(const TCHAR* Key, double& Out) const
		{
			if (!Object.IsValid() || !Object->TryGetNumberField(Key, Out))
			{
				Missing.Add(Prefix + Key);
			}
		}

		void Int(const TCHAR* Key, int32& Out) const
		{
			double Value = 0.0;
			if (!Object.IsValid() || !Object->TryGetNumberField(Key, Value))
			{
				Missing.Add(Prefix + Key);
				return;
			}
			Out = FMath::RoundToInt32(Value);
		}

		FParamReader Sub(const TCHAR* Key) const
		{
			const TSharedPtr<FJsonObject>* Child = nullptr;
			if (!Object.IsValid() || !Object->TryGetObjectField(Key, Child) || Child == nullptr)
			{
				Missing.Add(Prefix + Key);
				return FParamReader(Missing, nullptr, Prefix + Key + TEXT("."));
			}
			return FParamReader(Missing, *Child, Prefix + Key + TEXT("."));
		}
	};

	bool ReadJsonFile(const FString& FilePath, FString& OutText, TSharedPtr<FJsonObject>& OutRoot, FString& OutError)
	{
		if (!FFileHelper::LoadFileToString(OutText, *FilePath))
		{
			OutError = FString::Printf(TEXT("file not found or unreadable: %s"), *FilePath);
			return false;
		}
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(OutText);
		if (!FJsonSerializer::Deserialize(Reader, OutRoot) || !OutRoot.IsValid())
		{
			OutError = FString::Printf(TEXT("JSON parse error in %s: %s"), *FilePath, *Reader->GetErrorMessage());
			return false;
		}
		return true;
	}
}

FString RRLayoutGen::GetDefaultParamsPath()
{
	return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), TEXT("Tools/layout_gen/params/fine_default.json")));
}

bool RRLayoutGen::LoadParams(const FString& FilePath, FRRLayoutGenParams& OutParams, FString& OutError)
{
	OutParams = FRRLayoutGenParams();
	FString Text;
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(FilePath, Text, Root, OutError))
	{
		return false;
	}

	TArray<FString> Missing;
	const FParamReader R(Missing, Root, FString());
	R.Int(TEXT("width"), OutParams.Width);
	R.Int(TEXT("height"), OutParams.Height);
	R.Int(TEXT("noise_seed"), OutParams.NoiseSeed);

	{
		const FParamReader S = R.Sub(TEXT("outline"));
		FRRLayoutGenOutlineParams& O = OutParams.Outline;
		S.Num(TEXT("base"), O.Base);
		S.Int(TEXT("edge_margin"), O.EdgeMargin);
		S.Num(TEXT("noise_amp"), O.NoiseAmp);
		S.Num(TEXT("noise_scale"), O.NoiseScale);
		const TArray<TSharedPtr<FJsonValue>>* Harmonics = nullptr;
		if (S.Object.IsValid() && S.Object->TryGetArrayField(TEXT("harmonics"), Harmonics) && Harmonics != nullptr)
		{
			for (const TSharedPtr<FJsonValue>& Value : *Harmonics)
			{
				const TArray<TSharedPtr<FJsonValue>>* Triple = nullptr;
				if (!Value.IsValid() || !Value->TryGetArray(Triple) || Triple == nullptr || Triple->Num() != 3)
				{
					Missing.Add(TEXT("outline.harmonics (each entry must be [amplitude, frequency, phase])"));
					break;
				}
				O.Harmonics.Add(FVector((*Triple)[0]->AsNumber(), (*Triple)[1]->AsNumber(), (*Triple)[2]->AsNumber()));
			}
		}
		else
		{
			Missing.Add(TEXT("outline.harmonics"));
		}
	}
	{
		const FParamReader S = R.Sub(TEXT("notch"));
		FRRLayoutGenNotchParams& O = OutParams.Notch;
		S.Int(TEXT("max_run"), O.MaxRun);
		S.Int(TEXT("len_min"), O.LenMin);
		S.Int(TEXT("len_max"), O.LenMax);
		S.Int(TEXT("depth"), O.Depth);
		S.Int(TEXT("max_iter"), O.MaxIter);
	}
	{
		const FParamReader S = R.Sub(TEXT("spawn"));
		FRRLayoutGenSpawnParams& O = OutParams.Spawn;
		S.Int(TEXT("zone_half"), O.ZoneHalf);
		S.Num(TEXT("disc_radius"), O.DiscRadius);
		S.Num(TEXT("disc_wobble"), O.DiscWobble);
		S.Num(TEXT("disc_freq_x"), O.DiscFreqX);
		S.Num(TEXT("disc_freq_y"), O.DiscFreqY);
	}
	{
		const FParamReader S = R.Sub(TEXT("walk"));
		FRRLayoutGenWalkParams& O = OutParams.Walk;
		S.Num(TEXT("step"), O.Step);
		S.Num(TEXT("turn"), O.Turn);
		S.Num(TEXT("sharp_chance"), O.SharpChance);
		S.Num(TEXT("sharp_min"), O.SharpMin);
		S.Num(TEXT("sharp_max"), O.SharpMax);
		S.Num(TEXT("brush_min"), O.BrushMin);
		S.Num(TEXT("brush_max"), O.BrushMax);
		S.Num(TEXT("brush_drift"), O.BrushDrift);
		S.Num(TEXT("brush_noise_amp"), O.BrushNoiseAmp);
		S.Num(TEXT("brush_noise_scale"), O.BrushNoiseScale);
		S.Int(TEXT("void_margin"), O.VoidMargin);
	}
	{
		const FParamReader S = R.Sub(TEXT("main"));
		FRRLayoutGenMainParams& O = OutParams.Main;
		S.Num(TEXT("floor_share"), O.FloorShare);
		S.Int(TEXT("steps_min"), O.StepsMin);
		S.Int(TEXT("steps_max"), O.StepsMax);
		S.Num(TEXT("rho0_min"), O.Rho0Min);
		S.Num(TEXT("rho0_max"), O.Rho0Max);
		S.Int(TEXT("max_walks"), O.MaxWalks);
	}
	{
		const FParamReader S = R.Sub(TEXT("pockets"));
		FRRLayoutGenPocketParams& O = OutParams.Pockets;
		S.Int(TEXT("count"), O.Count);
		S.Int(TEXT("steps_base"), O.StepsBase);
		S.Int(TEXT("steps_rand"), O.StepsRand);
		S.Num(TEXT("turn_add"), O.TurnAdd);
		S.Num(TEXT("rho0_min"), O.Rho0Min);
		S.Num(TEXT("rho0_max"), O.Rho0Max);
		S.Int(TEXT("min_cells"), O.MinCells);
		S.Int(TEXT("dist_min"), O.DistMin);
		S.Int(TEXT("dist_max"), O.DistMax);
		S.Int(TEXT("guard"), O.Guard);
		S.Int(TEXT("tries"), O.Tries);
		S.Int(TEXT("margin"), O.Margin);
	}
	{
		const FParamReader S = R.Sub(TEXT("filler"));
		FRRLayoutGenFillerParams& O = OutParams.Filler;
		S.Int(TEXT("maxd"), O.MaxD);
		S.Int(TEXT("extra_min"), O.ExtraMin);
		S.Int(TEXT("extra_rand"), O.ExtraRand);
		S.Num(TEXT("turn_scale"), O.TurnScale);
		S.Num(TEXT("rho0_min"), O.Rho0Min);
		S.Num(TEXT("rho0_max"), O.Rho0Max);
		S.Num(TEXT("aim_noise"), O.AimNoise);
		S.Int(TEXT("max_iter"), O.MaxIter);
	}
	{
		const FParamReader S = R.Sub(TEXT("cleanup"));
		FRRLayoutGenCleanupParams& O = OutParams.Cleanup;
		S.Int(TEXT("min_region"), O.MinRegion);
		S.Int(TEXT("max_gap"), O.MaxGap);
		S.Int(TEXT("max_rock_island"), O.MaxRockIsland);
		S.Int(TEXT("passage"), O.Passage);
		S.Int(TEXT("repair_rounds"), O.RepairRounds);
		S.Int(TEXT("loops"), O.Loops);
	}
	{
		const FParamReader S = R.Sub(TEXT("heal"));
		FRRLayoutGenHealParams& O = OutParams.Heal;
		S.Int(TEXT("vein"), O.Vein);
		S.Int(TEXT("pocket_veins"), O.PocketVeins);
		S.Int(TEXT("wall_veins"), O.WallVeins);
		S.Int(TEXT("exclude_half"), O.ExcludeHalf);
		S.Int(TEXT("wall_min_rock"), O.WallMinRock);
		S.Int(TEXT("pocket_min_rock"), O.PocketMinRock);
		S.Int(TEXT("tries"), O.Tries);
	}
	{
		const FParamReader S = R.Sub(TEXT("barrels"));
		FRRLayoutGenBarrelParams& O = OutParams.Barrels;
		S.Int(TEXT("min"), O.Min);
		S.Int(TEXT("max"), O.Max);
		S.Int(TEXT("pairs_min"), O.PairsMin);
		S.Int(TEXT("pairs_max"), O.PairsMax);
		S.Num(TEXT("pair_dist_min"), O.PairDistMin);
		S.Num(TEXT("pair_dist_max"), O.PairDistMax);
		S.Int(TEXT("spawn_exclude"), O.SpawnExclude);
		S.Int(TEXT("heal_clear"), O.HealClear);
		S.Int(TEXT("tries"), O.Tries);
	}
	{
		const FParamReader S = R.Sub(TEXT("search"));
		FRRLayoutGenSearchParams& O = OutParams.Search;
		S.Int(TEXT("max_seeds"), O.MaxSeeds);
		S.Int(TEXT("pieces"), O.Pieces);
		S.Int(TEXT("max_outline_run"), O.MaxOutlineRun);
		S.Int(TEXT("max_far"), O.MaxFar);
		S.Int(TEXT("asteroid_min"), O.AsteroidMin);
		S.Int(TEXT("asteroid_max"), O.AsteroidMax);
		S.Num(TEXT("reach_min"), O.ReachMin);
		S.Num(TEXT("reach_max"), O.ReachMax);
		S.Int(TEXT("min_pockets"), O.MinPockets);
	}
	{
		const FParamReader S = R.Sub(TEXT("review"));
		FRRLayoutGenReviewParams& O = OutParams.Review;
		S.Int(TEXT("window"), O.Window);
		S.Int(TEXT("stride"), O.Stride);
		S.Num(TEXT("open_min"), O.OpenMin);
		S.Num(TEXT("open_max"), O.OpenMax);
		S.Num(TEXT("repair_open_min"), O.RepairOpenMin);
		S.Num(TEXT("repair_open_max"), O.RepairOpenMax);
		S.Int(TEXT("lane_max"), O.LaneMax);
		S.Int(TEXT("lane_width_min"), O.LaneWidthMin);
		S.Int(TEXT("lane_width_max"), O.LaneWidthMax);
		S.Int(TEXT("lane_tolerance"), O.LaneTolerance);
		S.Int(TEXT("pocket_max_dist"), O.PocketMaxDist);
		S.Num(TEXT("outcrop_rho_min"), O.OutcropRhoMin);
		S.Num(TEXT("outcrop_rho_max"), O.OutcropRhoMax);
		S.Num(TEXT("lane_rho_min"), O.LaneRhoMin);
		S.Num(TEXT("lane_rho_max"), O.LaneRhoMax);
		S.Num(TEXT("shape_noise"), O.ShapeNoise);
		S.Int(TEXT("max_actions"), O.MaxActions);
		S.Int(TEXT("max_tries"), O.MaxTries);
		S.Int(TEXT("passes"), O.Passes);
	}

	if (Missing.Num() > 0)
	{
		OutError = FString::Printf(TEXT("missing or invalid params in %s: %s"), *FilePath, *FString::Join(Missing, TEXT(", ")));
		return false;
	}
	if (OutParams.Width < 16 || OutParams.Height < 16 || OutParams.Width > 1024 || OutParams.Height > 1024)
	{
		OutError = FString::Printf(TEXT("width/height out of range [16, 1024] in %s"), *FilePath);
		return false;
	}
	return true;
}

bool RRLayoutGen::LoadBrief(const FString& FilePath, FRRLayoutBrief& OutBrief, FString& OutError)
{
	OutBrief = FRRLayoutBrief();
	TSharedPtr<FJsonObject> Root;
	if (!ReadJsonFile(FilePath, OutBrief.SourceText, Root, OutError))
	{
		return false;
	}

	double Value = 0.0;
	if (Root->TryGetNumberField(TEXT("width"), Value))
	{
		OutBrief.Width = FMath::RoundToInt32(Value);
	}
	if (Root->TryGetNumberField(TEXT("height"), Value))
	{
		OutBrief.Height = FMath::RoundToInt32(Value);
	}
	if (Root->TryGetNumberField(TEXT("min_passage_width"), Value))
	{
		OutBrief.MinPassageWidth = FMath::RoundToInt32(Value);
	}
	if (Root->TryGetNumberField(TEXT("reachable_floor_min"), Value))
	{
		OutBrief.bHasReachableFloorMin = true;
		OutBrief.ReachableFloorMin = Value;
	}
	const TArray<TSharedPtr<FJsonValue>>* Range = nullptr;
	if (Root->TryGetArrayField(TEXT("rock_coverage"), Range) && Range != nullptr && Range->Num() == 2)
	{
		OutBrief.bHasRockCoverage = true;
		OutBrief.RockCoverageMin = (*Range)[0]->AsNumber();
		OutBrief.RockCoverageMax = (*Range)[1]->AsNumber();
	}
	const TSharedPtr<FJsonObject>* Counts = nullptr;
	if (Root->TryGetObjectField(TEXT("counts"), Counts) && Counts != nullptr)
	{
		for (const TPair<FString, TSharedPtr<FJsonValue>>& Entry : (*Counts)->Values)
		{
			const TArray<TSharedPtr<FJsonValue>>* Bounds = nullptr;
			if (!Entry.Value.IsValid() || !Entry.Value->TryGetArray(Bounds) || Bounds == nullptr || Bounds->Num() != 2)
			{
				OutError = FString::Printf(TEXT("brief counts.%s must be [min, max] in %s"), *Entry.Key, *FilePath);
				return false;
			}
			OutBrief.Counts.Add(TPair<FString, FIntPoint>(Entry.Key,
				FIntPoint(FMath::RoundToInt32((*Bounds)[0]->AsNumber()), FMath::RoundToInt32((*Bounds)[1]->AsNumber()))));
		}
	}
	return true;
}
