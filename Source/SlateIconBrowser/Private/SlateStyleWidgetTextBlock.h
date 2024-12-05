#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetTextBlock : public ISlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override;
};
