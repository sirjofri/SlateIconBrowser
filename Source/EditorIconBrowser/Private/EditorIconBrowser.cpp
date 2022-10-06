// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "EditorIconBrowser.h"
#include "EditorIconBrowserStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName EditorIconBrowserTabName("EditorIconBrowser");

#define LOCTEXT_NAMESPACE "FEditorIconBrowserModule"

void FEditorIconBrowserModule::StartupModule()
{
	FEditorIconBrowserStyle::Initialize();
	FEditorIconBrowserStyle::ReloadTextures();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EditorIconBrowserTabName, FOnSpawnTab::CreateRaw(this, &FEditorIconBrowserModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FEditorIconBrowserTabTitle", "Editor Icon Browser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FEditorIconBrowserStyle::GetStyleSetName(), "EditorIconBrowser.OpenPluginWindow"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);
}

void FEditorIconBrowserModule::ShutdownModule()
{
	FEditorIconBrowserStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EditorIconBrowserTabName);
}

TSharedRef<SDockTab> FEditorIconBrowserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	CacheAllLines();

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("SearchHintText", "Search"))
				.OnTextChanged(FOnTextChanged::CreateRaw(this, &FEditorIconBrowserModule::InputTextChanged))
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0)
			[
				SAssignNew(ListView, SListView<TSharedPtr<FName>>)
				.OnGenerateRow_Raw(this, &FEditorIconBrowserModule::GenerateRow)
				.ListItemsSource(&Lines)
				.SelectionMode(ESelectionMode::Single)
			]
			+SVerticalBox::Slot()
			.Padding(FMargin(10, 5))
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CopyNote", "Double click a line to copy the FSlateIcon C++ code."))
			]
		];
}

void FEditorIconBrowserModule::CacheAllLines()
{
	const ISlateStyle* Style = &FEditorStyle::Get();
	check(Style);
	TSet<FName> keys = Style->GetStyleKeys();

	AllLines.Empty(keys.Num());
	AllLines.Reserve(keys.Num());

	for (FName key : keys) {
		const FSlateBrush* brush = Style->GetOptionalBrush(key);
		if (!brush || brush == FStyleDefaults::GetNoBrush())
			continue;
		AllLines.Add(key.ToString());
	}
	InputTextChanged(FText::GetEmpty());
}

void FEditorIconBrowserModule::CopyIconCodeToClipboard(FName Name)
{
	FString CopyText = FString::Printf(TEXT("FSlateIcon(FEditorStyle::GetStyleSetName(), \"%s\")"), *Name.ToString());
	FPlatformApplicationMisc::ClipboardCopy(*CopyText);
	UE_LOG(LogTemp, Warning, TEXT("Copy code to clipboard: %s"), *CopyText);

	FNotificationInfo Info(LOCTEXT("CopiedNotification", "Slate Icon code copied to clipboard"));
	Info.ExpireDuration = 3.f;
	Info.SubText = FText::FromString(CopyText);
	FSlateNotificationManager::Get().AddNotification(Info);
}

TSharedRef<ITableRow> FEditorIconBrowserModule::GenerateRow(TSharedPtr<FName> Name,
                                                            const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(SComboRow<TSharedRef<FName>>, TableViewBase)
	[
		SNew(SBorder)
		.OnMouseDoubleClick_Lambda([&](const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName N)
		{
			CopyIconCodeToClipboard(N);
			return FReply::Handled();
		}, *Name.Get())
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(STextBlock)
				.Text(FText::FromName(*Name.Get()))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(SImage)
				.Image_Lambda([=]
				{
					return FEditorStyle::Get().GetOptionalBrush(*Name.Get());
				})
			]
		]
	];
}

void FEditorIconBrowserModule::InputTextChanged(const FText& Text)
{
	FString FilterText = Text.ToString();
	Lines.Empty(AllLines.Num());
	
	if (FilterText.IsEmpty()) {
		for (FString s : AllLines)
			Lines.Add(MakeShareable(new FName(s)));
		if (ListView.IsValid())
			ListView.Get()->RequestListRefresh();
		return;
	}
	for (FString s : AllLines) {
		if (s.Contains(FilterText))
			Lines.Add(MakeShareable(new FName(s)));
		if (ListView.IsValid())
			ListView.Get()->RequestListRefresh();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorIconBrowserModule, EditorIconBrowser)