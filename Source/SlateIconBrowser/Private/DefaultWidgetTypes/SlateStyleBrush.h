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

	virtual bool HasDetails() override { return true; };
	virtual TMap<FString,FString> GetDetails() override
	{
		TMap<FString,FString> Ret;
		GetBrush();

#define ADDENUM(TYPE, DISP, FUNC) { \
		UEnum* Enum = StaticEnum<TYPE>(); \
		Ret.Add(TEXT(DISP), Enum->GetNameStringByValue(Brush->FUNC)); \
	}
		
		ADDENUM(ESlateBrushImageType::Type, "Image Type", GetImageType());
		
		if (Brush->GetImageType() != ESlateBrushImageType::NoImage) {
			Ret.Add(TEXT("Resource Name"), Brush->GetResourceName().ToString());
			Ret.Add(TEXT("Image Size"), Brush->GetImageSize().ToString());
		}

		FMargin Margin = Brush->GetMargin();
		if (Margin.Left == Margin.Right && Margin.Top == Margin.Bottom) {
			if (Margin.Left == Margin.Top) {
				if (Margin.Left != 0.)
					Ret.Add(TEXT("Margin"), FString::Printf(TEXT("%f"), Margin.Left));
			} else {
				Ret.Add(TEXT("Margin"), FString::Printf(TEXT("%f %f"), Margin.Left, Margin.Top));
			}
		} else {
			Ret.Add(TEXT("Margin"), FString::Printf(TEXT("%f %f %f %f"),
				Brush->GetMargin().Left,
				Brush->GetMargin().Top,
				Brush->GetMargin().Right,
				Brush->GetMargin().Bottom));
		}

		ADDENUM(ESlateBrushDrawType::Type, "Draw As", GetDrawType());
		ADDENUM(ESlateBrushMirrorType::Type, "Mirroring", GetMirroring());
		ADDENUM(ESlateBrushTileType::Type, "Tiling", GetTiling());

		Ret.Add(TEXT("Tint Color"), Brush->TintColor.GetSpecifiedColor().ToString());

#undef ADDENUM
		
		return Ret;
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
