// Rogue Rigs - full-screen black fade of a level transition (task-318-02), drawn above every transition widget.

#include "UI/RRScreenFadeWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"

void URRScreenFadeWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		// An image without a texture draws a solid box of its color.
		FadeImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("FadeImage"));
		FadeImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.f));
		WidgetTree->RootWidget = FadeImage;
	}
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void URRScreenFadeWidget::SetFadeAlpha(float Alpha)
{
	if (FadeImage)
	{
		FadeImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, FMath::Clamp(Alpha, 0.f, 1.f)));
	}
}
