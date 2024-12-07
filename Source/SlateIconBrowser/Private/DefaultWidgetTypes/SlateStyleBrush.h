#pragma once
#include "SlateStyleData.h"

class FSlateStyleBrush : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override;
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder) override;

	inline static FName TypeName = FName("Brush");
};
