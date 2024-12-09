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
	SMultiColumnTableRow<TSharedPtr<FSlateStyleData>>::Construct(FTableRowArgs()
		.ToolTipText(INVTEXT("Test")), InOwnerTableView);
}
TSharedRef<SWidget> SSlateStyleTableRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	TSharedPtr<FSlateStyleData> data = StyleData.Pin();
	if (!data.IsValid()) {
		return SNullWidget::NullWidget;
	}
	
	if (ColumnName == TEXT("StyleSetName")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(data->GetStyleName()))
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText.Subdued")))
			];
	}
	if (ColumnName == TEXT("PropertyName")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(data->GetPropertyName()))
			];
	}
	if (ColumnName == TEXT("WidgetType")) {
		return SNew(SBox)
			.VAlign(VAlign_Center)
			.Padding(5)
			[
				SNew(STextBlock)
				.Text(FText::FromName(data->GetType()))
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText.Subdued")))
			];
	}
	if (ColumnName == TEXT("Widget")) {
		return
			SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			// I would prefer to fetch this lazy (TODO: possible using OnOpening and custom SToolTip class)
			.ToolTip(GetToolTipWidget(StyleData))
			.Padding(5)
			[
				data->GenerateRowWidget()
			];
	}
	return SNullWidget::NullWidget;
}

TSharedPtr<IToolTip> SSlateStyleTableRow::GetToolTipWidget(TWeakPtr<FSlateStyleData> InStyleData)
{
	TSharedPtr<FSlateStyleData> data = InStyleData.Pin();
	if (!data.IsValid())
		return nullptr;
	if (!data->HasDetails())
		return nullptr;
	
	FPropertyEditorModule& Pem = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	TSharedRef<SVerticalBox> box = SNew(SVerticalBox);

	for (TTuple<FString,FString> kv : data->GetDetails()) {
		box->AddSlot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText.Subdued"))
				.Text(FText::FromString(kv.Key + TEXT(": ")))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
				.Text(FText::FromString(kv.Value))
			]
		];
	}
	
	return SNew(SToolTip)
		[
			SNew(SBorder)
			.BorderImage(EDITOR_STYLE_SAFE()::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
			.Padding(6)
			[
				box
			]
		];
}
