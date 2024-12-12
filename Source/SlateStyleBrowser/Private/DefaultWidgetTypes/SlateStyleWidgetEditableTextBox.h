#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetEditableTextBox : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FEditableTextBoxStyle* s = GetWidgetStyle<FEditableTextBoxStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SEditableTextBox)
			.Style(s)
			.Text(DefaultValues.HelloWorld);
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FEditableTextBoxStyle>();
	};
};
