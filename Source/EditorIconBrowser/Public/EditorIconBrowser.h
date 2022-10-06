// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
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
	void CopyIconCodeToClipboard(FName Name);
	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FName> Name, const TSharedRef<STableViewBase>& TableViewBase);
	void InputTextChanged(const FText& Text);
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	void CacheAllLines();

	TArray<TSharedPtr<FName>> Lines;
	TArray<FString> AllLines;

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<SListView<TSharedPtr<FName>>> ListView;
};
