// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "SlateIconBrowser.h"

#include "EditorStyleSet.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "SlateIconBrowserStyle.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Layout/WidgetPath.h"
#include "LevelEditor.h"
#include "UMGStyle.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Text/STextBlock.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/SlateStyleRegistry.h"

#if ENGINE_MAJOR_VERSION == 5
#include "Styling/UMGCoreStyle.h"
#endif

#include "SlateIconBrowserHacker.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Notifications/SNotificationList.h"

static const FName SlateIconBrowserTabName("SlateIconBrowser");

#define LOCTEXT_NAMESPACE "FSlateIconBrowserModule"

void FSlateIconBrowserModule::StartupModule()
{
	FSlateIconBrowserStyle::Initialize();
	FSlateIconBrowserStyle::ReloadTextures();

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SlateIconBrowserTabName, FOnSpawnTab::CreateRaw(this, &FSlateIconBrowserModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FSlateIconBrowserTabTitle", "Slate Icon Browser"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FSlateIconBrowserStyle::GetStyleSetName(), "SlateIconBrowser.Icon"))
		.SetMenuType(ETabSpawnerMenuType::Enabled);
}

void FSlateIconBrowserModule::ShutdownModule()
{
	FSlateIconBrowserStyle::Shutdown();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SlateIconBrowserTabName);
}

TSharedRef<SDockTab> FSlateIconBrowserModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	MakeValidConfiguration();
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
				SNew(SExpandableArea)
				.InitiallyCollapsed(true)
				.HeaderContent()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("CustomStyleExpandHeader", "Custom Style"))
				]
				.BodyContent()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("CustomStyleTextFieldLabel", "Custom Code Style:"))
						.ToolTipText(CustomStyleTooltipText)
						.Margin(FMargin(10, 5))
					]
					+SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SNew(SEditableTextBox)
						.HintText(LOCTEXT("CustomStyleTextFieldHint", "$1 will be replaced by the icon name"))
						.ToolTipText(CustomStyleTooltipText)
						.Text_Lambda([&]
						{
							return FText::FromString(GetConfig()->CustomStyle);
						})
						.OnTextCommitted(FOnTextCommitted::CreateLambda([&](const FText& Text, ETextCommit::Type Arg)
						{
							GetConfig()->CustomStyle = Text.ToString();
							GetConfig()->SaveConfig();
							CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
						}))
					]
				]
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSeparator)
				.Orientation(EOrientation::Orient_Horizontal)
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
						SNew(STextBlock).Text_Lambda([this]{ return FText::FromName(GetConfig()->SelectedStyle); })
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.0)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("SearchHintText", "Search"))
					.OnTextChanged(FOnTextChanged::CreateRaw(this, &FSlateIconBrowserModule::InputTextChanged))
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
					.OnGenerateRow_Raw(this, &FSlateIconBrowserModule::GenerateRow)
					.ListItemsSource(&Lines)
					.SelectionMode(ESelectionMode::Single)
					.Visibility_Lambda([this]
					{
						return Lines.Num() == 0 ? EVisibility::Collapsed : EVisibility::Visible;
					})
				]
				+SVerticalBox::Slot()
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(SBox)
					.HAlign(EHorizontalAlignment::HAlign_Center)
					.Visibility_Lambda([this]
					{
						return Lines.Num() == 0 ? EVisibility::Visible : EVisibility::Collapsed;
					})
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NoResults", "No results found."))
					]
				]
			]
			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(0,4,0,0))
			.AutoHeight()
			[
				SNew(SBorder)
#if ENGINE_MAJOR_VERSION == 5
				.BorderImage(EDITOR_STYLE_SAFE()::Get().GetBrush("Brushes.Panel"))
#else
				.BorderImage(EDITOR_STYLE_SAFE()::Get().GetBrush("ToolPanel.GroupBorder"))
#endif
				.Padding(FMargin(10, 5))
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(CopyNoteTextBlock, STextBlock)
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
						.Text_Lambda([this]{ return GetCodeStyleText(GetConfig()->CopyCodeStyle); })
					]
					+SHorizontalBox::Slot()
					.Padding(FMargin(4, 0))
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text_Lambda([]{ return LOCTEXT("CopyNote", "Double click a line to copy"); })
					]
				]
			]
		];
}

