#include "SlateStyleBrowserEditor.h"

#include "SlateStyleBrowserUserSettings.h"
#include "DefaultWidgetTypes/SlateStyleBrush.h"
#include "SlateStyleData.h"
#include "SlateStyleDataManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/SlateStyleTableRow.h"
#include "Widgets/TypeFilterWidget.h"

#if ENGINE_MAJOR_VERSION == 4
#define EDITOR_STYLE_SAFE() FEditorStyle
#else
#define EDITOR_STYLE_SAFE() FAppStyle
#endif

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

void SSlateStyleBrowserEditor::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& MajorTab, TWeakPtr<ISlateStyleDataManager> InSlateStyleDataManager)
{
	SlateStyleDataManager = InSlateStyleDataManager;
	
	MakeValidConfiguration();
	CacheAllStyleNames();
	InputTextChanged(FText::GetEmpty());

	DockTab = MajorTab;
	
	TabManager = FGlobalTabmanager::Get()->NewTabManager(MajorTab);
	TabManager->SetAllowWindowMenuBar(true);
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	BuildTabMenu(MenuBarBuilder);
	TabManager->SetMenuMultiBox(MenuBarBuilder.GetMultiBox(), MenuBarBuilder.MakeWidget());

	ChildSlot
	[
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("QuickStyleTextFieldLabel", "Quick Code Style:"))
				.ToolTipText(QuickStyleTooltipText)
				.Margin(FMargin(10, 5))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.f)
			[
				SNew(SEditableTextBox)
				.HintText(LOCTEXT("QuickStyleTextFieldHint", "$1 = property name, $2 = style name, escape $ with $$"))
				.ToolTipText(QuickStyleTooltipText)
				.Text_Lambda([&]
				{
					return FText::FromString(GetConfig()->QuickStyle);
				})
				.OnTextCommitted(FOnTextCommitted::CreateLambda([&](const FText& Text, ETextCommit::Type Arg)
				{
					GetConfig()->QuickStyle = Text.ToString();
					GetConfig()->SaveConfig();
				}))
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
				.ToolTipText(LOCTEXT("SelectTypeFilterToolTip", "Use Shift+Click to select exclusively"))
				.OnSelectionChanged_Lambda([&](const TArray<FName>& SelectedOptions)
				{
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
					return FText::FromString(FilterString);
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
				SAssignNew(ListView, SListView<TSharedPtr<FSlateStyleData>>)
				.SelectionMode(ESelectionMode::Single)
				.OnGenerateRow_Raw(this, &SSlateStyleBrowserEditor::GenerateRow)
				.OnContextMenuOpening_Raw(this, &SSlateStyleBrowserEditor::EntryContextMenu)
				.OnMouseButtonDoubleClick_Lambda([&](TSharedPtr<FSlateStyleData> data)
				{
					data->CopyDefault(DefaultCopyStyle, GetConfig()->QuickStyle);
				})
				.ListItemsSource(&Lines)
				.SelectionMode(ESelectionMode::Single)
				.HeaderRow(SNew(SHeaderRow)
					+SHeaderRow::Column("StyleSetName")
					.HAlignCell(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HeaderRowStyleSetName", "Style Set"))
					]
					+SHeaderRow::Column("PropertyName")
					.HAlignCell(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HeaderRowPropertyName", "Property Name"))
					]
					+SHeaderRow::Column("WidgetType")
					.HAlignCell(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HeaderRowWidgetType", "Widget Type"))
					]
					+SHeaderRow::Column("Widget")
					.HAlignCell(HAlign_Fill)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("HeaderRowWidget", "Visualization"))
					]
				)
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
				.Padding(FMargin(4, 0))
				.FillWidth(1.0)
				[
					SNew(STextBlock)
					.Text_Lambda([]{ return LOCTEXT("CopyNote", "Double click or right click a line to copy"); })
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

	TArray<FName> TypeOptions;
	TypeOptions.Add(TEXT("Brush"));
	TSharedPtr<ISlateStyleDataManager> DataMgr = SlateStyleDataManager.Pin();
	if (DataMgr.IsValid()) {
		TArray<FName> WidgetTypes;
		DataMgr->GetRegisteredTypes(WidgetTypes);
		TypeOptions.Append(WidgetTypes);
	}
	
	if (TypeFilterWidget.IsValid()) {
		TypeFilterWidget->SetOptions(TypeOptions);
		TypeFilterWidget->SelectAll();
	}
}

void SSlateStyleBrowserEditor::FillEditMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(TEXT("DefaultCopySettings"), LOCTEXT("DefaultCopySettingsSection", "Double click"));
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DefaultCopyFirstEntry", "First Entry"),
			LOCTEXT("DefaultCopyFirstEntryToolTip", "Copy using the first code style entry"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &SSlateStyleBrowserEditor::SelectCodeStyle, DCS_FirstEntry),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return DefaultCopyStyle == DCS_FirstEntry;
				})
			),
			NAME_None,
			EUserInterfaceActionType::RadioButton);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DefaultCopyQuickStyle", "Quick Style"),
			LOCTEXT("DefaultCopyQuickStyleToolTip", "Copy using the custom quick style"),
			FSlateIcon(),
			FUIAction(
				FExecuteAction::CreateRaw(this, &SSlateStyleBrowserEditor::SelectCodeStyle, DCS_QuickStyle),
				FCanExecuteAction(),
				FIsActionChecked::CreateLambda([&]
				{
					return DefaultCopyStyle == DCS_QuickStyle;
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
					FPlatformProcess::LaunchURL(TEXT("https://github.com/sirjofri/SlateStyleBrowser"), nullptr, nullptr);
				})
			));
	}
	MenuBuilder.EndSection();
}

