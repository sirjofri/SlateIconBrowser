// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "EditorIconBrowser.h"
#include "EditorIconBrowserStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "LevelEditor.h"
#include "UMGStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/UMGCoreStyle.h"
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
	CacheAllStyleNames();
	FillDefaultStyleSetCodes();
	SelectedStyle = FEditorStyle::Get().GetStyleSetName();
	FilterText = TEXT("");
	CacheAllLines();

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(StyleSelectionComboBox, SComboBox<TSharedPtr<FName>>)
					.OnSelectionChanged_Lambda([&](TSharedPtr<FName> InItem, ESelectInfo::Type InSelectionInfo)
					{
						SelectedStyle = *InItem.Get();
						CacheAllLines();
					})
					.OptionsSource(&AllStyles)
					.OnGenerateWidget_Lambda([=](TSharedPtr<FName> InItem)
					{
						return SNew(STextBlock).Text(FText::FromName(*InItem.Get()));
					})
					[
						SNew(STextBlock).Text_Lambda([=]{ return FText::FromName(SelectedStyle); })
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.0)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("SearchHintText", "Search"))
					.OnTextChanged(FOnTextChanged::CreateRaw(this, &FEditorIconBrowserModule::InputTextChanged))
				]
			]
			+SVerticalBox::Slot()
			.FillHeight(1.0)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.FillHeight(1.0)
				[
					SAssignNew(ListView, SListView<TSharedPtr<FName>>)
					.OnGenerateRow_Raw(this, &FEditorIconBrowserModule::GenerateRow)
					.ListItemsSource(&Lines)
					.SelectionMode(ESelectionMode::Single)
					.Visibility_Lambda([=]
					{
						return Lines.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
				+SVerticalBox::Slot()
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SBox)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.Visibility_Lambda([=]
					{
						return Lines.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed;
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoResults", "No results found."))
					]
				]
			]
			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FAppStyle::Get().GetBrush("Brushes.Panel"))
				.Padding(FMargin(10, 5))
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CopyNote", "Double click a line to copy the FSlateIcon C++ code."))
				]
			]
		];
}

FString FEditorIconBrowserModule::TranslateDefaultStyleSets(FName StyleSet)
{
	FString* StrPtr = DefaultStyleSetCode.Find(StyleSet);
	if (StrPtr)
		return *StrPtr;
	return FString::Printf(TEXT("FName(\"%s\")"), *StyleSet.ToString());
}

void FEditorIconBrowserModule::FillDefaultStyleSetCodes()
{
#define STYLECODE(A, B) DefaultStyleSetCode.Add(FName(A), TEXT(B));
	
	STYLECODE("EditorStyle", "FEditorStyle::GetStyleSetName()");
	STYLECODE("CoreStyle", "FAppStyle::GetAppStyleSetName()");
	STYLECODE("UMGCoreStyle", "FUMGCoreStyle::Get().GetStyleSetName()");

	;
#undef STYLECODE
}

void FEditorIconBrowserModule::CacheAllStyleNames()
{
	AllStyles.Empty(AllStyles.Num());
	
	FSlateStyleRegistry::IterateAllStyles(
		[&](const ISlateStyle& Style)
		{
			AllStyles.Add(MakeShareable(new FName(Style.GetStyleSetName())));
			return true;
		}
	);
}

void FEditorIconBrowserModule::CacheAllLines()
{
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(SelectedStyle);
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
	InputTextChanged(FText::FromString(FilterText));
}

void FEditorIconBrowserModule::CopyIconCodeToClipboard(FName Name)
{
	FString CopyText = FString::Printf(TEXT("FSlateIcon(%s, \"%s\")"), *TranslateDefaultStyleSets(SelectedStyle), *Name.ToString());
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
	auto Brush = FSlateStyleRegistry::FindSlateStyle(SelectedStyle)->GetOptionalBrush(*Name.Get());
	FVector2D DesiredIconSize = Brush->GetImageSize();
	if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
		DesiredIconSize = FVector2D(20);
	
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
				.DesiredSizeOverride(DesiredIconSize)
				.Image(Brush)
			]
		]
	];
}

void FEditorIconBrowserModule::InputTextChanged(const FText& Text)
{
	FilterText = Text.ToString();
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