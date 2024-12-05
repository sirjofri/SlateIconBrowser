#include "TypeFilterWidget.h"

#if ENGINE_MAJOR_VERSION == 4
#define EDITOR_STYLE_SAFE() FEditorStyle
#else
#define EDITOR_STYLE_SAFE() FAppStyle
#endif

void STypeFilterWidget::Construct(const FArguments& InArgs)
{
	SelectPrompt = InArgs._SelectPrompt;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	
	ChildSlot
	[
		SAssignNew(SelectionBox, SComboBox<TSharedPtr<FName>>)
		.OptionsSource(&Options)
		.OnGenerateWidget(this, &STypeFilterWidget::GenerateWidget)
		[
			SNew(STextBlock)
			.Text(SelectPrompt)
		]
	];
}

void STypeFilterWidget::SetOptions(TArray<FName> InOptions)
{
	Options.Empty(InOptions.Num());

	for (FName N : InOptions)
		Options.AddUnique(MakeShareable(new FName(N)));

	if (SelectionBox.IsValid())
		SelectionBox->RefreshOptions();
}

void STypeFilterWidget::SelectAll()
{
	SelectedOptions.Empty(Options.Num());
	for (TSharedPtr<FName> Name : Options)
		SelectedOptions.Add(*Name);
	
	OnSelectionChanged.ExecuteIfBound(SelectedOptions);
}

FReply STypeFilterWidget::ToggleSelection(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name)
{
	if (SelectedOptions.Contains(Name)) {
		SelectedOptions.Remove(Name);
	} else {
		SelectedOptions.Add(Name);
	}
	OnSelectionChanged.ExecuteIfBound(SelectedOptions);
	return FReply::Handled();
}

void STypeFilterWidget::OnCheckStateChanged(ECheckBoxState CheckBoxState, FName Name)
{
	switch (CheckBoxState) {
	case ECheckBoxState::Unchecked:
		SelectedOptions.Remove(Name);
		break;
	case ECheckBoxState::Checked:
		SelectedOptions.AddUnique(Name);
		break;
	default:
		return;
	}
	OnSelectionChanged.ExecuteIfBound(SelectedOptions);
}

TSharedRef<SWidget> STypeFilterWidget::GenerateWidget(TSharedPtr<FName> Name)
{
	return
		SNew(SBorder)
		.BorderImage(EDITOR_STYLE_SAFE()::Get().GetBrush("NoBorder"))
		.OnMouseButtonDown(this, &STypeFilterWidget::ToggleSelection, *Name)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SCheckBox)
				.IsChecked(this, &STypeFilterWidget::GetOptionCheckState, *Name)
				.OnCheckStateChanged(this, &STypeFilterWidget::OnCheckStateChanged, *Name)
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromName(*Name))
			]
		];
}

ECheckBoxState STypeFilterWidget::GetOptionCheckState(FName Name) const
{
	return SelectedOptions.Contains(Name) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
