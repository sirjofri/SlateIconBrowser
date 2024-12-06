// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SlateStyleBrowserUserSettings.generated.h"


enum EDefaultCopyStyle
{
	DCS_FirstEntry,
	DCS_QuickStyle,
};

USTRUCT()
struct FCopyStyleBundle
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config)
	TArray<FString> CopyStyles;
};

UCLASS(Config=EditorSettings, DisplayName="Slate Style Browser")
class USlateStyleBrowserUserSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USlateStyleBrowserUserSettings();
	virtual void PostReloadConfig(class FProperty* PropertyThatWasLoaded) override;

	void FillWithDefaultStyles();
	void FillWithDefaultReplacements();

	/**
	 * Enable this to make the "Reset to Default" button work.
	 */
	UPROPERTY(EditAnywhere, Category="Preferences")
	bool AllowResetToDefault = false;

	/**
	 * Fill this with default styles per type.
	 */
	UPROPERTY(EditAnywhere, Config, meta=(ShowOnlyInnerProperties, GetKeyOptions="GetValidTypes"), Category="Copy Code Styles")
	TMap<FName,FCopyStyleBundle> CopyStyles;

	/**
	 * Use this to improve readability of the copied code by replacing common styles.
	 */
	UPROPERTY(EditAnywhere, Config, Category="Adjustments")
	TMap<FString,FString> ReadabilityReplacements;

	UFUNCTION()
	TArray<FString> GetValidTypes();

public:
	UPROPERTY(Config)
	FString QuickStyle;
	
	UPROPERTY(Config)
	FName SelectedStyle;
};