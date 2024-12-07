// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

#include "CoreMinimal.h"
#include "SlateStyleDataManager.h"
#include "Input/Reply.h"
#include "Modules/ModuleManager.h"


class FSlateStyleDataManager;

class FSlateIconBrowserModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static TSharedPtr<ISlateStyleDataManager> GetSlateStyleDataManager();

private:
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<FSlateStyleDataManager> SlateStyleDataManager;
};
