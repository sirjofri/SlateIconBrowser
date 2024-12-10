#pragma once
#include "SlateStyleData.h"

#define LOCTEXT_NAMESPACE "SlateStyleBrowserBrush"

class FSlateStyleBrush : public FSlateStyleData
{
public:
	inline static FName TypeName = FName("Brush");
	
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		GetBrush();
		FVector2D DesiredIconSize = Brush->GetImageSize();
		if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
			DesiredIconSize = FVector2D(20);
	
		return SNew(SImage)
	#if ENGINE_MAJOR_VERSION == 5
			.DesiredSizeOverride(DesiredIconSize)
	#endif
			.Image(Brush);
	};

	virtual void InitializeDetails() override
	{
		GetBrush();

#define ADDENUM(TYPE, DISP, FUNC) Details.Add(TEXT(DISP), GetEnumValue<TYPE>(Brush->FUNC));
		
		ADDENUM(ESlateBrushImageType::Type, "Image Type", GetImageType());
		
		if (Brush->GetImageType() != ESlateBrushImageType::NoImage) {
			Details.Add(TEXT("Resource Name"), Brush->GetResourceName().ToString());
			Details.Add(TEXT("Image Size"), Brush->GetImageSize().ToString());
		}

		FMargin Margin = Brush->GetMargin();
		if (Margin.Left == Margin.Right && Margin.Top == Margin.Bottom) {
			if (Margin.Left == Margin.Top) {
				if (Margin.Left != 0.)
					Details.Add(TEXT("Margin"), FString::Printf(TEXT("%f"), Margin.Left));
			} else {
				Details.Add(TEXT("Margin"), FString::Printf(TEXT("%f %f"), Margin.Left, Margin.Top));
			}
		} else {
			Details.Add(TEXT("Margin"), FString::Printf(TEXT("%f %f %f %f"),
				Brush->GetMargin().Left,
				Brush->GetMargin().Top,
				Brush->GetMargin().Right,
				Brush->GetMargin().Bottom));
		}

		ADDENUM(ESlateBrushDrawType::Type, "Draw As", GetDrawType());
		ADDENUM(ESlateBrushMirrorType::Type, "Mirroring", GetMirroring());
		ADDENUM(ESlateBrushTileType::Type, "Tiling", GetTiling());

		Details.Add(TEXT("Tint Color"), Brush->TintColor.GetSpecifiedColor().ToString());

#undef ADDENUM
	};

private:
	const FSlateBrush* GetBrush()
	{
		if (Brush)
			return Brush;
		const ISlateStyle* Style = GetSlateStyle();
		Brush = Style->GetOptionalBrush(PropertyName);
		return Brush;
	}
	const FSlateBrush* Brush = nullptr;
};
#undef LOCTEXT_NAMESPACE
