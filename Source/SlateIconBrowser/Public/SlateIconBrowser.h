// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "SlateIconBrowserUserSettings.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class ITableRow;
class STableViewBase;
class STextBlock;
template<typename T> class SListView;
template<typename T> class SComboBox;

class FSlateIconBrowserModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	void CopyIconCodeToClipboard(FName Name, ECopyCodeStyle CodeStyle);
	FString GenerateCopyCode(FName Name, ECopyCodeStyle CodeStyle);
	FReply EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name);
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FName> Name, const TSharedRef<STableViewBase>& TableViewBase);
	void InputTextChanged(const FText& Text);
	void MakeValidConfiguration();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	void SelectCodeStyle(ECopyCodeStyle CopyStyle);
	FText GetCodeStyleText(ECopyCodeStyle CopyStyle);
	FText GetCodeStyleTooltip(ECopyCodeStyle CopyStyle);
	void FillSettingsMenu(FMenuBuilder& MenuBuilder);
	void FillHelpMenu(FMenuBuilder& MenuBuilder);
	TSharedRef<SWidget> MakeMainMenu();

	FString TranslateDefaultStyleSets(FName StyleSet);
	void FillDefaultStyleSetCodes();

	void CacheAllStyleNames();
	void CacheAllLines();

	TArray<TSharedPtr<FName>> Lines;
	TArray<FString> AllLines;

	TArray<TSharedPtr<FName>> AllStyles;

	USlateIconBrowserUserSettings* GetConfig();

private:
	TSharedPtr<SListView<TSharedPtr<FName>>> ListView;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> StyleSelectionComboBox;
	TSharedPtr<STextBlock> CopyNoteTextBlock;

	TMap<FName,FString> DefaultStyleSetCode;

	FText CustomStyleTooltipText = NSLOCTEXT("FSlateIconBrowserModule", "CustomStyleTooltipText", "Enter custom style. $1 will be replaced by the icon name.");
};
