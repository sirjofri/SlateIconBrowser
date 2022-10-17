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
#include "Widgets/Layout/SSeparator.h"
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
	CacheAllLines();


	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				MakeMainMenu()
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				.Visibility_Lambda([&]
				{
					return GetConfig()->CopyCodeStyle == CS_CustomStyle ? EVisibility::Visible : EVisibility::Collapsed;
				})
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CustomStyleTextFieldLabel", "Custom Style:"))
					.Margin(FMargin(10, 5))
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("CustomStyleTextFieldHint", "$1 will be replaced by the icon name"))
					.OnTextCommitted(FOnTextCommitted::CreateLambda([&](const FText& Text, ETextCommit::Type Arg)
					{
						GetConfig()->CustomStyle = Text.ToString();
						GetConfig()->SaveConfig();
						CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
					}))
				]
			]
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
						GetConfig()->SelectedStyle = *InItem.Get();
						GetConfig()->SaveConfig();
						CacheAllLines();
					})
					.OptionsSource(&AllStyles)
					.OnGenerateWidget_Lambda([=](TSharedPtr<FName> InItem)
					{
						return SNew(STextBlock).Text(FText::FromName(*InItem.Get()));
					})
					[
						SNew(STextBlock).Text_Lambda([=]{ return FText::FromName(GetConfig()->SelectedStyle); })
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.0)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("SearchHintText", "Search"))
					.OnTextChanged(FOnTextChanged::CreateRaw(this, &FEditorIconBrowserModule::InputTextChanged))
					.Text_Lambda([&]
					{
						return FText::FromString(GetConfig()->FilterString);
					})
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
					SAssignNew(CopyNoteTextBlock, STextBlock)
					.Text_Lambda([&]
					{
						return FText::Format(LOCTEXT("CopyNote", "Double click a line to copy the {0} C++ code."),
							GetCodeStyleText(GetConfig()->CopyCodeStyle));
					})
				]
			]
		];
}

void FEditorIconBrowserModule::SelectCodeStyle(ECopyCodeStyle CopyStyle)
{
	GetConfig()->CopyCodeStyle = CopyStyle;
	GetConfig()->SaveConfig();
	CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
}

FText FEditorIconBrowserModule::GetCodeStyleText(ECopyCodeStyle CopyStyle)
{
	switch (CopyStyle) {
	case CS_FSlateIcon:
		return FText::FromString(TEXT("FSlateIcon(...)"));
		break;
	case CS_FSlateIconFinderFindIcon:
		return FText::FromString(TEXT("FSlateIconFinder::FindIcon(...)"));
		break;
	case CS_CustomStyle:
		return FText::FromString(GetConfig()->CustomStyle.IsEmpty() ? TEXT("(Custom)") : GetConfig()->CustomStyle);
		break;
	}
	return FText::GetEmpty();
}

FText FEditorIconBrowserModule::GetCodeStyleTooltip(ECopyCodeStyle CopyStyle)
{
	return FText::Format(LOCTEXT("CopyStyleTooltip", "Copy code in {0} style"), GetCodeStyleText(CopyStyle));
}

void FEditorIconBrowserModule::FillSettingsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("CopySettings"), LOCTEXT("CopySettings", "Code to copy"));
	{
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIcon),
			GetCodeStyleTooltip(CS_FSlateIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FEditorIconBrowserModule::SelectCodeStyle, CS_FSlateIcon),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return GetConfig()->CopyCodeStyle == CS_FSlateIcon;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIconFinderFindIcon),
			GetCodeStyleTooltip(CS_FSlateIconFinderFindIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FEditorIconBrowserModule::SelectCodeStyle, CS_FSlateIconFinderFindIcon),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return GetConfig()->CopyCodeStyle == CS_FSlateIconFinderFindIcon;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("CustomStyle", "Custom style..."),
			LOCTEXT("CustomStyleTooltip", "Specify a custom style for copying a code snippet"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FEditorIconBrowserModule::SelectCodeStyle, CS_CustomStyle),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return GetConfig()->CopyCodeStyle == CS_CustomStyle;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
	}
	MenuBuilder.EndSection();
}

TSharedRef<SWidget> FEditorIconBrowserModule::MakeMainMenu()
{
	FMenuBarBuilder MenuBuilder(nullptr);
	{
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("SettingsMenu", "Settings"),
			LOCTEXT("SettingsMenuTooltip", "Opens the settings menu"),
			FNewMenuDelegate::CreateRaw(this, &FEditorIconBrowserModule::FillSettingsMenu));
	}
	TSharedRef<SWidget> Widget = MenuBuilder.MakeWidget();
	Widget->SetVisibility(EVisibility::Visible);
	return Widget;
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
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(GetConfig()->SelectedStyle);
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
	InputTextChanged(FText::FromString(GetConfig()->FilterString));
}

