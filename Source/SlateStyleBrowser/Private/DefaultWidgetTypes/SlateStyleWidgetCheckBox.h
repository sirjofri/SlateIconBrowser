#pragma once
#include "SlateStyleData.h"

class FSlateStyleWidgetCheckBox : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		bool found;
		const FCheckBoxStyle& s = GetWidgetStyle<FCheckBoxStyle>(found);
		if (!found)
			return SNullWidget::NullWidget;

		return SNew(SBox)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&s)
					.IsChecked_Lambda([]() { return ECheckBoxState::Unchecked; })
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&s)
					.IsChecked_Lambda([]() { return ECheckBoxState::Checked; })
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&s)
					.IsChecked_Lambda([]() { return ECheckBoxState::Undetermined;})
				]
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FCheckBoxStyle>();
	};
};
