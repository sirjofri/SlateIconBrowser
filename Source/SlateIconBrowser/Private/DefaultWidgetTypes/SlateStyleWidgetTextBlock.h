#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetTextBlock : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const ISlateStyle* Style = GetSlateStyle();
		if (!Style)
			return SNullWidget::NullWidget;
		const FTextBlockStyle s = Style->GetWidgetStyle<FTextBlockStyle>(PropertyName);
	
		return SNew(STextBlock)
			.TextStyle(&s)
			.Text(INVTEXT("Hello, World!"));
	};
};
