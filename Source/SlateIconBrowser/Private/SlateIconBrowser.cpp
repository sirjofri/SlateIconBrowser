// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "SlateIconBrowser.h"

#include "DefaultWidgetTypes/DefaultSlateStyleDataProvider.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateIconBrowserStyle.h"
#include "Layout/WidgetPath.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#include "SlateStyleBrowserEditor.h"
#include "SlateStyleDataManager.h"

static const FName SlateIconBrowserTabName("SlateIconBrowser");

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

void FSlateIconBrowserModule::StartupModule()
{
	FSlateIconBrowserStyle::Initialize();
	FSlateIconBrowserStyle::ReloadTextures();

	SlateStyleDataManager = MakeShared<FSlateStyleDataManager>();
	TSharedPtr<FDefaultSlateStyleDataProvider> DefaultProvider = MakeShared<FDefaultSlateStyleDataProvider>();
	SlateStyleDataManager->RegisterWidgetStyleProvider(DefaultProvider);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateIconBrowserTabName, FOnSpawnTab::CreateRaw(this, &FSlateIconBrowserModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("SlateStyleBrowserTabTitle", "Slate Style Browser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FSlateIconBrowserStyle::GetStyleSetName(), "SlateStyleBrowser.Icon"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);
}

void FSlateIconBrowserModule::ShutdownModule()
{
	FSlateIconBrowserStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SlateIconBrowserTabName);
}

TSharedPtr<ISlateStyleDataManager> FSlateIconBrowserModule::GetSlateStyleDataManager()
{
	FSlateIconBrowserModule& mod = FModuleManager::Get().LoadModuleChecked<FSlateIconBrowserModule>("SlateIconBrowser");
	return mod.SlateStyleDataManager;
}

TSharedRef<SDockTab> FSlateIconBrowserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab).TabRole(ETabRole::MajorTab);
	DockTab->SetContent(SNew(SSlateStyleBrowserEditor, DockTab, SlateStyleDataManager));
	return DockTab;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSlateIconBrowserModule, SlateIconBrowser)
