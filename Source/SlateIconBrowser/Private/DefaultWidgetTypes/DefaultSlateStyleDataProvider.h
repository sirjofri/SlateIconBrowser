#pragma once
#include "SlateStyleData.h"

class FDefaultSlateStyleDataProvider : public ISlateStyleDataProvider
{
public:
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) override;
	virtual TArray<FName> GetSupportedWidgetTypes() override;

private:
	inline static FName T_TextBlock = FName("TextBlock");
	inline static FName T_Button = FName("Button");
};
