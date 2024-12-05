#pragma once
#include "SlateStyleData.h"

class FSlateStyleBrush : public ISlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override;
};
