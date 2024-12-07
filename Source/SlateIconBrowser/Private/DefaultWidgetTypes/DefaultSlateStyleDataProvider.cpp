#include "DefaultSlateStyleDataProvider.h"

#include "SlateStyleWidgetButton.h"
#include "SlateStyleWidgetTextBlock.h"


TSharedPtr<FSlateStyleData> FDefaultSlateStyleDataProvider::MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName,
                                                                               FName WidgetType)
{
#define WIDGET(Tname, StyleType, ItemType) \
	if (WidgetType == Tname && SlateStyle->HasWidgetStyle<StyleType>(PropertyName)) { \
		const StyleType& ws = SlateStyle->GetWidgetStyle<StyleType>(PropertyName); \
		TSharedPtr<FSlateStyleData> StyleData = MakeShared<ItemType>(); \
		StyleData->Initialize(SlateStyle->GetStyleSetName(), PropertyName, Tname, ws.GetTypeName()); \
		return StyleData; \
	}

	WIDGET(T_TextBlock, FTextBlockStyle, FSlateStyleWidgetTextBlock);
	WIDGET(T_Button, FButtonStyle, FSlateStyleWidgetButton);
	
#undef WIDGET
	return nullptr;
}

TArray<FName> FDefaultSlateStyleDataProvider::GetSupportedWidgetTypes()
{
	return {
		T_TextBlock,
		T_Button,
	};
}
