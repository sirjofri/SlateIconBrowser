#pragma once
#include "SlateStyleData.h"
#include "Widgets/Notifications/SProgressBar.h"

class FWProgressBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FProgressBarStyle& s = GetWidgetStyle<FProgressBarStyle>();
		if (s.GetTypeName() == NAME_None)
			return SNullWidget::NullWidget;

		return
			SNew(SBox)
			.MinDesiredWidth(200.)
			[
				SNew(SProgressBar)
				.Style(&s)
				.Percent(0.5)
			];
	};
};
