#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetExpandableArea : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FExpandableAreaStyle* s = GetWidgetStyle<FExpandableAreaStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SExpandableArea)
			.InitiallyCollapsed(true)
			.Style(s);
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FExpandableAreaStyle>();
	};
};