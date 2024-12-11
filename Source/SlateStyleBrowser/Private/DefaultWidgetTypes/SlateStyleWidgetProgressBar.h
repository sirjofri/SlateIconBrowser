#pragma once
#include "SlateStyleData.h"
#include "Widgets/Notifications/SProgressBar.h"

class FSlateStyleWidgetProgressBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		bool found;
		const FProgressBarStyle& s = GetWidgetStyle<FProgressBarStyle>(found);
		if (!found)
			return SNullWidget::NullWidget;

		return
			SNew(SBox)
			.MinDesiredWidth(200.)
			[
				SNew(SProgressBar)
				.Style(&s)
				.Percent(0.75) // positive value for your mind
			];
	};

	virtual void InitializeDetails() override
	{
		bool found;
		const FProgressBarStyle& s = GetWidgetStyle<FProgressBarStyle>(found);
		if (!found)
			return;

		AddDetail(TEXT("Background Image"), GetEnumValue<ESlateBrushDrawType::Type>(s.BackgroundImage.DrawAs));
		AddDetail(TEXT("Fill Image"), GetEnumValue<ESlateBrushDrawType::Type>(s.FillImage.DrawAs));
		AddDetail(TEXT("Marquee Image"), GetEnumValue<ESlateBrushDrawType::Type>(s.MarqueeImage.DrawAs));
		
#if ENGINE_MAJOR_VERSION == 5
		AddDetail(TEXT("Enable Fill Animation"), s.EnableFillAnimation ? TEXT("Yes") : TEXT("No"));
#endif

		// Initialize preview
		ExtendedPreview = SNew(SBox)
			.MinDesiredWidth(200)
			[
				SNew(SProgressBar)
				.Style(&s)
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
