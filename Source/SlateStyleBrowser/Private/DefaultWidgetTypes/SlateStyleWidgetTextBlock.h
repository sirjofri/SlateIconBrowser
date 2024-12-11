#pragma once
#include "SlateStyleData.h"

#if ENGINE_MAJOR_VERSION == 5
#define BorCtoString() GetSpecifiedColor().ToString()
#else
#define BorCtoString() ToString()
#endif

class FSlateStyleWidgetTextBlock : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		bool found;
		const FTextBlockStyle& s = GetWidgetStyle<FTextBlockStyle>(found);
		if (!found)
			return SNullWidget::NullWidget;
	
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.TextStyle(&s)
				.Text(INVTEXT("Hello, World!"))
			];
	};

	virtual void InitializeDetails() override
	{
		bool found;
		const FTextBlockStyle& s = GetWidgetStyle<FTextBlockStyle>(found);
		if (!found)
			return;

#define OBJNAME(OBJ) (IsValid(OBJ) ? OBJ->GetName() : TEXT("None"))
		
		AddDetail(TEXT("Color"), s.ColorAndOpacity.GetSpecifiedColor().ToString());
		AddDetail(TEXT("Shadow Offset"), s.ShadowOffset.ToString());
		AddDetail(TEXT("Shadow Color"), s.ShadowColorAndOpacity.ToString());
		AddDetail(TEXT("Selected Background Color"), s.SelectedBackgroundColor.GetSpecifiedColor().ToString());
		AddDetail(TEXT("Highlight Color"), s.HighlightColor.BorCtoString());
		AddDetail(TEXT("Highlight Shape"), GetEnumValue<ESlateBrushDrawType::Type>(s.HighlightShape.DrawAs));
		AddDetail(TEXT("Strike Brush"), GetEnumValue<ESlateBrushDrawType::Type>(s.StrikeBrush.DrawAs));
		AddDetail(TEXT("Underline Brush"), GetEnumValue<ESlateBrushDrawType::Type>(s.UnderlineBrush.DrawAs));
#if ENGINE_MAJOR_VERSION == 5
		AddDetail(TEXT("Transform Policy"), GetEnumValue<ETextTransformPolicy>(s.TransformPolicy));
		AddDetail(TEXT("Overflow Policy"), GetEnumValue<ETextOverflowPolicy>(s.OverflowPolicy));
#endif

		AddDetail(TEXT("FONT"), TEXT(""));
		AddDetail(TEXT("\tFont Family"), OBJNAME(s.Font.FontObject));
		AddDetail(TEXT("\tSize"), FString::Printf(TEXT("%f"), s.Font.Size));
		AddDetail(TEXT("\tLetter Spacing"), FString::Printf(TEXT("%d"), s.Font.LetterSpacing));
#if ENGINE_MAJOR_VERSION == 5
		AddDetail(TEXT("\tSkew"), FString::Printf(TEXT("%f"), s.Font.SkewAmount));
#endif
		AddDetail(TEXT("\tTypefaceFontName"), s.Font.TypefaceFontName.ToString());
		
#if ENGINE_MAJOR_VERSION == 5
		if (s.Font.bForceMonospaced) {
			AddDetail(TEXT("\tMonospaced Width"), FString::Printf(TEXT("%f"), s.Font.MonospacedWidth));
		}
#endif
		AddDetail(TEXT("\tMaterial"), OBJNAME(s.Font.FontMaterial));
		AddDetail(TEXT("\tOUTLINE"), TEXT(""));
		AddDetail(TEXT("\t\tSize"), FString::Printf(TEXT("%d"), s.Font.OutlineSettings.OutlineSize));
#if ENGINE_MAJOR_VERSION == 5
		AddDetail(TEXT("\t\tCorners"), s.Font.OutlineSettings.bMiteredCorners ? TEXT("Mitered") : TEXT("Rounded"));
#endif
		AddDetail(TEXT("\t\tSeparate Fill Alpha"), s.Font.OutlineSettings.bSeparateFillAlpha ? TEXT("Yes") : TEXT("No"));
		AddDetail(TEXT("\t\tApply to Drop Shadows"), s.Font.OutlineSettings.bApplyOutlineToDropShadows ? TEXT("Yes") : TEXT("No"));
		AddDetail(TEXT("\t\tMaterial"), OBJNAME(s.Font.OutlineSettings.OutlineMaterial));
		AddDetail(TEXT("\t\tColor"), s.Font.OutlineSettings.OutlineColor.ToString());

#undef OBJNAME

		// Initialize Extended Preview
		ExtendedPreview = SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(&s)
				.Text(INVTEXT("the quick brown fox jumps over the lazy dog! 01234"))
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(&s)
				.Text(INVTEXT("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG! 56789"))
			];
	};
};
