#include "SlateStyleTableRow.h"

#include "SlateStyleData.h"

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

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
				.Text(FText::FromName(data->GetStyleSetName()))
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
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.ToolTip(SNew(SSlateStyleToolTip, data))
			.Padding(5)
			[
				data->GenerateRowWidget()
			];
	}
	return SNullWidget::NullWidget;
}

void SSlateStyleToolTip::Construct(const FArguments& InArgs, const TWeakPtr<FSlateStyleData>& InStyleData)
{
	StyleData = InStyleData;

	SToolTip::FArguments args = SToolTip::FArguments();

	TSharedPtr<FSlateStyleData> data = StyleData.Pin();
	if (data.IsValid() && (data->HasDetails() || data->GetExtendedPreview().IsValid())) {
		args.Content()
		[
			SNew(SBox)
		];
	}

	SToolTip::Construct(args);
}

void SSlateStyleToolTip::OnOpening()
{
	TSharedPtr<FSlateStyleData> data = StyleData.Pin();
	if (!data.IsValid())
		return;

	TSharedPtr<SWidget> preview = data->GetExtendedPreview();
	if (!(data->HasDetails() || preview.IsValid()))
		return;

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
		bool IsHeading = kv.Value.IsEmpty(); // IsUpper() needed?
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
	
	SetContentWidget(SNew(SBorder)
		.BorderImage(EDITOR_STYLE_SAFE()::GetBrush("ContentBrowser.TileViewTooltip.ContentBorder"))
		.Padding(6)
		[
			box
		]);
}
