// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.


#include "SlateStyleDataManager.h"

#include "SlateStyleData.h"

void FSlateStyleDataManager::RegisterWidgetStyleProvider(
	TSharedPtr<ISlateStyleDataProvider> InSlateStyleDataProvider)
{
	if (!InSlateStyleDataProvider.IsValid())
		return;

	TArray<FName> Types = InSlateStyleDataProvider->GetSupportedWidgetTypes();
	for (FName N : Types) {
		StyleDataProviders.Add(N, InSlateStyleDataProvider);
	}
}

TSharedPtr<ISlateStyleDataProvider> FSlateStyleDataManager::GetSlateStyleDataProvider(FName InWidgetType)
{
	TSharedPtr<ISlateStyleDataProvider>* SlateStyleDataProvider = StyleDataProviders.Find(InWidgetType);
	if (SlateStyleDataProvider)
		return *SlateStyleDataProvider;
	return nullptr;
}

TSharedPtr<FSlateStyleData> FSlateStyleDataManager::MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName,
                                                                       FName WidgetType)
{
	TSharedPtr<ISlateStyleDataProvider> SlateStyleDataProvider = GetSlateStyleDataProvider(WidgetType);
	if (!SlateStyleDataProvider.IsValid())
		return nullptr;

	return SlateStyleDataProvider->MakeSlateStyleData(SlateStyle, PropertyName, WidgetType);
}

void FSlateStyleDataManager::GetRegisteredTypes(TArray<FName>& OutTypes)
{
	StyleDataProviders.GetKeys(OutTypes);
}
