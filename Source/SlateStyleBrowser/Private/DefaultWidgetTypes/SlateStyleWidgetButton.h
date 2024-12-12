#pragma once
#include "SlateStyleData.h"

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

class FSlateStyleWidgetButton : public FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() override
	{
		const FButtonStyle* s = GetWidgetStyle<FButtonStyle>();
		if (!s)
			return SNullWidget::NullWidget;

		// Assumption:
		// - if we have no image, we want text
		// - if the image we have doesn't want to tile, we want text
		bool bWantsText = s->Normal.ImageType == ESlateBrushImageType::NoImage || s->Normal.Tiling != ESlateBrushTileType::NoTile;

		return SNew(SBox)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SButton)
				.ButtonStyle(s)
				.Text(bWantsText ? INVTEXT("Hello, World!") : FText::GetEmpty())
			];
	};

	virtual void InitializeDetails() override
	{
		FillDetailsWithProperties<FButtonStyle>();
	};

	virtual void InitializePreview() override
	{
		// TODO: Disable for now. Crash: UObject garbage collection due to culling of the SButtons.
		// Probably: resources in the FButtonStyle local copies.
#if 0
		const FButtonStyle* s = GetWidgetStyle<FButtonStyle>();
		if (!s)
			return;

		FButtonStyle sNormal = FButtonStyle(s);
		FButtonStyle sHovered = FButtonStyle(s).SetNormal(s.Hovered);
		FButtonStyle sPressed = FButtonStyle(s).SetNormal(s.Pressed);
		FButtonStyle sDisabled = FButtonStyle(s).SetNormal(s.Disabled);

		ExtendedPreview = SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Button.Normal", "Normal"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.ButtonStyle(&sNormal)
					.Text(PreviewText)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Button.Hovered", "Hovered"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.ButtonStyle(&sHovered)
					.Text(PreviewText)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Button.Pressed", "Pressed"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.ButtonStyle(&sPressed)
					.Text(PreviewText)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Button.Disabled", "Disabled"))
				]
				+SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Center)
				[
					SNew(SButton)
					.ButtonStyle(&sDisabled)
					.Text(PreviewText)
				]
			];
#endif
	};
};
