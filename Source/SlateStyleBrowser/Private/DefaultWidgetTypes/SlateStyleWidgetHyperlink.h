#pragma once
#include "SlateStyleData.h"
#include "Widgets/Input/SHyperlink.h"

class FSlateStyleWidgetHyperlink : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FHyperlinkStyle* s = GetWidgetStyle<FHyperlinkStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SHyperlink)
				.Style(s)
				.Text(DefaultValues.HelloWorld)
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FHyperlinkStyle>();
	};
};