UEditorIconBrowserUserSettings* FEditorIconBrowserModule::GetConfig()
{
	return GetMutableDefault<UEditorIconBrowserUserSettings>();
}

void FEditorIconBrowserModule::CopyIconCodeToClipboard(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText = GenerateCopyCode(Name, CodeStyle);
	FPlatformApplicationMisc::ClipboardCopy(*CopyText);
	UE_LOG(LogTemp, Warning, TEXT("Copy code to clipboard: %s"), *CopyText);

	FNotificationInfo Info(LOCTEXT("CopiedNotification", "Slate Icon code copied to clipboard"));
	Info.ExpireDuration = 3.f;
	Info.SubText = FText::FromString(CopyText);
	FSlateNotificationManager::Get().AddNotification(Info);
}

FString FEditorIconBrowserModule::GenerateCopyCode(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText(TEXT(""));
	switch (CodeStyle) {
	case CS_FSlateIcon:
		CopyText = FString::Printf(TEXT("FSlateIcon(%s, \"%s\")"), *TranslateDefaultStyleSets(GetConfig()->SelectedStyle), *Name.ToString());
		break;
	case CS_FSlateIconFinderFindIcon:
		CopyText = FString::Printf(TEXT("FSlateIconFinder::FindIcon(FName(\"%s\"))"), *Name.ToString());
		break;
	case CS_CustomStyle:
		CopyText = GetConfig()->CustomStyle.Replace(TEXT("$1"), *Name.ToString());
		break;
	}
	return CopyText;
}

FReply FEditorIconBrowserModule::EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name)
{
	if (PointerEvent.GetEffectingButton() != EKeys::RightMouseButton)
		return FReply::Unhandled();

	if (PointerEvent.GetEventPath() == nullptr)
		return FReply::Unhandled();

	FString CopyCode;
	auto Clipboard = [&](FName N, ECopyCodeStyle Style)
	{
		CopyIconCodeToClipboard(N, Style);
	};

	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.BeginSection("CopyOptions", LOCTEXT("CopyCodeOptions", "Copy Code"));
	{
		CopyCode = GenerateCopyCode(Name, CS_FSlateIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, Name, CS_FSlateIcon)));
		CopyCode = GenerateCopyCode(Name, CS_FSlateIconFinderFindIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, Name, CS_FSlateIconFinderFindIcon)));
		
		if (!GetConfig()->CustomStyle.IsEmpty()) {
			CopyCode = GenerateCopyCode(Name, CS_CustomStyle);
			MenuBuilder.AddMenuEntry(
				FText::FromString(CopyCode),
				FText::GetEmpty(),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda(Clipboard, Name, CS_CustomStyle)));
		}
	}
	MenuBuilder.EndSection();

	TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = *PointerEvent.GetEventPath();
	const FVector2D& Location = PointerEvent.GetScreenSpacePosition();
	FSlateApplication::Get().PushMenu(WidgetPath.Widgets.Last().Widget, WidgetPath, MenuWidget.ToSharedRef(), Location, FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
			
	return FReply::Handled();
}

TSharedRef<ITableRow> FEditorIconBrowserModule::GenerateRow(TSharedPtr<FName> Name,
                                                            const TSharedRef<STableViewBase>& TableViewBase)
{
	auto Brush = FSlateStyleRegistry::FindSlateStyle(GetConfig()->SelectedStyle)->GetOptionalBrush(*Name.Get());
	FVector2D DesiredIconSize = Brush->GetImageSize();
	if (Brush->GetImageType() == ESlateBrushImageType::NoImage)
		DesiredIconSize = FVector2D(20);
	
	return SNew(SComboRow<TSharedRef<FName>>, TableViewBase)
	[
		SNew(SBorder)
		.OnMouseDoubleClick_Lambda([&](const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName N)
		{
			CopyIconCodeToClipboard(N, GetConfig()->CopyCodeStyle);
			return FReply::Handled();
		}, *Name.Get())
		.OnMouseButtonUp_Raw(this, &FEditorIconBrowserModule::EntryContextMenu, *Name.Get())
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
	GetConfig()->FilterString = Text.ToString();
	GetConfig()->SaveConfig();
	Lines.Empty(AllLines.Num());
	
	if (GetConfig()->FilterString.IsEmpty()) {
		for (FString s : AllLines)
			Lines.Add(MakeShareable(new FName(s)));
		if (ListView.IsValid())
			ListView.Get()->RequestListRefresh();
		return;
	}
	for (FString s : AllLines) {
		if (s.Contains(GetConfig()->FilterString))
			Lines.Add(MakeShareable(new FName(s)));
		if (ListView.IsValid())
			ListView.Get()->RequestListRefresh();
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorIconBrowserModule, EditorIconBrowser)