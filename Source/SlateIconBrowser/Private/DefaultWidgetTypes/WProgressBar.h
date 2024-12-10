#pragma once
#include "SlateStyleData.h"
#include "Widgets/Notifications/SProgressBar.h"

class FWProgressBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		FProgressBarStyle s;
		if (!GetWidgetStyle<FProgressBarStyle>(s))
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
