#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetTextBlock : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override;

	inline static FName TypeName = FName("TextBlock");
};
