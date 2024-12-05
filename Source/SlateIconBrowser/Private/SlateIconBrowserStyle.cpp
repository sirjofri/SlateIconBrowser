// Copyright 2022 sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "SlateIconBrowserStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

#if ENGINE_MAJOR_VERSION == 5
#include "Styling/SlateStyleMacros.h"
#define RootToContentDir Style->RootToContentDir
#else
#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#endif



TSharedPtr<FSlateStyleSet> FSlateIconBrowserStyle::StyleInstance = nullptr;

void FSlateIconBrowserStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSlateIconBrowserStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FSlateIconBrowserStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SlateStyleBrowserStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FSlateIconBrowserStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SlateStyleBrowserStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SlateIconBrowser")->GetBaseDir() / TEXT("Resources"));

#if ENGINE_MAJOR_VERSION == 5
	Style->Set("SlateStyleBrowser.Icon", new IMAGE_BRUSH_SVG(TEXT("Icon"), Icon20x20));
#else
	Style->Set("SlateStyleBrowser.Icon", new IMAGE_BRUSH(TEXT("Icon128"), Icon20x20));
#endif
	return Style;
}

void FSlateIconBrowserStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSlateIconBrowserStyle::Get()
{
	return *StyleInstance;
}
