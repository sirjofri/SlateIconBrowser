#include "DefaultSlateStyleDataProvider.h"

#include "SlateStyleWidgetButton.h"
#include "SlateStyleWidgetCarouselNavigation.h"
#include "SlateStyleWidgetCheckBox.h"
#include "SlateStyleWidgetComboBox.h"
#include "SlateStyleWidgetComboButton.h"
#include "SlateStyleWidgetEditableText.h"
#include "SlateStyleWidgetEditableTextBox.h"
#include "SlateStyleWidgetExpandableArea.h"
#include "SlateStyleWidgetHyperlink.h"
#include "SlateStyleWidgetTextBlock.h"
#include "SlateStyleWidgetProgressBar.h"
#include "SlateStyleWidgetScrollBar.h"
#include "SlateStyleWidgetSearchBox.h"
#include "SlateStyleWidgetSlider.h"
#include "SlateStyleWidgetSpinBox.h"
#include "SlateStyleWidgetSplitter.h"
#include "Interfaces/IPluginManager.h"

#define DEFTYPES(OP) \
	OP(Button, FButtonStyle) \
	OP(CheckBox, FCheckBoxStyle) \
	OP(ComboBox, FComboBoxStyle) \
	OP(ComboButton, FComboButtonStyle) \
	OP(EditableText, FEditableTextStyle) \
	OP(EditableTextBox, FEditableTextBoxStyle) \
	OP(ExpandableArea, FExpandableAreaStyle) \
	OP(Hyperlink, FHyperlinkStyle) \
	OP(ProgressBar, FProgressBarStyle) \
	OP(ScrollBar, FScrollBarStyle) \
	OP(SearchBox, FSearchBoxStyle) \
	OP(Slider, FSliderStyle) \
	OP(SpinBox, FSpinBoxStyle) \
	OP(Splitter, FSplitterStyle) \
	OP(TextBlock, FTextBlockStyle) \
	OP(CarouselNavigationButton, FWidgetCarouselNavigationButtonStyle) \
	OP(CarouselNavigationBar, FWidgetCarouselNavigationBarStyle) \

namespace DefaultSupportedTypes
{
#define DONAMELIST(TYPE, STYLE) static FName T_##TYPE = FName(#TYPE);
	DEFTYPES(DONAMELIST)
#undef DONAMELIST
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

#define DOWIDGETLIST(TYPE, STYLE) WIDGET(DefaultSupportedTypes::T_##TYPE, STYLE, FSlateStyleWidget##TYPE);
	DEFTYPES(DOWIDGETLIST)
#undef DOWIDGETLIST
	
#undef WIDGET
	return nullptr;
}

TArray<FName> FDefaultSlateStyleDataProvider::GetSupportedWidgetTypes()
{
	return {
#define DOLIST(TYPE, STYLE) DefaultSupportedTypes::T_##TYPE,
		DEFTYPES(DOLIST)
#undef DOLIST
	};
}

TArray<FString> FDefaultSlateStyleDataProvider::GetDefaultCopyStyles(FName WidgetType)
{
	TArray<FString> Ret;
	FString Config = IPluginManager::Get().FindPlugin("SlateStyleBrowser")->GetBaseDir() / TEXT("Config") / TEXT("DefaultSlateStyleBrowser.ini");
	FConfigFile File;
	File.Read(Config);
	File.GetArray(TEXT("DefaultCopyStyles"), *WidgetType.ToString(), Ret);
	Ret.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"$2\")->GetWidgetStyle<$3>(\"$1\")"));
	return Ret;
}