void FSlateIconBrowserModule::SelectCodeStyle(ECopyCodeStyle CopyStyle)
{
	GetConfig()->CopyCodeStyle = CopyStyle;
	GetConfig()->SaveConfig();
	CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
}

FText FSlateIconBrowserModule::GetCodeStyleText(ECopyCodeStyle CopyStyle)
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

FText FSlateIconBrowserModule::GetCodeStyleTooltip(ECopyCodeStyle CopyStyle)
{
	return FText::Format(LOCTEXT("CopyStyleTooltip", "Copy code in {0} style"), GetCodeStyleText(CopyStyle));
}

void FSlateIconBrowserModule::FillSettingsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("CopySettings"), LOCTEXT("CopySettings", "Code to copy"));
	{
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIcon),
			GetCodeStyleTooltip(CS_FSlateIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &FSlateIconBrowserModule::SelectCodeStyle, CS_FSlateIcon),
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
				FExecuteAction::CreateRaw(this, &FSlateIconBrowserModule::SelectCodeStyle, CS_FSlateIconFinderFindIcon),
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
				FExecuteAction::CreateRaw(this, &FSlateIconBrowserModule::SelectCodeStyle, CS_CustomStyle),
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

void FSlateIconBrowserModule::FillHelpMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("HelpMenu"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("HelpDocumentation", "Documentation"),
			LOCTEXT("HelpDocumentationTooltip", "Opens the documentation"),
#if ENGINE_MAJOR_VERSION == 5
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Documentation"),
#else
			FSlateIcon(FEditorStyle::GetStyleSetName(), "Icons.Documentation"),
#endif
			
			FUIAction(
				FExecuteAction::CreateLambda([=]
				{
					FPlatformProcess::LaunchURL(TEXT("https://github.com/sirjofri/SlateIconBrowser"), nullptr, nullptr);
				})
			));
	}
	MenuBuilder.EndSection();
}

TSharedRef<SWidget> FSlateIconBrowserModule::MakeMainMenu()
{
	FMenuBarBuilder MenuBuilder(nullptr);
	{
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("SettingsMenu", "Settings"),
			LOCTEXT("SettingsMenuTooltip", "Opens the settings menu."),
			FNewMenuDelegate::CreateRaw(this, &FSlateIconBrowserModule::FillSettingsMenu));
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("HelpMenu", "Help"),
			LOCTEXT("HelpMenuTooltip", "Opens the help menu."),
			FNewMenuDelegate::CreateRaw(this, &FSlateIconBrowserModule::FillHelpMenu));
	}
	TSharedRef<SWidget> Widget = MenuBuilder.MakeWidget();
	Widget->SetVisibility(EVisibility::Visible);
	return Widget;
}

FString FSlateIconBrowserModule::TranslateDefaultStyleSets(FName StyleSet)
{
	FString* StrPtr = DefaultStyleSetCode.Find(StyleSet);
	if (StrPtr)
		return *StrPtr;
	return FString::Printf(TEXT("FName(\"%s\")"), *StyleSet.ToString());
}

void FSlateIconBrowserModule::FillDefaultStyleSetCodes()
{
#define STYLECODE(A, B) DefaultStyleSetCode.Add(FName(A), TEXT(B));

#if ENGINE_MAJOR_VERSION == 5
	STYLECODE("EditorStyle", "FAppStyle::GetAppStyleSetName()");
	STYLECODE("CoreStyle", "FAppStyle::GetAppStyleSetName()");
	STYLECODE("UMGCoreStyle", "FUMGCoreStyle::Get().GetStyleSetName()");
#else
	STYLECODE("EditorStyle", "FEditorStyle::GetStyleSetName()")
	STYLECODE("CoreStyle", "FCoreStyle::Get().GetStyleSetName()")
	STYLECODE("UMGStyle", "FUMGStyle::GetStyleSetName()")
#endif

#undef STYLECODE
}

