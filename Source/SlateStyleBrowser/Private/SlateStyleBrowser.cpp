// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "SlateStyleBrowser.h"

#include "DefaultWidgetTypes/DefaultSlateStyleDataProvider.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateStyleBrowserStyle.h"
#include "Layout/WidgetPath.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "SlateStyleBrowserEditor.h"
#include "SlateStyleDataManager.h"

static const FName SlateStyleBrowserTabName("SlateStyleBrowser");

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

void FSlateStyleBrowserModule::StartupModule()
{
	FSlateStyleBrowserStyle::Initialize();
	FSlateStyleBrowserStyle::ReloadTextures();

	SlateStyleDataManager = MakeShared<FSlateStyleDataManager>();
	TSharedPtr<FDefaultSlateStyleDataProvider> DefaultProvider = MakeShared<FDefaultSlateStyleDataProvider>();
	SlateStyleDataManager->RegisterWidgetStyleProvider(DefaultProvider);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateStyleBrowserTabName, FOnSpawnTab::CreateRaw(this, &FSlateStyleBrowserModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("SlateStyleBrowserTabTitle", "Slate Style Browser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FSlateStyleBrowserStyle::GetStyleSetName(), "SlateStyleBrowser.Icon"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);
}

void FSlateStyleBrowserModule::ShutdownModule()
{
	FSlateStyleBrowserStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SlateStyleBrowserTabName);
}

TSharedPtr<ISlateStyleDataManager> FSlateStyleBrowserModule::GetSlateStyleDataManager()
{
	FSlateStyleBrowserModule& mod = FModuleManager::Get().LoadModuleChecked<FSlateStyleBrowserModule>("SlateStyleBrowser");
	return mod.SlateStyleDataManager;
}

TSharedRef<SDockTab> FSlateStyleBrowserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab).TabRole(ETabRole::MajorTab);
	DockTab->SetContent(SNew(SSlateStyleBrowserEditor, DockTab, SlateStyleDataManager));
	return DockTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlateStyleBrowserModule, SlateStyleBrowser)
