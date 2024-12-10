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

	auto IsUpper = [](const FString& Str)
	{
		for (int i = 0; i < Str.Len(); i++) {
			if (!FChar::IsAlpha(Str[i]) || FChar::IsUpper(Str[i]))
				continue;
			return false;
		}
		return true;
	};

	TSharedRef<SVerticalBox> box = SNew(SVerticalBox);

	TSharedPtr<SWidget> preview = data->GetExtendedPreview();
	if (preview.IsValid()) {
		box->AddSlot().AutoHeight()
		[
			SNew(SBorder)
			.Padding(10, 5)
			[
				preview.ToSharedRef()
			]
		];
		box->AddSlot().AutoHeight()
		[
			SNew(SSpacer)
			.Size(FVector2D(10.))
		];
	}

	for (TTuple<FString,FString> kv : data->GetDetails()) {
		bool IsHeading = IsUpper(kv.Key) && kv.Value.IsEmpty();
		box->AddSlot().AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(IsHeading ? "NormalText.Important" : "NormalText.Subdued"))
				.Text(FText::FromString(IsHeading ? kv.Key : kv.Key + TEXT(": ")))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>("NormalText"))
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
