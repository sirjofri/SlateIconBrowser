#include "SlateStyleBrowserEditor.h"

#include "SlateIconBrowserUserSettings.h"
#include "SlateStyleBrush.h"
#include "SlateStyleData.h"
#include "SlateStyleWidgetTextBlock.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/TypeFilterWidget.h"
#include "Widgets/Notifications/SNotificationList.h"

#if ENGINE_MAJOR_VERSION == 4
#define EDITOR_STYLE_SAFE() FEditorStyle
#else
#define EDITOR_STYLE_SAFE() FAppStyle
#endif

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

void SSlateStyleBrowserEditor::Construct(const FArguments& InArgs)
{
	MakeValidConfiguration();
	CacheAllStyleNames();
	FillDefaultStyleSetCodes();
	InputTextChanged(FText::GetEmpty());
	
	ChildSlot
	[
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
					UpdateList();
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
			.AutoWidth()
			[
				SAssignNew(TypeFilterWidget, STypeFilterWidget)
				.SelectPrompt(LOCTEXT("SelectTypeFilterPrompt", "Select Types"))
				.OnSelectionChanged_Lambda([&](const TArray<FName>& SelectedOptions)
				{
					UE_LOG(LogTemp, Display, TEXT("Selected Options: %d"), SelectedOptions.Num());
					for (auto n : SelectedOptions) {
						UE_LOG(LogTemp, Display, TEXT("Selected: %s"), *n.ToString());
					}
					FilterTypes = SelectedOptions;
					UpdateList();
				})
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("SearchHintText", "Search"))
				.OnTextChanged(FOnTextChanged::CreateRaw(this, &SSlateStyleBrowserEditor::InputTextChanged))
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
				SAssignNew(ListView, SListView<TSharedPtr<ISlateStyleData>>)
				.OnGenerateRow_Raw(this, &SSlateStyleBrowserEditor::GenerateRow)
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
				.FillWidth(1.0)
				[
					SNew(STextBlock)
					.Text_Lambda([]{ return LOCTEXT("CopyNote", "Double click a line to copy"); })
				]
				+SHorizontalBox::Slot()
				.Padding(FMargin(4, 0))
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text_Lambda([&]{ return FText::AsNumber(Lines.Num()); })
				]
			]
		]
	];

	// TODO: Supported types. Optimize/Refactor
	TArray<FName> Options;
	Options.Add(FSlateStyleBrush::TypeName);
	Options.Add(FSlateStyleWidgetTextBlock::TypeName);
	
	if (TypeFilterWidget.IsValid()) {
		TypeFilterWidget->SetOptions(Options);
		TypeFilterWidget->SelectAll();
	}
}

TSharedRef<SWidget> SSlateStyleBrowserEditor::MakeMainMenu()
{
	FMenuBarBuilder MenuBuilder(nullptr);
	{
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("SettingsMenu", "Settings"),
			LOCTEXT("SettingsMenuTooltip", "Opens the settings menu."),
			FNewMenuDelegate::CreateRaw(this, &SSlateStyleBrowserEditor::FillSettingsMenu));
		MenuBuilder.AddPullDownMenu(
			LOCTEXT("HelpMenu", "Help"),
			LOCTEXT("HelpMenuTooltip", "Opens the help menu."),
			FNewMenuDelegate::CreateRaw(this, &SSlateStyleBrowserEditor::FillHelpMenu));
	}
	TSharedRef<SWidget> Widget = MenuBuilder.MakeWidget();
	Widget->SetVisibility(EVisibility::Visible);
	return Widget;
}

