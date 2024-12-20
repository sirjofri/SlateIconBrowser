﻿// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

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


#if ENGINE_MAJOR_VERSION == 4
#define EDITOR_STYLE_SAFE() FEditorStyle
#else
#define EDITOR_STYLE_SAFE() FAppStyle
#endif

struct FIconLine
{
	FName Style;
	FName Name;
	
	FIconLine() = default;
	FIconLine(FName InStyle, FName InName) : Style(InStyle), Name(InName) {};
};


class FSlateIconBrowserModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	void CopyIconCodeToClipboard(TSharedPtr<FIconLine> IconLine, ECopyCodeStyle CodeStyle);
	FString GenerateCopyCode(TSharedPtr<FIconLine> IconLine, ECopyCodeStyle CodeStyle);
	FReply EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, TSharedPtr<FIconLine> IconLine);
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FIconLine> IconLine, const TSharedRef<STableViewBase>& TableViewBase);
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

	TArray<TSharedPtr<FIconLine>> Lines;
	TArray<TTuple<FName,FString>> AllLines;

	TArray<TSharedPtr<FName>> AllStyles;

	USlateIconBrowserUserSettings* GetConfig();

private:
	TSharedPtr<SListView<TSharedPtr<FIconLine>>> ListView;
	TSharedPtr<SComboBox<TSharedPtr<FName>>> StyleSelectionComboBox;
	TSharedPtr<STextBlock> CopyNoteTextBlock;

	TMap<FName,FString> DefaultStyleSetCode;

	FName Name_AllStyles = FName("(All Styles)");
	FText CustomStyleTooltipText = NSLOCTEXT("FSlateIconBrowserModule", "CustomStyleTooltipText", "Enter custom style. $1 will be replaced with the icon name, $2 with the style name.");
};
