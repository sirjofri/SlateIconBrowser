#pragma once
#include "SlateIconBrowserUserSettings.h"

class ISlateStyleData;
class USlateIconBrowserUserSettings;

class SSlateStyleBrowserEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateStyleBrowserEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedRef<SWidget> MakeMainMenu();
	void FillSettingsMenu(FMenuBuilder& MenuBuilder);
	void FillHelpMenu(FMenuBuilder& MenuBuilder);
	
	FText GetCodeStyleTooltip(ECopyCodeStyle CopyCodeStyle);

	TSharedRef<ITableRow> GenerateRow(TSharedPtr<ISlateStyleData> SlateStyleData, const TSharedRef<STableViewBase>& TableViewBase);
	FReply EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, TSharedPtr<ISlateStyleData> SlateStyleData);
	
	void CacheAllStyleNames();
	void FillDefaultStyleSetCodes();
	void SelectCodeStyle(ECopyCodeStyle CopyCodeStyle);
	void InputTextChanged(const FText& Text);
	
	FText GetCodeStyleText(ECopyCodeStyle CopyStyle);
	
	TSharedPtr<ISlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName Style, FName PropertyName);

	USlateIconBrowserUserSettings* GetConfig();
	void MakeValidConfiguration();
	FString TranslateDefaultStyleSets(FName StyleSet);
	FString GenerateCopyCode(TSharedPtr<ISlateStyleData> SlateStyleData, ECopyCodeStyle CodeStyle);
	void CopyIconCodeToClipboard(TSharedPtr<ISlateStyleData> SlateStyleData, ECopyCodeStyle CodeStyle);

private:
	TArray<TSharedPtr<ISlateStyleData>> Lines;
	TArray<TSharedPtr<FName>> AllStyles;

private:
	TSharedPtr<SListView<TSharedPtr<ISlateStyleData>>> ListView;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> StyleSelectionComboBox;
	TSharedPtr<STextBlock> CopyNoteTextBlock;

	TMap<FName,FString> DefaultStyleSetCode;

	FName Name_AllStyles = FName("(All Styles)");
	FText CustomStyleTooltipText = NSLOCTEXT("SlateStyleBrowser", "CustomStyleTooltipText", "Enter custom style. $1 will be replaced with the icon name, $2 with the style name.");
};
