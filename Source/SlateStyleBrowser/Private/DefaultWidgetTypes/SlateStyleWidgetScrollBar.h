#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetScrollBar : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FScrollBarStyle* s = GetWidgetStyle<FScrollBarStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		TSharedPtr<SScrollBar> sb = SNew(SScrollBar)
			.Style(s)
			.AlwaysShowScrollbar(true)
			.Orientation(Orient_Horizontal);

		sb->SetOnUserScrolled(FOnUserScrolled::CreateLambda([&](float f, TSharedPtr<SScrollBar> sbar)
		{
			if (sbar.IsValid())
				sbar->SetState(f, .2);
		}, sb));
		sb->SetState(.7, .2);

		return sb.ToSharedRef();
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FScrollBarStyle>();
	};
};