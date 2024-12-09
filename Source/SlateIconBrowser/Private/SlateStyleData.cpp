#include "SlateStyleData.h"

#include "SlateStyleBrowserUserSettings.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Styling/SlateStyleRegistry.h"

#define LOCTEXT_NAMESPACE "SlateStyleBrowser"

void FSlateStyleData::ClipboardCode(const FString& CopyCode)
{
	if (CopyCode.IsEmpty()) {
		return;
	}
	FPlatformApplicationMisc::ClipboardCopy(*CopyCode);
	UE_LOG(LogTemp, Warning, TEXT("Copy code to clipboard: %s"), *CopyCode);

	FNotificationInfo Info(LOCTEXT("CopiedNotification", "C++ code copied to clipboard"));
	Info.ExpireDuration = 3.f;
#if ENGINE_MAJOR_VERSION == 5
	Info.SubText = FText::FromString(CopyCode);
#else
	Info.Text = FText::FromString(CopyText);
#endif
	FSlateNotificationManager::Get().AddNotification(Info);
}

const ISlateStyle* FSlateStyleData::GetSlateStyle()
{
	return FSlateStyleRegistry::FindSlateStyle(GetStyleName());
}

void FSlateStyleData::FillRowContextMenu(FMenuBuilder& MenuBuilder)
{
	const USlateStyleBrowserUserSettings* Settings = GetDefault<USlateStyleBrowserUserSettings>();
	const FCopyStyleBundle* CopyStyles = Settings->CopyStyles.Find(Type);
	if (!CopyStyles) {
		return;
	}

	auto Clipboard = [&](const FString& CopyCode)
	{
		ClipboardCode(CopyCode);
	};

	FText CopyCodeToolTip = LOCTEXT("CopyCodeToolTip", "Copy this code to clipboard");
	
	MenuBuilder.BeginSection("CopyOptions", LOCTEXT("CopyCodeOptions", "Copy Code"));
	for (const FString& Style : CopyStyles->CopyStyles) {
		FString CopyCode = GenerateCopyCode(Style);
		MenuBuilder.AddMenuEntry(
			FText::FromString(CopyCode),
			CopyCodeToolTip,
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda(Clipboard, CopyCode)));
	}
	if (!Settings->QuickStyle.IsEmpty()) {
		FString CopyCode = GenerateCopyCode(Settings->QuickStyle);
		if (!CopyCode.IsEmpty()) {
			MenuBuilder.AddMenuEntry(
				FText::Format(LOCTEXT("CopyQuickStyleMenuEntry", "Quick Style: {0}"), FText::FromString(CopyCode)),
				CopyCodeToolTip,
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda(Clipboard, CopyCode)));
		}
	}
	MenuBuilder.EndSection();
}

void FSlateStyleData::CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle)
{
	if (DefaultCopyStyle == DCS_QuickStyle) {
		ClipboardCode(GenerateCopyCode(QuickStyle));
		return;
	}
	const USlateStyleBrowserUserSettings* Settings = GetDefault<USlateStyleBrowserUserSettings>();
	const FCopyStyleBundle* CopyStyles = Settings->CopyStyles.Find(Type);
	if (!CopyStyles) {
		return;
	}

	if (CopyStyles->CopyStyles.Num() == 0) {
		return;
	}

	ClipboardCode(GenerateCopyCode(CopyStyles->CopyStyles[0]));
}

FString FSlateStyleData::ReadabilityReplace(const FString& Code)
{
	const USlateStyleBrowserUserSettings* Settings = GetDefault<USlateStyleBrowserUserSettings>();

	FString NewCode = Code;
	for (TTuple<FString,FString> kv : Settings->ReadabilityReplacements) {
		NewCode.ReplaceInline(*kv.Key, *kv.Value);
	}
	return NewCode;
}

FString FSlateStyleData::GenerateCopyCode(const FString& StyleCode)
{
	FString code = StyleCode
		.Replace(TEXT("$1"), *GetPropertyName().ToString())
		.Replace(TEXT("$2"), *GetStyleName().ToString())
		.Replace(TEXT("$3"), WidgetStyleType.IsNone() ? TEXT("$3") : *WidgetStyleType.ToString())
		.Replace(TEXT("$$"), TEXT("$"));

	return ReadabilityReplace(code);
}
