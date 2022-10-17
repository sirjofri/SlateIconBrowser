// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "EditorIconBrowserUserSettings.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FEditorIconBrowserModule : public IModuleInterface
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
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	void SelectCodeStyle(ECopyCodeStyle CopyStyle);
	FText GetCodeStyleText(ECopyCodeStyle CopyStyle);
	FText GetCodeStyleTooltip(ECopyCodeStyle CopyStyle);
	void FillSettingsMenu(FMenuBuilder& MenuBuilder);
	TSharedRef<SWidget> MakeMainMenu();

	FString TranslateDefaultStyleSets(FName StyleSet);
	void FillDefaultStyleSetCodes();

	void CacheAllStyleNames();
	void CacheAllLines();

	TArray<TSharedPtr<FName>> Lines;
	TArray<FString> AllLines;

	TArray<TSharedPtr<FName>> AllStyles;

	UEditorIconBrowserUserSettings* GetConfig();

private:
	TSharedPtr<SListView<TSharedPtr<FName>>> ListView;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> StyleSelectionComboBox;
	TSharedPtr<STextBlock> CopyNoteTextBlock;

	TMap<FName,FString> DefaultStyleSetCode;
};
