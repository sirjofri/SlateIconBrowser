#include "DefaultSlateStyleDataProvider.h"

#include "SlateStyleWidgetTextBlock.h"


TSharedPtr<FSlateStyleData> FDefaultSlateStyleDataProvider::MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName,
                                                                               FName WidgetType)
{
#define WIDGET(Tname, StyleType, ItemType) \
	if (WidgetType == Tname && SlateStyle->HasWidgetStyle<StyleType>(PropertyName)) { \
		const StyleType& ws = SlateStyle->GetWidgetStyle<StyleType>(PropertyName); \
		TSharedPtr<FSlateStyleData> StyleData = MakeShared<ItemType>(); \
		StyleData->Initialize(SlateStyle->GetStyleSetName(), PropertyName, ItemType::TypeName, ws.GetTypeName()); \
		return StyleData; \
	}

	if (WidgetType == T_TextBlock && SlateStyle->HasWidgetStyle<FTextBlockStyle>(PropertyName)) {
		const FTextBlockStyle& ws = SlateStyle->GetWidgetStyle<FTextBlockStyle>(PropertyName);
		TSharedPtr<FSlateStyleData> StyleData = MakeShared<FSlateStyleWidgetTextBlock>();
		StyleData->Initialize(SlateStyle->GetStyleSetName(), PropertyName, FSlateStyleWidgetTextBlock::TypeName, ws.GetTypeName());
		return StyleData;
	};
	
#undef WIDGET
	return nullptr;
}

TArray<FName> FDefaultSlateStyleDataProvider::GetSupportedWidgetTypes()
{
	return {
		T_TextBlock,
	};
}
