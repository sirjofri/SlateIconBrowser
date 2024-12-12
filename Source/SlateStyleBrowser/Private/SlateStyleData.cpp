#include "SlateStyleData.h"

#include "SlateStyleBrowserUserSettings.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Styling/SlateStyleRegistry.h"
#include "UObject/PropertyIterator.h"

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
	Info.Text = FText::FromString(CopyCode);
#endif
	FSlateNotificationManager::Get().AddNotification(Info);
}

const ISlateStyle* FSlateStyleData::GetSlateStyle()
{
	return FSlateStyleRegistry::FindSlateStyle(GetStyleSetName());
}

void FSlateStyleData::AddDetail(const FString& Name, const FString& Value)
{
	Details.Add(TTuple<FString,FString>(Name, Value));
}

FString FSlateStyleData::MarginString(FMargin Margin)
{
	if (Margin.Left == Margin.Right && Margin.Top == Margin.Bottom) {
		if (Margin.Left == Margin.Top) {
			return FString::Printf(TEXT("%f"), Margin.Left);
		} else {
			return FString::Printf(TEXT("%f %f"), Margin.Left, Margin.Top);
		}
	}
	return FString::Printf(TEXT("%f %f %f %f"),
		Margin.Left,
		Margin.Top,
		Margin.Right,
		Margin.Bottom);
}

void FSlateStyleData::FillDetailsInternal(const UScriptStruct* Struct, const void* Container, int Level)
{
	// TODO: could do something with the property category
	if (Level > 2)
		return;

	auto lvl = [&](const FString& Str)
	{
		FString n;
		int l = Level;
		while (l--)
			n.AppendChar('\t');
		return n.Append(Str);
	};

	auto addslatebrush = [&](FString title, const FSlateBrush* Brush)
	{
		if (Brush->DrawAs == ESlateBrushDrawType::Type::NoDrawType)
			return;
		FString DrawAs = GetEnumValue<ESlateBrushDrawType::Type>(Brush->DrawAs);
		FString Tint = Brush->TintColor.GetSpecifiedColor().ToFColor(false).ToHex();
		if (Brush->ImageType == ESlateBrushImageType::Type::NoImage) {
			AddDetailf(title, TEXT("%s #%s"), *DrawAs, *Tint);
			return;
		}
		FString ImageType = GetEnumValue<ESlateBrushImageType::Type>(Brush->ImageType);
		FString ImageSize = FString::Printf(TEXT("(%f, %f)"), Brush->ImageSize.X, Brush->ImageSize.Y);
		AddDetailf(title, TEXT("%s %s %s #%s"), *DrawAs, *ImageType, *ImageSize, *Tint);
	};
	
	for (TFieldIterator<FProperty> It(Struct); It; ++It) {
		if (!It->HasAnyPropertyFlags(CPF_Edit | CPF_BlueprintVisible))
			continue;
		
		if (FStructProperty* p = CastField<FStructProperty>(*It)) {
			const void* SubStruct = p->ContainerPtrToValuePtr<void>(Container);
			
			// default types
			if (p->Struct == TBaseStructure<FVector>::Get()) {
				FVector* vec = (FVector*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), vec->ToString());
				continue;
			}
			if (p->Struct == TBaseStructure<FVector2D>::Get()) {
				FVector2D* vec = (FVector2D*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), vec->ToString());
				continue;
			}
			if (p->Struct == TBaseStructure<FRotator>::Get()) {
				FRotator* vec = (FRotator*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), vec->ToString());
				continue;
			}
			if (p->Struct == TBaseStructure<FColor>::Get()) {
				FColor* vec = (FColor*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), vec->ToString());
				continue;
			}
			if (p->Struct == TBaseStructure<FLinearColor>::Get()) {
				FLinearColor* col = (FLinearColor*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), col->ToString());
				continue;
			}

			// custom standard types
#if ENGINE_MAJOR_VERSION == 4 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 1)
			if (p->Struct == FDeprecateSlateVector2D::StaticStruct()) {
				FDeprecateSlateVector2D* ptr = (FDeprecateSlateVector2D*)SubStruct;
				AddDetail(lvl(p->GetDisplayNameText().ToString()), ptr->ToString());
				continue;
			}
