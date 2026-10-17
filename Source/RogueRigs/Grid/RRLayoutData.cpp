// Rogue Rigs - in-memory layout of one level and the thread-safe parser of the layout JSON (level-layout-schema).

#include "Grid/RRLayoutData.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RogueRigs.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FString RRLayout::GetLayoutFilePath(const FString& LayoutName)
{
	return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("RogueRigs/Data/Levels"), LayoutName + TEXT(".json"));
}

bool RRLayout::ParseCellType(const FString& TypeName, ERRCellType& OutType)
{
	// Function-local static: initialised once (thread-safe in C++11), read-only afterwards.
	static const TMap<FString, ERRCellType> TypeByName = {
		{ TEXT("void"), ERRCellType::Void },
		{ TEXT("rock"), ERRCellType::Rock },
		{ TEXT("ore"), ERRCellType::Ore },
		{ TEXT("heal_ore"), ERRCellType::HealOre },
		{ TEXT("special_ore"), ERRCellType::SpecialOre },
		{ TEXT("barrel"), ERRCellType::Barrel },
		{ TEXT("spawn"), ERRCellType::Spawn },
	};

	if (const ERRCellType* Found = TypeByName.Find(TypeName))
	{
		OutType = *Found;
		return true;
	}
	return false;
}

bool RRLayout::LoadLayoutFile(const FString& LayoutName, const FString& FilePath, FRRLayoutData& OutLayout)
{
	OutLayout = FRRLayoutData();
	OutLayout.Name = LayoutName;

	FString FileText;
	if (!FFileHelper::LoadFileToString(FileText, *FilePath))
	{
		OutLayout.Error = FString::Printf(TEXT("layout file not found or unreadable: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OutLayout.Error = FString::Printf(TEXT("JSON parse error in %s: %s"), *FilePath, *Reader->GetErrorMessage());
		return false;
	}

	int32 Version = 0;
	int32 Width = 0;
	int32 Height = 0;
	const TArray<TSharedPtr<FJsonValue>>* CellValues = nullptr;
	if (!Root->TryGetNumberField(TEXT("version"), Version) || Version != 1)
	{
		OutLayout.Error = FString::Printf(TEXT("missing or unsupported 'version' (expected 1) in %s"), *FilePath);
		return false;
	}
	if (!Root->TryGetNumberField(TEXT("width"), Width) || !Root->TryGetNumberField(TEXT("height"), Height) || Width <= 0 || Height <= 0)
	{
		OutLayout.Error = FString::Printf(TEXT("missing or invalid 'width'/'height' in %s"), *FilePath);
		return false;
	}
	if (!Root->TryGetArrayField(TEXT("cells"), CellValues) || CellValues == nullptr)
	{
		OutLayout.Error = FString::Printf(TEXT("missing 'cells' array in %s"), *FilePath);
		return false;
	}

	TArray<ERRCellType> NewCells;
	NewCells.Init(ERRCellType::Floor, Width * Height);
	FIntPoint NewSpawn(-1, -1);

	for (int32 Index = 0; Index < CellValues->Num(); ++Index)
	{
		const TSharedPtr<FJsonObject>* CellObject = nullptr;
		if (!(*CellValues)[Index].IsValid() || !(*CellValues)[Index]->TryGetObject(CellObject) || CellObject == nullptr)
		{
			OutLayout.Error = FString::Printf(TEXT("cells[%d] is not an object in %s"), Index, *FilePath);
			return false;
		}

		int32 X = 0;
		int32 Y = 0;
		FString TypeName;
		ERRCellType Type = ERRCellType::Floor;
		if (!(*CellObject)->TryGetNumberField(TEXT("x"), X) || !(*CellObject)->TryGetNumberField(TEXT("y"), Y)
			|| !(*CellObject)->TryGetStringField(TEXT("type"), TypeName))
		{
			OutLayout.Error = FString::Printf(TEXT("cells[%d] lacks x, y or type in %s"), Index, *FilePath);
			return false;
		}
		if (X < 0 || X >= Width || Y < 0 || Y >= Height)
		{
			OutLayout.Error = FString::Printf(TEXT("cells[%d] (%d, %d) is out of bounds in %s"), Index, X, Y, *FilePath);
			return false;
		}
		if (!ParseCellType(TypeName, Type))
		{
			OutLayout.Error = FString::Printf(TEXT("cells[%d] has unknown type '%s' in %s"), Index, *TypeName, *FilePath);
			return false;
		}

		NewCells[Y * Width + X] = Type;
		if (Type == ERRCellType::Spawn)
		{
			NewSpawn = FIntPoint(X, Y);
		}
	}

	if (NewSpawn.X < 0)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RRLayout: no spawn cell in %s"), *FilePath);
	}

	OutLayout.Width = Width;
	OutLayout.Height = Height;
	OutLayout.Cells = MoveTemp(NewCells);
	OutLayout.SpawnCell = NewSpawn;
	return true;
}
