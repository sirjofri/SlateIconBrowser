#pragma once
#include "SlateStyleData.h"

#define LOCTEXT_NAMESPACE "SlateStyleBrowserBrush"

namespace FSlateStyleBrushDefaultValues
{
	FName TypeName = FName("Brush");
};

class FSlateStyleBrush : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		GetBrush();
		FVector2D DesiredIconSize = Brush->GetImageSize();
		if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
			DesiredIconSize = FVector2D(20);
	
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SImage)
	#if ENGINE_MAJOR_VERSION == 5
				.DesiredSizeOverride(DesiredIconSize)
	#endif
				.Image(Brush)
			];
	};

	virtual void Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType) override
	{
		FSlateStyleData::Initialize(InStyle, InPropertyName, FSlateStyleBrushDefaultValues::TypeName, InWidgetStyleType);
	};

	virtual void InitializeDetails() override
	{
		GetBrush();

#define ADDENUM(TYPE, DISP, FUNC) AddDetail(TEXT(DISP), GetEnumValue<TYPE>(Brush->FUNC));
		
		ADDENUM(ESlateBrushImageType::Type, "Image Type", GetImageType());
		
		if (Brush->GetImageType() != ESlateBrushImageType::NoImage) {
			AddDetail(TEXT("Resource Name"), Brush->GetResourceName().ToString());
			AddDetail(TEXT("Image Size"), Brush->GetImageSize().ToString());
		}

		AddDetail(TEXT("Margin"), MarginString(Brush->GetMargin()));

		ADDENUM(ESlateBrushDrawType::Type, "Draw As", GetDrawType());
		ADDENUM(ESlateBrushMirrorType::Type, "Mirroring", GetMirroring());
		ADDENUM(ESlateBrushTileType::Type, "Tiling", GetTiling());

		AddDetail(TEXT("Tint Color"), Brush->TintColor.GetSpecifiedColor().ToString());

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
