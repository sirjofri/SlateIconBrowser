#include "SlateStyleTableRow.h"

#include "SlateStyleData.h"

#if ENGINE_MAJOR_VERSION == 4
#define EDITOR_STYLE_SAFE() FEditorStyle
#else
#define EDITOR_STYLE_SAFE() FAppStyle
#endif

void SSlateStyleTableRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	StyleData = InArgs._StyleData;
	SMultiColumnTableRow<TSharedPtr<FSlateStyleData>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}
TSharedRef<SWidget> SSlateStyleTableRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (!StyleData.IsValid()) {
		return SNullWidget::NullWidget;
	}
	
	if (ColumnName == TEXT("StyleSetName")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(StyleData->GetStyleName()))
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText.Subdued")))
			];
	}
	if (ColumnName == TEXT("PropertyName")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(StyleData->GetPropertyName()))
			];
	}
	if (ColumnName == TEXT("WidgetType")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(StyleData->GetType()))
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText.Subdued")))
			];
	}
	if (ColumnName == TEXT("Widget")) {
		return
			SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.)
				[
					SNew(SSpacer)
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					StyleData->GenerateRowWidget()
				]
			];
	}
	return SNullWidget::NullWidget;
}
