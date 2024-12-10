#include "SlateStyleBrowserUserSettings.h"


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

	{
		TArray<FString> styles {
			TEXT("FSlateIcon(FName(\"$2\"), \"$1\")"),
			TEXT("FSlateIconFinder::FindIcon(\"$1\")"),
		};
		CopyStyles.Add(FName("Brush"), FCopyStyleBundle(styles));
	}
	{
		// TODO: find a way to automate adding these based on ValidTypes
		TArray<FString> styles {
			TEXT("FSlateStyleRegistry::FindSlateStyle(\"$2\")->GetWidgetStyle<FTextBlockStyle>(\"$1\")"),
		};
		CopyStyles.Add(FName("TextBlock"), FCopyStyleBundle(styles));
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

TArray<FString> USlateStyleBrowserUserSettings::GetValidTypes()
{
	return {
		TEXT("Brush"),
		TEXT("TextBlock"),
	};
}

bool USlateStyleBrowserUserSettings::AllEmpty()
{
	return QuickStyle.IsEmpty() && SelectedStyle == NAME_None && CopyStyles.Num() == 0 && ReadabilityReplacements.Num() == 0;
}
