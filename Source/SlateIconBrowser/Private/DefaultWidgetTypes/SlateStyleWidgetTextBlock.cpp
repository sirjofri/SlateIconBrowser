#include "SlateStyleWidgetTextBlock.h"

#include "Styling/SlateStyleRegistry.h"

TSharedRef<SWidget> FSlateStyleWidgetTextBlock::GenerateRowWidget()
{
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(StyleName);
	if (!Style)
		return SNullWidget::NullWidget;
	const FTextBlockStyle s = Style->GetWidgetStyle<FTextBlockStyle>(PropertyName);
	
	return SNew(STextBlock)
		.TextStyle(&s)
		.Text(INVTEXT("Hello, World!"));
}