#endif

			// custom types
			if (p->Struct == FSlateBrush::StaticStruct()) {
				addslatebrush(lvl(p->GetDisplayNameText().ToString()), p->ContainerPtrToValuePtr<FSlateBrush>(Container));
				continue;
			}
			if (p->Struct == FSlateSound::StaticStruct()) {
				const FSlateSound* snd = p->ContainerPtrToValuePtr<FSlateSound>(Container);
				FString n = TEXT("None");
				if (IsValid(snd->GetResourceObject()))
					n = snd->GetResourceObject()->GetName();
				AddDetail(lvl(p->GetDisplayNameText().ToString()), n);
				continue;
			}
			if (p->Struct == FMargin::StaticStruct()) {
				AddDetail(lvl(p->GetDisplayNameText().ToString()), MarginString(*p->ContainerPtrToValuePtr<FMargin>(Container)));
				continue;
			}
			if (p->Struct == FSlateColor::StaticStruct()) {
				AddDetail(lvl(p->GetDisplayNameText().ToString()),
					p->ContainerPtrToValuePtr<FSlateColor>(Container)->GetSpecifiedColor().ToString());
				continue;
			}
			AddDetail(lvl(p->GetDisplayNameText().ToString()), TEXT(""));
			FillDetailsInternal(p->Struct, SubStruct, Level+1);
			continue;
		}
		if (FByteProperty* p = CastField<FByteProperty>(*It)) {
			if (p->IsEnum()) {
				FString Value = p->Enum->GetNameStringByValue(p->GetPropertyValue_InContainer(Container));
				AddDetail(lvl(p->GetDisplayNameText().ToString()), Value);
			} else {
				AddDetailf(lvl(p->GetDisplayNameText().ToString()), TEXT("%d"), p->GetPropertyValue_InContainer(Container));
			}
			continue;
		}
		if (FEnumProperty* p = CastField<FEnumProperty>(*It)) {
			const FNumericProperty* Enum = p->GetUnderlyingProperty();
			int64 Value = -1;
			if (Enum->GetClass() == FByteProperty::StaticClass()) {
				const FByteProperty* bprop = CastField<FByteProperty>(Enum);
				uint8 val = *p->ContainerPtrToValuePtr<uint8>(Container);
				Value = val;
			} else if (Enum->GetClass() == FIntProperty::StaticClass()) {
				const FIntProperty* iprop = CastField<FIntProperty>(Enum);
				Value = *p->ContainerPtrToValuePtr<int64>(Container);
			}
			AddDetail(lvl(p->GetDisplayNameText().ToString()), p->GetEnum()->GetNameStringByValue(Value));
			continue;
		}
		if (FBoolProperty* p = CastField<FBoolProperty>(*It)) {
			const bool* ptr = p->ContainerPtrToValuePtr<bool>(Container);
			AddDetail(lvl(p->GetDisplayNameText().ToString()), *ptr ? TEXT("true") : TEXT("false"));
			continue;
		}
		if (FIntProperty* p = CastField<FIntProperty>(*It)) {
			const int* ptr = p->ContainerPtrToValuePtr<int>(Container);
			AddDetailf(lvl(p->GetDisplayNameText().ToString()), TEXT("%d"), *ptr);
			continue;
		}
		if (FFloatProperty* p = CastField<FFloatProperty>(*It)) {
			const float* ptr = p->ContainerPtrToValuePtr<float>(Container);
			AddDetailf(lvl(p->GetDisplayNameText().ToString()), TEXT("%f"), *ptr);
			continue;
		}
		if (FDoubleProperty* p = CastField<FDoubleProperty>(*It)) {
			const double* ptr = p->ContainerPtrToValuePtr<double>(Container);
			AddDetailf(lvl(p->GetDisplayNameText().ToString()), TEXT("%f"), *ptr);
			continue;
		}
		if (FNameProperty* p = CastField<FNameProperty>(*It)) {
			const FName* name = p->ContainerPtrToValuePtr<FName>(Container);
			AddDetail(lvl(p->GetDisplayNameText().ToString()), name->ToString());
			continue;
		}
		if (FObjectProperty* p = CastField<FObjectProperty>(*It)) {
			const UObject* obj = *p->ContainerPtrToValuePtr<UObject*>(Container);
			AddDetail(lvl(p->GetDisplayNameText().ToString()), IsValid(obj) ? obj->GetName() : TEXT("None"));
			continue;
		}
	}
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

void FSlateStyleData::Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType) {
	StyleSetName = InStyle;
	PropertyName = InPropertyName;
	Type = InType;
	WidgetStyleType = InWidgetStyleType;

	Details.Empty();
	
	InitializeDetails();
	InitializePreview();
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
		.Replace(TEXT("$2"), *GetStyleSetName().ToString())
		.Replace(TEXT("$3"), WidgetStyleType.IsNone() ? TEXT("$3") : *WidgetStyleType.ToString())
		.Replace(TEXT("$$"), TEXT("$"));

	return ReadabilityReplace(code);
}
