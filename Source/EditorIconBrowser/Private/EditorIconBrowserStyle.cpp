// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#include "EditorIconBrowserStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FEditorIconBrowserStyle::StyleInstance = nullptr;

void FEditorIconBrowserStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FEditorIconBrowserStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FEditorIconBrowserStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("EditorIconBrowserStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FEditorIconBrowserStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("EditorIconBrowserStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("EditorIconBrowser")->GetBaseDir() / TEXT("Resources"));

	Style->Set("EditorIconBrowser.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("Icon"), Icon20x20));

	return Style;
}

void FEditorIconBrowserStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FEditorIconBrowserStyle::Get()
{
	return *StyleInstance;
}