void FSlateIconBrowserModule::CacheAllStyleNames()
{
	AllStyles.Empty(AllStyles.Num());
	
	FSlateStyleRegistry::IterateAllStyles(
		[&](const ISlateStyle& Style)
		{
			AllStyles.Add(MakeShareable(new FName(Style.GetStyleSetName())));
			return true;
		}
	);

	AllStyles.Sort([](const TSharedPtr<FName>& A, const TSharedPtr<FName>& B)
	{
		const FString AString = A->ToString();
		const FString BString = B->ToString();
		return AString.Compare(BString) < 0;
	});
}

void FSlateIconBrowserModule::CacheAllLines()
{
	const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(GetConfig()->SelectedStyle);
	const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(Style);
	if (!StyleSet) { ensureMsgf(false, TEXT("Unexpected nullptr")); }

#if ENGINE_MAJOR_VERSION == 5
	TSet<FName> keys = Style->GetStyleKeys();
#else
	TSet<FName> keys = HackerMisc::GetStyleKeys(StyleSet);
#endif
	
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

USlateIconBrowserUserSettings* FSlateIconBrowserModule::GetConfig()
{
	return GetMutableDefault<USlateIconBrowserUserSettings>();
}

void FSlateIconBrowserModule::CopyIconCodeToClipboard(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText = GenerateCopyCode(Name, CodeStyle);
	FPlatformApplicationMisc::ClipboardCopy(*CopyText);
	UE_LOG(LogTemp, Warning, TEXT("Copy code to clipboard: %s"), *CopyText);

	FNotificationInfo Info(LOCTEXT("CopiedNotification", "C++ code copied to clipboard"));
	Info.ExpireDuration = 3.f;
#if ENGINE_MAJOR_VERSION == 5
	Info.SubText = FText::FromString(CopyText);
#else
	Info.Text = FText::FromString(CopyText);
#endif
	FSlateNotificationManager::Get().AddNotification(Info);
}

FString FSlateIconBrowserModule::GenerateCopyCode(FName Name, ECopyCodeStyle CodeStyle)
{
	FString CopyText(TEXT(""));
	switch (CodeStyle) {
	case CS_FSlateIcon:
		CopyText = FString::Printf(TEXT("FSlateIcon(%s, \"%s\")"), *TranslateDefaultStyleSets(GetConfig()->SelectedStyle), *Name.ToString());
		break;
	case CS_FSlateIconFinderFindIcon:
		CopyText = FString::Printf(TEXT("FSlateIconFinder::FindIcon(\"%s\")"), *Name.ToString());
		break;
	case CS_CustomStyle:
		CopyText = GetConfig()->CustomStyle.Replace(TEXT("$1"), *Name.ToString());
		break;
	}
	return CopyText;
}

FReply FSlateIconBrowserModule::EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent, FName Name)
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

TSharedRef<ITableRow> FSlateIconBrowserModule::GenerateRow(TSharedPtr<FName> Name,
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
		.OnMouseButtonUp_Raw(this, &FSlateIconBrowserModule::EntryContextMenu, *Name.Get())
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
#if ENGINE_MAJOR_VERSION == 5
				.DesiredSizeOverride(DesiredIconSize)
#endif
				.Image(Brush)
			]
		]
	];
}

void FSlateIconBrowserModule::InputTextChanged(const FText& Text)
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

void FSlateIconBrowserModule::MakeValidConfiguration()
{
	if (GetConfig()->SelectedStyle.IsNone())
#if ENGINE_MAJOR_VERSION == 5
		GetConfig()->SelectedStyle = FAppStyle::GetAppStyleSetName();
#else
		GetConfig()->SelectedStyle = FEditorStyle::GetStyleSetName();
#endif
}

#undef LOCTEXT_NAMESPACE

#if ENGINE_MAJOR_VERSION == 5
IMPLEMENT_MODULE(FSlateIconBrowserModule, EditorIconBrowser)
#else
IMPLEMENT_MODULE(FSlateIconBrowserModule, SlateIconBrowser)
#endif