void SSlateStyleBrowserEditor::FillSettingsMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("CopySettings"), LOCTEXT("CopySettings", "Code to copy"));
	{
		MenuBuilder.AddMenuEntry(
			GetCodeStyleText(CS_FSlateIcon),
			GetCodeStyleTooltip(CS_FSlateIcon),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &SSlateStyleBrowserEditor::SelectCodeStyle, CS_FSlateIcon),
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
				FExecuteAction::CreateRaw(this, &SSlateStyleBrowserEditor::SelectCodeStyle, CS_FSlateIconFinderFindIcon),
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
				FExecuteAction::CreateRaw(this, &SSlateStyleBrowserEditor::SelectCodeStyle, CS_CustomStyle),
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

void SSlateStyleBrowserEditor::FillHelpMenu(FMenuBuilder& MenuBuilder)
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

FText SSlateStyleBrowserEditor::GetCodeStyleTooltip(ECopyCodeStyle CopyCodeStyle)
{
	return FText::Format(LOCTEXT("CopyStyleTooltip", "Copy code in {0} style"), GetCodeStyleText(CopyCodeStyle));
}

TSharedRef<ITableRow> SSlateStyleBrowserEditor::GenerateRow(TSharedPtr<ISlateStyleData> SlateStyleData,
                                                            const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(SComboRow<TSharedRef<ISlateStyleData>>, TableViewBase)
	[
		SNew(SBorder)
		.OnMouseDoubleClick_Lambda([&](const FGeometry& Geometry, const FPointerEvent& PointerEvent, TSharedPtr<ISlateStyleData> data)
		{
			CopyIconCodeToClipboard(data, GetConfig()->CopyCodeStyle);
			return FReply::Handled();
		}, SlateStyleData)
		.OnMouseButtonUp_Raw(this, &SSlateStyleBrowserEditor::EntryContextMenu, SlateStyleData)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(STextBlock)
				.Text(FText::FromName(SlateStyleData->GetStyleName()))
				.TextStyle(&EDITOR_STYLE_SAFE()::Get().GetWidgetStyle<FTextBlockStyle>(TEXT("NormalText.Subdued")))
			]
			+SHorizontalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(STextBlock)
				.Text(FText::FromName(SlateStyleData->GetPropertyName()))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1)
			.VAlign(EVerticalAlignment::VAlign_Center)
			.Padding(FMargin(10, 5))
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.FillWidth(1.)
				[
					SNew(SSpacer)
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SlateStyleData->GenerateRowWidget()
				]
			]
		]
	];
}

FReply SSlateStyleBrowserEditor::EntryContextMenu(const FGeometry& Geometry, const FPointerEvent& PointerEvent,
	TSharedPtr<ISlateStyleData> SlateStyleData)
{
	if (PointerEvent.GetEffectingButton() != EKeys::RightMouseButton)
		return FReply::Unhandled();

	if (PointerEvent.GetEventPath() == nullptr)
		return FReply::Unhandled();

	FString CopyCode;
	auto Clipboard = [&](TSharedPtr<ISlateStyleData> N, ECopyCodeStyle Style)
	{
		CopyIconCodeToClipboard(N, Style);
	};

	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.BeginSection("CopyOptions", LOCTEXT("CopyCodeOptions", "Copy Code"));
	{
		CopyCode = GenerateCopyCode(SlateStyleData, CS_FSlateIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, SlateStyleData, CS_FSlateIcon)));
		CopyCode = GenerateCopyCode(SlateStyleData, CS_FSlateIconFinderFindIcon);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			FText::GetEmpty(),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, SlateStyleData, CS_FSlateIconFinderFindIcon)));
		
		if (!GetConfig()->CustomStyle.IsEmpty()) {
			CopyCode = GenerateCopyCode(SlateStyleData, CS_CustomStyle);
			MenuBuilder.AddMenuEntry(
				FText::FromString(CopyCode),
				FText::GetEmpty(),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda(Clipboard, SlateStyleData, CS_CustomStyle)));
		}
	}
	MenuBuilder.EndSection();

	TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	FWidgetPath WidgetPath = *PointerEvent.GetEventPath();
	const FVector2D& Location = PointerEvent.GetScreenSpacePosition();
	FSlateApplication::Get().PushMenu(WidgetPath.Widgets.Last().Widget, WidgetPath, MenuWidget.ToSharedRef(), Location, FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
			
	return FReply::Handled();
}

