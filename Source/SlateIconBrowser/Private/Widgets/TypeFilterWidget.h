#pragma once

class STypeFilterWidget : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnSelectionChanged, TArray<FName> SelectedOptions);
	
	SLATE_BEGIN_ARGS(STypeFilterWidget)
		{}
		
		SLATE_ATTRIBUTE(FText, SelectPrompt);
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void SetOptions(TArray<FName> InOptions);
	void SelectAll();

private:
	FReply ToggleSelection(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name);
	TSharedRef<SWidget> GenerateWidget(TSharedPtr<FName> Name);
	ECheckBoxState GetOptionCheckState(FName Name) const;
	void OnCheckStateChanged(ECheckBoxState CheckBoxState, FName Name);

private:
	TSharedPtr<SComboBox<TSharedPtr<FName>>> SelectionBox;
	
	TArray<TSharedPtr<FName>> Options;
	TArray<FName> SelectedOptions;

private:
	TAttribute<FText> SelectPrompt;
	FOnSelectionChanged OnSelectionChanged;
};
