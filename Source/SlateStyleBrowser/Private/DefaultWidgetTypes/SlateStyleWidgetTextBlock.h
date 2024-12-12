#pragma once
#include "SlateStyleData.h"

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

		FillDetailsWithProperties<FTextBlockStyle>();

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
