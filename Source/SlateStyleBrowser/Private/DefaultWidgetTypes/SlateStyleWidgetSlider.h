#pragma once
#include "SlateStyleData.h"
#include "Widgets/Input/SSlider.h"

class FSlateStyleWidgetSlider : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FSliderStyle* s = GetWidgetStyle<FSliderStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SSlider)
			.Orientation(Orient_Horizontal)
			.Style(s)
			.Value(0.7);
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FSliderStyle>();
	};
};
