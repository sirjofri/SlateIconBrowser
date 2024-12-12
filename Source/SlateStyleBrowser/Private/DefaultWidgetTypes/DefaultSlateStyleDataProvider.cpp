#include "DefaultSlateStyleDataProvider.h"

#include "SlateStyleWidgetButton.h"
#include "SlateStyleWidgetCheckBox.h"
#include "SlateStyleWidgetTextBlock.h"
#include "SlateStyleWidgetProgressBar.h"

namespace DefaultSupportedTypes
{
	static FName T_TextBlock = FName("TextBlock");
	static FName T_Button = FName("Button");
	static FName T_ProgressBar = FName("ProgressBar");
	static FName T_CheckBox = FName("CheckBox");
};

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

	WIDGET(DefaultSupportedTypes::T_TextBlock, FTextBlockStyle, FSlateStyleWidgetTextBlock);
	WIDGET(DefaultSupportedTypes::T_Button, FButtonStyle, FSlateStyleWidgetButton);
	WIDGET(DefaultSupportedTypes::T_ProgressBar, FProgressBarStyle, FSlateStyleWidgetProgressBar);
	WIDGET(DefaultSupportedTypes::T_CheckBox, FCheckBoxStyle, FSlateStyleWidgetCheckBox);
	
#undef WIDGET
	return nullptr;
}

TArray<FName> FDefaultSlateStyleDataProvider::GetSupportedWidgetTypes()
{
	return {
		DefaultSupportedTypes::T_TextBlock,
		DefaultSupportedTypes::T_Button,
		DefaultSupportedTypes::T_ProgressBar,
		DefaultSupportedTypes::T_CheckBox,
	};
}
