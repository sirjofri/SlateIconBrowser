// Copyright sirjofri. Licensed under MIT license. See License.txt for full license text.

#pragma once

enum EDefaultCopyStyle
{
	DCS_FirstEntry,
	DCS_QuickStyle,
};

class FSlateStyleData
{
public:
	virtual TSharedRef<SWidget> GenerateRowWidget() = 0;
	virtual TSharedPtr<SWidget> GetExtendedPreview() { return SNullWidget::NullWidget; };

	virtual bool HasDetails() { return Details.Num() > 0; };
	virtual TMap<FString,FString>& GetDetails() { return Details; };
	virtual void InitializeDetails() {};
	
	virtual void FillRowContextMenu(FMenuBuilder& MenuBuilder);
	virtual void CopyDefault(EDefaultCopyStyle DefaultCopyStyle, const FString& QuickStyle);

	
	virtual ~FSlateStyleData() = default;

	virtual void Initialize(FName InStyle, FName InPropertyName, FName InType, FName InWidgetStyleType)
	{
		StyleName = InStyle;
		PropertyName = InPropertyName;
		Type = InType;
		WidgetStyleType = InWidgetStyleType;

		Details.Empty();
		InitializeDetails();
	};
	
	virtual FName GetStyleName() { return StyleName;};
	virtual FName GetPropertyName() { return PropertyName; };
	virtual FName GetType() { return Type; };

protected:
	FString ReadabilityReplace(const FString& Code);
	FString GenerateCopyCode(const FString& StyleCode);
	virtual void ClipboardCode(const FString& CopyCode);

	const ISlateStyle* GetSlateStyle();

	template<typename T>
	bool GetWidgetStyle(T& WidgetStyle)
	{
		const ISlateStyle* Style = GetSlateStyle();
		if (!(Style && Style->HasWidgetStyle<T>(GetPropertyName())))
			return false;
		WidgetStyle = GetSlateStyle()->GetWidgetStyle<T>(GetPropertyName());
		return true;
	};

	template<typename T>
	static FString GetEnumValue(T Value)
	{
		UEnum* Enum = StaticEnum<T>();
		return Enum->GetNameStringByValue((int64)Value);
	}

protected:
	FName StyleName;
	FName PropertyName;
	FName Type;
	FName WidgetStyleType;

	TMap<FString,FString> Details;
};

class ISlateStyleDataProvider
{
public:
	virtual ~ISlateStyleDataProvider() = default;
	virtual TSharedPtr<FSlateStyleData> MakeSlateStyleData(const ISlateStyle* SlateStyle, FName PropertyName, FName WidgetType) = 0;
	virtual TArray<FName> GetSupportedWidgetTypes() = 0;
};
