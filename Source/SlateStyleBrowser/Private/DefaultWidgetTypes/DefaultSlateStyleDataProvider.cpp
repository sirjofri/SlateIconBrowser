#include "DefaultSlateStyleDataProvider.h"

#include "SlateStyleWidgetButton.h"
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

#define DEFTYPES(OP) \
	OP(TextBlock, FTextBlockStyle) \
	OP(Button, FButtonStyle) \
	OP(ComboBox, FComboBoxStyle) \
	OP(ComboButton, FComboButtonStyle) \
	OP(ProgressBar, FProgressBarStyle) \
	OP(CheckBox, FCheckBoxStyle) \
	OP(Slider, FSliderStyle) \
	OP(ExpandableArea, FExpandableAreaStyle) \
	OP(EditableTextBox, FEditableTextBoxStyle) \
	OP(EditableText, FEditableTextStyle) \
	OP(Hyperlink, FHyperlinkStyle) \
	OP(SpinBox, FSpinBoxStyle) \
	OP(ScrollBar, FScrollBarStyle) \
	OP(SearchBox, FSearchBoxStyle) \

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
