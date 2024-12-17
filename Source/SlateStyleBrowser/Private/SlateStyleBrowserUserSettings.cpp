#include "SlateStyleBrowserUserSettings.h"

#include "SlateStyleBrowser.h"


USlateStyleBrowserUserSettings::USlateStyleBrowserUserSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("Slate Style Browser");
}

void USlateStyleBrowserUserSettings::PostReloadConfig(class FProperty* PropertyThatWasLoaded)
{
	Super::PostReloadConfig(PropertyThatWasLoaded);
	
	if (AllowResetToDefault || AllEmpty()) {
		FillWithDefaultStyles();
		FillWithDefaultReplacements();
		SaveConfig();
	}

	AllowResetToDefault = false;
}

void USlateStyleBrowserUserSettings::PostInitProperties()
{
	Super::PostInitProperties();

	FConfigFile* file = GConfig->Find(GetConfigFilename(this));
	if (!file || !file->DoesSectionExist(TEXT("/Script/SlateIconBrowser.SlateStyleBrowserUserSettings"))) {
		FillWithDefaultStyles();
		FillWithDefaultReplacements();
		SaveConfig();
	}
}

void USlateStyleBrowserUserSettings::FillWithDefaultStyles()
{
	CopyStyles.Empty();
	
	{ // Special case: brushes
		TArray<FString> styles {
			TEXT("FSlateIcon(FName(\"$2\"), \"$1\")"),
			TEXT("FSlateIconFinder::FindIcon(\"$1\")"),
		};
		CopyStyles.Add(FName("Brush"), FCopyStyleBundle(styles));
	}

	// load from registry defaults
	TArray<FName> RegisteredTypes;
	FSlateStyleBrowserModule& mod = FModuleManager::LoadModuleChecked<FSlateStyleBrowserModule>(TEXT("SlateStyleBrowser"));
	TSharedPtr<ISlateStyleDataManager> mgr = mod.GetSlateStyleDataManager();
	mgr->GetRegisteredTypes(RegisteredTypes);
	
	for (FName t : RegisteredTypes) {
		CopyStyles.Add(t, FCopyStyleBundle(mgr->GetDefaultCopyStyles(t)));
	}
}

void USlateStyleBrowserUserSettings::FillWithDefaultReplacements()
{
	ReadabilityReplacements.Empty();

	// ref getter
#define LIST(op) \
	op(AdvancedWidgets, FAdvancedWidgetsStyle::Get()) \
	op(AppStyle, FAppStyle::Get()) \
	op(AudioWidgetsStyle, FAudioWidgetsStyle::Get()) \
	op(BspModeStyle, FBspModeStyle::Get()) \
	op(CoreStyle, FCoreStyle::Get()) \
	op(EditorStyle, FEditorStyle::Get()) \
	op(GameplayTagStyle, FGameplayTagStyle::Get()) \
	op(GeometryCollectionEditorStyle, FGeometryCollectionEditorStyle::Get()) \
	op(IKRetargetEditorStyle, FIKRetargetEditorStyle::Get()) \
	op(IKRigEditorStyle, FIKRigEditorStyle::Get()) \
	op(LevelSequenceEditorStyle, FLevelSequenceEditorStyle::Get()) \
	op(MediaCompositingEditorStyle, FMediaCompositingEditorStyle::Get()) \
	op(MediaPlateEditorStyle, FMediaPlateEditorStyle::Get()) \
	op(ModelingToolsStyle, FModelingToolsEditorModeStyle::Get()) \
	op(NGAEditorStyle, FNGAEditorStyle::Get()) \
	op(NiagaraEditorStyle, FNiagaraEditorStyle::Get()) \
	op(NiagaraEditorWidgetsStyle, FNiagaraEditorWidgetsStyle::Get()) \
	op(ObjectMixerEditorStyle, FObjectMixerEditorStyle::Get()) \
	op(RigVMEditorStyle, FRigVMEditorStyle::Get()) \
	op(ToolkitStyle, FToolkitStyle::Get()) \
	op(TreeMapStyle, FTreeMapStyle::Get()) \
	op(UMGCoreStyle, FUMGCoreStyle::Get()) \
	op(UMGStyle, FUMGStyle::Get()) \
	op(UVStyle, FUVEditorStyle::Get()) \
	op(VariantManagerEditorStyle, FVariantManagerStyle::Get()) \

#define OP(StyleSetName, Repl) ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"") TEXT(#StyleSetName) TEXT("\")"), TEXT(#Repl));

	LIST(OP);

#undef OP
#undef LIST

	// ptr getter
#define LIST(op) \
	op(DatasmithContentEditorStyle, FDatasmithContentEditorStyle::Get()) \
	
#define OP(StyleSetName, Repl) ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"") TEXT(#StyleSetName) TEXT("\")."), TEXT(#Repl) TEXT("->"));

	LIST(OP);

#undef OP
#undef LIST

	ReadabilityReplacements.KeySort([](const FString& A, const FString& B)
	{
		return A < B;
	});
}

TArray<FName> USlateStyleBrowserUserSettings::GetValidTypes()
{
	TArray<FName> RegisteredTypes;
	FSlateStyleBrowserModule& mod = FModuleManager::LoadModuleChecked<FSlateStyleBrowserModule>(TEXT("SlateStyleBrowser"));
	TSharedPtr<ISlateStyleDataManager> mgr = mod.GetSlateStyleDataManager();
	mgr->GetRegisteredTypes(RegisteredTypes);
	return RegisteredTypes;
}

bool USlateStyleBrowserUserSettings::AllEmpty()
{
	return QuickStyle.IsEmpty() && SelectedStyle == NAME_None && CopyStyles.Num() == 0 && ReadabilityReplacements.Num() == 0;
}
