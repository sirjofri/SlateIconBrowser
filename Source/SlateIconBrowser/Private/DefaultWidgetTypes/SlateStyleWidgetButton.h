#pragma once
#include "SlateStyleData.h"
#include "Styling/SlateStyleRegistry.h"

class FSlateStyleWidgetButton : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(StyleName);
		if (!Style)
			return SNullWidget::NullWidget;

		const FButtonStyle& s = Style->GetWidgetStyle<FButtonStyle>(PropertyName);

		// Assumption:
		// - if we have no image, we want text
		// - if the image we have doesn't want to tile, we want text
		bool bWantsText = s.Normal.ImageType == ESlateBrushImageType::NoImage || s.Normal.Tiling != ESlateBrushTileType::NoTile;

		return SNew(SButton)
			.ButtonStyle(&s)
			.Text(bWantsText ? INVTEXT("Hello, World!") : FText::GetEmpty());
	};
};
