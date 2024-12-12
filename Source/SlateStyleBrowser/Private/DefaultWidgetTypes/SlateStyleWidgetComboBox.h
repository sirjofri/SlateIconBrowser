#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetComboBox : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FComboBoxStyle* s = GetWidgetStyle<FComboBoxStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.ComboBoxStyle(s)
				[
					SNew(STextBlock)
					.Text(INVTEXT("Hello, World!"))
				]
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FComboBoxStyle>();
	};
};