void SSlateStyleBrowserEditor::CacheAllStyleNames()
{
	AllStyles.Empty(AllStyles.Num());

	AllStyles.Add(MakeShareable(new FName(Name_AllStyles)));
	
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

void SSlateStyleBrowserEditor::FillDefaultStyleSetCodes()
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

void SSlateStyleBrowserEditor::SelectCodeStyle(ECopyCodeStyle CopyCodeStyle)
{
	GetConfig()->CopyCodeStyle = CopyCodeStyle;
	GetConfig()->SaveConfig();
	CopyNoteTextBlock->Invalidate(EInvalidateWidgetReason::Paint);
}

void SSlateStyleBrowserEditor::UpdateList()
{
	auto LoadFilteredForStyle = [&](FName& StyleName)
	{
		const ISlateStyle* Style = FSlateStyleRegistry::FindSlateStyle(StyleName);
		if (!Style) {
			ensureMsgf(false, TEXT("Unexpected nullptr"));
			return;
		}

		TSet<FName> keys;
#if ENGINE_MAJOR_VERSION == 5
		keys = Style->GetStyleKeys();
#else
		{
			const FSlateStyleSet* StyleSet = static_cast<const FSlateStyleSet*>(Style);
			if (!StyleSet) {
				ensureMsgf(false, TEXT("Unexpected nullptr"));
				return;
			}
			keys = HackerMisc::GetStyleKeys(StyleSet);
		}
#endif

		Lines.Reserve(Lines.Num() + keys.Num());

		for (FName key : keys) {
			if (GetConfig()->FilterString.IsEmpty() || key.ToString().Contains(GetConfig()->FilterString)) {
				TSharedPtr<ISlateStyleData> d = MakeSlateStyleData(Style, StyleName, key);
				if (!d)
					continue;
				if (!FilterTypes.Contains(d->GetType()))
					continue;
				Lines.Add(d);
			}
		}
	};

	Lines.Empty(Lines.Num());

	if (GetConfig()->SelectedStyle == Name_AllStyles) {
		for (TSharedPtr<FName> key : AllStyles) {
			if (*key == Name_AllStyles)
				continue;
			LoadFilteredForStyle(*key);
		}
	} else {
		LoadFilteredForStyle(GetConfig()->SelectedStyle);
	}
	if (ListView.IsValid())
		ListView->RequestListRefresh();
}

void SSlateStyleBrowserEditor::InputTextChanged(const FText& Text)
{
	GetConfig()->FilterString = Text.ToString();
	GetConfig()->SaveConfig();
	UpdateList();
}

FText SSlateStyleBrowserEditor::GetCodeStyleText(ECopyCodeStyle CopyStyle)
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

TSharedPtr<ISlateStyleData> SSlateStyleBrowserEditor::MakeSlateStyleData(const ISlateStyle* SlateStyle, FName Style,
                                                                         FName PropertyName)
{
	TSharedPtr<ISlateStyleData> StyleData;

	/*
	 * Important: Get brushes last, so that DefaultBrushes don't interfere!
	 */
	
#define WIDGET(StyleType, ItemType) \
	if (SlateStyle->HasWidgetStyle<StyleType>(PropertyName)) { \
		auto ws = SlateStyle->GetWidgetStyle<StyleType>(PropertyName); \
		StyleData = MakeShared<ItemType>(); \
		StyleData->Initialize(Style, PropertyName, ItemType::TypeName); \
		return StyleData; \
	}

	WIDGET(FTextBlockStyle, FSlateStyleWidgetTextBlock);

#undef WIDGET
	
	const FSlateBrush* Brush = SlateStyle->GetBrush(PropertyName);
	// This is not ideal! Default brushes won't appear at all this way.
	if (Brush && Brush != SlateStyle->GetDefaultBrush()) {
		StyleData = MakeShared<FSlateStyleBrush>();
		StyleData->Initialize(Style, PropertyName, FSlateStyleBrush::TypeName);
		return StyleData;
	}
	
	return nullptr;
}

USlateIconBrowserUserSettings* SSlateStyleBrowserEditor::GetConfig()
{
	return GetMutableDefault<USlateIconBrowserUserSettings>();
}

void SSlateStyleBrowserEditor::MakeValidConfiguration()
{
	if (GetConfig()->SelectedStyle.IsNone()) {
#if ENGINE_MAJOR_VERSION == 5
		GetConfig()->SelectedStyle = FAppStyle::GetAppStyleSetName();
#else
		GetConfig()->SelectedStyle = FEditorStyle::GetStyleSetName();
#endif
	}
}

FString SSlateStyleBrowserEditor::TranslateDefaultStyleSets(FName StyleSet)
{
	FString* StrPtr = DefaultStyleSetCode.Find(StyleSet);
	if (StrPtr)
		return *StrPtr;
	return FString::Printf(TEXT("FName(\"%s\")"), *StyleSet.ToString());
}

FString SSlateStyleBrowserEditor::GenerateCopyCode(TSharedPtr<ISlateStyleData> SlateStyleData, ECopyCodeStyle CodeStyle)
{
	FString CopyText(TEXT(""));
	switch (CodeStyle) {
	case CS_FSlateIcon:
		CopyText = FString::Printf(TEXT("FSlateIcon(%s, \"%s\")"), *TranslateDefaultStyleSets(SlateStyleData->GetStyleName()), *SlateStyleData->GetPropertyName().ToString());
		break;
	case CS_FSlateIconFinderFindIcon:
		CopyText = FString::Printf(TEXT("FSlateIconFinder::FindIcon(\"%s\")"), *SlateStyleData->GetPropertyName().ToString());
		break;
	case CS_CustomStyle:
		CopyText = GetConfig()->CustomStyle
			.Replace(TEXT("$1"), *SlateStyleData->GetPropertyName().ToString())
			.Replace(TEXT("$2"), *SlateStyleData->GetStyleName().ToString());
		break;
	}
	return CopyText;
}

void SSlateStyleBrowserEditor::CopyIconCodeToClipboard(TSharedPtr<ISlateStyleData> SlateStyleData, ECopyCodeStyle CodeStyle)
{
	FString CopyText = GenerateCopyCode(SlateStyleData, CodeStyle);
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

