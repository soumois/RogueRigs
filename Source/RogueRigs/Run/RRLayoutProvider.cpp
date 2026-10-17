// Rogue Rigs - layout provider slot and its JSON file implementation (task-206-02).

#include "Run/RRLayoutProvider.h"

#include "Grid/RRLayoutData.h"
#include "Misc/Paths.h"

FRRJsonLayoutProvider::FRRJsonLayoutProvider()
	: LevelsDirectory(FPaths::GetPath(RRLayout::GetLayoutFilePath(TEXT("layout"))))
{
}

TSharedRef<FRRLayoutData> FRRJsonLayoutProvider::BuildLayout(const FRRLayoutRequest& Request) const
{
	TSharedRef<FRRLayoutData> Layout = MakeShared<FRRLayoutData>();
	if (Request.LayoutName.IsEmpty())
	{
		Layout->Error = FString::Printf(TEXT("no layout name for level %d"), Request.LevelIndex);
		return Layout;
	}
	RRLayout::LoadLayoutFile(Request.LayoutName, FPaths::Combine(LevelsDirectory, Request.LayoutName + TEXT(".json")), *Layout);
	return Layout;
}