void SSlateStyleBrowserEditor::BuildTabMenu(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("EditMenu", "Edit"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateRaw(this, &SSlateStyleBrowserEditor::FillEditMenu),
		"Edit");
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("HelpMenu", "Help"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SSlateStyleBrowserEditor::FillHelpMenu),
		"Help");
}

TSharedRef<ITableRow> SSlateStyleBrowserEditor::GenerateRow(TSharedPtr<FSlateStyleData> SlateStyleData,
                                                            const TSharedRef<STableViewBase>& TableViewBase)
{
	return SNew(SSlateStyleTableRow, TableViewBase)
		.StyleData(SlateStyleData);
}

TSharedPtr<SWidget> SSlateStyleBrowserEditor::EntryContextMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	TArray<TSharedPtr<FSlateStyleData>> Items = ListView->GetSelectedItems();
	if (Items.Num() != 1)
		return SNullWidget::NullWidget;
	TSharedPtr<SDockTab> tab = DockTab.Pin();
	Items[0]->FillRowContextMenu(MenuBuilder);
	return MenuBuilder.MakeWidget();
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

void SSlateStyleBrowserEditor::SelectCodeStyle(EDefaultCopyStyle CopyCodeStyle)
{
	DefaultCopyStyle = CopyCodeStyle;
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
			if (FilterString.IsEmpty() || key.ToString().Contains(FilterString)) {
				TSharedPtr<FSlateStyleData> d = MakeSlateStyleData(Style, StyleName, key);
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
	FilterString = Text.ToString();
	UpdateList();
}

TSharedPtr<FSlateStyleData> SSlateStyleBrowserEditor::MakeSlateStyleData(const ISlateStyle* SlateStyle, FName Style,
                                                                         FName PropertyName)
{
	TSharedPtr<FSlateStyleData> StyleData;
	TSharedPtr<ISlateStyleDataManager> DataMgr = SlateStyleDataManager.Pin();
	if (!DataMgr.IsValid())
		return nullptr;

	TArray<FName> WidgetTypes;
	DataMgr->GetRegisteredTypes(WidgetTypes);

	for (FName WidgetType : WidgetTypes) {
		StyleData = DataMgr->MakeSlateStyleData(SlateStyle, PropertyName, WidgetType);
		if (StyleData.IsValid())
			return StyleData;
	}

	/*
	 * Important: Get brushes last, so that DefaultBrushes don't interfere!
	 */
	
	const FSlateBrush* Brush = SlateStyle->GetBrush(PropertyName);
	// This is not ideal! Default brushes won't appear at all this way.
	if (Brush && Brush != SlateStyle->GetDefaultBrush()) {
		StyleData = MakeShared<FSlateStyleBrush>();
		StyleData->Initialize(Style, PropertyName, FSlateStyleBrush::TypeName, NAME_None);
		return StyleData;
	}
	
	return nullptr;
}

USlateStyleBrowserUserSettings* SSlateStyleBrowserEditor::GetConfig()
{
	return GetMutableDefault<USlateStyleBrowserUserSettings>();
}

void SSlateStyleBrowserEditor::MakeValidConfiguration()
{
	if (GetConfig()->SelectedStyle.IsNone()) {
		GetConfig()->SelectedStyle = Name_AllStyles;
	}
}
