#pragma once

class FSlateStyleData;

class SSlateStyleTableRow : public SMultiColumnTableRow<TSharedPtr<FSlateStyleData>>
{
public:
	SLATE_BEGIN_ARGS(SSlateStyleTableRow)
		{}
		SLATE_ARGUMENT(TSharedPtr<FSlateStyleData>, StyleData);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TSharedPtr<FSlateStyleData> StyleData;
};
