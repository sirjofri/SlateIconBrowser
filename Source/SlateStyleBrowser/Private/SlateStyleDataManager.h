// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once
#include "SlateStyleBrowser/Public/SlateStyleDataManager.h"

class ISlateStyleDataProvider;

class FSlateStyleDataManager : public ISlateStyleDataManager
{
public:
	virtual void RegisterWidgetStyleProvider(TSharedPtr<ISlateStyleDataProvider> InSlateStyleDataProvider) override;
	virtual TSharedPtr<ISlateStyleDataProvider> GetSlateStyleDataProvider(FName InWidgetType) override;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) override;
	virtual void GetRegisteredTypes(TArray<FName>& OutTypes) override;

private:
	TMap<FName,TSharedPtr<ISlateStyleDataProvider>> StyleDataProviders;
};
