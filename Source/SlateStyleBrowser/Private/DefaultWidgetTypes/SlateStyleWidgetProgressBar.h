#pragma once
#include "SlateStyleData.h"
#include "Widgets/Notifications/SProgressBar.h"

class FSlateStyleWidgetProgressBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FProgressBarStyle* s = GetWidgetStyle<FProgressBarStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return
			SNew(SBox)
			.MinDesiredWidth(200.)
			[
				SNew(SProgressBar)
				.Style(s)
				.Percent(0.75) // positive value for your mind
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FProgressBarStyle>();
	};

	virtual void InitializePreview() override
	{
		const FProgressBarStyle* s = GetWidgetStyle<FProgressBarStyle>();
		if (!s)
			return;

		ExtendedPreview = SNew(SBox)
			.MinDesiredWidth(200)
			[
				SNew(SProgressBar)
				.Style(s)
				.Percent_Lambda([&]()
				{
					// random value with refresh rate doesn't work, because RefreshRate has no impact
					FDateTime dt = FDateTime::Now();
					int ux = dt.GetSecond();
					return FMath::Abs(FMath::Sin((float)ux));
				})
			];
	};
};
