#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetTextBlock : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		FTextBlockStyle s;
		if (!GetWidgetStyle<FTextBlockStyle>(s))
			return SNullWidget::NullWidget;
	
		return SNew(STextBlock)
			.TextStyle(&s)
			.Text(INVTEXT("Hello, World!"));
	};

	virtual void InitializeDetails() override
	{
		FTextBlockStyle s;
		if (!GetWidgetStyle<FTextBlockStyle>(s))
			return;

#define OBJNAME(OBJ) (IsValid(OBJ) ? OBJ->GetName() : TEXT("None"))
		
		Details.Add(TEXT("Color"), s.ColorAndOpacity.GetSpecifiedColor().ToString());
		Details.Add(TEXT("Shadow Offset"), s.ShadowOffset.ToString());
		Details.Add(TEXT("Shadow Color"), s.ShadowColorAndOpacity.ToString());
		Details.Add(TEXT("Selected Background Color"), s.SelectedBackgroundColor.GetSpecifiedColor().ToString());
		Details.Add(TEXT("Highlight Color"), s.HighlightColor.GetSpecifiedColor().ToString());
		Details.Add(TEXT("Highlight Shape"), s.HighlightShape.IsSet() ? TEXT("Yes") : TEXT("No"));
		Details.Add(TEXT("Strike Brush"), s.StrikeBrush.IsSet() ? TEXT("Yes") : TEXT("No"));
		Details.Add(TEXT("Underline Brush"), s.UnderlineBrush.IsSet() ? TEXT("Yes") : TEXT("No"));
		Details.Add(TEXT("Transform Policy"), GetEnumValue<ETextTransformPolicy>(s.TransformPolicy));
		Details.Add(TEXT("Overflow Policy"), GetEnumValue<ETextOverflowPolicy>(s.OverflowPolicy));

		Details.Add(TEXT("FONT"), TEXT(""));
		Details.Add(TEXT("\tFont Family"), OBJNAME(s.Font.FontObject));
		Details.Add(TEXT("\tSize"), FString::Printf(TEXT("%f"), s.Font.Size));
		Details.Add(TEXT("\tLetter Spacing"), FString::Printf(TEXT("%d"), s.Font.LetterSpacing));
		Details.Add(TEXT("\tSkew"), FString::Printf(TEXT("%f"), s.Font.SkewAmount));
		Details.Add(TEXT("\tTypefaceFontName"), s.Font.TypefaceFontName.ToString());
		if (s.Font.bForceMonospaced) {
			Details.Add(TEXT("\tMonospaced Width"), FString::Printf(TEXT("%f"), s.Font.MonospacedWidth));
		}
		Details.Add(TEXT("\tMaterial"), OBJNAME(s.Font.FontMaterial));
		Details.Add(TEXT("\tOUTLINE"), TEXT(""));
		Details.Add(TEXT("\t\tSize"), FString::Printf(TEXT("%d"), s.Font.OutlineSettings.OutlineSize));
		Details.Add(TEXT("\t\tCorners"), s.Font.OutlineSettings.bMiteredCorners ? TEXT("Mitered") : TEXT("Rounded"));
		Details.Add(TEXT("\t\tSeparate Fill Alpha"), s.Font.OutlineSettings.bSeparateFillAlpha ? TEXT("Yes") : TEXT("No"));
		Details.Add(TEXT("\t\tApply to Drop Shadows"), s.Font.OutlineSettings.bApplyOutlineToDropShadows ? TEXT("Yes") : TEXT("No"));
		Details.Add(TEXT("\t\tMaterial"), OBJNAME(s.Font.OutlineSettings.OutlineMaterial));
		Details.Add(TEXT("\t\tColor"), s.Font.OutlineSettings.OutlineColor.ToString());

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
