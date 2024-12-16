#pragma once
#include "SlateStyleData.h"

class FDefaultSlateStyleDataProvider : public ISlateStyleDataProvider
{
public:
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) override;
	virtual TArray<FName> GetSupportedWidgetTypes() override;
	virtual TArray<FString> GetDefaultCopyStyles(FName WidgetType) override;
};
