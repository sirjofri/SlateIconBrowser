#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetComboButton : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FComboButtonStyle* s = GetWidgetStyle<FComboButtonStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SComboButton)
				.ComboButtonStyle(s)
				.ButtonContent()
				[
					SNew(STextBlock)
					.Text(DefaultValues.HelloWorld)
				]
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FComboButtonStyle>();
	};
};