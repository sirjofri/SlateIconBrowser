#pragma once
#include "SlateStyleData.h"

#if (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION < 3) || ENGINE_MAJOR_VERSION == 4
#include "Widgets/Layout/SExpandableArea.h"
#endif

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
