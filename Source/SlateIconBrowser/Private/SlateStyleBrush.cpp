#include "SlateStyleBrush.h"

#include "Styling/SlateStyleRegistry.h"

TSharedRef<SWidget> FSlateStyleBrush::GenerateRowWidget()
{
	auto Brush = FSlateStyleRegistry::FindSlateStyle(StyleName)->GetOptionalBrush(PropertyName);
	FVector2D DesiredIconSize = Brush->GetImageSize();
	if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
		DesiredIconSize = FVector2D(20);
	
	return SNew(SImage)
#if ENGINE_MAJOR_VERSION == 5
		.DesiredSizeOverride(DesiredIconSize)
#endif
		.Image(Brush);
}

void FSlateStyleBrush::FillRowContextMenu(FMenuBuilder& MenuBuilder)
{
	FSlateStyleData::FillRowContextMenu(MenuBuilder);
}
