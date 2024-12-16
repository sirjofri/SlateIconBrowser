#pragma once
#include "SlateStyleData.h"
#include "Widgets/Input/SSearchBox.h"

class FSlateStyleWidgetSearchBox : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FSearchBoxStyle* s = GetWidgetStyle<FSearchBoxStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SSearchBox)
				.Style(s)
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FSearchBoxStyle>();
	};
};
