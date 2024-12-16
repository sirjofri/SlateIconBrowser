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
	ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"AppStyle\")"), TEXT("FAppStyle::Get()"));
	ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"CoreStyle\")"), TEXT("FCoreStyle::Get()"));
	ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"EditorStyle\")"), TEXT("FEditorStyle::Get()"));
	ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"UMGCoreStyle\")"), TEXT("FUMGCoreStyle::Get()"));
	ReadabilityReplacements.Add(TEXT("FSlateStyleRegistry::FindSlateStyle(\"UMGStyle\")"), TEXT("FUMGStyle::Get()"));
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
