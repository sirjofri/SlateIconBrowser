// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SlateIconBrowserUserSettings.generated.h"

UENUM()
enum ECopyCodeStyle
{
	CS_FSlateIcon,
	CS_FSlateIconFinderFindIcon,
	CS_CustomStyle,
};

UCLASS(Config=EditorPerProjectUserSettings, ConfigDoNotCheckDefaults)
class USlateIconBrowserUserSettings : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	FString FilterString;

	UPROPERTY(Config)
	FString CustomStyle;

	UPROPERTY(Config)
	FName SelectedStyle;

	UPROPERTY(Config)
	TEnumAsByte<ECopyCodeStyle> CopyCodeStyle;
};
