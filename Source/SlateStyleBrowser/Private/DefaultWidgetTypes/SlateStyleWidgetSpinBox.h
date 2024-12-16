#pragma once
#include "SlateStyleData.h"
#include "Widgets/Input/SSpinBox.h"

class FSlateStyleWidgetSpinBox : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FSpinBoxStyle* s = GetWidgetStyle<FSpinBoxStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SSpinBox<float>)
				.Style(s)
				.Value(0.7)
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FSpinBoxStyle>();
	};
};
