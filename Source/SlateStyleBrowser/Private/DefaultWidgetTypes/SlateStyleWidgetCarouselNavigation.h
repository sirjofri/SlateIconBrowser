#pragma once
#include "SCarouselNavigationBar.h"
#include "SCarouselNavigationButton.h"
#include "SlateStyleData.h"
#include "WidgetCarouselStyle.h"

class FSlateStyleWidgetCarouselNavigationButton : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FWidgetCarouselNavigationButtonStyle* s = GetWidgetStyle<FWidgetCarouselNavigationButtonStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				SNew(SCarouselNavigationButton)
				.Style(s)
				.Direction(SCarouselNavigationButton::ENavigationButtonDirection::Left)
			]
			+SHorizontalBox::Slot()
			[
				SNew(SCarouselNavigationButton)
				.Style(s)
				.Direction(SCarouselNavigationButton::ENavigationButtonDirection::Right)
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FWidgetCarouselNavigationButtonStyle>();
	};
};

class FSlateStyleWidgetCarouselNavigationBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FWidgetCarouselNavigationBarStyle* s = GetWidgetStyle<FWidgetCarouselNavigationBarStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		return SNew(SCarouselNavigationBar)
			.Style(s)
			.ItemCount(5)
			.CurrentItemIndex(3);
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FWidgetCarouselNavigationBarStyle>();
	};
};